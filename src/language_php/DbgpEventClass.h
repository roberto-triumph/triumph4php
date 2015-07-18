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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef T4P_DBGPEVENTCLASS_H
#define T4P_DBGPEVENTCLASS_H

#include <wx/string.h>
#include <wx/event.h>
#include <vector>

namespace t4p {

/**
 * All of these events are events that we receive from the webserver (xdebug)
 * See http://xdebug.org/docs-dbgp.php for a thorough description of
 * the event attribute meanings.
 */

enum DbgpXmlErrors {

	/**
	 * no error when parsing the response
	 */
	DBGP_XML_ERROR_NONE,
	
	/**
	 * could not parse XML (no well-formed)
	 */
	DBGP_XML_ERROR_PARSE,

	/**
	 * xml does not contain the expected tag
	 */
	DBGP_XML_ERROR_TAG,

	/**
	 * xml does not contain the expected attribute
	 */
	DBGP_XML_ERROR_ATTRIBUTE
};

/* 
 * The init response is sent by the debug engine (xdebug) when
 * the script starts in debug mode.
 * See section 5.2
 */
class DbgpInitEventClass : public wxEvent {

public:

	wxString AppId;
	wxString IdeKey;
	wxString Session;
	wxString Thread;
	wxString Parent;
	wxString Language;
	wxString ProtocolVersion;
	wxString FileUri;

	DbgpInitEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * debugger engine error; an error with the 
 * engine itself and not the running script
 * See Section 6.5 
 */
class DbgpErrorEventClass : public wxEvent {

public:

	wxString Command;
	wxString TransactionId;
	int ErrorCode;
	wxString AppErrorCode;
	wxString Message;
	
	DbgpErrorEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * response to a query for debugger engine status; is the debugger up?
 * See Section 7.1
 */
enum DbgpStatus {
	DBGP_STATUS_UNKNOWN,
	DBGP_STATUS_STARTING,
	DBGP_STATUS_STOPPING,
	DBGP_STATUS_STOPPED,
	DBGP_STATUS_RUNNING,
	DBGP_STATUS_BREAK
};

enum DbgpReason {
	DBGP_REASON_UNKNOWN,
	DBGP_REASON_OK,
	DBGP_REASON_ERROR,
	DBGP_REASON_ABORTED,
	DBGP_REASON_EXCEPTION
};

class DbgpStatusEventClass : public wxEvent {

public:
	
	/** starting|stopping|stopped|running|break */
	t4p::DbgpStatus Status;

	/* ok|error|aborted|exception */
	t4p::DbgpReason Reason;
	wxString TransactionId;
	wxString MessageData;
	
	DbgpStatusEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * Response to query for the value of a feature
 * See Section 7.2.2 
 */
enum DbgpFeatures {
	DBGP_FEATURE_UNKNOWN,
	DBGP_FEATURE_LANGUAGE_SUPPORTS_THREADS,
	DBGP_FEATURE_LANGUAGE_NAME,
	DBGP_FEATURE_LANGUAGE_VERSION,
	DBGP_FEATURE_ENCODING,
	DBGP_FEATURE_PROTOCOL_VERSION,
	DBGP_FEATURE_DATA_ENCODING,
	DBGP_FEATURE_BREAKPOINT_LANGUAGES,
	DBGP_FEATURE_BREAKPOINT_TYPES,
	DBGP_FEATURE_MULTIPLE_SESSIONS,
	DBGP_FEATURE_MAX_CHILDREN,
	DBGP_FEATURE_MAX_DATA,
	DBGP_FEATURE_MAX_DEPTH
};

class DbgpFeatureGetEventClass : public wxEvent {

public:

	wxString Command;
	t4p::DbgpFeatures Feature;

	/*
	 * supported means that the feature_name given was a 
	 * a valid feature name, not that the feature itself is
	 * available. whether or not the feature is available ca
	 * be seen by looking at the data 
     */
	bool Supported;
	wxString TransactionId;
	wxString Data;

	DbgpFeatureGetEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * Response to a feature_set command. Was the feature set?
 * See Section 7.2.3
 */
class DbgpFeatureSetEventClass : public wxEvent {

public:

	wxString Command;
	t4p::DbgpFeatures Feature;
	bool Success;
	wxString TransactionId;
	
	DbgpFeatureSetEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * Response to the debugger being told to resume running the script.
 * See Section 7.5
 */
enum DbgpContinuations {
	DBGP_CONTINUE_UNKNOWN,
	DBGP_CONTINUE_RUN,
	DBGP_CONTINUE_STEP_INTO,
	DBGP_CONTINUE_STEP_OVER,
	DBGP_CONTINUE_STEP_OUT,
	DBGP_CONTINUE_STOP,
};

class DbgpContinueEventClass : public wxEvent {

public:

	wxString Command;
	t4p::DbgpStatus Status;
	t4p::DbgpReason Reason;
	wxString TransactionId;
	
	DbgpContinueEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * Response to a breakpoint being set
 * See Section 7.6.1
 */
class DbgpBreakpointSetEventClass : public wxEvent {

public:

	wxString Command;
	wxString TransactionId;
	bool Enabled;
	wxString BreakpointId;
	
	DbgpBreakpointSetEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

class DbgpBreakpointClass {

public:

	wxString BreakpointId;
	wxString BreakpointType;
	bool IsEnabled;
	wxString Filename;

	/**
	 * 1-based
	 */
	int LineNumber;
	wxString Function;
	wxString Exception;
	int HitValue;
	wxString HitCondition;
	int HitCount;
	wxString Expression;

	DbgpBreakpointClass();

	DbgpBreakpointClass(const t4p::DbgpBreakpointClass& src);

	t4p::DbgpBreakpointClass& operator=(const t4p::DbgpBreakpointClass& src);

	void Copy(const t4p::DbgpBreakpointClass& src);
};

/**
 * Response to a breakpoint query
 * See Section 7.6.2
 */
class DbgpBreakpointGetEventClass : public wxEvent {

public:

	t4p::DbgpBreakpointClass Breakpoint;
	
	DbgpBreakpointGetEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * Response to a breakpoint being updated
 * See Section 7.6.3 
 */
class DbgpBreakpointUpdateEventClass : public wxEvent {

public:
	
	wxString Command;
	wxString TransactionId;

	DbgpBreakpointUpdateEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * Response to a breakpoint being removed
 * See Section 7.6.4
 */
class DbgpBreakpointRemoveEventClass : public wxEvent {

public:
	
	wxString Command;
	wxString TransactionId;

	DbgpBreakpointRemoveEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * Response to a breakpoint list 
 * See Section 7.6.5 
 */
class DbgpBreakpointListEventClass : public wxEvent {

public:
	
	wxString Command;
	wxString TransactionId;
	std::vector<t4p::DbgpBreakpointClass> Breakpoints;

	DbgpBreakpointListEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * Response to stack depth
 * See Section 7.7
 */
class DbgpStackDepthEventClass : public wxEvent {

public:

	int Depth;
	
	DbgpStackDepthEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

class DbgpStackClass {

public:

	int Level;
	wxString Type;
	wxString Filename;
	
	/**
	 * 1-based
	 */
	int LineNumber;
	wxString Where;
	wxString CmdBegin;
	wxString CmdEnd;

	DbgpStackClass();

	DbgpStackClass(const t4p::DbgpStackClass& src);

	t4p::DbgpStackClass& operator=(const t4p::DbgpStackClass& src);

	void Copy(const t4p::DbgpStackClass& src);

};

/**
 * Response to stack command.
 * See Section 7.8
 */
class DbgpStackGetEventClass : public wxEvent {

public:

	std::vector<t4p::DbgpStackClass> Stack;

	DbgpStackGetEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * Response to the context_names command. context_names are
 * scopes; either locals, globals, or class
 * See Section 7.9
 */
class DbgpContextNamesEventClass : public wxEvent {

public:

	std::vector<wxString> Names;
	std::vector<int> Ids;
	
	DbgpContextNamesEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * A DbgpProperty is a variable along with its value.
 * See Section 7.11
 */
class DbgpPropertyClass {

public:

	wxString Name;
	wxString FullName;
	wxString DataType;
	wxString Facet;
	wxString ClassName;
	bool Constant;
	bool HasChildren;
	int Size;
	int Page;
	int PageSize;
	int Address;
	wxString Key;
	wxString Encoding;

	// this is all of the children that this property has, counting
	// the ones that the debug engine did not return because of
	// maxChildren feature
	int NumChildren;
	wxString Value;

	// this may not include all of the children of this property
	std::vector<t4p::DbgpPropertyClass> ChildProperties;

	DbgpPropertyClass();

	DbgpPropertyClass(const t4p::DbgpPropertyClass& src);

	t4p::DbgpPropertyClass& operator=(const t4p::DbgpPropertyClass& src);

	void Copy(const t4p::DbgpPropertyClass& src);
};

/**
 * Response to the context_get command, context_get retrieves
 * all of the variables in the context (ie local class or global
 * scope).
 *
 * See Section 7.10
 */
class DbgpContextGetEventClass : public wxEvent {

public:

	std::vector<t4p::DbgpPropertyClass> Properties;
	
	int ContextId;
	
	DbgpContextGetEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * Response to the property_get command, gets a single 
 * variable and its value. Note that this event 
 * will return only up to the max data length (it may not
 * return the entire variable value)
 *
 * See Section  7.13
 */
class DbgpPropertyGetEventClass : public wxEvent {

public:

	wxString Command;
	wxString TransactionId;
	t4p::DbgpPropertyClass Property;
	
	DbgpPropertyGetEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};


/**
 * Response to the property_value command, gets a single 
 * variable and its value. Note that this event 
 * WILL return THE ENTIRE variable value
 *
 * See Section  7.13
 */
class DbgpPropertyValueEventClass : public wxEvent {

public:

	wxString Command;
	wxString TransactionId;
	wxString Value;
	
	DbgpPropertyValueEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};


/**
 * Response to the property_set command
 * See Section  7.13
 */
class DbgpPropertySetEventClass : public wxEvent {

public:

	wxString Command;
	wxString TransactionId;
	bool Success;
	
	DbgpPropertySetEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * Response to a break command;
 * See Section 8.3 
 */
class DbgpBreakEventClass : public wxEvent {

public:

	wxString Command;
	wxString TransactionId;
	bool Success;
	
	DbgpBreakEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * Response to a eval command
 * See Section 8.3.1 
 */
class DbgpEvalEventClass : public wxEvent {

public:

	wxString Command;
	wxString TransactionId;
	t4p::DbgpPropertyClass Property;
	bool Success;
	
	DbgpEvalEventClass();

	bool FromXml(const wxString& xml, t4p::DbgpXmlErrors& error);

	wxEvent* Clone() const;
};

/**
 * this class represents the debugger commands that triumph sends
 * to the debugger engine.
 * The class will take care of properly serializing the command
 * only.
 * See Section 6.3
 */
class DbgpCommandClass {

public:

	DbgpCommandClass();

	/**
	 * Get the debugger engine status
	 * See section 7.1
	 */
	std::string Status();

	/**
	 * Gets the status for a single feature
	 * See section 7.2.2
	 */
	std::string FeatureGet(const wxString& featureName);

	/**
	 * Sets the value for a single feature
	 * See section 7.2.3
	 */
	std::string FeatureSet(const wxString& featureName, const wxString& value);

	/**
	 * Resumes / stops the execution of the script
	 * See section 7.5
	 */
	std::string Run();
	std::string StepInto();
	std::string StepOver();
	std::string StepOut();
	std::string Stop();

	/**
	 * Sets a file breakpoint
	 * See section 7.6
	 */
	std::string BreakpointFile(const wxString& filename, int lineNumber, bool enabled, int hitValue = 0, const wxString& hitCondition = wxEmptyString);

	/**
	 * Sets a call function breakpoint
	 * See section 7.6
	 */
	std::string BreakpointCall(const wxString& function, bool enabled, int hitValue = 0, const wxString& hitCondition = wxEmptyString);

	/**
	 * Sets a return function breakpoint
	 * See section 7.6
	 */
	std::string BreakpointReturn(const wxString& function, bool enabled, int hitValue = 0, const wxString& hitCondition = wxEmptyString);

	/**
	 * Sets an exception breakpoint
	 * See section 7.6
	 */
	std::string BreakpointException(const wxString& exception, bool enabled);

	/**
	 * Sets a conditonal (file) breakpoint
	 * See section 7.6
	 */
	std::string BreakpointConditional(const wxString& filename, int lineNumber, const wxString& expression, bool enabled);

	/**
	 * Sets a watch breakpoint
	 * See section 7.6.1
	 */
	std::string BreakpointWatch(const wxString& expression, bool enabled);

	/**
	 * Sets a temporary breakpoint
	 * See section 7.6.1
	 */
	std::string BreakpointRunToCursor(const wxString& filename, int lineNumber);

	/**
	 * gets breakpoint info
	 * See section 7.6.2
	 */
	std::string BreakpointGet(const wxString& breakpointId);

	/**
	 * disable a breakpoint
	 * See section 7.6.3
	 */
	std::string BreakpointDisable(const wxString& breakpointId);

	/**
	 * enables a breakpoint
	 * See section 7.6.3
	 */
	std::string BreakpointEnable(const wxString& breakpointId);

	/**
	 * removes a breakpoint 
	 * See section 7.6.4
	 */
	std::string BreakpointRemove(const wxString& breakpointId);

	/**
	 * gets all of the breakpoints
	 * See section 7.6.5
	 */
	std::string BreakpointList();

	/**
	 * get the stack depth
	 * See section 7.7
	 */
	std::string StackDepth();

	/**
	 * get the stack
	 * See section 7.8
	 */
	std::string StackGet(int stackDepth);

	/**
	 * get the context name
	 * See section 7.9
	 */
	std::string ContextNames(int stackDepth);

	/**
	 * get all properties in a context
	 * See section 7.10
	 */
	std::string ContextGet(int stackDepth, int contextId);

	/**
	 * get a single property (restricted to a max size)
	 * See section 7.13
	 */
	std::string PropertyGet(int stackDepth, int contextId, const wxString& propertyFullName, const wxString& propertyKey);

	/**
	 * set a single property
	 * See section 7.13
	 */
	std::string PropertySet(int stackDepth, int contextId, const wxString& propertyFullName, const wxString& dataType, const wxString& value);

	/**
	 * get a single property (unrestricted size)
	 * See section 7.1
	 */
	std::string PropertyValue(int stackDepth, int contextId, const wxString& propertyFullName, const wxString& propertyKey);

	/**
	 * interrupt the debugger while the engine is running  
	 * See section 8.2
	 */
	std::string Break();

	/**
	 * evaluates the given expression in the current context.
	 * See section 8.3
	 */
	std::string Eval(const wxString& expression);	

	/**
	 * @return the transaction Id that will be used for the
	 *         next command.
	 */
	std::string CurrentTransactionId() const;

private:

	// all three are used to produce a unique transaction id
	int TransactionId;
	int Pid;
	wxString MachineName;

	std::string Build(const std::string& cmd, const wxString& args, const wxString& data = wxEmptyString);

	wxString EscapeArg(const wxString& arg);
};

/**
 * the code below is needed to make use of the wxWidgets event
 * propagation system (event tables)
 */ 

extern const wxEventType EVENT_DBGP_INIT;
extern const wxEventType EVENT_DBGP_ERROR;
extern const wxEventType EVENT_DBGP_STATUS;
extern const wxEventType EVENT_DBGP_FEATUREGET;
extern const wxEventType EVENT_DBGP_FEATURESET;
extern const wxEventType EVENT_DBGP_CONTINUE;
extern const wxEventType EVENT_DBGP_BREAKPOINTSET;
extern const wxEventType EVENT_DBGP_BREAKPOINTGET;
extern const wxEventType EVENT_DBGP_BREAKPOINTUPDATE;
extern const wxEventType EVENT_DBGP_BREAKPOINTREMOVE;
extern const wxEventType EVENT_DBGP_BREAKPOINTLIST;
extern const wxEventType EVENT_DBGP_STACKDEPTH;
extern const wxEventType EVENT_DBGP_STACKGET;
extern const wxEventType EVENT_DBGP_CONTEXTNAMES;
extern const wxEventType EVENT_DBGP_CONTEXTGET;
extern const wxEventType EVENT_DBGP_PROPERTYGET;
extern const wxEventType EVENT_DBGP_PROPERTYVALUE;
extern const wxEventType EVENT_DBGP_PROPERTYSET;
extern const wxEventType EVENT_DBGP_BREAK;
extern const wxEventType EVENT_DBGP_EVAL;


typedef void (wxEvtHandler::*DbgpInitEventClassFunction)(t4p::DbgpInitEventClass&);

#define EVT_DBGP_INIT(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_INIT, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpInitEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpErrorEventClassFunction)(t4p::DbgpErrorEventClass&);

#define EVT_DBGP_ERROR(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_ERROR, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpErrorEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpStatusEventClassFunction)(t4p::DbgpStatusEventClass&);

#define EVT_DBGP_STATUS(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_STATUS, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpStatusEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpFeatureGetEventClassFunction)(t4p::DbgpFeatureGetEventClass&);

#define EVT_DBGP_FEATUREGET(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_FEATUREGET, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpFeatureGetEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpFeatureSetEventClassFunction)(t4p::DbgpFeatureSetEventClass&);

#define EVT_DBGP_FEATURESET(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_FEATURESET, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpFeatureSetEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpContinueEventClassFunction)(t4p::DbgpContinueEventClass&);

#define EVT_DBGP_CONTINUE(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_CONTINUE, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpContinueEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpBreakpointSetEventClassFunction)(t4p::DbgpBreakpointSetEventClass&);

#define EVT_DBGP_BREAKPOINTSET(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_BREAKPOINTSET, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpBreakpointSetEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpBreakpointGetEventClassFunction)(t4p::DbgpBreakpointGetEventClass&);

#define EVT_DBGP_BREAKPOINTGET(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_BREAKPOINTGET, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpBreakpointGetEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpBreakpointUpdateEventClassFunction)(t4p::DbgpBreakpointUpdateEventClass&);

#define EVT_DBGP_BREAKPOINTUPDATE(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_BREAKPOINTUPDATE, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpBreakpointUpdateEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpBreakpointRemoveEventClassFunction)(t4p::DbgpBreakpointRemoveEventClass&);

#define EVT_DBGP_BREAKPOINTREMOVE(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_BREAKPOINTREMOVE, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpBreakpointRemoveEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpBreakpointListEventClassFunction)(t4p::DbgpBreakpointListEventClass&);

#define EVT_DBGP_BREAKPOINTLIST(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_BREAKPOINTLIST, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpBreakpointListEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpStackDepthEventClassFunction)(t4p::DbgpStackDepthEventClass&);

#define EVT_DBGP_STACKDEPTH(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_STACKDEPTH, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpStackDepthEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpStackGetEventClassFunction)(t4p::DbgpStackGetEventClass&);

#define EVT_DBGP_STACKGET(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_STACKGET, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpStackGetEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpContextNamesEventClassFunction)(t4p::DbgpContextNamesEventClass&);

#define EVT_DBGP_CONTEXTNAMES(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_CONTEXTNAMES, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpContextNamesEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpContextGetEventClassFunction)(t4p::DbgpContextGetEventClass&);

#define EVT_DBGP_CONTEXTGET(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_CONTEXTGET, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpContextGetEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpPropertyGetEventClassFunction)(t4p::DbgpPropertyGetEventClass&);

#define EVT_DBGP_PROPERTYGET(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_PROPERTYGET, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpPropertyGetEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpPropertyValueEventClassFunction)(t4p::DbgpPropertyValueEventClass&);

#define EVT_DBGP_PROPERTYVALUE(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_PROPERTYVALUE, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpPropertyValueEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpPropertySetEventClassFunction)(t4p::DbgpPropertySetEventClass&);

#define EVT_DBGP_PROPERTYSET(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_PROPERTYSET, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpPropertySetEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpBreakEventClassFunction)(t4p::DbgpBreakEventClass&);

#define EVT_DBGP_BREAK(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_BREAK, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpBreakEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*DbgpEvalEventClassFunction)(t4p::DbgpEvalEventClass&);

#define EVT_DBGP_EVAL(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DBGP_EVAL, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( DbgpEvalEventClassFunction, & fn ), (wxObject *) NULL ),


}

#endif
