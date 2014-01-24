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
#include <globals/Errors.h>
#include <search/FindInFilesClass.h>
#include <unicode/ustring.h>
#include <unicode/ucnv.h>
#include <unicode/ucsdet.h>

#include <wx/string.h>
#include <wx/regex.h>
#include <wx/filename.h>

mvceditor::FindInFilesClass::FindInFilesClass(const UnicodeString& expression, mvceditor::FinderClass::Modes mode) 
	: Expression(expression)
	, ReplaceExpression()
	, Source()
	, Mode(mode)
	, Finder(expression, mode) 
	, FFile()
	, File(NULL)
	, CurrentLine()
	, LineNumber(0) 
	, LineOffset(0)
	, FileOffset(0)
	, LineStartOffset(0) {
}

mvceditor::FindInFilesClass::FindInFilesClass(const FindInFilesClass& findInFiles)
	: Expression()
	, ReplaceExpression()
	, Source()
	, Mode()
	, Finder() 
	, FFile()
	, File(NULL)
	, CurrentLine()
	, LineNumber(0) 
	, LineOffset(0)
	, FileOffset(0)
	, LineStartOffset(0) {
	Copy(findInFiles);
}

mvceditor::FindInFilesClass::~FindInFilesClass() {
	CleanupStreams();
}

bool mvceditor::FindInFilesClass::Prepare() {
	Finder.Expression = Expression;
	Finder.Mode = Mode;
	Finder.ReplaceExpression = ReplaceExpression;
	return Finder.Prepare();
}

bool mvceditor::FindInFilesClass::Walk(const wxString& fileName) {
	bool found = false;
	LineNumber = 0;
	LineOffset = 0;
	FileOffset = 0;
	LineStartOffset = 0;
	CurrentLine.remove();
	CleanupStreams();
	if (!fileName.empty()) {
		
		// use wxWidgets file class as it allows us to properly open
		// unicode filenames
		if (FFile.Open(fileName, wxT("r"))) {
			File = u_finit(FFile.fp(), NULL, NULL);
			return FindNext();
		}
	}
	return found;
}

bool mvceditor::FindInFilesClass::FindNext() {
	LineOffset = 0;
	int32_t length = 0;
	bool found = false;
	if (File) {
		while (!u_feof(File)) {
			++LineNumber;
			
			// ATTN: docs lie,... giving it a -1 for arg 2 does not work
			UChar* buf = CurrentLine.getBuffer(2048);
			u_fgets(buf, 2047, File);
			CurrentLine.releaseBuffer(-1);
			found = Finder.FindNext(CurrentLine, LineOffset) && Finder.GetLastMatch(LineOffset, length);
			if (found) {
				FileOffset = LineStartOffset + LineOffset;
				LineStartOffset += CurrentLine.length();
				break;
			}
			LineStartOffset += CurrentLine.length();
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

int mvceditor::FindInFilesClass::GetLineOffset() {
	return LineOffset;
}

int mvceditor::FindInFilesClass::GetFileOffset() {
	return FileOffset;
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

		// ATTN: problems here: this code will load entire file into memory not too efficient
		// but the regular expression classes do not work with strings
		UnicodeString fileContents;
		mvceditor::FindInFilesClass::OpenErrors error = FileContents(fileName, fileContents);
			if (NONE == error) {
			matches += ReplaceAllMatches(fileContents);
			UFILE* file = u_fopen(fileName.ToAscii(), "wb", NULL, NULL);
			if (NULL != file) {
				u_file_write(fileContents.getBuffer(), fileContents.length(), file);
				u_fclose(file);
			}
		}
		else if (mvceditor::FindInFilesClass::CHARSET_DETECTION == error) {
			mvceditor::EditorLogError(mvceditor::ERR_CHARSET_DETECTION, fileName);
		}
	}
	return matches;
}

void mvceditor::FindInFilesClass::CopyFinder(FinderClass& dest) {
	dest.Expression = Finder.Expression;
	dest.ReplaceExpression = Finder.ReplaceExpression;
	dest.Mode = Finder.Mode;
	dest.Wrap = Finder.Wrap;
}

mvceditor::FindInFilesClass::OpenErrors mvceditor::FindInFilesClass::FileContents(const wxString& fileName, UnicodeString& fileContents) {
	OpenErrors error = NONE;
	wxFFile fFile(fileName, wxT("rb"));
	if (fFile.IsOpened()) {

		// need to detect the character set so we can decode it correctly
		// at this point I could only find the character set detection code that works for
		// byte arrays; not sure one exists to detect charset from files

		// obtain file size and read the raw data
		// got this code by looking at the code of wxFFile::ReadAll
		wxFileOffset offset = fFile.Length();
		size_t bufferSize = wx_truncate_cast(size_t, fFile.Length());
		if ((wxFileOffset)bufferSize == offset) { 
			char* buffer = new char[bufferSize];
			fFile.Read(buffer, bufferSize);
			fFile.Close();

			// what encoding is it??
			error = CHARSET_DETECTION;
			UErrorCode status = U_ZERO_ERROR;
			UCharsetDetector* csd = ucsdet_open(&status);
			ucsdet_setText(csd, buffer, bufferSize, &status);
			if(U_SUCCESS(status)) {
				const UCharsetMatch *ucm = ucsdet_detect(csd, &status);
				if(U_SUCCESS(status) && ucm) {
					const char* name = ucsdet_getName(ucm, &status);

					// encode to Unicode from the detected charset. file already opened, just translate the
					// buffer from memory
					UConverter* converter = ucnv_open(name, &status);
					if (U_SUCCESS(status)) {
						UChar* dest = fileContents.getBuffer(bufferSize + 1);
						int32_t read = ucnv_toUChars(converter, dest, bufferSize + 1, buffer, bufferSize, &status);
						fileContents.releaseBuffer(read);
						if (U_SUCCESS(status)) {
							error = NONE;
						}
						ucnv_close(converter);
					}
				}
				ucsdet_close(csd);
			}
			delete[] buffer;
		}
		else {
			error = FILE_NOT_FOUND;
		}
	}
	else {
		error = FILE_NOT_FOUND;
	}
	return error;
}

mvceditor::FindInFilesClass& mvceditor::FindInFilesClass::operator=(const FindInFilesClass& src) {
	Copy(src);
	return *this;
}

void mvceditor::FindInFilesClass::Copy(const FindInFilesClass& src) {
	CleanupStreams();

	Expression = src.Expression;
	ReplaceExpression = src.ReplaceExpression;
	Source = src.Source;
	Mode = src.Mode;
	Finder.Expression = src.Finder.Expression;
	Finder.Mode = src.Finder.Mode;

	LineNumber = 0;
	LineOffset = 0;
	FileOffset = 0;
	LineStartOffset = 0;
}

void mvceditor::FindInFilesClass::CleanupStreams() {
	if (NULL != File) {
		u_fclose(File);
		File = NULL;
	}
	FFile.Close();
}