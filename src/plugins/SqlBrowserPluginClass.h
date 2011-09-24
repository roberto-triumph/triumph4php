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
#include <wx/thread.h>
#include <vector>
#include <unicode/unistr.h>

namespace mvceditor {

/**
 * This event will be propagated when the SQL query completes
 * execution
 * event.GetInt() will have a non-zero value if the query executed successfully
 */
const wxEventType QUERY_COMPLETE_EVENT = wxNewEventType();

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

class SqlBrowserPanelClass : public SqlBrowserPanelGeneratedClass, wxThreadHelper {

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
	 */
	SqlBrowserPanelClass(wxWindow* parent, int id, mvceditor::StatusBarWithGaugeClass* gauge,
		const SqlQueryClass& query);
	
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

protected:

	void* Entry();

private:

	void OnRunButton(wxCommandEvent& event);
	
	void OnTimer(wxTimerEvent& event);
	
	/**
	 * Transfers all of the variables from the controls to the Query data structure 
	 * and will return true if all values are valid
	 */
	bool Check();
	
	/**
	 * cleans up the current query and closes the connection
	 */
	void Close();
	
	/**
	 * To send queries to the server
	 */
	SqlQueryClass Query;
	
	/**
	 * Connection handle
	 */
	soci::session Session;
	
	/**
	 * result cursor
	 */
	soci::statement* Stmt;
	
	/**
	 * A record. Since the query is determined at run time, we must use dynamically binded rows
	 */
	soci::row Row;
	
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
	 * Make the gauge update smoothly
	 */
	wxTimer Timer;
	
	/**
	 * Filled in with the last error string from the database
	 */
	UnicodeString LastError;
	
	/**
	 * The contents of the code control that are currenltly being executed.
	 */
	UnicodeString LastQuery;
	
	/**
	 * The time that the query has begun executing
	 */
	wxLongLong QueryStart;
	
	/**
	 * TRUE if a query is running. 
	 */
	bool IsRunning;
	
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
	
protected:
	
	void OnProjectOpened();	
	
private:

	void OnSqlBrowserToolsMenu(wxCommandEvent& event);
	
	void OnSqlConnectionMenu(wxCommandEvent& event);
	
	void OnRun(wxCommandEvent& event);
	
	void OnWorkInProgress(wxCommandEvent& event);
	
	void OnWorkComplete(wxCommandEvent& event);
	
	SqlBrowserPanelClass* CreateResultsPanel(CodeControlClass* ctrl);
	
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