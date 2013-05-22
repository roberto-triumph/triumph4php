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
#include <globals/ProjectClass.h>
#include <wx/tokenzr.h>
#include <globals/Assets.h>
#include <vector>

static wxString Join(const std::vector<wxString>& list, const wxString& separator) {
	wxString joined;
	for (size_t i = 0; i < list.size(); ++i) {
		joined.Append(list[i]);
		if (i < (list.size() - 1)) {
			joined.Append(separator);
		}
	}
	return joined;
}


mvceditor::ProjectClass::ProjectClass()
	: Label()
	, Sources()
	, PhpFileExtensions()
	, CssFileExtensions()
	, SqlFileExtensions()
	, MiscFileExtensions()
	, IsEnabled(true) {
}

mvceditor::ProjectClass::ProjectClass(const mvceditor::ProjectClass& project)
	: Label(project.Label)
	, Sources(project.Sources)
	, PhpFileExtensions(project.PhpFileExtensions)
	, CssFileExtensions(project.CssFileExtensions)
	, SqlFileExtensions(project.SqlFileExtensions) 
	, MiscFileExtensions(project.MiscFileExtensions)
	, IsEnabled(project.IsEnabled) {
}

void mvceditor::ProjectClass::operator=(const mvceditor::ProjectClass& project) {
	Label = project.Label;
	Sources = project.Sources;
	IsEnabled = project.IsEnabled;
	PhpFileExtensions = project.PhpFileExtensions;
	CssFileExtensions = project.CssFileExtensions;
	SqlFileExtensions = project.SqlFileExtensions;
	MiscFileExtensions = project.MiscFileExtensions;
}

void mvceditor::ProjectClass::AddSource(const mvceditor::SourceClass& src) { 
	Sources.push_back(src);
}

void mvceditor::ProjectClass::ClearSources() {
	Sources.clear();
}

std::vector<mvceditor::SourceClass> mvceditor::ProjectClass::AllPhpSources() const {
	wxString phpExtensionsString = Join(PhpFileExtensions, wxT(";"));
	
	std::vector<mvceditor::SourceClass>::const_iterator src;
	std::vector<mvceditor::SourceClass> phpSources;
	for (src = Sources.begin(); src != Sources.end(); ++src) {
		mvceditor::SourceClass phpSrc;
		phpSrc.RootDirectory = src->RootDirectory;
		phpSrc.SetIncludeWildcards(phpExtensionsString);
		phpSrc.SetExcludeWildcards(src->ExcludeWildcardsString());
		phpSources.push_back(phpSrc);
	}
	return phpSources;
}

std::vector<mvceditor::SourceClass> mvceditor::ProjectClass::AllSources() const {
	
	// get all extensions we know about
	std::vector<wxString> allExtensions;
	allExtensions.insert(allExtensions.end(), CssFileExtensions.begin(), CssFileExtensions.end());
	allExtensions.insert(allExtensions.end(), MiscFileExtensions.begin(), MiscFileExtensions.end());
	allExtensions.insert(allExtensions.end(), PhpFileExtensions.begin(), PhpFileExtensions.end());
	allExtensions.insert(allExtensions.end(), SqlFileExtensions.begin(), SqlFileExtensions.end());
	wxString allExtensionsString = Join(allExtensions, wxT(";"));

	// make the new sources to return
	std::vector<mvceditor::SourceClass>::const_iterator src;
	std::vector<mvceditor::SourceClass> allSources;
	for (src = Sources.begin(); src != Sources.end(); ++src) {
		mvceditor::SourceClass allSrc;
		allSrc.RootDirectory = src->RootDirectory;
		allSrc.SetIncludeWildcards(allExtensionsString);
		allSrc.SetExcludeWildcards(src->ExcludeWildcardsString());
		allSources.push_back(allSrc);
	}
	return allSources;
}

bool mvceditor::ProjectClass::IsAPhpSourceFile(const wxString& fullPath) const {
	bool matches = false;
	
	// a file is considered a PHP file if its in a source directory and it matches
	// the PHP file extensions
	std::vector<mvceditor::SourceClass> phpSources = AllPhpSources();
	for (size_t i = 0; i < phpSources.size() && !matches; ++i) {
		matches = phpSources[i].Contains(fullPath);
	}
	return matches;
}

bool mvceditor::ProjectClass::IsASourceFile(const wxString& fullPath) const {
	bool matches = false;

	// first do a directory check  only. since AllSources() clones the 
	// Sources list; it can be expensive due to regex re-compilation of the
	// wildcards. for performance, we make sure that the file in question
	// is definitely in the sources before we copy the sources list
	std::vector<mvceditor::SourceClass>::const_iterator source;
	for (source = Sources.begin(); source < Sources.end() && !matches; ++source) {
		matches = source->IsInRootDirectory(fullPath);
	}
	if (matches) {
		matches = false;

		// a file is considered a file if its in a source directory and it matches
		// any of the recognized file extensions
		std::vector<wxString> allExtensions;
		allExtensions.insert(allExtensions.end(), CssFileExtensions.begin(), CssFileExtensions.end());
		allExtensions.insert(allExtensions.end(), MiscFileExtensions.begin(), MiscFileExtensions.end());
		allExtensions.insert(allExtensions.end(), PhpFileExtensions.begin(), PhpFileExtensions.end());
		allExtensions.insert(allExtensions.end(), SqlFileExtensions.begin(), SqlFileExtensions.end());

		std::vector<wxString>::const_iterator s;
		for (s = allExtensions.begin(); !matches && s != allExtensions.end(); ++s) {
			matches = wxMatchWild(*s, fullPath);
		}
	}
	return matches;
}

bool mvceditor::ProjectClass::HasSources() const {
	return !Sources.empty();
}

wxString mvceditor::ProjectClass::RelativeFileName(const wxString& fullPath) const {
	wxString relativeName;
	for (size_t i = 0; i < Sources.size(); ++i) {
		wxString root = Sources[i].RootDirectory.GetFullPath();
		if (fullPath.Find(root) == 0) {
			relativeName = fullPath.Mid(root.Length());
			break;
		}
	}
	return relativeName;
}

std::vector<wxString> mvceditor::ProjectClass::AllNonPhpExtensions() const {
	std::vector<wxString> allExtensions;
	allExtensions.insert(allExtensions.end(), CssFileExtensions.begin(), CssFileExtensions.end());
	allExtensions.insert(allExtensions.end(), MiscFileExtensions.begin(), MiscFileExtensions.end());
	allExtensions.insert(allExtensions.end(), SqlFileExtensions.begin(), SqlFileExtensions.end());
	return allExtensions;
}