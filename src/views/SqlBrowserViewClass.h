/**
 * @copyright  2009-2011 Roberto Perpuly
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

#ifndef SQL_EDITOR_VIEWCLASS_H
#define SQL_EDITOR_VIEWCLASS_H

#include <views/FeatureViewClass.h>
#include <features/SqlBrowserFeatureClass.h>
#include <views/wxformbuilder/SqlBrowserFeatureForms.h>
#include <globals/ProjectClass.h>
#include <language_sql/DatabaseTagClass.h>
#include <language_sql/SqlLexicalAnalyzerClass.h>
#include <actions/SqlMetaDataActionClass.h>
#include <globals/Events.h>
#include <code_control/CodeControlClass.h>
#include <wx/txtstrm.h>
#include <vector>
#include <unicode/unistr.h>

namespace t4p {
// forward declaration, defined below
class SqlBrowserViewClass;

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
	 * @param SqlBrowserFeatureClass used to start running queries in the background
	 * @param SqlBrowserViewClass used to create a new panel (and attach it to the tools window) for
	 *        result sets
	 */
	SqlBrowserPanelClass(wxWindow* parent, int id, t4p::StatusBarWithGaugeClass* gauge,
		const SqlQueryClass& query, SqlBrowserFeatureClass& feature,
		SqlBrowserViewClass& view);

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

	/**
	 * @return int the index of the selected connection of this panel
	 */
	int SelectedConnectionIndex();

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

	void OnHelpButton(wxCommandEvent& event);

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
	 * needed to start running queries in the background.
	 */
	SqlBrowserFeatureClass& Feature;

	/**
	 * needed to create the results panel and attach it to the tools window.
	 */
	SqlBrowserViewClass& View;

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
	TableDefinitionPanelClass(wxWindow* parent, int id, t4p::SqlBrowserFeatureClass& feature,
		t4p::SqlBrowserViewClass& view);
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
	 * Needed to create new code controls
	 */
	t4p::SqlBrowserViewClass& View;

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
 * the sql code completion provider will provide SQL-related suggestions
 *  (table names, column names, sql keywords) for the user when the user is edting SQL
 */
class SqlCodeCompletionProviderClass : public t4p::CodeCompletionProviderClass {
	public:
	SqlCodeCompletionProviderClass(t4p::GlobalsClass& globals);

	bool DoesSupport(t4p::FileType type);

	virtual void Provide(t4p::CodeControlClass* ctrl, std::vector<t4p::CodeCompletionItemClass>& suggestions, wxString& completeStatus);

	void SetDbTag(const t4p::DatabaseTagClass& dbTag);

	private:
	std::vector<wxString> HandleAutoCompletionMySql(const UnicodeString& word);

	/**
	 * the connection to fetch database metadata for (auto completion)
	 */
	t4p::DatabaseTagClass CurrentDbTag;

	/**
	 * to get the connection to the tags database (sqlite)
	 */
	t4p::GlobalsClass& Globals;
};

class SqlBraceMatchStylerClass : public t4p::BraceMatchStylerClass {
	public:
	SqlBraceMatchStylerClass();

	bool DoesSupport(t4p::FileType type);

	void Style(t4p::CodeControlClass* ctrl, int posToCheck);
};

/**
 * This is a view to manage SQL connections and make queries to the database.
 */
class SqlBrowserViewClass : public FeatureViewClass {
	public:
	SqlBrowserViewClass(t4p::SqlBrowserFeatureClass& feature);

	~SqlBrowserViewClass();

	/**
	 * This feature will have menu to create ad-hoc queries
	 */
	void AddNewMenu(wxMenuBar* menuBar);

	void AddToolBarItems(wxAuiToolBar* toolBar);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

	SqlBrowserPanelClass* CreateResultsPanel(CodeControlClass* ctrl);

	void AuiManagerUpdate();

	void NewSqlBuffer(const wxString& sql);

	void NewTextBuffer(const wxString& text);

	/**
	 * signal to the feature that the user changed the connection.
	 * sql code completion will use the given connection info
	 * to get tables and column names
	 */
	void SetCurrentInfo(const DatabaseTagClass& info);

	private:
	void OnSqlBrowserToolsMenu(wxCommandEvent& event);

	void OnSqlConnectionMenu(wxCommandEvent& event);

	void OnSqlDetectMenu(wxCommandEvent& event);

	void OnRun(wxCommandEvent& event);

	void OnAppExit(wxCommandEvent& event);

	void OnCmdTableDataOpen(t4p::OpenDbTableCommandEventClass& event);

	void OnCmdTableDefinitionOpen(t4p::OpenDbTableCommandEventClass& event);

	void OnAppFileOpened(t4p::CodeControlEventClass& event);

	/**
	 * synchronize the SQL query tab in the code control notebook with
	 * the results tool notebook.  Will switch the page of the
	 * tools notebook to show the results panel that was created with
	 * the currently shown code control.
	 */
	void OnAppFilePageChanged(t4p::CodeControlEventClass& event);

	/**
	 * will unlink any results panel that is linked to the code control
	 * that is about to be closed.
	 */
	void OnAppFileClosed(t4p::CodeControlEventClass& event);

	/**
	 * when a results panel is closed, kill the running query
	 */
	void OnToolsNotebookPageClose(wxAuiNotebookEvent& event);

	/**
	 * to start the sql detect metadata action
	 */
	t4p::SqlBrowserFeatureClass& Feature;

	/**
	 * used to show the user suggestions of sql tables, columns
	 * and keywords
	 */
	t4p::SqlCodeCompletionProviderClass SqlCodeCompletionProvider;

	/**
	 * to style sql parenthesis ()
	 */
	t4p::SqlBraceMatchStylerClass SqlBraceMatchStyler;

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
