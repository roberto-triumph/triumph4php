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

#ifndef SQL_EDITOR_PLUGIN_CLASS_H
#define SQL_EDITOR_PLUGIN_CLASS_H

#include <PluginClass.h>
#include <plugins/wxformbuilder/SqlBrowserPluginGeneratedClass.h>
#include <environment/ProjectClass.h>
#include <environment/DatabaseInfoClass.h>
#include <widgets/ThreadWithHeartbeatClass.h>
#include <language/SqlLexicalAnalyzerClass.h>
#include <wx/thread.h>
#include <vector>
#include <unicode/unistr.h>

namespace mvceditor {

/**
 * This event will be propagated when the SQL query completes
 * execution
 * event.GetClientData() will have a pointer to a SqlResultClass pointer. ClientData pointer will contain 
 * everything necessary to iterate through a result. Event handlers will own the pointer and will need to delete it when they
 * are done reading the result. This pointer may be NULL when there are no results.
 * event.GetId() will contain the ID of query (as given to MultiplSqlExecuteClass::Init).
 */
const wxEventType QUERY_COMPLETE_EVENT = wxNewEventType();

// forward declaration; definiton is at the bottom of this file
class SqlBrowserPluginClass;

class SqlConnectionDialogClass : public SqlConnectionDialogGeneratedClass, wxThreadHelper {

public:

	/**
	 * @param wxWindow* the parent window
	 * @param vector<DatabaseInfoClass> will get populated with the values that the user entered. The list can
	 *        also contain the connections that were detected by the DatabaseDetector 
	 *        user will NOT be able to edit the detected connections.
	 * @param size_t& chosenIndex the info item that the user selected 
	 */
	SqlConnectionDialogClass(wxWindow* parent, std::vector<mvceditor::DatabaseInfoClass>& infos, size_t& chosenIndex);
	
private:

	void OnOkButton(wxCommandEvent& event);
	
	void OnCancelButton(wxCommandEvent& event);
	
	void OnTestButton(wxCommandEvent& event);

	void OnAddButton(wxCommandEvent& event);

	void OnDeleteButton(wxCommandEvent& event);

	void OnLabelText(wxCommandEvent& event);
	
	void OnListboxSelected(wxCommandEvent& event);

	void OnHelpButton(wxCommandEvent& event);
	
	/**
	 * Connection test will happen in a separate thread so dialog stays responsive
	 */
	void* Entry();
	
	void ShowTestResults(wxCommandEvent& event);
	
	/**
	 * cleans up the current query and closes the connection
	 */
	void Close();

	void UpdateTextInputs();
	
	/**
	 * The info list to modify AFTER the user clicks OK
	 */
	std::vector<DatabaseInfoClass>& Infos;

	/**
	 * The info list to user modifies. This will contain both the user-created
	 * connections and the detected ones
	 */
	std::vector<DatabaseInfoClass> EditedInfos;
	
	SqlQueryClass TestQuery;
	
	size_t& ChosenIndex;
	
	DECLARE_EVENT_TABLE()
};

/**
 * Class that will take a string of SQL statements and will execute them.
 */
class MultipleSqlExecuteClass : public ThreadWithHeartbeatClass {
	
public:

	/**
	 * @param handler the object that will receive the EVENT_WORK_* events when queries are
	 *        finished being executed
	 * @param queryId this ID will be used by the handler; the handler should only handle its own
	 *        query results.
	 */
	MultipleSqlExecuteClass(wxEvtHandler& handler, mvceditor::RunningThreadsClass& runningThreads, int queryId);
	
	/**
	 * Prepares queries to be run
	 * @param sql the entire SQL contents to be executed. This may contain more than one query.
	 * @param query the connection options used to connect
	 * @return true if sql is not empty
	 */
	bool Init(const UnicodeString& sql, const SqlQueryClass& query);
	
	/**
	 * start a new thread and execute the current query.
	 * @return bool true if a new thread was started
	 */
	bool Execute();
	
	/**
	 * cleans up the current connection. After a call to this session, stmt, and row are no longer
	 * valid.
	 */
	void Close();
	
protected:

	void Entry();

private:

	/**
	 * we will handle multiple queries here by splitting the contents of the 
	 * code control into multiple queries and send one query at a time to the server.
	 * Doing it this way because it will work for all SQL backends uniformly
	 * 
	 * To handle multiple queries. When execute happens; a query object is made 
	 * for each query.  Each query is executed one by one in a background thread (one query will
	 * be executed; the next query will only be executed once the first query returns.).
	 * When a query is finished, an event [wis propagated. 
	 * This object will handle the event by rendering the result of the query and
	 * cleaning up the result set. The results rendering
	 * depends on whether a query returned a result set. If a query returned a result
	 * set (a SELECT) then a new SqlBrowserPanel is created, if a result set is not
	 * returned then set to a special grid.
	 */
	SqlLexicalAnalyzerClass SqlLexer;
	
	/**
	 * To execute the queries
	 */
	SqlQueryClass Query;

	/**
	 * Connection handle
	 */
	soci::session Session;

	/**
	 * This ID will be used to differentiate between the events that the various panels will generate.
	 * Each panel will only handles the events generated from its own MultipleSqlExecute class.
	 */
	int QueryId;
};

/**
 * Panel which shows results from a SQL query.
 */
class SqlBrowserPanelClass : public SqlBrowserPanelGeneratedClass {

public:
	
	/**
	 * Creates a new SQL browser panel. After creation, the panel needs to be linked to a CodeControl
	 * in order for Execute() to work properly.
	 * Need for the caller of this method to create the code control as this class doesn't
	 * have access to the user options (coloring, spacing, fonts, etc...).
	 * @param wxWindow* the parent of this window
	 * @param int id the window ID
	 * @param mvceditor::StatusBarWithGaugeClass* the gauge control. this class will NOT own the pointer
	 * @param mvceditor::SqlQueryClass connection settings to prime the browser with
	 * @param SqlBrowserPluginClass used to create a new panel (and attach it to the tools window) for 
	 *        result sets
	 */
	SqlBrowserPanelClass(wxWindow* parent, int id, mvceditor::StatusBarWithGaugeClass* gauge,
		const SqlQueryClass& query, SqlBrowserPluginClass* plugin, mvceditor::RunningThreadsClass& runningThreads);
	
	/**
	 * Runs the query that is in the text control (in a separate thread).
	 */
	void Execute();

	/**
	 * When a query has finished running display the results in the grid
	 */
	void OnQueryComplete(wxCommandEvent& event);
	
	/**
	 * signal to this panel that the user changed the connection.
	 */
	void SetCurrentInfo(const DatabaseInfoClass& info);
	
	/**
	 * Sets the connection label and the result label.
	 * This is how the user knows what connection is being used by this panel.
	 */
	void UpdateLabels(const wxString& result);
	
	/**
	 * this will make it possible to link a code control to this results panel;
	 * when the user is focused on the codeControl and user click 'Run SQL Query' (Execute method) the
	 * results of the SQL will be written to this code control.
	 */
	void LinkToCodeControl(CodeControlClass* codeControl);
	
	/**
	 * This method is used to tell if these results came from the query that's in codeControl.
	 * is tied to the given window. (this panel will execute the SQL that is
	 * in the control panel when the user runs the query).
	 * @param codeControl the code control to check
	 * @param notebook this class will NOT own the pointer
	 * @param toolsNotebook this class will NOT own the pointer
	 */
	bool IsLinkedToCodeControl(CodeControlClass* codeControl);
	
	/**
	 * this method should be called when the user has closed the code control; need to do this
	 * so that this results panel knows that the code control has been deleted and should not
	 * be used. This method essentially disables the Execute() method.
	 */
	void UnlinkFromCodeControl();

private:
	
	/**
	 * Transfers all of the variables from the controls to the Query data structure 
	 * and will return true if all values are valid
	 */
	bool Check();
	
	void OnWorkInProgress(wxCommandEvent& event);
	
	/**
	 * close the connection here
	 */
	void OnWorkComplete(wxCommandEvent& event);
	
	/**
	 * Fill the grid with the a single SQL result
	 */
	void Fill(SqlResultClass* results);
	
	/**
	 * Fill the grid with ALL SQL results
	 */
	void RenderAllResults();
	
	/**
	 * The connection info
	 */
	SqlQueryClass Query;
	
	/**
	 * Filled in with the last error string from the database
	 */
	UnicodeString LastError;

	/**
 	 * The contents of the code control that are currenltly being executed.
	 */
	UnicodeString LastQuery;
	
	/**
	 * used to process the SQL and execute the queries one at a time
	 */
	MultipleSqlExecuteClass MultipleSqlExecute;
	
	/**
	 * the accumulated results. This class will DELETE the pointers once it has rendered them.
	 */
	std::vector<SqlResultClass*> Results;
	
	/**
	 * To get the query that needs to be run. One results panel will be linked with exactly one code control.
	 * When the code control is deleted then this results panel will be read-only.
	 * This pointer can be NULL. This class will NOT own this pointer.
	 */
	CodeControlClass* CodeControl;
	
	/**
	 * Show progress when the query is running
	 */
	StatusBarWithGaugeClass* Gauge;
	
	/**
	 * needed to create the results panel and attach it to the tools window.
	 */
	SqlBrowserPluginClass* Plugin;

	/**
	 * This ID will be used to differentiate between the events that the various panels will generate.
	 * Each panel will only handles the events generated from its own MultipleSqlExecute class.
	 */
	int QueryId;
	
	DECLARE_EVENT_TABLE()
};

/**
 * This class will performt SQL metadata indexing (grabbing table and column names)
 * from all of the connections of the current project).
 */
class SqlMetaDataFetchClass : public ThreadWithHeartbeatClass {

public:

	/**
	 * @param handler will get notified with EVENT_WORK_* events
	 */
	SqlMetaDataFetchClass(wxEvtHandler& handler, mvceditor::RunningThreadsClass& runningThreads);
	
	/**
	 * starts a background thread to read the metadata. Generates events while work
	 * is in progress.
	 * @see mvceditor::ThreadWithHearbeatClass
	 * @param infos the connections to fetch info for.
	 * @return bool TRUE if thread was started
	 */
	bool Read(std::vector<DatabaseInfoClass> infos);

	/**
	 * Once the background thread signals that it has finished
	 * reading the metadata, call this method to get the results
	 * of the work.
	 *
	 * @param dest the results of the background thread will be copied
	 *        to dest
	 */
	void WriteResultsInto(SqlResourceFinderClass& dest);

	/**
	 * Get any connection errors that occurred in the background thread.
	 * The errors are only from the last run.
	 * Be careful to no access this while the background thread is running.
	 */
	std::vector<UnicodeString> GetErrors();
	
protected:

	void Entry();
	
	std::vector<DatabaseInfoClass> Infos;

	std::vector<UnicodeString> Errors;
	
	/**
	 * This is a clean resource object; will only be accessed
	 * background thread will only ever write to this object.
	 */
	SqlResourceFinderClass NewResources;
};

/**
 * This is a plugin to manage SQL connections and make queries to the database.
 */
class SqlBrowserPluginClass : public PluginClass {
public:
	SqlBrowserPluginClass(mvceditor::AppClass& app);
	
	~SqlBrowserPluginClass();

	/**
	 * This plugin will have menu to create ad-hoc queries
	 */
	void AddNewMenu(wxMenuBar* menuBar);

	void AddToolBarItems(wxAuiToolBar* toolBar);

	void LoadPreferences(wxConfigBase* config);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
	SqlBrowserPanelClass* CreateResultsPanel(CodeControlClass* ctrl);
	
	void AuiManagerUpdate();
	
private:

	void OnProjectsUpdated(wxCommandEvent& event);

	void OnSqlBrowserToolsMenu(wxCommandEvent& event);
	
	void OnSqlConnectionMenu(wxCommandEvent& event);

	void OnSqlDetectMenu(wxCommandEvent& event);
	
	void OnRun(wxCommandEvent& event);
	
	void OnWorkInProgress(wxCommandEvent& event);
	
	void OnWorkComplete(wxCommandEvent& event);
	
	/**
	 * synchronize the SQL query tab in the code control notebook with
	 * the results tool notebook.  Will switch the page of the
	 * tools notebook to show the results panel that was created with
	 * the currently shown code control.
	 */
	void OnContentNotebookPageChanged(wxAuiNotebookEvent& event);
	
	/**
	 * will unlink any results panel that is linked to the code control
	 * that is about to be closed.
	 */
	void OnContentNotebookPageClose(wxAuiNotebookEvent& event);

	/**
	 * Will start the SQL meta data background task
	 */
	void DetectMetadata();

	/**
	 * Saves the user-created infos using the global config.
	 */
	void SavePreferences();
		
	/**
	 * To detect the SQL connections
	 */
	SqlMetaDataFetchClass SqlMetaDataFetch;
	
	/**
	 * index into Infos vector; the connection that is currently active.
	 * Any new results panels that are created will use this connection. Any
	 * existing results panels will keep their original connection.
	 */
	size_t ChosenIndex;
	
	DECLARE_EVENT_TABLE()
};

}

#endif