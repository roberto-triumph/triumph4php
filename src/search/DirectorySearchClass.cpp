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
#include <algorithm>


/**
 * @param sources the source directories to check
 * @return bool TRUE if ALL of the given sources exist on disk AND
 *         there is at least 1 source in the vector
 */
static bool AllSourcesExist(const std::vector<t4p::SourceClass>& sources) {
	for (size_t i = 0; i < sources.size(); ++i) {
		if (!sources[i].RootDirectory.DirExists()) {
			return false;
		}
	}
	return !sources.empty();
	
}

t4p::SourceClass::SourceClass()  
	: RootDirectory()
	, IncludeWildcards() 
	, ExcludeWildcards() {
	IncludeRegEx = NULL;
	ExcludeRegEx = NULL;
}

t4p::SourceClass::SourceClass(const t4p::SourceClass& src)  
	: RootDirectory()
	, IncludeWildcards() 
	, ExcludeWildcards() {
	IncludeRegEx = NULL;
	ExcludeRegEx = NULL;
	Copy(src);
}

t4p::SourceClass::~SourceClass() {
	if (IncludeRegEx) {
		delete IncludeRegEx;
	}
	if (ExcludeRegEx) {
		delete ExcludeRegEx;
	}
}

void t4p::SourceClass::Copy(const t4p::SourceClass& src) {
	SetIncludeWildcards(src.IncludeWildcardsString());
	SetExcludeWildcards(src.ExcludeWildcardsString());

	wxString path;
	path.Append(src.RootDirectory.GetPath());
	RootDirectory.AssignDir(path);
}

t4p::SourceClass& t4p::SourceClass::operator=(const t4p::SourceClass& src) {
	Copy(src);
	return *this;
}

void t4p::SourceClass::ClearIncludeWildcards() {
	SetIncludeWildcards(wxT("*"));
}

void t4p::SourceClass::ClearExcludeWildcards() {
	SetExcludeWildcards(wxT(""));
}

void t4p::SourceClass::SetIncludeWildcards(const wxString& wildcardString) {
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

void t4p::SourceClass::SetExcludeWildcards(const wxString& wildcardString) {
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

wxString t4p::SourceClass::IncludeWildcardsString() const {
	return IncludeWildcards;
}

wxString t4p::SourceClass::ExcludeWildcardsString() const {
	return ExcludeWildcards;
}

bool t4p::SourceClass::Contains(const wxString& fullPath) {
	
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

bool t4p::SourceClass::IsInRootDirectory(const wxString& fullPath) const {

	// make sure to normalize so that the search can be case sensitive depending on the OS
	// note: we first compare without normalizing, as normalize() involves file
	// system calls.  we do this as an optimization, since DirectorySearchClass
	// calls this method (indeirectly) for every file, it is a pretty critical method
	// to optimize.
	// this makes A HUGE differrence on network shares, especiall on MSW.

	wxFileName af;
	af.AssignDir(fullPath);

	wxFileName bf(RootDirectory);
	
	if (af.GetPathWithSep().Find(bf.GetPathWithSep()) == 0) {
		return true;
	}
	
	// not a match, normalize in case the OS uses case sensitive
	// file names.
	af.Normalize();
	bf.Normalize();
	return af.GetPathWithSep().Find(bf.GetPathWithSep()) == 0;
}

wxString t4p::SourceClass::WildcardRegEx(const wxString& wildCardString) {
	wxString escapedExpression(wildCardString);
	
	// allow ? and * wildcards, turn ';' into '|'
	wxString symbols = wxT("!@#$%^&()[]{}\\-+.\"|,~");
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

bool t4p::SourceClass::Exists() const {
	return RootDirectory.DirExists();
}

bool t4p::CompareSourceLists(const std::vector<t4p::SourceClass>& a, const std::vector<t4p::SourceClass>& b) {
	if (a.size() != b.size()) {
		return false;
	}
	std::vector<t4p::SourceClass>::const_iterator aSource, bSource;

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

t4p::DirectorySearchClass::DirectorySearchClass()
	: MatchedFiles() 
	, CurrentFiles()
	, Directories()
	, InitSources()
	, Sources()
	, TotalFileCount(0)
	, DoHiddenFiles(false)
	, HasCalledBegin(false)
	, HasCalledEnd(false) {
}

bool t4p::DirectorySearchClass::Init(const wxString& path, Modes mode, bool doHiddenFiles) {
	t4p::SourceClass src;
	src.RootDirectory.AssignDir(path);
	src.SetIncludeWildcards(wxT("*"));
	std::vector<t4p::SourceClass> sources;
	sources.push_back(src);
	return Init(sources, mode, doHiddenFiles);
}

bool t4p::DirectorySearchClass::Init(const std::vector<t4p::SourceClass>& sources, Modes mode, bool doHidden) {
	TotalFileCount = 0;
	DoHiddenFiles = doHidden;
	while (!CurrentFiles.empty()) {
		CurrentFiles.pop();
	}
	while (!Directories.empty()) {
		Directories.pop();
	}
	InitSources.clear();
	MatchedFiles.clear();
	
	if (!AllSourcesExist(sources)) {
		return false;
	}
	Sources = sources;

	for (size_t i = 0; i < Sources.size(); ++i) {
		t4p::SourceClass source = Sources[i];
		wxString pathWithSeparator = source.RootDirectory.GetPathWithSep();	
		if (wxDir::Exists(pathWithSeparator)) {
			InitSources.push_back(pathWithSeparator);
			if (RECURSIVE == mode) {
				Directories.push(pathWithSeparator);
			}
		}
	}
	
	if (PRECISE == mode) {
		for (size_t i = 0; i < Sources.size(); ++i) {
			t4p::SourceClass source = Sources[i];
			wxString pathWithSeparator = source.RootDirectory.GetPathWithSep();	
			

			// put the source in the list so that when we are walking through
			// the files we know when we have started to walk a new source dir
			// do it after we push the files, since CurrentFiles is a stack we want
			// the source dir to be popped first
			// need to make sure to enumerate files once Sources has been set,
			// as Sources contains the wildcards that we want to use
			EnumerateAllFiles(pathWithSeparator);
			CurrentFiles.push(pathWithSeparator);
		}
	}
	HasCalledBegin = false;
	HasCalledEnd = false;
	return true;
}

bool t4p::DirectorySearchClass::More() {
	return !Directories.empty() || !CurrentFiles.empty();
}

void t4p::DirectorySearchClass::EnumerateNextDir(t4p::DirectoryWalkerClass& walker) {
	while (CurrentFiles.empty() && !Directories.empty()) {
		wxString path = Directories.top();

		// if we have come across the new source dir, notify the walker that we are beginning a search
		// on a new source directory
		std::vector<wxString>::const_iterator it = std::find(InitSources.begin(), InitSources.end(), path);
		if (it != InitSources.end()) {
			if (!HasCalledBegin) {
				walker.BeginSearch(*it);
				HasCalledBegin = true;
				HasCalledEnd = false;
			}
			else {
				walker.EndSearch();
				walker.BeginSearch(*it);
				HasCalledBegin = true;
				HasCalledEnd = false;
			}
		}

		// enumerate the next directory, stop when we have a file to search
		Directories.pop();
		wxDir dir(path);
		if (dir.IsOpened()) {			
			AddSubDirectories(dir);
			AddFiles(dir);
		}		
	}
}

// TODO(roberto): there is a bug in here when using recursive mode and 
// there is this structure
// |
// ==> apache_config
//       |
//       == httpd.conf
// ==> test.php
//
// see the failed test for ApacheTestClass,
// the test was modified to use PRECISE mode, because the bug
// is really in this class
// see commit after 0038fea7e36cbe2e3b4c4795904f6
bool t4p::DirectorySearchClass::Walk(t4p::DirectoryWalkerClass& walker) {
	EnumerateNextDir(walker);
	bool hit = false;
	if (!CurrentFiles.empty()) {
		wxString filename = CurrentFiles.top();
		CurrentFiles.pop();
		
		// if we have come across the new source dir, notify the walker that we are beginning a search
		// on a new source directory
		std::vector<wxString>::const_iterator it = std::find(InitSources.begin(), InitSources.end(), filename);
		if (it != InitSources.end()) {
			if (!HasCalledBegin) {
				walker.BeginSearch(*it);
				HasCalledBegin = true;
				HasCalledEnd = false;
			}
			else {
				walker.EndSearch();
				walker.BeginSearch(*it);
				HasCalledBegin = true;
				HasCalledEnd = false;
			}
			if (!CurrentFiles.empty()) {
				wxString filename = CurrentFiles.top();
				CurrentFiles.pop();
				hit = walker.Walk(filename); 
				if (hit) {
					MatchedFiles.push_back(filename);
				}
			}
		}
		else {
			hit = walker.Walk(filename); 
			if (hit) {
				MatchedFiles.push_back(filename);
			}
		}
	}
	if (HasCalledBegin && !HasCalledEnd && !More()) {
		walker.EndSearch();
		HasCalledEnd = true;
	}
	return hit;
}

const std::vector<wxString>& t4p::DirectorySearchClass::GetMatchedFiles() {
	return MatchedFiles;
}

void t4p::DirectorySearchClass::EnumerateAllFiles(const wxString& path) {
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
				if (MatchesWildcards(fullPath)) {
					TotalFileCount++;
					CurrentFiles.push(fullPath);
				}
			}
			next = dir.GetNext(&filename);
		}
	}
}

int t4p::DirectorySearchClass::GetTotalFileCount() {
	return TotalFileCount;
}

bool t4p::DirectorySearchClass::MatchesWildcards(const wxString &fullPath) {
	bool matches = false;
	for (size_t i = 0; i < Sources.size() && !matches; ++i) {
		matches = Sources[i].Contains(fullPath);
	}
	return matches;
}

void t4p::DirectorySearchClass::AddSubDirectories(wxDir& dir) {
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

void t4p::DirectorySearchClass::AddFiles(wxDir& dir) {
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
