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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __MVCEDITOR_SQLMETADATAACTIONCLASS_H__
#define __MVCEDITOR_SQLMETADATAACTIONCLASS_H__

#include <wx/event.h>
#include <vector>
#include <actions/GlobalActionClass.h>

namespace mvceditor {

/**
 * This event is generated once all of the meta data has been
 * fetched from all of the SQL connections
 */
extern const wxEventType EVENT_SQL_META_DATA_COMPLETE;

class SqlMetaDataEventClass : public wxEvent {

public:

	/**
	 * Get any connection errors that occurred in the background thread.
	 */
	std::vector<UnicodeString> Errors;
	
	SqlMetaDataEventClass(int eventId, const std::vector<UnicodeString>& errors);

	wxEvent* Clone() const;

};

/**
 * This class will performt SQL metadata indexing (grabbing table and column names)
 * from all of the connections of the current project). Once the list of tables 
 * and columns has been retrieved, a SqlMetaDataEventClass event is generated 
 * with the new resources
 */
class SqlMetaDataActionClass : public GlobalActionClass {

public:

	/**
	 * @param runningThreads will get notified with EVENT_WORK_* events
	 *        and the EVENT_SQL_META_DATA_COMPLETE event
	 */
	SqlMetaDataActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);
	
	/**
	 * Initialize the connections to be scanned.  This needs to be done before
	 * the thread is started (CreateSingleInstance).
	 *
	 * @see mvceditor::ThreadWithHearbeatClass
	 * @param infos the connections to fetch info for.
	 * @return bool TRUE if infos is non-empty
	 */
	bool Init(mvceditor::GlobalsClass& globals);

	void BackgroundWork();

	wxString GetLabel() const;

private:
	
	/**
	 * The connections to query; where the tables / columns will be fetched from 
	 */
	std::vector<DatabaseTagClass> DatabaseTags;
	
	/**
	 * location where sql table names will be stored
	 */
	wxFileName CacheDbFileName;
};

class SqlMetaDataInitActionClass : public mvceditor::InitializerGlobalActionClass {
	
public:

	SqlMetaDataInitActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);
	
	void Work(mvceditor::GlobalsClass& globals);
	
protected:
	
	wxString GetLabel() const;
};

}

typedef void (wxEvtHandler::*SqlMetaDataEventClassFunction)(mvceditor::SqlMetaDataEventClass&);


#define EVT_SQL_META_DATA_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_SQL_META_DATA_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( SqlMetaDataEventClassFunction, & fn ), (wxObject *) NULL ),

#endif