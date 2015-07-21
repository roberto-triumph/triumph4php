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
#include "features/OutlineFeatureClass.h"
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <unicode/regex.h>
#include <wx/tokenzr.h>
#include <algorithm>
#include <vector>
#include "globals/Assets.h"
#include "globals/Errors.h"
#include "globals/Number.h"
#include "globals/Sqlite.h"
#include "globals/String.h"
#include "globals/TagList.h"
#include "language_php/SymbolTableClass.h"
#include "language_php/TagParserClass.h"
#include "Triumph.h"

const wxEventType t4p::EVENT_OUTLINE_SEARCH_COMPLETE = wxNewEventType();

t4p::OutlineSearchCompleteClass::OutlineSearchCompleteClass()
	: Label()
	, Tags() {
}

t4p::OutlineSearchCompleteClass::OutlineSearchCompleteClass(const t4p::OutlineSearchCompleteClass& src)
	: Label()
	, Tags() {
	Copy(src);
}

void t4p::OutlineSearchCompleteClass::Copy(const t4p::OutlineSearchCompleteClass& src) {
	Label = src.Label;
	Tags = src.Tags;
}

bool t4p::OutlineSearchCompleteClass::IsLabelFileName() const {
	return Label.Find(wxT(".")) != wxNOT_FOUND;
}

t4p::OutlineSearchCompleteEventClass::OutlineSearchCompleteEventClass(int eventId,
																	const std::vector<t4p::OutlineSearchCompleteClass>& tags)
	: wxEvent(eventId, t4p::EVENT_OUTLINE_SEARCH_COMPLETE)
	, Tags(tags) {
}

wxEvent* t4p::OutlineSearchCompleteEventClass::Clone() const {
	return new t4p::OutlineSearchCompleteEventClass(GetId(), Tags);
}

t4p::OutlineTagCacheSearchActionClass::OutlineTagCacheSearchActionClass(t4p::RunningThreadsClass& runningThreads,
																int eventId)
	: ActionClass(runningThreads, eventId)
	, TagCache()
	, SearchStrings()
	, EnabledSourceDirs() {
}

void t4p::OutlineTagCacheSearchActionClass::SetSearch(const std::vector<UnicodeString>& searches, t4p::GlobalsClass& globals) {
	SearchStrings = searches;
	t4p::TagFinderListClass* cache = new t4p::TagFinderListClass;
	cache->InitGlobalTag(globals.TagCacheDbFileName,
		globals.FileTypes.GetPhpFileExtensions(), globals.FileTypes.GetMiscFileExtensions(),
		globals.Environment.Php.Version);
	cache->InitNativeTag(t4p::NativeFunctionsAsset());
	cache->InitDetectorTag(globals.DetectorCacheDbFileName);
	TagCache.RegisterGlobal(cache);

	EnabledSourceDirs.clear();
	std::vector<t4p::ProjectClass>::const_iterator project;
	std::vector<t4p::SourceClass>::const_iterator source;
	for (project = globals.Projects.begin(); project != globals.Projects.end(); ++project) {
		if (project->IsEnabled) {
			for (source = project->Sources.begin(); source != project->Sources.end(); ++source) {
				wxFileName rootDir(source->RootDirectory);
				EnabledSourceDirs.push_back(rootDir);
			}
		}
	}
}

void t4p::OutlineTagCacheSearchActionClass::BackgroundWork() {
	std::vector<t4p::OutlineSearchCompleteClass> topLevelTags;
	if (!IsCancelled()) {
		std::vector<UnicodeString>::const_iterator search;
		for (search = SearchStrings.begin(); !IsCancelled() && search != SearchStrings.end(); ++search) {
			std::vector<t4p::PhpTagClass> tags;
			std::vector<t4p::PhpTagClass>::const_iterator tag;
			OutlineSearchCompleteClass tagSearchComplete;
			tagSearchComplete.Label = t4p::IcuToWx(*search);
			if (search->indexOf(UNICODE_STRING_SIMPLE(".")) >= 0) {
				// searching for all tags in the file
				tags = TagCache.AllClassesFunctionsDefines(t4p::IcuToWx(*search));

				// now for each class, collect all methods/properties for any class.
				for (tag = tags.begin(); tag != tags.end(); ++tag) {
					if (tag->Type == t4p::PhpTagClass::CLASS) {
						wxString classLabel = t4p::IcuToWx(tag->Identifier);
						if (!tag->NamespaceName.isEmpty()) {
							classLabel += wxT(": ") + t4p::IcuToWx(tag->NamespaceName);
						}
						tagSearchComplete.Tags[classLabel] = TagCache.AllMemberTags(tag->FullyQualifiedClassName(), tag->FileTagId, EnabledSourceDirs);
					} else {
						tagSearchComplete.Tags[wxT("")].push_back(*tag);
					}
				}
			} else {
				// searching for all members in a class name
				t4p::TagResultClass* results = TagCache.ExactTags(*search, EnabledSourceDirs);
				if (results) {
					tags = results->Matches();
					delete results;
				}

				if (!tags.empty()) {
					tag = tags.begin();
				}
				wxString classLabel = t4p::IcuToWx(tag->Identifier);
				if (classLabel.Contains(wxT("\\"))) {
					classLabel = classLabel.AfterLast(wxT('\\'));
				}
				if (!tag->NamespaceName.isEmpty()) {
					classLabel += wxT(": ") + t4p::IcuToWx(tag->NamespaceName);
				}
				tagSearchComplete.Label = classLabel;
				tagSearchComplete.Tags[wxT("")] = TagCache.AllMemberTags(tag->FullyQualifiedClassName(), tag->FileTagId, EnabledSourceDirs);
			}
			topLevelTags.push_back(tagSearchComplete);
		}
	}
	if (!IsCancelled()) {
		// PostEvent will set the correct event ID
		t4p::OutlineSearchCompleteEventClass evt(wxID_ANY, topLevelTags);
		PostEvent(evt);
	}
}
wxString t4p::OutlineTagCacheSearchActionClass::GetLabel() const {
	return wxT("Tag Cache Search");
}

t4p::OutlineFeatureClass::OutlineFeatureClass(t4p::AppClass& app)
	: FeatureClass(app) {
}
