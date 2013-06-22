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
#ifndef __MVCEDITOR_TAGCACHESEARCHACTIONCLASS_H__
#define __MVCEDITOR_TAGCACHESEARCHACTIONCLASS_H__

#include <actions/ActionClass.h>
#include <globals/GlobalsClass.h>
#include <language/TagCacheClass.h>
#include <unicode/unistr.h>
#include <wx/string.h>
#include <vector>

namespace mvceditor {

/**
 * event that is generated when a tag query is completed.  this event
 * contains the results of the search.
 */
class TagCacheSearchCompleteEventClass : public wxEvent {
        
    public:

	/**
	 * The string that was searched for.
	 */
	UnicodeString SearchString;

    /**
     * Will contain all of the resulting tags.
	 * This pointer should be deleted by the event handler
     */
	std::vector<mvceditor::TagClass> Tags;

    TagCacheSearchCompleteEventClass(int eventId, const UnicodeString& searchString, 
		const std::vector<mvceditor::TagClass>& tags);
    
    wxEvent* Clone() const;

};

extern const wxEventType EVENT_TAG_CACHE_SEARCH_COMPLETE;

typedef void (wxEvtHandler::*TagCacheSearchCompleteEventClassFunction)(TagCacheSearchCompleteEventClass&);

#define EVENT_TAG_CACHE_SEARCH_COMPLETE(id, fn) \
        DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_TAG_CACHE_SEARCH_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( TagCacheSearchCompleteEventClassFunction, & fn ), (wxObject *) NULL ),


/**
 * class that will execute a query against the tag cache in the background.
 * the results will be posted in an event of type EVENT_TAG_CACHE_SEARCH_COMPLETE.
 */
class TagCacheSearchActionClass : public mvceditor::ActionClass {
	
public:

	TagCacheSearchActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * set the search parameters.  this should be called before the action is
	 * added to the run queue
	 * This won't be a straight equals search; it will be a "near match"
	 * as defined by ParsedTagFinderClass::CollectNearMathResources()
	 * when the given search string is more than 2 characters long, and will
	 * be an exact search as defined by CollectFullyQualifiedResources when the 
	 * search string is 2 characters long
	 * This method assumes that index is up-to-date, if not matches may be stale.
	 *
	 * @parm globals to get the locations of the tag dbs
	 * @param search the search string, can be either file name, full path, or class names, function names
	 * @param dirs directories to restrict matching tags in. If empty, then the entire cache will be searched.
	 */
	void SetSearch(mvceditor::GlobalsClass& globals, const wxString& search, const std::vector<wxFileName>& dirs);

	wxString GetLabel() const;

protected:

	void BackgroundWork();

private:

	/**
	 * the thing to search in
	 */
	mvceditor::TagCacheClass TagCache;

	/**
	 * the string to look for. could be a partial class name, file name, function name
	 */
	UnicodeString SearchString;

	/**
	 * directories to search in. If empty, then the entire cache will be searched.
	 */
	std::vector<wxFileName> SearchDirs;
};
}

#endif