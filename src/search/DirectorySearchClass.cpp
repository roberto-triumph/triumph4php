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

mvceditor::SourceClass& mvceditor::SourceClass::operator=(const mvceditor::SourceClass& src) {
	Copy(src);
	return *this;
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
	IncludeWildcards = wxT("");

	// tokenize so that we can ignore multiple consecutive semicolons
	// also remove spaces at the end of each wilcard since they are not significant
	wxStringTokenizer tok(wildcardString, wxT(";"), wxTOKEN_STRTOK);
	while (tok.HasMoreTokens()) {
		wxString next = tok.NextToken();
		next.Trim(false).Trim(true);
		if (!next.IsEmpty()) {
			IncludeWildcards += next;
			IncludeWildcards += wxT(";");
		}
	}
	if (IncludeWildcards.EndsWith(wxT(";"))) {
		IncludeWildcards.RemoveLast();
	}

	// don't compile the wildcard reg ex yet. lets perform lazy initialization since
	// so that we dont incur the hit of reg ex compilation until we need to.
}

void mvceditor::SourceClass::SetExcludeWildcards(const wxString& wildcardString) {
	if (ExcludeRegEx) {
		delete ExcludeRegEx;
		ExcludeRegEx = NULL;
	}

	ExcludeWildcards = wxT("");

	// tokenize so that we can ignore multiple consecutive semicolons
	// also remove spaces at the end of each wilcard since they are not significant
	wxStringTokenizer tok(wildcardString, wxT(";"), wxTOKEN_STRTOK);
	while (tok.HasMoreTokens()) {
		wxString next = tok.NextToken();
		next.Trim(false).Trim(true);
		if (!next.IsEmpty()) {
			ExcludeWildcards += next;
			ExcludeWildcards += wxT(";");
		}
	}
	if (ExcludeWildcards.EndsWith(wxT(";"))) {
		ExcludeWildcards.RemoveLast();
	}

	// don't compile the wildcard reg ex yet. lets perform lazy initialization since
	// so that we dont incur the hit of reg ex compilation until we need to.
}

wxString mvceditor::SourceClass::IncludeWildcardsString() const {
	return IncludeWildcards;
}

wxString mvceditor::SourceClass::ExcludeWildcardsString() const {
	return ExcludeWildcards;
}

bool mvceditor::SourceClass::Contains(const wxString& fullPath) {
	
	// validations: 
	// 1. fullPath must be in RootDirectory
	// 2. fullPath must NOT match the exclude wildcards if set
	// 2. fullPath must match wildcards
	if (!IsInRootDirectory(fullPath)) {
		return false;
	}
	if (!ExcludeRegEx && !ExcludeWildcards.IsEmpty()) {
		ExcludeRegEx = new wxRegEx(WildcardRegEx(ExcludeWildcards), wxRE_ICASE | wxRE_ADVANCED);
	}
	if (ExcludeRegEx && ExcludeRegEx->IsValid()) {
		if (ExcludeRegEx->Matches(fullPath)) {
			return false;
		}
	}
	if (!IncludeRegEx && !IncludeWildcards.IsEmpty()) {		
		IncludeRegEx = new wxRegEx(WildcardRegEx(IncludeWildcards), wxRE_ICASE | wxRE_ADVANCED);
	}
	bool matchedInclude = IncludeRegEx  && IncludeRegEx->IsValid() && IncludeRegEx->Matches(fullPath);
	return matchedInclude;
}

bool mvceditor::SourceClass::IsInRootDirectory(const wxString& fullPath) const {

	// ATTN: should this be case sensitive for different OSes?
	return fullPath.Find(RootDirectory.GetFullPath()) == 0;
}

wxString mvceditor::SourceClass::WildcardRegEx(const wxString& wildCardString) {
	wxString escapedExpression(wildCardString);
	
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

bool mvceditor::SourceClass::Exists() const {
	return RootDirectory.DirExists();
}

bool mvceditor::CompareSourceLists(const std::vector<mvceditor::SourceClass>& a, const std::vector<mvceditor::SourceClass>& b) {
	if (a.size() != b.size()) {
		return false;
	}
	std::vector<mvceditor::SourceClass>::const_iterator aSource, bSource;

	// must compare this way since there lists are not guaranteed to be in any particular order
	for (aSource = a.begin(); aSource != a.end(); ++aSource) {
		bool found = false;
		for (bSource = b.begin(); bSource != b.end(); ++bSource) {
			if (aSource->RootDirectory == bSource->RootDirectory &&
				aSource->IncludeWildcardsString() == bSource->IncludeWildcardsString() &&
				aSource->ExcludeWildcardsString() == bSource->ExcludeWildcardsString()) {
				found = true;
				break;
			}
		}
		if (!found) {

			// as soon as 1 item in list a is not found in list b, the lists are not the same
			return false;
		}
	}

	// all items in list a were found in list b
	return true;
}

mvceditor::DirectorySearchClass::DirectorySearchClass()
	: MatchedFiles() 
	, CurrentFiles()
	, Directories()
	, Sources()
	, TotalFileCount(0)
	, DoHiddenFiles(false)
	, HasCalledBegin(false)
	, HasCalledEnd(false) {
}

bool mvceditor::DirectorySearchClass::Init(const wxString& path, Modes mode, bool doHiddenFiles) {
	mvceditor::SourceClass src;
	src.RootDirectory.AssignDir(path);
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
		Directories.pop();
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
					Directories.push(pathWithSeparator);
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
			Directories.pop();
		}
		return false;
	}
	Sources.clear();
	for (size_t i = 0; i < sources.size(); ++i) {
		mvceditor::SourceClass srcCopy;
		srcCopy.Copy(sources[i]);
		Sources.push_back(srcCopy);
	}
	HasCalledBegin = false;
	HasCalledEnd = false;
	return true;
}

bool mvceditor::DirectorySearchClass::More() {
	return !Directories.empty() || !CurrentFiles.empty();
}

bool mvceditor::DirectorySearchClass::Walk(mvceditor::DirectoryWalkerClass& walker) {
	while (CurrentFiles.empty() && !Directories.empty()) {
		
		// enumerate the next directory, stop when we have a file to search
		wxString path;
		path.Append(Directories.top());
		Directories.pop();
		wxDir dir(path);
		if (dir.IsOpened()) {			
			wxString filename;
			AddSubDirectories(dir);
			AddFiles(dir);
		}
	}
	bool hit = false;
	if (!CurrentFiles.empty()) {
		wxString filename;
		filename.Append(CurrentFiles.top());
		CurrentFiles.pop();
		if (!HasCalledBegin) {
			walker.BeginSearch();
			HasCalledBegin = true;
		}
		hit = walker.Walk(filename); 
		if (hit) {
			MatchedFiles.push_back(filename);
		}
	}
	if (HasCalledBegin && !HasCalledEnd && !More()) {
		walker.EndSearch();
		HasCalledEnd = true;
	}
	return hit;
}

const std::vector<wxString>& mvceditor::DirectorySearchClass::GetMatchedFiles() {
	return MatchedFiles;
}

void mvceditor::DirectorySearchClass::EnumerateAllFiles(const wxString& path) {
	wxDir dir;
	if (dir.Open(path)) {
		int flags =  wxDIR_DIRS;
		if (DoHiddenFiles) {
			flags |=  wxDIR_HIDDEN;
		}
		wxString subDirName;
		bool next = dir.GetFirst(&subDirName, wxEmptyString, flags);
		while (next) {
			if (!subDirName.IsEmpty()) {
				wxString fullPath = path + subDirName;
				EnumerateAllFiles(fullPath + wxFileName::GetPathSeparator());
			}
			next = dir.GetNext(&subDirName);
		}

		flags = wxDIR_FILES;
		if (DoHiddenFiles) {
			flags |=  wxDIR_HIDDEN;
		}
		wxString filename;
		next = dir.GetFirst(&filename, wxEmptyString, flags);
		while (next) {
			if (!filename.IsEmpty()) {
				wxString fullPath = path + filename;
				TotalFileCount++;
				CurrentFiles.push(fullPath);
			}
			next = dir.GetNext(&filename);
		}
	}
}

int mvceditor::DirectorySearchClass::GetTotalFileCount() {
	return TotalFileCount;
}

bool mvceditor::DirectorySearchClass::MatchesWildcards(const wxString &fullPath) {
	bool matches = false;
	for (size_t i = 0; i < Sources.size() && !matches; ++i) {
		matches = Sources[i].Contains(fullPath);
	}
	return matches;
}

void mvceditor::DirectorySearchClass::AddFiles(wxDir& dir) {
	int flags =  wxDIR_DIRS;
	if (DoHiddenFiles) {
		flags |=  wxDIR_HIDDEN;
	}
	wxString subDirName;
	bool next = dir.GetFirst(&subDirName, wxEmptyString, flags);
	while (next) {
		if (!subDirName.IsEmpty()) {
			wxString subDirFullPath;
			subDirFullPath.Append(dir.GetName());
			subDirFullPath.Append(wxFileName::GetPathSeparator());
			subDirFullPath.Append(subDirName);
			Directories.push(subDirFullPath);
		}
		next = dir.GetNext(&subDirName);
	}
}

void mvceditor::DirectorySearchClass::AddSubDirectories(wxDir& dir) {
	int flags =  wxDIR_FILES;
	if (DoHiddenFiles) {
		flags |=  wxDIR_HIDDEN;
	}
	wxString subFileName;
	bool next = dir.GetFirst(&subFileName, wxEmptyString, flags);
	while (next) {
		if (!subFileName.IsEmpty()) {
			wxString subFileFullPath;
			subFileFullPath.Append(dir.GetName());
			subFileFullPath.Append(wxFileName::GetPathSeparator());
			subFileFullPath.Append(subFileName);
			if (MatchesWildcards(subFileFullPath)) {
				CurrentFiles.push(subFileFullPath);
			}
		}
		next = dir.GetNext(&subFileName);
	}
}