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
#include <php_frameworks/ProjectClass.h>
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
 * event.GetClientData() will have a pointer to a SqlResultClass; it will contain everything necessary to
 * iterate through a result. Event handlers will own the pointer and will need to delete it when they
 * are done reading the result.
 */
const wxEventType QUERY_COMPLETE_EVENT = wxNewEventType();


class SqlBrowserPluginClass;

class SqlConnectionDialogClass : public SqlConnectionDialogGeneratedClass, wxThreadHelper {

public:

	/**
	 * @param wxWindow* the parent window
	 * @param vector<DatabaseInfoClass> will get populated with the values that the user entered.
	 * @param size_t& chosenIndex the info item that the user selected 
	 * @param bool allowEdit if TRUE user will be allowed to modify the info properties
	 */
	SqlConnectionDialogClass(wxWindow* parent, std::vector<mvceditor::DatabaseInfoClass>&info, size_t& chosenIndex, bool allowEdit);
	
private:

	void OnOkButton(wxCommandEvent& event);
	
	void OnCancelButton(wxCommandEvent& event);
	
	void OnTestButton(wxCommandEvent& event);
	
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
	
	std::vector<DatabaseInfoClass>& Infos;
	
	SqlQueryClass TestQuery;
	
	size_t& ChosenIndex;
	
	DECLARE_EVENT_TABLE()
};

class MultipleSqlExecuteClass : public ThreadWithHeartbeatClass {
	
public:

	/**
	 * @param handler the object that will receive the EVENT_WORK_* events
	 */
	MultipleSqlExecuteClass(wxEvtHandler& handler);
	
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
	
	/**
	 * cleans up the given result set but keeps the connection alive [for another query]
	 * after a call to this method; result is no longer a valid pointer
	 */
	void CleanResult(mvceditor::SqlResultClass* result);
	
protected:

	void* Entry();

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
	 * To prevent more than one thread from running at the same time.
	 */
	bool IsRunning;
	
	bool Connected;
	
};


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
		const SqlQueryClass& query, SqlBrowserPluginClass* plugin);
	
	/**
	 * Runs the query that is in the text control (in a separate thread).
	 */
	void Execute();
	
	void ExecuteMore();

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
	 * Fill the grid with the SQL results
	 */
	void Fill(SqlResultClass* results);
	
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
	 * when running multiple queries; queries that have no result (INSERT, DELETE, CREATE...) 
	 * are put in this panel
	 */
	SqlBrowserPanelClass* OutputPanel;
	
	/**
	 * needed to create the results panel and attach it to the tools window.
	 */
	SqlBrowserPluginClass* Plugin;
	
	/**
	 * true if this grid is filled with data.
	 */
	bool IsFilled;
	
	DECLARE_EVENT_TABLE()
};

/**
 * This class will performt SQL metadata indexing (grabbing table and column names)
 * from all of the connections of the current project).
 */
class SqlMetaDataFetchClass : public ThreadWithHeartbeatClass {

public:

	SqlMetaDataFetchClass(wxEvtHandler& handler);
	
	/**
	 * starts a background thread to read the metadata. Generates events while work
	 * is in progress.
	 * @see mvceditor::ThreadWithHearbeatClass
	 * @return bool TRUE if thread was started
	 */
	bool Read(std::vector<DatabaseInfoClass>* infos, ProjectClass* project);
	
protected:

	void* Entry();
	
	std::vector<DatabaseInfoClass>* Infos;
	
	ProjectClass* Project;
};

/**
 * This is a plugin for SQL interface.
 */
class SqlBrowserPluginClass : public PluginClass {
public:
	SqlBrowserPluginClass();
	
	~SqlBrowserPluginClass();

	/**
	 * This plugin will have menu to create ad-hoc queries
	 */
	virtual void AddToolsMenuItems(wxMenu* toolsMenu);
	
	SqlBrowserPanelClass* CreateResultsPanel(CodeControlClass* ctrl);
	
	void AuiManagerUpdate();
	
protected:
	
	void OnProjectOpened();	
	
private:

	void OnSqlBrowserToolsMenu(wxCommandEvent& event);
	
	void OnSqlConnectionMenu(wxCommandEvent& event);
	
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
	
	std::vector<DatabaseInfoClass> Infos;
	
	SqlMetaDataFetchClass SqlMetaDataFetch;
	
	size_t ChosenIndex;
	
	/**
	 * @var bool TRUE if there were zero database info objects for the current project
	 * in this case we will allow the user to edit the connections info
	 */
	bool WasEmptyDetectedInfo;
	
	DECLARE_EVENT_TABLE()
};

}

#endif