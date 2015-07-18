/**
 * The MIT License
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

#ifndef T4P_TOTALTAGSEARCHACTIONCLASS_H
#define T4P_TOTALTAGSEARCHACTIONCLASS_H

#include <wx/event.h>
#include <actions/ActionClass.h>
#include <language_php/DatabaseTableTagClass.h>
#include <language_php/PhpTagClass.h>
#include <globals/GlobalsClass.h>
#include <unicode/unistr.h>
#include <vector>

namespace t4p {

class TotalTagResultClass {

	public:

	enum Types {
		FILE_TAG,
		CLASS_TAG,
		METHOD_TAG,
		FUNCTION_TAG,
		TABLE_DATA_TAG,
		TABLE_DEFINITION_TAG
	};

	/**
	 * will only be valid when this result is a file
	 */
	t4p::FileTagClass FileTag;

	/**
	 * will only be valid when this result is a class, function, or method
	 */
	t4p::PhpTagClass PhpTag;

	/**
	 * will only be valid when this result is a database table
	 */
	t4p::DatabaseTableTagClass TableTag;

	Types Type;

	TotalTagResultClass();
	TotalTagResultClass(const t4p::FileTagClass& file);
	TotalTagResultClass(const t4p::PhpTagClass& tag);
	TotalTagResultClass(const t4p::DatabaseTableTagClass& table);

	TotalTagResultClass(const t4p::TotalTagResultClass& src);

	void Copy(const t4p::TotalTagResultClass& src);
};

/**
 * event that is generated when a tag query is completed.  this event
 * contains the results of the search.
 */
class TotalTagSearchCompleteEventClass : public wxEvent {

	public:

	/**
	 * The string that was searched for.
	 */
	UnicodeString SearchString;

	/**
	 * line number parsed from the user query (ie. the end after colon; user.php:299)
	 */
	int LineNumber;

    /**
     * Will contain all of the resulting tags.
     */
	std::vector<t4p::TotalTagResultClass> Tags;

    TotalTagSearchCompleteEventClass(int eventId, const UnicodeString& searchString,
		int lineNumber, const std::vector<t4p::TotalTagResultClass>& tags);

    wxEvent* Clone() const;

};

extern const wxEventType EVENT_TOTAL_TAG_SEARCH_COMPLETE;

typedef void (wxEvtHandler::*TotalTagSearchCompleteEventClassFunction)(TotalTagSearchCompleteEventClass&);

#define EVENT_TOTAL_TAG_SEARCH_COMPLETE(id, fn) \
        DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_TOTAL_TAG_SEARCH_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent(TotalTagSearchCompleteEventClassFunction, & fn), (wxObject *) NULL),


class TotalTagSearchActionClass : public t4p::ActionClass {

	public:

	TotalTagSearchActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

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
	void SetSearch(t4p::GlobalsClass& globals, const wxString& search, const std::vector<wxFileName>& dirs);

	wxString GetLabel() const;

	protected:

	void BackgroundWork();

	private:

	/**
	 * perform a search on all items using exact, case
	 * insensitive searches
	 *
	 * @param matches the found items will be added to this vector
	 * @return bool TRUE if at least 1 match was found
	 */
	bool SearchExact(std::vector<t4p::TotalTagResultClass>& matches);

	/**
	 * performs a search on all items using near-match
	 * logic
	 *
	 * @param matches the found items will be added to this vector
	 * @return bool TRUE if at least 1 match was found
	 */
	bool SearchNearMatch(std::vector<t4p::TotalTagResultClass>& matches);

	/**
	 * the thing to search in
	 */
	t4p::TagCacheClass TagCache;

	/**
	 * connection to tags db
	 */
	soci::session Session;

	/**
	 * the thing to search for database table tags in
	 */
	t4p::SqlResourceFinderClass SqlTagCache;

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

#endif // T4P_TOTALTAGSEARCHACTIONCLASS_H
