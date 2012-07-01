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
#include <search/DirectorySearchClass.h>
#include <wx/dir.h>
#include <wx/tokenzr.h>
#include <wx/filename.h>

mvceditor::SourceClass::SourceClass()  
	: RootDirectory()
	, IncludeWildcards() 
	, ExcludeWildcards() {
	IncludeRegEx = NULL;
	ExcludeRegEx = NULL;
}

mvceditor::SourceClass::SourceClass(const mvceditor::SourceClass& src)  
	: RootDirectory()
	, IncludeWildcards() 
	, ExcludeWildcards() {
	IncludeRegEx = NULL;
	ExcludeRegEx = NULL;
	Copy(src);
}

mvceditor::SourceClass::~SourceClass() {
	if (IncludeRegEx) {
		delete IncludeRegEx;
	}
	if (ExcludeRegEx) {
		delete ExcludeRegEx;
	}
}

void mvceditor::SourceClass::Copy(const mvceditor::SourceClass& src) {
	SetIncludeWildcards(src.IncludeWildcardsString());
	SetExcludeWildcards(src.ExcludeWildcardsString());

	wxString path;
	path.Append(src.RootDirectory.GetPath());
	RootDirectory.AssignDir(path);
}

void mvceditor::SourceClass::operator=(const mvceditor::SourceClass& src) {
	Copy(src);
}

void mvceditor::SourceClass::ClearIncludeWildcards() {
	SetIncludeWildcards(wxT("*"));
}

void mvceditor::SourceClass::ClearExcludeWildcards() {
	SetExcludeWildcards(wxT(""));
}

void mvceditor::SourceClass::SetIncludeWildcards(const wxString& wildcardString) {
	if (IncludeRegEx) {
		delete IncludeRegEx;
		IncludeRegEx = NULL;
	}
	if (!wildcardString.IsEmpty()) {
		IncludeWildcards = wildcardString;
		IncludeRegEx = new wxRegEx(WildcardRegEx(IncludeWildcards), wxRE_ICASE | wxRE_ADVANCED);
	}
}

void mvceditor::SourceClass::SetExcludeWildcards(const wxString& wildcardString) {
	if (ExcludeRegEx) {
		delete ExcludeRegEx;
		ExcludeRegEx = NULL;
	}
	if (!wildcardString.IsEmpty()) {
		ExcludeWildcards = wildcardString;
		ExcludeRegEx = new wxRegEx(WildcardRegEx(ExcludeWildcards), wxRE_ICASE | wxRE_ADVANCED);
	}
}

wxString mvceditor::SourceClass::IncludeWildcardsString() const {
	return IncludeWildcards;
}

wxString mvceditor::SourceClass::ExcludeWildcardsString() const {
	return ExcludeWildcards;
}

bool mvceditor::SourceClass::Contains(const wxString& fullPath) const {
	
	// validations: 
	// 1. fullPath must be in RootDirectory
	// 2. fullPath must NOT match the exclude wildcards if set
	// 2. fullPath must match wildcards
	if (fullPath.Find(RootDirectory.GetFullPath()) != 0) {
		return false;
	}
	if (ExcludeRegEx && ExcludeRegEx->IsValid()) {
		if (ExcludeRegEx->Matches(fullPath)) {
			return false;
		}
	}
	bool matchedInclude = IncludeRegEx  && IncludeRegEx->IsValid() && IncludeRegEx->Matches(fullPath);
	return matchedInclude;
}

wxString mvceditor::SourceClass::WildcardRegEx(const wxString& wildCardString) {
	wxString escapedExpression = wildCardString;
	
	// allow ? and * wildcards, turn ';' into '|'
	wxString symbols = wxT("!@#$%^&()[]{}\\-+.\"|,");
	int pos = escapedExpression.find_first_of(symbols, 0);
	while (-1 != pos) {
		wxString symbol = escapedExpression.substr(pos, 1);
		symbol = wxT("\\") + symbol;
		escapedExpression.replace(pos, 1, symbol, 2);
		pos = escapedExpression.find_first_of(symbols, pos + 2);
	}

	// '$'  because we want to match the end of text
	// we want each OR expression to be grouped together with parenthesis
	escapedExpression = wxT("(") + escapedExpression;
	escapedExpression.Replace(wxT(";"), wxT("$)|("));
	escapedExpression.Replace(wxT("*"), wxT(".*"));
	escapedExpression.Replace(wxT("?"), wxT(".?"));
	escapedExpression.Append(wxT("$)"));
	return escapedExpression;
}

mvceditor::DirectorySearchClass::DirectorySearchClass()
	: MatchedFiles() 
	, CurrentFiles()
	, Directories()
	, Sources()
	, TotalFileCount(0)
	, DoHiddenFiles(false) {
}

mvceditor::DirectorySearchClass::~DirectorySearchClass() {
	while (!Directories.empty()) {
		wxString* dir = Directories.top();
		Directories.pop();
		delete dir;
	}
}

bool mvceditor::DirectorySearchClass::Init(const wxString& path, Modes mode, bool doHiddenFiles) {
	mvceditor::SourceClass src;
	src.RootDirectory.Assign(path);
	src.SetIncludeWildcards(wxT("*"));
	std::vector<mvceditor::SourceClass> sources;
	sources.push_back(src);
	return Init(sources, mode, doHiddenFiles);
}

bool mvceditor::DirectorySearchClass::Init(const std::vector<mvceditor::SourceClass>& sources, Modes mode, bool doHidden) {
	TotalFileCount = 0;
	DoHiddenFiles = doHidden;
	while (!CurrentFiles.empty()) {
		CurrentFiles.pop();
	}
	while (!Directories.empty()) {
		wxString* dir = Directories.top();
		Directories.pop();
		delete dir;
	}
	MatchedFiles.clear();
	wxChar separator = wxFileName::GetPathSeparator();

	size_t total = 0;
	for (size_t i = 0; i < sources.size(); ++i) {
		mvceditor::SourceClass source = sources[i];
		wxString path = source.RootDirectory.GetFullPath();
		if (!path.IsEmpty()) {
			wxString pathWithSeparator = path;
			if (!pathWithSeparator.IsEmpty() && pathWithSeparator[path.length() - 1] != separator) {
				pathWithSeparator.Append(separator);
			}	
			if (wxDir::Exists(path)) {
				total++;
				if (RECURSIVE == mode) {
					wxString* newString = new wxString(pathWithSeparator);
					Directories.push(newString);
				}
				else {
					EnumerateAllFiles(pathWithSeparator);
				}
			}
		}
	}
	if (sources.empty() || total < sources.size()) {
		while (!CurrentFiles.empty()) {
			CurrentFiles.pop();
		}
		while (!Directories.empty()) {
			wxString* dir = Directories.top();
			Directories.pop();
			delete dir;
		}
		return false;
	}
	Sources.clear();
	for (size_t i = 0; i < sources.size(); ++i) {
		mvceditor::SourceClass srcCopy;
		srcCopy.Copy(sources[i]);
		Sources.push_back(srcCopy);
	}
	return true;
}

bool mvceditor::DirectorySearchClass::More() {
	return !Directories.empty() || !CurrentFiles.empty();
}

bool mvceditor::DirectorySearchClass::Walk(mvceditor::DirectoryWalkerClass& walker) {
	while (CurrentFiles.empty() && !Directories.empty()) {
		
		// enumerate the next directory, stop when we have a file to search
		wxString* path = Directories.top();
		Directories.pop();
		if (wxFileName::IsDirReadable(*path)) {
			wxDir dir(*path);
			wxString filename;
			int flags =  wxDIR_FILES | wxDIR_DIRS;
			if (DoHiddenFiles) {
				flags |=  wxDIR_HIDDEN;
			}
			bool next = dir.GetFirst(&filename, wxEmptyString, flags);
			while (next) {
				if (!filename.IsEmpty()) {
					wxString* fullPath = new wxString(*path);
					fullPath->Append(filename);
					if (wxDirExists(*fullPath)) {
						fullPath->Append(wxFileName::GetPathSeparator());
						Directories.push(fullPath);
					}
					else {
						if (MatchesWildcards(*fullPath)) {
							CurrentFiles.push(*fullPath);
						}
						delete fullPath;
					}
				}
				next = dir.GetNext(&filename);
			}
		}
		delete path;
	}
	bool hit = false;
	if (!CurrentFiles.empty()) {
		wxString filename = CurrentFiles.top();
		CurrentFiles.pop();
		hit = walker.Walk(filename); 
		if (hit) {
			MatchedFiles.push_back(filename);
		}
	}
	return hit;
}

const std::vector<wxString>& mvceditor::DirectorySearchClass::GetMatchedFiles() {
	return MatchedFiles;
}

void mvceditor::DirectorySearchClass::EnumerateAllFiles(const wxString& path) {
	wxDir dir;
	if (wxFileName::IsDirReadable(path) && dir.Open(path)) {
		wxString filename;
		int flags =  wxDIR_FILES | wxDIR_DIRS;
		if (DoHiddenFiles) {
			flags |=  wxDIR_HIDDEN;
		}
		bool next = dir.GetFirst(&filename, wxEmptyString, flags);
		while (next) {
			if (!filename.IsEmpty()) {
				wxString fullPath = path + filename;
				if (wxDirExists(fullPath)) {
					EnumerateAllFiles(fullPath + wxFileName::GetPathSeparator());
				}
				else {
					TotalFileCount++;
					CurrentFiles.push(fullPath);
				}
			}
			next = dir.GetNext(&filename);
		}
	}
}

int mvceditor::DirectorySearchClass::GetTotalFileCount() {
	return TotalFileCount;
}

bool mvceditor::DirectorySearchClass::MatchesWildcards(const wxString &fullPath) const {
	bool matches = false;
	for (size_t i = 0; i < Sources.size() && !matches; ++i) {
		matches = Sources[i].Contains(fullPath);
	}
	return matches;
}