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
#include "code_control/ResourceCacheBuilderClass.h"
#include <vector>
#include "globals/Assets.h"
#include "globals/GlobalsClass.h"
#include "language_php/TagFinderList.h"

t4p::WorkingCacheBuilderClass::WorkingCacheBuilderClass(
	t4p::RunningThreadsClass& runningThreads, int eventId)
	: ActionClass(runningThreads, eventId)
	, TagCacheDbFileName()
	, Code()
	, FileName()
	, SourceDir()
	, FileIdentifier()
	, PreviousSymbolTable()
	, Version(pelet::PHP_53)
	, FileIsNew(true)
	, DoParseTags(true) {
}

void t4p::WorkingCacheBuilderClass::Update(t4p::GlobalsClass& globals,
												 const wxString& fileName,
												 const wxString& fileIdentifier,
												 const UnicodeString& code, bool isNew, pelet::Versions version,
												 bool doParseTags) {
	// make sure these is are deep copies since we access the variables in a separate thread
	TagCacheDbFileName.Assign(globals.TagCacheDbFileName.GetFullPath());
	Code = code;
	FileName = fileName.c_str();
	FileIdentifier = fileIdentifier.c_str();
	Version = version;
	FileIsNew = isNew;
	DoParseTags = doParseTags;
	SourceDir = wxT("");

	t4p::WorkingCacheClass* workingCache = globals.TagCache.GetWorking(fileIdentifier);
	if (workingCache) {
		PreviousSymbolTable.Copy(workingCache->SymbolTable);
	}

	std::vector<t4p::ProjectClass>::const_iterator project;
	std::vector<t4p::SourceClass>::const_iterator source;
	for (project = globals.Projects.begin(); project != globals.Projects.end(); ++project) {
		if (project->IsEnabled) {
			for (source = project->Sources.begin(); source != project->Sources.end(); ++source) {
				if (source->IsInRootDirectory(FileName)) {
					SourceDir = source->RootDirectory.GetPathWithSep();
					break;
				}
			}
		}
		if (!SourceDir.empty()) {
			break;
		}
	}
}

void t4p::WorkingCacheBuilderClass::BackgroundWork() {
	// tag caches may not exist if the user screwed up and pointed their settings
	// dir to a non-existing location.
	if (!TagCacheDbFileName.FileExists()) {
		return;
	}

	if (!FileIdentifier.IsEmpty()) {
		if (FileIsNew) {
			// new files will not have a name, use the identifier as the name
			FileName = FileIdentifier;
		}

		// make sure to use the local variables and not the class ones
		// since this code is outside the mutex
		// even if code is empty, lets create a working cache so that the
		// file is registered and code completion works
		t4p::TagFinderListClass* tagFinderlist = new t4p::TagFinderListClass();
		std::vector<wxString> phpFileExtensions;
		wxFileName wxf(FileName);
		phpFileExtensions.push_back(wxf.GetFullName());
		std::vector<wxString> miscFileExtensions;
		tagFinderlist->InitGlobalTag(TagCacheDbFileName, phpFileExtensions, miscFileExtensions, Version);

		t4p::WorkingCacheClass* workingCache = new t4p::WorkingCacheClass();
		workingCache->Init(FileName, FileIdentifier, FileIsNew, Version, true, PreviousSymbolTable);
		bool good = workingCache->Update(Code, PreviousSymbolTable);
		if (good && !IsCancelled()) {
			// parse any tags from the source code
			// note that we only parse the file if it is valid syntax
			// since BuildResourceCacheForFile kills existing tags in the file
			// we want to keep previous tags if the code contains a syntax error
			if (DoParseTags) {
				tagFinderlist->TagParser.BuildResourceCacheForFile(SourceDir, FileName, Code, FileIsNew);
			}

			// only send the event if the code passes the lint check
			// otherwise we will delete a good symbol table, we want auto completion
			// to work even if the code is broken
			// PostEvent will set the correct event Id
			t4p::WorkingCacheCompleteEventClass evt(wxID_ANY, FileName, FileIdentifier, workingCache);
			PostEvent(evt);
		} else {
			// we still own the pointer since we did not send the event
			delete workingCache;
		}
		delete tagFinderlist;
	}
}

wxString t4p::WorkingCacheBuilderClass::GetLabel() const {
	return wxT("Working Tag Cache");
}
