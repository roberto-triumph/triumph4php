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
 * @author     $Author: robertop2004@gmail.com $
 * @date       $Date: 2011-08-13 20:31:56 -0700 (Sat, 13 Aug 2011) $
 * @version    $Rev: 596 $ 
 */
#include <language/LexicalAnalyzerClass.h>
#include <search/FindInFilesClass.h>
#include <unicode/ustring.h>
#include <unicode/ucnv.h>
#include <unicode/ucsdet.h>

#include <wx/wx.h>
#include <wx/filename.h>

mvceditor::FindInFilesClass::FindInFilesClass(const UnicodeString& expression, int mode) 
	: Expression(expression)
	, ReplaceExpression()
	, FilesFilter(wxT("*.*"))
	, Mode(mode)
	, CaseSensitive(true)
	, Finder(expression, mode) 
	, FilesFilterRegEx()
	, File(NULL)
	, CurrentLine()
	, LineNumber(0) {
}

mvceditor::FindInFilesClass::FindInFilesClass(const FindInFilesClass& findInFiles) {
	Expression = findInFiles.Expression;
	ReplaceExpression = findInFiles.ReplaceExpression;
	FilesFilter = findInFiles.FilesFilter;
	Mode = findInFiles.Mode;
	CaseSensitive = findInFiles.CaseSensitive;
	Finder.Expression = findInFiles.Finder.Expression;
	Finder.Mode = findInFiles.Finder.Mode;
	File = NULL;
	LineNumber = 0;
}

mvceditor::FindInFilesClass::~FindInFilesClass() {
	CleanupStreams();
}

bool mvceditor::FindInFilesClass::Prepare() {
	Finder.Expression = Expression;
	Finder.Mode = Mode;
	Finder.ReplaceExpression = ReplaceExpression;
	Finder.CaseSensitive = CaseSensitive;
	FilesFilterRegEx.Compile(CreateFilesFilterRegEx(), wxRE_ADVANCED);
	return Finder.Prepare() && FilesFilterRegEx.IsValid();
}

bool mvceditor::FindInFilesClass::Walk(const wxString& fileName) {
	bool found = false;
	LineNumber = 0;
	CurrentLine.remove();
	CleanupStreams();
	if (FilesFilterRegEx.IsValid() && FilesFilterRegEx.Matches(fileName)) {
		if (!fileName.empty()) {
			
			// ATTN: unicode file names?
			File = u_fopen(fileName.ToAscii(), "r", NULL, NULL);
			if (File) {
				return FindNext();
			}
		}
	}
	return found;
}

bool mvceditor::FindInFilesClass::FindNext() {
	int32_t pos = 0,
		length;
	bool found = false;
	if (File) {
		while (!u_feof(File)) {
			++LineNumber;
			
			// ATTN: docs lie,... giving it a -1 for arg 2 does not work
			u_fgets(CurrentLine.getBuffer(2048), 2048, File);
			CurrentLine.releaseBuffer(-1);			
			found = Finder.FindNext(CurrentLine, pos) && Finder.GetLastMatch(pos, length);
			if (found) {
				break;
			}
		}
		if (u_feof(File)) {
			CleanupStreams();
		}
	}
	return found;
}

int mvceditor::FindInFilesClass::GetCurrentLineNumber() {
	return LineNumber;
}

UnicodeString mvceditor::FindInFilesClass::GetCurrentLine() {
	return CurrentLine;
}

bool mvceditor::FindInFilesClass::GetLastReplacementText(const UnicodeString& text, UnicodeString& replacementText) const {
	return Finder.GetLastReplacementText(text, replacementText);
}

int mvceditor::FindInFilesClass::ReplaceAllMatches(UnicodeString& text) const {
	return Finder.ReplaceAllMatches(text);
}

int mvceditor::FindInFilesClass::ReplaceAllMatchesInFile(const wxString& fileName) const {
	int matches = 0;
	if (!fileName.empty() && wxFileName::IsFileReadable(fileName)) {

		// TODO: problems here: this code will load entire file into memory not too efficient
		// TODO: double memory hit going from ICU string to WX string
		UnicodeString fileContents;
		FileContents(fileName, fileContents);
		matches += ReplaceAllMatches(fileContents);
		UFILE* file = u_fopen(fileName.ToAscii(), "wb", NULL, NULL);
		if (NULL != file) {
			u_file_write(fileContents.getBuffer(), fileContents.length(), file);
			u_fclose(file);
		}
	}
	return matches;
}

void mvceditor::FindInFilesClass::CopyFinder(FinderClass& dest) {
	dest.Expression = Finder.Expression;
	dest.ReplaceExpression = Finder.ReplaceExpression;
	dest.Mode = Finder.Mode;
	dest.CaseSensitive = Finder.CaseSensitive;
	dest.Wrap = Finder.Wrap;
}

void mvceditor::FindInFilesClass::FileContents(const wxString& fileName, UnicodeString& fileContents) {

	// TODO: inefficient; will load entire file into memory ... twice!!
	// TODO: handle unicode file names
	FILE* rawFile = fopen(fileName.ToAscii(), "rb");
	if (rawFile) {
		
		// obtain file size and read the raw data
		fseek(rawFile, 0, SEEK_END);
		int32_t size = ftell(rawFile);
		rewind(rawFile);		
		char* tmp = new char[size];
		fread(tmp, 1, size, rawFile);
		fclose(rawFile);

		// what encoding is it??
		UErrorCode status = U_ZERO_ERROR;
		const char* name = 0;
		UCharsetDetector* csd = ucsdet_open(&status);
		const UCharsetMatch *ucm;		
		ucsdet_setText(csd, tmp, size, &status);
		if(U_SUCCESS(status)) {
			ucm = ucsdet_detect(csd, &status);
			if(U_SUCCESS(status) && ucm != NULL) {
				name = ucsdet_getName(ucm, &status);
				if(U_SUCCESS(status)) {
					//wxString s = wxString::FromAscii(name);
					//int32_t cc = ucsdet_getConfidence(ucm, &status);
					//s += wxT(" confidence=");
					//s += wxString::Format(wxT("%d"), cc);
					//wxMessageBox(s);
				} else wxMessageBox(wxT("could not get detected charset name"));
			} //else wxMessageBox(wxT("could not detect charset"));
			ucsdet_close(csd);
		} //else wxMessageBox(wxT("could init detector"));
		
		delete[] tmp;
		
		// encode to string. file already opened, no need to check again
		// open without newline translations; very important to find out
		// tricky newline problems
		UFILE* file = u_fopen(fileName.ToAscii(), "rb", NULL, NULL);
		if (name != 0) {
			int error = u_fsetcodepage(name, file);
			if(0 == error) {
				int32_t read = u_file_read(fileContents.getBuffer(size + 1), size, file);
				fileContents.releaseBuffer(read);
			}
		}
		u_fclose(file);
	}
}
	
wxString mvceditor::FindInFilesClass::CreateFilesFilterRegEx() const {
	wxString escapedExpression = FilesFilter;
	
	// allow ? and * wildcards, turn ',' into '|'
	wxString symbols = wxT("!@#$%^&()[]{}\\-+.\"|;");
	int pos = escapedExpression.find_first_of(symbols, 0);
	while (-1 != pos) {
		wxString symbol = escapedExpression.substr(pos, 1);
		symbol = wxT("\\") + symbol;
		escapedExpression.replace(pos, 1, symbol, 2);
		pos = escapedExpression.find_first_of(symbols, pos + 2);
	}
	escapedExpression.Replace(wxT(","), wxT("$|"));
	escapedExpression.Replace(wxT("*"), wxT(".*"));
	escapedExpression.Replace(wxT("?"), wxT(".?"));
	escapedExpression.Append(wxT("$"));
	return escapedExpression;
}

mvceditor::FindInFilesClass& mvceditor::FindInFilesClass::operator=(const FindInFilesClass& findInFiles) {
	Expression = findInFiles.Expression;
	ReplaceExpression = findInFiles.ReplaceExpression;
	FilesFilter = findInFiles.FilesFilter;
	Mode = findInFiles.Mode;
	CaseSensitive = findInFiles.CaseSensitive;
	Finder.Expression = findInFiles.Finder.Expression;
	Finder.Mode = findInFiles.Finder.Mode;
	return *this;
}

void mvceditor::FindInFilesClass::CleanupStreams() {
	if (NULL != File) {
		u_fclose(File);
		File = NULL;
	}
}