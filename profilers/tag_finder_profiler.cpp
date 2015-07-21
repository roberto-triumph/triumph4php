/**
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#include <pelet/LexicalAnalyzerClass.h>
#include <pelet/ParserClass.h>
#include <pelet/TokenClass.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <unicode/uclean.h>
#include <wx/ffile.h>
#include <wx/filefn.h>
#include <wx/timer.h>
#include <wx/utils.h>
#include <iostream>
#include <string>
#include <vector>
#include "globals/Assets.h"
#include "globals/Sqlite.h"
#include "language_php/ParsedTagFinderClass.h"
#include "language_php/PhpFunctionCallLintClass.h"
#include "language_php/PhpIdentifierLintClass.h"
#include "language_php/PhpVariableLintClass.h"
#include "language_php/SymbolTableClass.h"
#include "language_php/TagCacheClass.h"
#include "language_php/TagFinderList.h"
#include "search/DirectorySearchClass.h"

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
 * Profiles the variable linter on a large project (500k LOC).
 */
void ProfileVariableLintOnLargeProject();

/**
 * Profiles the ParsedTagFinderClass.  This function requires that the
 * tag cache has already been built.
 */
void ProfileTagSearch();

/**
 * This class will help in parsing the large project
 */
class ParserDirectoryWalkerClass : public t4p::DirectoryWalkerClass {
	public:
	ParserDirectoryWalkerClass();

	virtual bool Walk(const wxString& file);

	int WithErrors;

	int WithNoErrors;

	private:
	pelet::ParserClass Parser;
};


/**
 * This class will help in linting a large project
 */
class VariableLinterWalkerClass : public t4p::DirectoryWalkerClass {
	public:
	VariableLinterWalkerClass(t4p::TagCacheClass& tagCache);

	virtual bool Walk(const wxString& file);

	int WithErrors;

	int WithNoErrors;

	private:
	t4p::PhpVariableLintOptionsClass Options;
	t4p::PhpVariableLintClass Linter;
	t4p::PhpIdentifierLintClass IdentifierLinter;
	t4p::PhpFunctionCallLintClass CallLinter;
};


/**
 * Profiles a large project (500k LOC). It profiles the time it takes to build
 * the tag cache.
 */
void ProfileTagParserOnLargeProject();

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

int main(int argc, char** argv) {
	int major,
		minor;
	wxOperatingSystemId os = wxGetOsVersion(&major, &minor);
	if (os == wxOS_WINDOWS_NT) {
		FileName = wxT("C:\\Users\\roberto\\Documents\\triumph4php\\php_detectors\\lib\\Zend\\Config.php");
		DirName = wxT("C:\\Users\\roberto\\sample_php_project");
		DbFileName = wxT("resource.db");
	} else {
		FileName = wxT("/home/roberto/workspace/triumph4php/php_detectors/lib/Zend/Config.php");
		DirName = wxT("/home/roberto/public_html/wordpress");
		DbFileName = wxT("resource.db");
	}
	std::string test;
	if (argc < 2) {
		std::cout << "Choose a test:" << std::endl
			<< "1. Lexer" << std::endl
			<< "2. Parser" << std::endl
			<< "3. Lint (On Project)" << std::endl
			<< "4. TagParser (On Project)" << std::endl
			<< "5. TagFinder (On Project)" << std::endl
			<< "6. Variable Linter (On Project)" << std::endl;
		std::string in;
		std::cin >> test;
	} else {
		test = argv[1];
	}

	if (test == "1") {
		ProfileLexer();
	} else if (test == "2") {
		ProfileParser();
	} else if (test == "3") {
		ProfileParserOnLargeProject();
	} else if (test == "4") {
		ProfileTagParserOnLargeProject();
	} else if (test == "5") {
		ProfileTagSearch();
	} else if (test == "6") {
		ProfileVariableLintOnLargeProject();
	} else if (test == "--help" || test == "-h") {
		std::cout << "this is a program that is used to profile PHP parsing ang tag cache" << std::endl
			<< "usage: tag_finder_profiler [test]" << std::endl
			 << "[test] can be one of:" << std::endl
			<< "1 (for lexer)" << std::endl
			<< "2 (for parser)" << std::endl
			<< "3 (for lint On Project)" << std::endl
			<< "4 (for tagparser On Project)" << std::endl
			<< "5 (for tagfinder On Project)" << std::endl
			<< "6 (for variable linter On Project)" << std::endl;
	} else {
		std::cout << "unknown test:" << test << std::endl;
	}

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
	} else {
		UFILE *out = u_finit(stdout, NULL, NULL);
		u_fprintf(out, "%S on file %s around line %d\n", error.Error.getTerminatedBuffer(),
			(const char*)FileName.ToAscii(), error.LineNumber);
		u_fclose(out);
	}
	time = wxGetLocalTimeMillis() - time;
	printf("time for parsing a native.php:%ld ms\n", time.ToLong());
}

void ProfileTagParserOnLargeProject() {
	printf("*******\n");
	// initialize the sqlite db
	soci::session session;
	try {
		session.open(*soci::factory_sqlite3(), t4p::WxToChar(DbFileName));
		wxString error;
		if (!t4p::SqliteSqlScript(t4p::ResourceSqlSchemaAsset(), session, error)) {
			wxASSERT_MSG(false, error);
		}
	} catch(std::exception const& e) {
		session.close();
		wxString msg = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}

	t4p::TagParserClass tagParser;
	t4p::ParsedTagFinderClass tagFinder(session);

	t4p::DirectorySearchClass search;
	wxLongLong time;
	if (DirName.IsEmpty() || !wxDirExists(DirName)) {
		printf("Nor running ProfileResourceFinderOnLargeProject because file was not found: %s",
			(const char*)DirName.ToAscii());
		return;
	}
	time = wxGetLocalTimeMillis();
	tagParser.PhpFileExtensions.push_back(wxT("*.php"));
	tagParser.Init(&session);
	search.Init(DirName);
	while (search.More()) {
		search.Walk(tagParser);
	}
	time = wxGetLocalTimeMillis() - time;
	printf("time for tagFinder on entire project:%ld ms\n", time.ToLong());
}

void ProfileTagSearch() {
	soci::session session(*soci::factory_sqlite3(), t4p::WxToChar(DbFileName));
	t4p::ParsedTagFinderClass tagFinder(session);
	wxLongLong time;
	size_t found = 0;

	time = wxGetLocalTimeMillis();

	std::vector<t4p::PhpTagClass> matches;
	t4p::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("Record::get"));
	t4p::TagResultClass* result = tagSearch.CreateNearMatchResults();
	tagFinder.Exec(result);
	found = 0;
	while (result->More()) {
		result->Next();
		found++;
	}
	delete result;
	found = matches.size();
	time = wxGetLocalTimeMillis() - time;
	printf("time for tagFinder on entire project after caching:%ld ms found:%ld\n", time.ToLong(), found);
}

ParserDirectoryWalkerClass::ParserDirectoryWalkerClass()
	: WithErrors(0)
	, WithNoErrors(0)
	, Parser() {
	Parser.SetVersion(pelet::PHP_54);
}

bool ParserDirectoryWalkerClass::Walk(const wxString& file) {
	if (file.EndsWith(wxT(".php"))) {
		if (file.Find(wxT("FlattenExceptionTest.php")) != wxNOT_FOUND) {
			printf("file= %s\n", t4p::WxToChar(file).c_str());
		}
		pelet::LintResultsClass error;
		wxFFile ffile(file);
		UnicodeString uniFile = t4p::WxToIcu(file);
		if (Parser.LintFile(ffile.fp(), uniFile, error)) {
			WithNoErrors++;
		} else {
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
	t4p::DirectorySearchClass search;
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


VariableLinterWalkerClass::VariableLinterWalkerClass(t4p::TagCacheClass& tagCache)
	: WithErrors(0)
	, WithNoErrors(0)
	, Options()
	, Linter()
	, IdentifierLinter()
	, CallLinter() {
	Options.CheckGlobalScope = false;
	Options.Version = pelet::PHP_54;
	Linter.SetOptions(Options);
	IdentifierLinter.SetVersion(pelet::PHP_54);
	Linter.Init(tagCache);
	IdentifierLinter.Init(tagCache);
	CallLinter.Init(tagCache);
	CallLinter.SetVersion(pelet::PHP_54);
}

bool VariableLinterWalkerClass::Walk(const wxString& file) {
	if (file.EndsWith(wxT(".php"))) {
		std::vector<t4p::PhpVariableLintResultClass> results;
		std::vector<t4p::PhpIdentifierLintResultClass> identifierResults;
		std::vector<t4p::PhpFunctionCallLintResultClass> callResults;
		std::string stdFile(file.ToAscii());
		UFILE *out = u_finit(stdout, NULL, NULL);
		if (Linter.ParseFile(wxFileName(file), results)) {
			for (size_t i = 0; i < results.size(); ++i) {
				u_fprintf(out, "unitialized variable `%S` on file %S around line %d\n",
					results[i].VariableName.getTerminatedBuffer(),
					results[i].File.getTerminatedBuffer(),
					results[i].LineNumber);
			}
		}
		if (IdentifierLinter.ParseFile(wxFileName(file), identifierResults)) {
			for (size_t i = 0; i < identifierResults.size(); ++i) {
				u_fprintf(out, "unknown %d `%S` on file %S around line %d\n",
					identifierResults[i].Type,
					identifierResults[i].Identifier.getTerminatedBuffer(),
					identifierResults[i].File.getTerminatedBuffer(),
					identifierResults[i].LineNumber);
			}
		}
		if (CallLinter.ParseFile(wxFileName(file), callResults)) {
			for (size_t i = 0; i < callResults.size(); ++i) {
				if (callResults[i].Type == t4p::PhpFunctionCallLintResultClass::TOO_FEW_ARGS) {
					u_fprintf(out,
						"missing arguments to function `%S`: expected %d but calling with %d arguments on file %S around line %d\n",
						callResults[i].Identifier.getTerminatedBuffer(),
						callResults[i].ExpectedCount,
						callResults[i].ActualCount,
						callResults[i].File.getTerminatedBuffer(),
						callResults[i].LineNumber);
				} else if (callResults[i].Type == t4p::PhpFunctionCallLintResultClass::TOO_MANY_ARGS) {
					u_fprintf(out,
						"too many arguments to function `%S`: expected %d but calling with %d arguments on file %S around line %d\n",
						callResults[i].Identifier.getTerminatedBuffer(),
						callResults[i].ExpectedCount,
						callResults[i].ActualCount,
						callResults[i].File.getTerminatedBuffer(),
						callResults[i].LineNumber);
				}
			}
		}

		u_fclose(out);

		if (results.empty() && identifierResults.empty() && callResults.empty()) {
			WithNoErrors++;
		} else {
			WithErrors++;
		}
		return true;
	}
	return false;
}

void ProfileVariableLintOnLargeProject() {
	printf("*******\n");
	t4p::DirectorySearchClass search;
	t4p::TagCacheClass tagCache;
	wxFileName nativeDbFileName = t4p::NativeFunctionsAsset();
	wxFileName tagDbFileName("/home/roberto/.triumph4php/tags.db");
	std::vector<wxString> phpFileExtensions;
	phpFileExtensions.push_back(wxT("*.php"));
	std::vector<wxString> miscFileExtensions;

	t4p::TagFinderListClass* tagFinderList = new t4p::TagFinderListClass();
	tagFinderList->InitNativeTag(nativeDbFileName);
	tagFinderList->InitGlobalTag(tagDbFileName, phpFileExtensions, miscFileExtensions, pelet::PHP_54);
	tagCache.RegisterGlobal(tagFinderList);
	wxLongLong time;
	if (DirName.IsEmpty() || !wxDirExists(DirName) || !search.Init(DirName)) {
		printf("Nor running ProfileParserOnLargeProject because directory was not found or is empty: %s",
			(const char*)DirName.ToAscii());
		return;
	}
	time = wxGetLocalTimeMillis();
	VariableLinterWalkerClass walker(tagCache);
	while (search.More()) {
		search.Walk(walker);
	}
	time = wxGetLocalTimeMillis() - time;
	printf("time for PhpVariable Linter on entire project:%ld ms\n", time.ToLong());
	printf("Files with errors:%d\nFiles with no errors:%d\n", walker.WithErrors, walker.WithNoErrors);
}
