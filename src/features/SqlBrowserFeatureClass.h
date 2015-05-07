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
extern const wxEventType QUERY_COMPLETE_EVENT;

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
 * This is a feature to manage SQL connections and make queries to the database.
 */
class SqlBrowserFeatureClass : public FeatureClass {
public:
	SqlBrowserFeatureClass(t4p::AppClass& app);
	
	~SqlBrowserFeatureClass();

	void LoadPreferences(wxConfigBase* config);
	
	/**
	 * Will start the SQL meta data background task
	 */
	void DetectMetadata();

	/**
	 * Saves the user-created infos using the global config.
	 */
	void SavePreferences();

private:	
	
};

}

#endif