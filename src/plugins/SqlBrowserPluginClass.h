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
	 * Creates a new SQL browser panel, using the given codeControl as the 
	 * text editor.
	 * Need for the caller of this method to create it as this class doesn't
	 * have access to the user options.
	 * @param wxWindow* the parent of this window
	 * @param int id the window ID
	 * @param mvceditor::CodeControlClass* the code control; this class will own the pointer
	 * @param mvceditor::StatusBarWithGaugeClass* the gauge control. this class will NOT own the pointer
	 * @param mvceditor::SqlQueryClass connection settings to prime the browser with
	 */
	SqlBrowserPanelClass(wxWindow* parent, int id, mvceditor::CodeControlClass* codeControl, mvceditor::StatusBarWithGaugeClass* gauge,
		const SqlQueryClass& query);
	
	/**
	 * Runs the query that is in the text control (in a separate thread).
	 */
	void Execute();
	
	/**
	 * Gets the entire text that is currently in the code control.
	 */
	wxString GetText();
	
	/**
	 * When a query has finished running display the results in the grid
	 */
	void OnQueryComplete(wxCommandEvent& event);

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
	
	void UpdateLabels(const wxString& result);
	
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
	 * For SQL editing.
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
	
	std::vector<DatabaseInfoClass> Infos;
	
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