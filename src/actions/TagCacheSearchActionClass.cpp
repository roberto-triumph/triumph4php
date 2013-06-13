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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <actions/TagCacheSearchActionClass.h>
#include <globals/FileName.h>
#include <globals/Assets.h>
#include <globals/TagList.h>

mvceditor::TagCacheSearchCompleteEventClass::TagCacheSearchCompleteEventClass(int eventId,
																	const UnicodeString& searchString,
																	const std::vector<mvceditor::TagClass>& tags)
	: wxEvent(eventId, mvceditor::EVENT_TAG_CACHE_SEARCH_COMPLETE)
	, SearchString(searchString)
	, Tags(tags) {

}

wxEvent* mvceditor::TagCacheSearchCompleteEventClass::Clone() const {
	return new mvceditor::TagCacheSearchCompleteEventClass(GetId(), SearchString, Tags);
}

mvceditor::TagCacheSearchActionClass::TagCacheSearchActionClass(mvceditor::RunningThreadsClass& runningThreads,
																int eventId)
	: ActionClass(runningThreads, eventId)
	, TagCache()
	, SearchString() 
	, SearchDirs() {

}

void mvceditor::TagCacheSearchActionClass::SetSearch(mvceditor::GlobalsClass& globals, const wxString& search, const std::vector<wxFileName>& dirs) {

	// deep copy the string, wxString not thread safe
	SearchString = mvceditor::WxToIcu(search);
	SearchDirs = mvceditor::DeepCopyFileNames(dirs);

	mvceditor::GlobalCacheClass* cache = new mvceditor::GlobalCacheClass;
	cache->InitGlobalTag(globals.TagCacheDbFileName, globals.GetPhpFileExtensions(), globals.GetMiscFileExtensions(),
		globals.Environment.Php.Version);
	///cache->InitNativeTag(mvceditor::NativeFunctionsAsset());
	///cache->InitDetectorTag(globals.DetectorCacheDbFileName);
	TagCache.RegisterGlobal(cache);
}

void mvceditor::TagCacheSearchActionClass::BackgroundWork() {
	std::vector<mvceditor::TagClass> matches;
	bool exactOnly = SearchString.length() <= 2;
	if (IsCancelled()) {
		return;
	}
	if (exactOnly) {
		mvceditor::TagResultClass* results = TagCache.ExactTags(SearchString, SearchDirs);
		if (results) {
			matches = results->Matches();
			delete results;
		}
	}
	else {
		matches = TagCache.NearMatchTags(SearchString, SearchDirs);
	}

	// no need to show jump to results for native functions
	// TODO: NearMatchTags shows resources from files that were recently deleted
	// need to hide them / remove them
	mvceditor::TagListRemoveNativeMatches(matches);
	if (!IsCancelled()) {

		// PostEvent will set the correct event ID
		mvceditor::TagCacheSearchCompleteEventClass evt(wxID_ANY, SearchString, matches);
		PostEvent(evt);
	}
}
wxString mvceditor::TagCacheSearchActionClass::GetLabel() const {
	return wxT("Tag Cache Search");
}

const wxEventType mvceditor::EVENT_TAG_CACHE_SEARCH_COMPLETE = wxNewEventType();