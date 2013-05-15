/**
 * This software is released under the terms of the MIT License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <pelet/LexicalAnalyzerClass.h>
#include <pelet/TokenClass.h>
#include <pelet/ParserClass.h>
#include <language/SymbolTableClass.h>
#include <language/TagCacheClass.h>
#include <search/DirectorySearchClass.h>
#include <search/TagFinderClass.h>
#include <globals/Assets.h>
#include <globals/Sqlite.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <wx/filefn.h>
#include <wx/timer.h>
#include <wx/utils.h>
#include <unicode/uclean.h>

/**
 * Profiles the LexicalAnalyzerClass using the native.php file in resources 
 * directory.  It also profiles code that will loop through the file and
 * append it to a string(in order to calculate the overhead of the 
 * Lexical Analyzer).
 */
void ProfileLexer();

/**
 * Profiles the ParserClass lint method.
 */
void ProfileParser();

/**
 * Profiles a large project (500k LOC).
 */
void ProfileParserOnLargeProject();

/**
 * Profiles the TagPaserClass and ParsedTagFinderClass using the php.db file 
 * in resources directory.  It profiles two consecutive times in order to
 * compare caching. By using this in combination with the results of 
 * ProfileLexer test, we can find out how much overhead initializing a db file takes.
 */
void ProfileNativeFunctionsParsing();

/** 
 * This class will help in parsing the large project
 */
class ParserDirectoryWalkerClass : public mvceditor::DirectoryWalkerClass {
public:

	ParserDirectoryWalkerClass();
	
	virtual bool Walk(const wxString& file);
	
	int WithErrors;
	
	int WithNoErrors;
	
private:

	pelet::ParserClass Parser;
};

/**
 * Profiles a large project (500k LOC). It profiles two consecutive times in order to
 * compare caching.
 */
void ProfileResourceFinderOnLargeProject();

/**
 * Full path to a big php file that will be used to profile the execution time of the lexer & parser.
 */
wxString FileName;

/**
 * Full path to a big directory of php files that will be used to profile the execution time of the resource finder.
 */
wxString DirName;

/**
 * Full path to the location of the SQLITE resource db.  This file will be created if it does not exist,
 */
wxString DbFileName;

int main() {
	int major,
		minor;
	wxOperatingSystemId os = wxGetOsVersion(&major, &minor);
	if (os == wxOS_WINDOWS_NT) {
		FileName = wxT("C:\\Users\\roberto\\Documents\\mvc-editor\\php_detectors\\lib\\Zend\\Config.php");
		DirName = wxT("C:\\Users\\roberto\\sample_php_project");
		DbFileName = wxT("C:\\Users\\roberto\\Desktop\\resource.db");
	}
	else {
		FileName = wxT("/home/roberto/workspace/mvc-editor/php_detectors/lib/Zend/Config.php");
		DirName = wxT("/home/roberto/workspace/sample_php_project/");
		DbFileName = wxT("/home/roberto/workspace/resource.db");
	}
	//ProfileLexer();
	//ProfileParser();
	ProfileParserOnLargeProject();
	//ProfileNativeFunctionsParsing();
	ProfileResourceFinderOnLargeProject();
	
	// calling cleanup here so that we can run this binary through a memory leak detector 
	// ICU will cache many things and that will cause the detector to output "possible leaks"
	u_cleanup();
	return 0;
}

void ProfileLexer() {
	printf("*******\n");
	pelet::LexicalAnalyzerClass lexer;
	if (FileName.IsEmpty() || !wxFileExists(FileName)) {
		printf("Nor running Profile Lexer because file was not found: %s", (const char*)FileName.ToAscii());
		return;
	}
	wxLongLong time;

	time = wxGetLocalTimeMillis();
	std::string stdFile(FileName.ToAscii());
	lexer.OpenFile(stdFile);
	int token = 0, 
		tokenCount = 0;
	UnicodeString uniLexeme;
	do  {
		token = lexer.NextToken();
		lexer.GetLexeme(uniLexeme);
		++tokenCount;
	} while (!pelet::IsTerminatingToken(token));
	time = wxGetLocalTimeMillis() - time;
	printf("time for lexer:%ld ms tokenCount=%d\n", time.ToLong(), tokenCount);

	
	time = wxGetLocalTimeMillis();
	UFILE* file = u_fopen(FileName.ToAscii(), "r", NULL, NULL);
	UnicodeString ustr;
	UChar ch = u_fgetc(file);
	while (ch != U_EOF) {
		ustr.append(ch);
		ch = u_fgetc(file);
	}
	u_fclose(file);
	time = wxGetLocalTimeMillis() - time;
	printf("time for file one-by-one iteration:%ld ms chars=%d\n", time.ToLong(), ustr.length());

	time = wxGetLocalTimeMillis();
	file = u_fopen(FileName.ToAscii(), "r", NULL, NULL);
	ustr = UNICODE_STRING_SIMPLE("");
	UChar chars[512];
	do {
		int read = u_file_read(chars, 512, file);
		if (read > 0) {
			for (int i = 0; i < read; ++i) {
				ustr.append(chars[i]);
			}
		}
	} while (!u_feof(file));
	u_fclose(file);
	time = wxGetLocalTimeMillis() - time;
	printf("time for file chunked iteration:%ld ms chars=%d\n", time.ToLong(), ustr.length());	
}

void ProfileParser() {
	printf("*******\n");
	wxLongLong time;
	time = wxGetLocalTimeMillis();
	pelet::ParserClass parser;
	pelet::LintResultsClass error;

	std::string stdFile(FileName.ToAscii());
	if (parser.LintFile(stdFile, error)) {
		printf("No syntax errors on %s\n", (const char *)FileName.ToAscii());
	}
	else {
		UFILE *out = u_finit(stdout, NULL, NULL);
		u_fprintf(out, "%S on file %s around line %d\n", error.Error.getTerminatedBuffer(),
			(const char*)FileName.ToAscii(), error.LineNumber);
		u_fclose(out);
	}
	time = wxGetLocalTimeMillis() - time;
	printf("time for parsing a native.php:%ld ms\n", time.ToLong());
}


void ProfileNativeFunctionsParsing() {
	printf("*******\n");
	
	// initialize the sqlite db
	soci::session session;
	try {
		session.open(*soci::factory_sqlite3(), mvceditor::WxToChar(mvceditor::NativeFunctionsAsset().GetFullPath()));
	} catch(std::exception const& e) {
		session.close();
		wxString msg = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
	mvceditor::ParsedTagFinderClass tagFinder;

	wxLongLong time;
	size_t found;

	time = wxGetLocalTimeMillis();
	tagFinder.Init(&session);
	
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("stristr"));
	std::vector<mvceditor::TagClass> matches = tagFinder.NearMatchTags(tagSearch);
	found = !matches.empty();
	time = wxGetLocalTimeMillis() - time;
	printf("time for tagFinder on php.db:%ld ms found:%d\n", time.ToLong(), (int)found);
	
	time = wxGetLocalTimeMillis();
	matches = tagFinder.NearMatchTags(UNICODE_STRING_SIMPLE("mysql_query"));
	time = wxGetLocalTimeMillis() - time;
	found = !matches.empty();
	printf("time for tagFinder on php.db after caching:%ld ms found:%d\n", time.ToLong(), (int)found);
}

void ProfileResourceFinderOnLargeProject() {
	printf("*******\n");
	// initialize the sqlite db
	soci::session session;
	try {
		session.open(*soci::factory_sqlite3(), mvceditor::WxToChar(DbFileName));
		wxString error;
		if (!mvceditor::SqliteSqlScript(mvceditor::ResourceSqlSchemaAsset(), session, error)) {
			wxASSERT_MSG(false, error);
		}
	} catch(std::exception const& e) {
		session.close();
		wxString msg = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}

	mvceditor::TagParserClass tagParser;
	mvceditor::ParsedTagFinderClass tagFinder;

	
	mvceditor::DirectorySearchClass search;
	wxLongLong time;
	if (DirName.IsEmpty() || !wxDirExists(DirName)) {
		printf("Nor running ProfileResourceFinderOnLargeProject because file was not found: %s", 
			(const char*)DirName.ToAscii());
		return;
	}
	time = wxGetLocalTimeMillis();
	tagParser.PhpFileExtensions.push_back(wxT("*.php"));
	tagParser.Init(&session);
	tagFinder.Init(&session);
	search.Init(DirName);
	while (search.More()) {
		search.Walk(tagParser);
	}
	std::vector<mvceditor::TagClass> matches = tagFinder.NearMatchTags(UNICODE_STRING_SIMPLE("ExtendedRecordSetForUnitTestAddGetLeftJoin"));
	time = wxGetLocalTimeMillis() - time;
	size_t found =  matches.size();
	printf("time for tagFinder on entire project:%ld ms found:%d\n", time.ToLong(), (int)found);

	time = wxGetLocalTimeMillis();
	search.Init(DirName);
	while (search.More()) {
		search.Walk(tagParser);
	}
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("Record::get"));
	matches = tagFinder.NearMatchTags(tagSearch);
	time = wxGetLocalTimeMillis() - time;
	found = matches.size();
	printf("time for tagFinder on entire project after caching:%ld ms found:%d\n", time.ToLong(), (int)found);
}

ParserDirectoryWalkerClass::ParserDirectoryWalkerClass() 
	: WithErrors(0)
	, WithNoErrors(0)
	, Parser() {
		
}

bool ParserDirectoryWalkerClass::Walk(const wxString& file) {
	if (file.EndsWith(wxT(".php"))) {
		pelet::LintResultsClass error;
		std::string stdFile(file.ToAscii());
		if (Parser.LintFile(stdFile, error)) {
			WithNoErrors++;
		}
		else {
			UFILE *out = u_finit(stdout, NULL, NULL);
			u_fprintf(out, "%S on file %s around line %d\n", error.Error.getTerminatedBuffer(), 
				(const char*)file.ToAscii(), error.LineNumber);
			u_fclose(out);
			WithErrors++;
		}
		return true;
	}
	return false;
}

void ProfileParserOnLargeProject() {
	printf("*******\n");
	mvceditor::DirectorySearchClass search;
	wxLongLong time;
	if (DirName.IsEmpty() || !wxDirExists(DirName) || !search.Init(DirName)) {
		printf("Nor running ProfileParserOnLargeProject because directory was not found or is empty: %s", 
			(const char*)DirName.ToAscii());
		return;
	}
	time = wxGetLocalTimeMillis();
	ParserDirectoryWalkerClass walker;
	while (search.More()) {
		search.Walk(walker);
	}
	time = wxGetLocalTimeMillis() - time;
	printf("time for ParserClass LintFile on entire project:%ld ms\n", time.ToLong());
	printf("Files with errors:%d\nFiles with no errors:%d\n", walker.WithErrors, walker.WithNoErrors);
}