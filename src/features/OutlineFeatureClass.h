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
#ifndef OUTLINE_VIEW_FEATURECLASS_H
#define OUTLINE_VIEW_FEATURECLASS_H

#include <features/FeatureClass.h>
#include <globals/Events.h>
#include <pelet/TokenClass.h>
#include <actions/ActionClass.h>
#include <language_php/TagCacheClass.h>
#include <vector>

namespace t4p {
/**
 * grouping of a tag and its member tags.
 */
class OutlineSearchCompleteClass {
	public:
	wxString Label;

	std::map<wxString, std::vector<t4p::PhpTagClass> > Tags;

	OutlineSearchCompleteClass();

	OutlineSearchCompleteClass(const t4p::OutlineSearchCompleteClass& src);

	void Copy(const t4p::OutlineSearchCompleteClass& src);

	bool IsLabelFileName() const;
};

/**
 * event that is generated when a tag query is completed.  this event
 * contains the results of the search.
 */
class OutlineSearchCompleteEventClass : public wxEvent {
	public:
	/**
	 * Will contain all of the resulting tags.
	 */
	std::vector<t4p::OutlineSearchCompleteClass> Tags;

	OutlineSearchCompleteEventClass(int eventId, const std::vector<t4p::OutlineSearchCompleteClass>& tags);

	wxEvent* Clone() const;
};

extern const wxEventType EVENT_OUTLINE_SEARCH_COMPLETE;

typedef void (wxEvtHandler::*OutlineSearchCompleteEventClassFunction)(OutlineSearchCompleteEventClass&);

#define EVENT_OUTLINE_SEARCH_COMPLETE(id, fn) \
        DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_OUTLINE_SEARCH_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent(OutlineSearchCompleteEventClassFunction, & fn), (wxObject *) NULL),

/**
 * class that will execute a query against the tag cache in the background.
 * the results will be posted in an event.
 */
class OutlineTagCacheSearchActionClass : public t4p::ActionClass {
	public:
	OutlineTagCacheSearchActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * set the search parameters.  this should be called before the action is
	 * added to the run queue
	 *
	 * @param searches the search strings, can be either file names, full paths, or class names
	 * @param globals to get the locations of the tag dbs
	 */
	void SetSearch(const std::vector<UnicodeString>& searches, t4p::GlobalsClass& globals);

	wxString GetLabel() const;

	protected:
	void BackgroundWork();

	private:
	t4p::TagCacheClass TagCache;

	std::vector<UnicodeString> SearchStrings;

	/**
	 * the directories to look in
	 */
	std::vector<wxFileName> EnabledSourceDirs;
};

/**
 * This is a feature that is designed to let the user see the classes / methods of
 * the opened files and of related files.  The related files / classes / methods that are mentioned
 * in the opened files.
 */
class OutlineFeatureClass : public FeatureClass {
	public:
	/**
	 * Creates a new OutlineViewFeature.
	 */
	OutlineFeatureClass(t4p::AppClass& app);
};
}

#endif
