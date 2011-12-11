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
#include <wx/filename.h>

mvceditor::DirectorySearchClass::DirectorySearchClass()
	: MatchedFiles() 
	, CurrentFiles()
	, Directories()
	, TotalFileCount(0) {
}

bool mvceditor::DirectorySearchClass::Init(const wxString& path, Modes mode) {
	TotalFileCount = 0;
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
	wxString pathWithSeparator = path;
	if (!pathWithSeparator.IsEmpty() && pathWithSeparator[path.length() - 1] != separator) {
		pathWithSeparator.Append(separator);
	}	
	if (wxDir::Exists(path)) {
		if (RECURSIVE == mode) {
			wxString* newString = new wxString(pathWithSeparator);
			Directories.push(newString);
		}
		else {
			EnumerateAllFiles(pathWithSeparator);
		}
		return true;
	}
	return false;
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
			bool next = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES | wxDIR_DIRS | wxDIR_HIDDEN);
			while (next) {
				if (!filename.IsEmpty()) {
					wxString* fullPath = new wxString(*path);
					fullPath->Append(filename);
					if (wxDirExists(*fullPath)) {
						fullPath->Append(wxFileName::GetPathSeparator());
						Directories.push(fullPath);
					}
					else {
						CurrentFiles.push(*fullPath);
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
		bool next = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES | wxDIR_DIRS | wxDIR_HIDDEN);
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