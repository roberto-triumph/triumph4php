/**
 * @copyright  2013 Roberto Perpuly
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
#include "actions/TagCacheSearchActionClass.h"
#include <vector>
#include "globals/Assets.h"
#include "globals/FileName.h"
#include "globals/TagList.h"
#include "language_php/TagFinderList.h"

t4p::TagCacheSearchCompleteEventClass::TagCacheSearchCompleteEventClass(int eventId,
        const UnicodeString& searchString,
        const std::vector<t4p::PhpTagClass>& tags)
    : wxEvent(eventId, t4p::EVENT_TAG_CACHE_SEARCH_COMPLETE)
    , SearchString(searchString)
    , Tags(tags) {
}

wxEvent* t4p::TagCacheSearchCompleteEventClass::Clone() const {
    return new t4p::TagCacheSearchCompleteEventClass(GetId(), SearchString, Tags);
}

t4p::TagCacheSearchActionClass::TagCacheSearchActionClass(t4p::RunningThreadsClass& runningThreads,
        int eventId)
    : ActionClass(runningThreads, eventId)
    , TagCache()
    , SearchString()
    , SearchDirs() {
}

void t4p::TagCacheSearchActionClass::SetSearch(t4p::GlobalsClass& globals, const wxString& search, const std::vector<wxFileName>& dirs) {
    // deep copy the string, wxString not thread safe
    SearchString = t4p::WxToIcu(search);
    SearchDirs = t4p::DeepCopyFileNames(dirs);

    t4p::TagFinderListClass* cache = new t4p::TagFinderListClass;

    // only need to initialize the global tag cache, will not show native tags
    // because there is no file that needs to be opened
    cache->InitGlobalTag(globals.TagCacheDbFileName, globals.FileTypes.GetPhpFileExtensions(),
                         globals.FileTypes.GetMiscFileExtensions(),
                         globals.Environment.Php.Version);
    TagCache.RegisterGlobal(cache);
}

void t4p::TagCacheSearchActionClass::BackgroundWork() {
    bool exactOnly = SearchString.length() <= 2;
    if (IsCancelled()) {
        return;
    }
    std::vector<t4p::PhpTagClass> matches;

    // do exact match first, if that succeeds then don't bother doing near matches
    t4p::TagResultClass* results = TagCache.ExactTags(SearchString, SearchDirs);
    matches = results->Matches();
    if (matches.empty() && !exactOnly) {
        delete results;
        results = TagCache.NearMatchTags(SearchString, SearchDirs);
        matches = results->Matches();
        if (matches.empty()) {
            t4p::FileTagResultClass* fileTagResults = TagCache.ExactFileTags(SearchString, SearchDirs);
            matches = fileTagResults->MatchesAsTags();
            if (matches.empty()) {
                delete fileTagResults;
                fileTagResults = TagCache.NearMatchFileTags(SearchString, SearchDirs);
                matches = fileTagResults->MatchesAsTags();
            }
            delete fileTagResults;
        }
    }
    delete results;
    if (!IsCancelled()) {
        // PostEvent will set the correct event ID
        t4p::TagCacheSearchCompleteEventClass evt(wxID_ANY, SearchString, matches);
        PostEvent(evt);
    }
}
wxString t4p::TagCacheSearchActionClass::GetLabel() const {
    return wxT("Tag Cache Search");
}

const wxEventType t4p::EVENT_TAG_CACHE_SEARCH_COMPLETE = wxNewEventType();
