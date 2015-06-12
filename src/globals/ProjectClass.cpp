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

t4p::ProjectClass::ProjectClass()
	: Label()
	, Sources()
	, IsEnabled(true) {
}

t4p::ProjectClass::ProjectClass(const t4p::ProjectClass& project)
	: Label(project.Label)
	, Sources(project.Sources)
	, IsEnabled(project.IsEnabled) {
}

void t4p::ProjectClass::operator=(const t4p::ProjectClass& project) {
	Label = project.Label;
	Sources = project.Sources;
	IsEnabled = project.IsEnabled;
}

void t4p::ProjectClass::AddSource(const t4p::SourceClass& src) { 
	Sources.push_back(src);
}

void t4p::ProjectClass::ClearSources() {
	Sources.clear();
}

std::vector<t4p::SourceClass> t4p::ProjectClass::AllPhpSources(const t4p::FileTypeClass& fileType) const {
	std::vector<t4p::SourceClass>::const_iterator src;
	std::vector<t4p::SourceClass> phpSources;
	for (src = Sources.begin(); src != Sources.end(); ++src) {
		t4p::SourceClass phpSrc;
		phpSrc.RootDirectory = src->RootDirectory;
		phpSrc.SetIncludeWildcards(fileType.PhpFileExtensionsString);
		phpSrc.SetExcludeWildcards(src->ExcludeWildcardsString());
		phpSources.push_back(phpSrc);
	}
	return phpSources;
}

std::vector<t4p::SourceClass> t4p::ProjectClass::AllSources(const t4p::FileTypeClass& fileType) const {
	
	// get all extensions we know about
	wxString allExtensionsString = fileType.GetAllSourceFileExtensionsString();
	
	// make the new sources to return
	std::vector<t4p::SourceClass>::const_iterator src;
	std::vector<t4p::SourceClass> allSources;
	for (src = Sources.begin(); src != Sources.end(); ++src) {
		t4p::SourceClass allSrc;
		allSrc.RootDirectory = src->RootDirectory;
		allSrc.SetIncludeWildcards(allExtensionsString);
		allSrc.SetExcludeWildcards(src->ExcludeWildcardsString());
		allSources.push_back(allSrc);
	}
	return allSources;
}

bool t4p::ProjectClass::IsAPhpSourceFile(const wxString& fullPath, const t4p::FileTypeClass& fileType) const {
	bool matches = false;
	
	// a file is considered a PHP file if its in a source directory and it matches
	// the PHP file extensions
	std::vector<t4p::SourceClass> phpSources = AllPhpSources(fileType);
	for (size_t i = 0; i < phpSources.size() && !matches; ++i) {
		matches = phpSources[i].Contains(fullPath);
	}
	return matches;
}

bool t4p::ProjectClass::IsASourceFile(const wxString& fullPath, const t4p::FileTypeClass& fileType) const {
	bool matches = false;

	// first do a directory check  only. since AllSources() clones the 
	// Sources list; it can be expensive due to regex re-compilation of the
	// wildcards. for performance, we make sure that the file in question
	// is definitely in the sources before we copy the sources list
	std::vector<t4p::SourceClass>::const_iterator source;
	for (source = Sources.begin(); source < Sources.end() && !matches; ++source) {
		matches = source->IsInRootDirectory(fullPath);
	}
	if (matches) {
		matches = false;

		// a file is considered a file if its in a source directory and it matches
		// any of the recognized file extensions
		std::vector<wxString> allExtensions = fileType.GetAllSourceFileExtensions();
		std::vector<wxString>::const_iterator s;
		for (s = allExtensions.begin(); !matches && s != allExtensions.end(); ++s) {
			matches = wxMatchWild(*s, fullPath);
		}
	}
	return matches;
}

bool t4p::ProjectClass::HasSources() const {
	return !Sources.empty();
}

wxString t4p::ProjectClass::RelativeFileName(const wxString& fullPath) const {
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

std::vector<wxString> t4p::ProjectClass::GetNonPhpExtensions(const t4p::FileTypeClass& fileType) const {
	std::vector<wxString> allExtensions = fileType.GetNonPhpFileExtensions();
	return allExtensions;
}
