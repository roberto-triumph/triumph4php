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

#ifndef SQL_EDITOR_FEATURECLASS_H__
#define SQL_EDITOR_FEATURECLASS_H__

#include <features/FeatureClass.h>
#include <features/wxformbuilder/SqlBrowserFeatureForms.h>
#include <globals/ProjectClass.h>
#include <globals/DatabaseTagClass.h>
#include <language/SqlLexicalAnalyzerClass.h>
#include <actions/SqlMetaDataActionClass.h>
#include <wx/thread.h>
#include <wx/txtstrm.h>
#include <vector>
#include <unicode/unistr.h>

namespace t4p {

/**
 * This event will be propagated when the SQL query completes
 * execution
 * event.GetClientData() will have a pointer to a SqlResultClass pointer. ClientData pointer will contain 
 * everything necessary to iterate through a result. Event handlers will own the pointer and will need to delete it when they
 * are done reading the result. This pointer may be NULL when there are no results.
 * event.GetId() will contain the ID of query (as given to MultiplSqlExecuteClass::Init).
 */
const wxEventType QUERY_COMPLETE_EVENT = wxNewEventType();

class QueryCompleteEventClass : public wxEvent {

public: 
	/**
	 * this class will not delete the pointer; the 
	 * event handler should
	 */
	t4p::SqlResultClass* Results;
	
	
	QueryCompleteEventClass(t4p::SqlResultClass* results, int eventId);
	
	wxEvent* Clone() const;

};

typedef void (wxEvtHandler::*QueryCompleteEventClassFunction)(QueryCompleteEventClass&);

#define EVT_QUERY_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::QUERY_COMPLETE_EVENT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( QueryCompleteEventClassFunction, & fn ), (wxObject *) NULL ),



// forward declarations; definitons are at the bottom of this file
class SqlBrowserFeatureClass;
class MultipleSqlExecuteClass;

/**
 * dialog to create / edit an sqlite connection
 */
class SqliteConnectionDialogClass : public SqliteConnectionDialogGeneratedClass {

public:

	SqliteConnectionDialogClass(wxWindow* parent, t4p::DatabaseTagClass& tag);

};


/**
 * dialog to create / edit / test MySQL connection
 */
class MysqlConnectionDialogClass : public MysqlConnectionDialogGeneratedClass {

public:

	/**
	 * @param wxWindow* the parent window
	 * @param DatabaseTagClass& will get populated with the values that the user entered. 
	 * @param size_t& chosenIndex the info item that the user selected 
	 */
	MysqlConnectionDialogClass(wxWindow* parent, t4p::DatabaseTagClass& dbTag,
		t4p::RunningThreadsClass& runningThreads);
	
	~MysqlConnectionDialogClass();
	
private:
	
	void OnTestButton(wxCommandEvent& event);
	
	/**
	 * cleans up the current query and closes the connection
	 */
	void OnCancelButton(wxCommandEvent& event);
		
	void ShowTestResults(t4p::QueryCompleteEventClass& event);
	
	/**
	 * to execute the test query
	 */
	t4p::SqlQueryClass TestQuery;
	
	/**
	 * to keep track of the background thread that will
	 * test the connection
	 */
	t4p::RunningThreadsClass& RunningThreads;
	
	/**
	 * to kill the test query if needed
	 */
	t4p::ConnectionIdentifierClass ConnectionIdentifier;

	/**
	 * to kill the test query thread if needed
	 */
	int RunningActionId;
	
	DECLARE_EVENT_TABLE()
};

/**
 * dialog that shows all connections; user can add / edit / delete them
 */
class SqlConnectionListDialogClass : public SqlConnectionListDialogGeneratedClass {

public:

	SqlConnectionListDialogClass(wxWindow* parent, std::vector<t4p::DatabaseTagClass>& dbTags, t4p::RunningThreadsClass& runningThreads);
	
	~SqlConnectionListDialogClass();

private:

	void OnOkButton(wxCommandEvent& event);
	
	void OnCancelButton(wxCommandEvent& event);
	
	void OnAddMysqlButton(wxCommandEvent& event);
	
	void OnAddSqliteButton(wxCommandEvent& event);
	
	void OnCloneButton(wxCommandEvent& event);
	
	void OnTestSelectedButton(wxCommandEvent& event);
	
	void OnRemoveSelectedButton(wxCommandEvent& event);
	
	void OnRemoveAllButton(wxCommandEvent& event);
	
	void OnCheckToggled(wxCommandEvent& event);
	
	void OnListDoubleClick(wxCommandEvent& event);
	
	void ShowTestResults(t4p::QueryCompleteEventClass& event);
	
	/**
	 * add a tag to the interface and to the backing list
	 */
	void Push(const t4p::DatabaseTagClass& tag);

	/**
	 * The info list to modify AFTER the user clicks OK
	 */
	std::vector<DatabaseTagClass>& DatabaseTags;

	/**
	 * The info list to user modifies. This will contain both the user-created
	 * connections and the detected ones
	 */
	std::vector<DatabaseTagClass> EditedDatabaseTags;
	
	/**
	 * to execute the test query
	 */
	t4p::SqlQueryClass TestQuery;
	
	/**
	 * to keep track of the background thread that will
	 * test the connection
	 */
	t4p::RunningThreadsClass& RunningThreads;
	
	/**
	 * to kill the test query if needed
	 */
	t4p::ConnectionIdentifierClass ConnectionIdentifier;
	
	/**
	 * to kill the test query thread if needed
	 */
	int RunningActionId;
	
	DECLARE_EVENT_TABLE()
};

/**
 * Class that will take a string of SQL statements and will execute them.
 */
class MultipleSqlExecuteClass : public t4p::ActionClass {
	
public:

	/**
	 * @param runningThreads the object that will receive the EVENT_WORK_* events when queries are
	 *        finished being executed
	 * @param queryId this ID will be used by the handler; the handler should only handle its own
	 *        query results.
	 * @param connectionIdentifier the connection ID will be set, and the main thread can read
	 *        the connection ID to kill a query when the user wants to cancel it
	 */
	MultipleSqlExecuteClass(t4p::RunningThreadsClass& runningThreads, int queryId, t4p::ConnectionIdentifierClass& connectionIdentifier);
	
	/**
	 * Prepares queries to be run
	 * @param sql the entire SQL contents to be executed. This may contain more than one query.
	 * @param query the connection options used to connect
	 * @return true if sql is not empty
	 */
	bool Init(const UnicodeString& sql, const SqlQueryClass& query);

	wxString GetLabel() const;
	
protected:

	void BackgroundWork();

	/**
	 * when the query needs to be cancelled, tell SQlite to stop
	 * the running query
	 */
	void DoCancel();

private:

	/**
	 * cleans up the current connection. After a call to this session, stmt, and row are no longer
	 * valid.
	 */
	void Close();

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
	 * the background thread and the main thread will share the connection ID;
	 * the connection ID will be read by the main thread to kill a query when the user wants to cancel it
	 */
	ConnectionIdentifierClass& ConnectionIdentifier;

	/**
	 * This ID will be used to differentiate between the events that the various panels will generate.
	 * Each panel will only handles the events generated from its own MultipleSqlExecute class.
	 */
	int QueryId;
};

/**
 * class that holds what delimiters to use
 * for columns/rows when exporting to clipboard
 * or to a file
 */
class SqlCopyOptionsClass {
	
public:
	
	/**
	 * string to put before / after each cell
	 *  (except first and last)
	 */
	wxString ColumnDelim;
	
	/**
	 * string to put before / after each cell
	 * but before / after each delimiter. Any
	 * 
	 */
	wxString ColumnEnclosure;
	
	/**
	 * string to put before / after each row
	 *  (except first and last)
	 */
	wxString RowDelim;

	/**
	 * string to fill in when a NULL cell is
	 * seen
	 */
	wxString NullFiller;
	
	SqlCopyOptionsClass();
	
	SqlCopyOptionsClass(const t4p::SqlCopyOptionsClass& src);
	
	t4p::SqlCopyOptionsClass& operator=(const t4p::SqlCopyOptionsClass& src);
	
	void Copy(const t4p::SqlCopyOptionsClass& src);
	
	/**
	 * writes the given rows to the given output stream using
	 * the delimiters that are set 
	 */
	void Export(std::vector<wxString> values, wxTextOutputStream& stream);
	
	/**
	 * write the row delimiter to the stream
	 */
	void EndRow(wxTextOutputStream& stream);
};

/**
 * class that will convert a row from the SQL results to
 * a SQL INSERT statement
 */
class RowToSqlInsertClass {
	
	public:

	/**
	 * The values of the row; to be put in
	 * the INSERT statement
	 */
	std::vector<UnicodeString> Values;
	
	/**
	 * All of the available columns of the table
	 */
	std::vector<UnicodeString> Columns;
	
	/**
	 * The name of the table; to put in the 
	 * INSERT statement
	 */
	UnicodeString TableName;
	
	/**
	 * The columns that will be used in the INSERT
	 * statement
	 */
	std::vector<UnicodeString> CheckedColumns;
	
	/**
	 * The values that will be used in the INSERT
	 * statement
	 */
	std::vector<UnicodeString> CheckedValues;
	
	/**
	 * 0 == put entire statement in one line
	 * 1 == put statement in multiple lines
	 */
	int LineMode;
	
	enum {
		SINGLE_LINE,
		MULTI_LINE
	};
	
	RowToSqlInsertClass();
	
	RowToSqlInsertClass(const t4p::RowToSqlInsertClass& src);
	
	UnicodeString CreateStatement(t4p::DatabaseTagClass::Drivers driver) const;
	
	t4p::RowToSqlInsertClass& operator=(const t4p::RowToSqlInsertClass& src);
	
	void Copy(const t4p::RowToSqlInsertClass& src);
};

/**
 * Class that converts a row of SQL data to a PHP
 * array
 */
class RowToPhpClass {
	
public:

	/**
	 * All of the available columns of the table
	 */
	std::vector<UnicodeString> Columns;
	
	/**
	 * All of the row's values
	 */
	std::vector<UnicodeString> Values;
	
	/**
	 * The columns that will be used in the INSERT
	 * statement
	 */
	std::vector<UnicodeString> CheckedColumns;
	
	/**
	 * The values that will be used in the INSERT
	 * statement
	 */
	std::vector<UnicodeString> CheckedValues;
	
	/**
	 * 0 == the array will contain the row's values as
	 * the array values
	 * 1 == the array will contain empty strings as
	 * the array values
	 */
	int CopyValues;
	
	enum {
		VALUES_ROW,
		VALUES_EMPTY
	};
	
	/**
	 * 0 == the array will be written using PHP < 5.4 
	 * syntax (array keyword)
	 * 1 ==  the array will be written using PHP 5.4+
	 * syntax ( short array syntax '[')
	 */
	int ArraySyntax;
	
	enum {
		SYNTAX_KEYWORD,
		SYNTAX_OPERATOR
	};
	
	RowToPhpClass();
	
	RowToPhpClass(const t4p::RowToPhpClass& src);
	
	t4p::RowToPhpClass& operator=(const t4p::RowToPhpClass& src);
	
	void Copy(const t4p::RowToPhpClass& src);
	
	UnicodeString CreatePhpArray();
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
	 * @param t4p::StatusBarWithGaugeClass* the gauge control. this class will NOT own the pointer
	 * @param t4p::SqlQueryClass connection settings to prime the browser with
	 * @param SqlBrowserFeatureClass used to create a new panel (and attach it to the tools window) for 
	 *        result sets
	 */
	SqlBrowserPanelClass(wxWindow* parent, int id, t4p::StatusBarWithGaugeClass* gauge,
		const SqlQueryClass& query, SqlBrowserFeatureClass* feature);

	~SqlBrowserPanelClass();

	/**
	 * Runs the query that is in the text control (in a separate thread).
	 */
	void ExecuteCodeControl();
	
	/**
	 * Runs the given query on the given connection (in a separate thread).
	 */
	void ExecuteQuery(const wxString& sql, const t4p::DatabaseTagClass& tag);

	/**
	 * kills a running query in a nice way.
	 */
	void Stop();

	/**
	 * When a query has finished running display the results in the grid
	 */
	void OnQueryComplete(t4p::QueryCompleteEventClass& event);
	
	/**
	 * signal to this panel that the user changed the connection.
	 */
	void SetCurrentInfo(const DatabaseTagClass& info);
	
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
	
	/**
	 *  fill the connection list with the configured, enabled connections
	 */
	void FillConnectionList();

private:
	
	/**
	 * Transfers all of the variables from the controls to the Query data structure 
	 * and will return true if all values are valid
	 */
	bool Check();
	
	void OnActionProgress(t4p::ActionProgressEventClass& event);
	
	/**
	 * close the connection here
	 */
	void OnActionComplete(t4p::ActionEventClass& event);
	
	/**
	 * Fill the grid with the a single SQL result
	 */
	void Fill(SqlResultClass* results);
	
	/**
	 * when the user makes a change in connection, make sure
	 * to update the database tag on the code control so that
	 * auto complete works on the columns/tables of the selected
	 * connecton
	 */
	void OnConnectionChoice(wxCommandEvent& event);
	
	/**
	 * Fill the grid with ALL SQL results
	 */
	void RenderAllResults();

	/**
	 * Refesh the table results by re-executing the query
	 */
	void OnRefreshButton(wxCommandEvent& event);
	
	/**
	 * When the user right-mouse clicks on a cell, show a context
	 * menu
	 */
	void OnGridRightClick(wxGridEvent& event);
	
	// grid context menu handlers
	void OnCopyAllRows(wxCommandEvent& event);
	void OnCopyRow(wxCommandEvent& event);
	void OnCopyRowAsSql(wxCommandEvent& event);
	void OnCopyRowAsPhp(wxCommandEvent& event);
	void OnCopyCellData(wxCommandEvent& event);
	void OnOpenInEditor(wxCommandEvent& event);
	int SelectedCol;
	int SelectedRow;

	/**
	 * The connection info
	 */
	SqlQueryClass Query;

	/**
	 * this enable the query thread to give us the connection
	 * ID so that we can issue a kill command to stop
	 * a query when this panel is destroyed.
	 */
	t4p::ConnectionIdentifierClass ConnectionIdentifier;
	
	/**
	 * Filled in with the last error string from the database
	 */
	UnicodeString LastError;

	/**
 	 * The contents of the code control that are currently being executed.
	 */
	UnicodeString LastQuery;
	
	/**
	 * used to stop a running query if this panel is closed.
	 */
	int RunningActionId;
	
	/**
	 * the accumulated results. This class will DELETE the pointers once it has rendered them.
	 */
	std::vector<SqlResultClass*> Results;
	
	/**
	 * to convert a row of data into a sql INSERT statement
	 */
	t4p::RowToSqlInsertClass RowToSqlInsert;
	
	/**
	 * to convert a row of a data into a
	 * PHP array
	 */
	t4p::RowToPhpClass RowToPhp;
	
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
	SqlBrowserFeatureClass* Feature;
	
	/**
	 * delimiters for rows/cols when exporting data
	 */
	SqlCopyOptionsClass CopyOptions;
	
	/**
	 * This ID will be used to differentiate between the events that the various panels will generate.
	 * Each panel will only handles the events generated from its own MultipleSqlExecute class.
	 */
	int QueryId;
	
	DECLARE_EVENT_TABLE()
};

/**
 * panel that shows a grid of all indices of a database table
 */
class DefinitionIndicesPanelClass : public DefinitionIndicesPanelGeneratedClass {

public:

	DefinitionIndicesPanelClass(wxWindow* parent);

	void Fill(t4p::SqlResultClass* result);
};

/**
 * panel that shows a grid of all columns of a database table
 */
class DefinitionColumnsPanelClass : public DefinitionColumnsPanelGeneratedClass {

public:

	DefinitionColumnsPanelClass(wxWindow* parent);

	void Fill(t4p::SqlResultClass* result);

};

class TableDefinitionPanelClass : public TableDefinitionPanelGeneratedClass {
	
public:
	
	TableDefinitionPanelClass(wxWindow* parent, int id, t4p::SqlBrowserFeatureClass& feature);
	~TableDefinitionPanelClass();
	
	/**
	 * set the table definition to be shown
	 */
	void ShowTable(const t4p::DatabaseTagClass& tag, const wxString& tableName);
	
	void FillConnectionList();
	
private:

	void OnColumnSqlComplete(t4p::QueryCompleteEventClass& event);
	
	void OnIndexSqlComplete(t4p::QueryCompleteEventClass& event);
	
	void OnTableNameEnter(wxCommandEvent& event);
	
	void OnSqlButton(wxCommandEvent& event);
	
	void OnCreateSqlComplete(t4p::QueryCompleteEventClass& event);
	
	void OnRefreshButton(wxCommandEvent& event);

	/**
	 * needed to get the connection list
	 */
	SqlBrowserFeatureClass& Feature;

	/**
	 * used to execute queries in the background
	 * use our own instance so that we can close any 
	 * running queries if the user clicks the
	 * close app button (closes the program)
	 */
	t4p::RunningThreadsClass RunningThreads;
	
	/**
	 * to kill a running query
	 */
	t4p::ConnectionIdentifierClass TableConnectionIdentifier;
	
	/**
	 * to kill a running query
	 */
	t4p::ConnectionIdentifierClass IndexConnectionIdentifier;

	t4p::DefinitionIndicesPanelClass* DefinitionIndicesPanel;

	t4p::DefinitionColumnsPanelClass* DefinitionColumnsPanel;
	
	DECLARE_EVENT_TABLE()
};


/**
 * This is a feature to manage SQL connections and make queries to the database.
 */
class SqlBrowserFeatureClass : public FeatureClass {
public:
	SqlBrowserFeatureClass(t4p::AppClass& app);
	
	~SqlBrowserFeatureClass();

	/**
	 * This feature will have menu to create ad-hoc queries
	 */
	void AddNewMenu(wxMenuBar* menuBar);

	void AddToolBarItems(wxAuiToolBar* toolBar);

	void LoadPreferences(wxConfigBase* config);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
	SqlBrowserPanelClass* CreateResultsPanel(CodeControlClass* ctrl);
	
	void AuiManagerUpdate();
	
	void NewSqlBuffer(const wxString& sql);
	
	void NewTextBuffer(const wxString& text);
	
private:

	void OnSqlBrowserToolsMenu(wxCommandEvent& event);
	
	void OnSqlConnectionMenu(wxCommandEvent& event);

	void OnSqlDetectMenu(wxCommandEvent& event);
	
	void OnRun(wxCommandEvent& event);
	
	void OnAppExit(wxCommandEvent& event);
	
	void OnCmdTableDataOpen(t4p::OpenDbTableCommandEventClass& event);
	
	void OnCmdTableDefinitionOpen(t4p::OpenDbTableCommandEventClass& event);
	
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
	 * when a results panel is closed, kill the running query
	 */
	void OnToolsNotebookPageClose(wxAuiNotebookEvent& event);

	/**
	 * Will start the SQL meta data background task
	 */
	void DetectMetadata();

	/**
	 * Saves the user-created infos using the global config.
	 */
	void SavePreferences();
	
	DECLARE_EVENT_TABLE()
};


/**
 * dialog that prompts the user what delimiters to use
 * for columns/rows when exporting to clipboard
 * or to a file
 */
class SqlCopyDialogClass : public SqlCopyDialogGeneratedClass {

public:
	
	SqlCopyDialogClass(wxWindow* parent, int id, t4p::SqlCopyOptionsClass& options);

protected:

	void OnOkButton(wxCommandEvent& event);
	
	void OnCancelButton(wxCommandEvent& event);

private:

	t4p::SqlCopyOptionsClass EditedOptions;
	
	t4p::SqlCopyOptionsClass& OriginalOptions;
};

/**
 * This dilaog will prompt the user to select the columns that
 * will be included in the SQL INSERT statement that is copied
 * to the clipboard
 */
class SqlCopyAsInsertDialogClass : public SqlCopyAsInsertDialogGeneratedClass {

public:
	
	SqlCopyAsInsertDialogClass(wxWindow* parent, int id, 
		t4p::RowToSqlInsertClass& rowToSql);
	
private:

	void OnOkButton(wxCommandEvent& event);
	
	void OnCancelButton(wxCommandEvent& event);
	
	void OnCheckAll(wxCommandEvent& event);
	
	/**
	 * the object being edited
	 */
	t4p::RowToSqlInsertClass EditedRowToSql;
	
	/**
	 * the object passed in the constructor, only 
	 * changed when the user clicks OK
	 */
	t4p::RowToSqlInsertClass& RowToSql;
	
	bool HasCheckedAll;
};

class SqlCopyAsPhpDialogClass : public SqlCopyAsPhpDialogGeneratedClass {
	
public:

	SqlCopyAsPhpDialogClass(wxWindow* parent, int id, t4p::RowToPhpClass& rowToPhp);
	
private:

	void OnOkButton(wxCommandEvent& event);
	
	void OnCancelButton(wxCommandEvent& event);
	
	void OnCheckAll(wxCommandEvent& event);
	
	t4p::RowToPhpClass EditedRowToPhp;
	
	t4p::RowToPhpClass& RowToPhp;
	
	bool HasCheckedAll;
};

}

#endif