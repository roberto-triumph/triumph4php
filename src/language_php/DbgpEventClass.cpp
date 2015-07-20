/**
 * @copyright  2014 Roberto Perpuly
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
#include <language_php/DbgpEventClass.h>
#include <globals/String.h>
#include <wx/xml/xml.h>
#include <wx/sstream.h>
#include <wx/base64.h>
#include <string>

/**
 * @param document the parsed xml
 * @param xmlString the xml to be parsed
 * @param rootName the name that the root tag should have
 * @return bool TRUE if
 *         xmlString is well-formed xml AND it's root tag
 *         has the given name.
 */
static bool EnsureXmlRoot(wxXmlDocument& document, const wxString& xmlString, const wxString& rootName, t4p::DbgpXmlErrors& error) {
	error = t4p::DBGP_XML_ERROR_NONE;

	// not sure if the encoding that we get from xdebug feature get command
	// is compatible with system (wxWidgets) encodings, for now use
	// as the default (utf-8)
	bool valid = false;
	wxStringInputStream is(xmlString);
	if (document.Load(is)) {
		wxXmlNode* root = document.GetRoot();
		valid = root->GetName().CmpNoCase(rootName) == 0;
		if (!valid) {
			error = t4p::DBGP_XML_ERROR_TAG;
		}
	} else {
		error = t4p::DBGP_XML_ERROR_PARSE;
	}
	return valid;
}

/**
 * @param node the XML node to search in
 * @param attributeName the attribute to search for
 * @param value the attribute's value is written to this variable
 * @param error reason for failure will be written to this variable
 * @return bool TRUE if the attribute exists
 */
static bool GetNodeAttributeString(wxXmlNode* node, const wxString& attributeName, wxString& value, t4p::DbgpXmlErrors& error) {
	if (!node->GetAttribute(attributeName, &value)) {
		error = t4p::DBGP_XML_ERROR_ATTRIBUTE;
		return false;
	}
	return true;
}

/**
 * @param node the XML node to search in
 * @param attributeName the attribute to search for
 * @param value the attribute's value is written to this variable. if attribute does
 *        not exist or is not a number, or is less than or equal to zero then 0 will be written
 */
static void GetNodeAttributeBool(wxXmlNode* node, const wxString& attributeName, bool& value) {
	value = false;
	wxString str;
	t4p::DbgpXmlErrors error = t4p::DBGP_XML_ERROR_NONE;
	if (GetNodeAttributeString(node, attributeName, str, error)) {
		long lval = 0;
		if (str.ToLong(&lval) && lval > 0) {
			value = true;
		}
	}
}

/**
 * @param node the XML node to search in
 * @param attributeName the attribute to search for
 * @param value the attribute's value is written to this variable. if attribute does
 *        not exist or is not a number then 0 will be written
 */
static void GetNodeAttributeInt(wxXmlNode* node, const wxString& attributeName, int& value) {
	wxString str;
	value = 0;
	t4p::DbgpXmlErrors error = t4p::DBGP_XML_ERROR_NONE;
	if (GetNodeAttributeString(node, attributeName, str, error)) {
		long lval = 0;
		if (str.ToLong(&lval) && lval > 0) {
			value = static_cast<int>(lval);
		}
	}
}

/**
 * @param node the parent node to search in
 * @param childName the tag name to look for
 * @param error reason for failure will be written to this variable
 * @return the child node, NULL if there is no tag with the given name
 */
static wxXmlNode* GetNodeChild(wxXmlNode* node, const wxString& childName, t4p::DbgpXmlErrors& error) {
	wxXmlNode* child = node->GetChildren();
	while (child) {
		if (child->GetName().CmpNoCase(childName) == 0) {
			return child;
		}
		child = child->GetNext();
	}
	error = t4p::DBGP_XML_ERROR_TAG;
	return child;
}

/**
 * @param node the parent node to search in
 * @param contents the node's contents will be written to this variable
 */
static void GetNodeText(wxXmlNode* node, wxString& contents, bool doBase64Decode = true) {
	// dbgp base64 encodes the contents, we must decode it
	// TODO(roberto): what about when there are NULLs we would like to
	// show them
	contents = node->GetNodeContent();
	if (doBase64Decode) {
		wxMemoryBuffer buf = wxBase64Decode(contents);
		wxString decoded((const char*)buf.GetData(), buf.GetDataLen());
		contents = decoded;
	}
}

static t4p::DbgpStatus StatusFromString(const wxString& str) {
	wxString lower = str.Lower();
	if (str == wxT("starting")) {
		return t4p::DBGP_STATUS_STARTING;
	} else if (str == wxT("stopping")) {
		return t4p::DBGP_STATUS_STOPPING;
	} else if (str == wxT("stopped")) {
		return t4p::DBGP_STATUS_STOPPED;
	} else if (str == wxT("running")) {
		return t4p::DBGP_STATUS_RUNNING;
	} else if (str == wxT("break")) {
		return t4p::DBGP_STATUS_BREAK;
	}
	return t4p::DBGP_STATUS_UNKNOWN;
}

static t4p::DbgpReason ReasonFromString(const wxString& str) {
	wxString lower = str.Lower();
	if (str == wxT("ok")) {
		return t4p::DBGP_REASON_OK;
	} else if (str == wxT("aborted")) {
		return t4p::DBGP_REASON_ABORTED;
	} else if (str == wxT("error")) {
		return t4p::DBGP_REASON_ERROR;
	} else if (str == wxT("exception")) {
		return t4p::DBGP_REASON_EXCEPTION;
	}
	return t4p::DBGP_REASON_UNKNOWN;
}

static t4p::DbgpFeatures FeatureFromString(const wxString& str) {
	wxString lower = str.Lower();
	if (str == wxT("language_supports_threads")) {
		return t4p::DBGP_FEATURE_LANGUAGE_SUPPORTS_THREADS;
	} else if (str == wxT("language_name")) {
		return t4p::DBGP_FEATURE_LANGUAGE_NAME;
	} else if (str == wxT("language_version")) {
		return t4p::DBGP_FEATURE_LANGUAGE_VERSION;
	} else if (str == wxT("encoding")) {
		return t4p::DBGP_FEATURE_ENCODING;
	} else if (str == wxT("protcol_version")) {
		return t4p::DBGP_FEATURE_PROTOCOL_VERSION;
	} else if (str == wxT("data_encoding")) {
		return t4p::DBGP_FEATURE_DATA_ENCODING;
	} else if (str == wxT("breakpoint_languages")) {
		return t4p::DBGP_FEATURE_BREAKPOINT_LANGUAGES;
	} else if (str == wxT("breakpoint_types")) {
		return t4p::DBGP_FEATURE_BREAKPOINT_TYPES;
	} else if (str == wxT("multiple_sessions")) {
		return t4p::DBGP_FEATURE_MULTIPLE_SESSIONS;
	} else if (str == wxT("max_children")) {
		return t4p::DBGP_FEATURE_MAX_CHILDREN;
	} else if (str == wxT("max_data")) {
		return t4p::DBGP_FEATURE_MAX_DATA;
	} else if (str == wxT("max_depth")) {
		return t4p::DBGP_FEATURE_MAX_DEPTH;
	}
	return t4p::DBGP_FEATURE_UNKNOWN;
}

static bool BreakpointFromXmlNode(wxXmlNode* breakpointNode, t4p::DbgpBreakpointClass& breakpoint, t4p::DbgpXmlErrors& error) {
	if (!GetNodeAttributeString(breakpointNode, "id", breakpoint.BreakpointId, error)) {
		return false;
	}
	if (!GetNodeAttributeString(breakpointNode, "type", breakpoint.BreakpointType, error)) {
		return false;
	}
	wxString stateStr;
	if (!GetNodeAttributeString(breakpointNode, "state", stateStr, error)) {
		return false;
	}
	breakpoint.IsEnabled = stateStr.CmpNoCase(wxT("enabled")) == 0;

	t4p::DbgpXmlErrors ignoredError;

	// filename, function, expression, line number are optional because
	// they depend on the breakpoint type
	GetNodeAttributeString(breakpointNode, "filename", breakpoint.Filename, ignoredError);
	GetNodeAttributeInt(breakpointNode, "lineno", breakpoint.LineNumber);
	GetNodeAttributeString(breakpointNode, "function", breakpoint.Function, ignoredError);
	GetNodeAttributeString(breakpointNode, "exception", breakpoint.Exception, ignoredError);

	GetNodeAttributeInt(breakpointNode, "hit_value", breakpoint.HitValue);
	GetNodeAttributeInt(breakpointNode, "hit_count", breakpoint.HitCount);
	if (!GetNodeAttributeString(breakpointNode, "hit_condition", breakpoint.HitCondition, error)) {
		return false;
	}
	wxXmlNode* exprNode = GetNodeChild(breakpointNode, "expression", ignoredError);
	if (exprNode) {
		GetNodeText(exprNode, breakpoint.Expression);
	}
	return true;
}

static bool StackFromXmlNode(wxXmlNode* stackNode, t4p::DbgpStackClass& stack, t4p::DbgpXmlErrors& error) {
	GetNodeAttributeInt(stackNode, "level", stack.Level);

	if (!GetNodeAttributeString(stackNode, "type", stack.Type, error)) {
		return false;
	}
	if (!GetNodeAttributeString(stackNode, "filename", stack.Filename, error)) {
		return false;
	}
	GetNodeAttributeInt(stackNode, "lineno", stack.LineNumber);

	t4p::DbgpXmlErrors ignoredError;

	// these are optional
	GetNodeAttributeString(stackNode, "where", stack.Where, ignoredError);
	GetNodeAttributeString(stackNode, "cmdbegin", stack.CmdBegin, ignoredError);
	GetNodeAttributeString(stackNode, "cmdend", stack.CmdEnd, ignoredError);

	return true;
}

static bool PropertyFromXmlNode(wxXmlNode* node, t4p::DbgpPropertyClass& prop, t4p::DbgpXmlErrors& error) {
	// name is optional when we parse properties from an eval response
	// class name, full name are optional
	t4p::DbgpXmlErrors ignored;

	if (!GetNodeAttributeString(node, "type", prop.DataType, error)) {
		return false;
	}
	GetNodeAttributeString(node, "name", prop.Name, ignored);
	GetNodeAttributeString(node, "fullname", prop.FullName, ignored);
	GetNodeAttributeString(node, "classname", prop.ClassName, ignored);

	GetNodeAttributeInt(node, "page", prop.Page);
	GetNodeAttributeInt(node, "pagesize", prop.PageSize);

	// more optional response items
	GetNodeAttributeString(node, "facet", prop.Facet, ignored);
	GetNodeAttributeInt(node, "size", prop.Size);
	GetNodeAttributeBool(node, "children", prop.HasChildren);
	GetNodeAttributeInt(node, "numchildren", prop.NumChildren);
	GetNodeAttributeString(node, "key", prop.Key, ignored);
	GetNodeAttributeInt(node, "address", prop.Address);
	GetNodeAttributeString(node, "encoding", prop.Encoding, ignored);

	wxString value;
	GetNodeText(node, value, "base64" == prop.Encoding);
	prop.Value = value;

	// check to see if this property has child properties
	wxXmlNode* child = node->GetChildren();
	while (child) {
		if (child->GetName().CmpNoCase("property") == 0) {
			t4p::DbgpPropertyClass childProp;
			if (!PropertyFromXmlNode(child, childProp, error)) {
				return false;
			}
			prop.ChildProperties.push_back(childProp);
		}
		child = child->GetNext();
	}

	return true;
}

t4p::DbgpBreakpointClass::DbgpBreakpointClass()
: BreakpointId()
, BreakpointType()
, IsEnabled()
, Filename()
, LineNumber()
, Function()
, Exception()
, HitValue(0)
, HitCondition()
, HitCount(0)
, Expression() {
}

t4p::DbgpBreakpointClass::DbgpBreakpointClass(const t4p::DbgpBreakpointClass& src)
: BreakpointId()
, BreakpointType()
, IsEnabled()
, Filename()
, LineNumber()
, Function()
, Exception()
, HitValue()
, HitCondition()
, HitCount()
, Expression() {
	Copy(src);
}

t4p::DbgpBreakpointClass& t4p::DbgpBreakpointClass::operator=(const t4p::DbgpBreakpointClass& src) {
	Copy(src);
	return *this;
}

void t4p::DbgpBreakpointClass::Copy(const t4p::DbgpBreakpointClass& src) {
	BreakpointId = src.BreakpointId.c_str();
	BreakpointType = src.BreakpointType.c_str();
	IsEnabled = src.IsEnabled;
	Filename = src.Filename.c_str();
	LineNumber = src.LineNumber;
	Function = src.Function.c_str();
	Exception = src.Exception.c_str();
	HitValue = src.HitValue;
	HitCondition = src.HitCondition.c_str();
	HitCount = src.HitCount;
	Expression = src.Expression.c_str();
}

t4p::DbgpStackClass::DbgpStackClass()
: Level()
, Type()
, Filename()
, LineNumber()
, Where()
, CmdBegin()
, CmdEnd() {
}

t4p::DbgpStackClass::DbgpStackClass(const t4p::DbgpStackClass& src)
: Level()
, Type()
, Filename()
, LineNumber()
, Where()
, CmdBegin()
, CmdEnd() {
	Copy(src);
}

t4p::DbgpStackClass& t4p::DbgpStackClass::operator=(const t4p::DbgpStackClass& src) {
	Copy(src);
	return *this;
}

void t4p::DbgpStackClass::Copy(const t4p::DbgpStackClass& src) {
	Level = src.Level;
	Type = src.Type.c_str();
	Filename = src.Filename.c_str();
	LineNumber = src.LineNumber;
	Where = src.Where.c_str();
	CmdBegin = src.CmdBegin.c_str();
	CmdEnd = src.CmdEnd.c_str();
}

t4p::DbgpPropertyClass::DbgpPropertyClass()
: Name()
, FullName()
, DataType()
, Facet()
, ClassName()
, Constant()
, HasChildren()
, Size()
, Page()
, PageSize()
, Address()
, Key()
, Encoding()
, NumChildren()
, Value()
, ChildProperties() {
}

t4p::DbgpPropertyClass::DbgpPropertyClass(const t4p::DbgpPropertyClass& src)
: Name()
, FullName()
, DataType()
, Facet()
, ClassName()
, Constant()
, HasChildren()
, Size()
, Page()
, PageSize()
, Address()
, Key()
, Encoding()
, NumChildren()
, Value()
, ChildProperties() {
	Copy(src);
}

t4p::DbgpPropertyClass& t4p::DbgpPropertyClass::operator=(const t4p::DbgpPropertyClass& src) {
	Copy(src);
	return *this;
}

void t4p::DbgpPropertyClass::Copy(const t4p::DbgpPropertyClass& src) {
	Name = src.Name.c_str();
	FullName = src.FullName.c_str();
	DataType = src.DataType.c_str();
	Facet = src.Facet.c_str();
	ClassName = src.ClassName.c_str();
	Constant = src.Constant;
	HasChildren = src.HasChildren;
	Size = src.Size;
	Page = src.Page;
	PageSize = src.PageSize;
	Address = src.Address;
	Key = src.Key.c_str();
	Encoding = src.Encoding.c_str();
	NumChildren = src.NumChildren;
	Value = src.Value.c_str();
	ChildProperties = src.ChildProperties;
}

const wxEventType t4p::EVENT_DBGP_INIT = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_ERROR = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_STATUS = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_FEATUREGET = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_FEATURESET = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_CONTINUE = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_BREAKPOINTSET = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_BREAKPOINTGET = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_BREAKPOINTUPDATE = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_BREAKPOINTREMOVE = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_BREAKPOINTLIST = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_STACKDEPTH = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_STACKGET = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_CONTEXTNAMES = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_CONTEXTGET = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_PROPERTYGET = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_PROPERTYVALUE = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_PROPERTYSET = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_BREAK = wxNewEventType();
const wxEventType t4p::EVENT_DBGP_EVAL = wxNewEventType();


// ---------------
// events start
// ---------------
t4p::DbgpInitEventClass::DbgpInitEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_INIT)
, AppId()
, IdeKey()
, Session()
, Thread()
, Parent()
, Language()
, ProtocolVersion()
, FileUri() {
}

wxEvent* t4p::DbgpInitEventClass::Clone() const {
	t4p::DbgpInitEventClass* cloned = new t4p::DbgpInitEventClass();
	cloned->AppId = AppId.c_str();
	cloned->IdeKey = IdeKey.c_str();
	cloned->Session = Session.c_str();
	cloned->Thread = Thread.c_str();
	cloned->Parent = Parent.c_str();
	cloned->Language = Language.c_str();
	cloned->ProtocolVersion = ProtocolVersion.c_str();
	cloned->FileUri = FileUri.c_str();
	return cloned;
}

bool t4p::DbgpInitEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("init"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	if (!GetNodeAttributeString(root, "appid", AppId, error)) {
		return false;
	}
	if (!GetNodeAttributeString(root, "idekey", IdeKey, error)) {
		return false;
	}

	// optional
	t4p::DbgpXmlErrors ignoredError;
	GetNodeAttributeString(root, "session", Session, ignoredError);
	GetNodeAttributeString(root, "thread", Thread, error);
	GetNodeAttributeString(root, "parent", Parent, error);


	if (!GetNodeAttributeString(root, "language", Language, error)) {
		return false;
	}
	if (!GetNodeAttributeString(root, "protocol_version", ProtocolVersion, error)) {
		return false;
	}
	if (!GetNodeAttributeString(root, "fileuri", FileUri, error)) {
		return false;
	}
	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpErrorEventClass::DbgpErrorEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_ERROR)
, Command()
, TransactionId()
, ErrorCode()
, AppErrorCode()
, Message() {
}

wxEvent* t4p::DbgpErrorEventClass::Clone() const {
	t4p::DbgpErrorEventClass* cloned = new t4p::DbgpErrorEventClass();
	cloned->Command = Command.c_str();
	cloned->TransactionId = TransactionId.c_str();
	cloned->ErrorCode = ErrorCode;
	cloned->AppErrorCode = AppErrorCode.c_str();
	cloned->Message = Message.c_str();
	return cloned;
}

bool t4p::DbgpErrorEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();

	if (!GetNodeAttributeString(root, "command", Command, error)) {
		return false;
	}
	if (!GetNodeAttributeString(root, "transaction_id", TransactionId, error)) {
		return false;
	}

	wxXmlNode* errorNode = GetNodeChild(root, "error", error);
	if (!errorNode) {
		return false;
	}

	GetNodeAttributeInt(errorNode, "code", ErrorCode);

	wxXmlNode* messageNode = GetNodeChild(errorNode, "message", error);
	if (!messageNode) {
		return false;
	}
	GetNodeText(messageNode, Message, false);

	t4p::DbgpXmlErrors ignored;
	GetNodeAttributeString(errorNode, "apperr", AppErrorCode, ignored);


	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpStatusEventClass::DbgpStatusEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_STATUS)
, Status(t4p::DBGP_STATUS_UNKNOWN)
, Reason(t4p::DBGP_REASON_UNKNOWN)
, TransactionId()
, MessageData() {
}

wxEvent* t4p::DbgpStatusEventClass::Clone() const {
	t4p::DbgpStatusEventClass* cloned = new t4p::DbgpStatusEventClass();
	cloned->Status = Status;
	cloned->Reason = Reason;
	cloned->TransactionId = TransactionId.c_str();
	cloned->MessageData = MessageData.c_str();
	return cloned;
}

bool t4p::DbgpStatusEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	wxString statusStr;
	if (!GetNodeAttributeString(root, "status", statusStr, error)) {
		return false;
	}
	Status = StatusFromString(statusStr);

	wxString reasonStr;
	if (!GetNodeAttributeString(root, "reason", reasonStr, error)) {
		return false;
	}
	Reason = ReasonFromString(reasonStr);

	if (!GetNodeAttributeString(root, "transaction_id", TransactionId, error)) {
		return false;
	}
	if (!GetNodeAttributeString(root, "message_data", MessageData, error)) {
		return false;
	}
	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpFeatureGetEventClass::DbgpFeatureGetEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_FEATUREGET)
, Command()
, Feature(t4p::DBGP_FEATURE_UNKNOWN)
, Supported()
, TransactionId()
, Data() {
}

wxEvent* t4p::DbgpFeatureGetEventClass::Clone() const {
	t4p::DbgpFeatureGetEventClass* cloned = new t4p::DbgpFeatureGetEventClass();
	cloned->Command = Command.c_str();
	cloned->Feature = Feature;
	cloned->Supported = Supported;
	cloned->TransactionId = TransactionId.c_str();
	cloned->Data = Data.c_str();
	return cloned;
}

bool t4p::DbgpFeatureGetEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	if (!GetNodeAttributeString(root, "command", Command, error)) {
		return false;
	}
	wxString featureStr;
	if (!GetNodeAttributeString(root, "feature_name", featureStr, error)) {
		return false;
	}
	Feature = FeatureFromString(featureStr);

	GetNodeAttributeBool(root, "supported", Supported);
	if (!GetNodeAttributeString(root, "transaction_id", TransactionId, error)) {
		return false;
	}
	GetNodeText(root, Data);

	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpFeatureSetEventClass::DbgpFeatureSetEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_FEATURESET)
, Command()
, Feature(t4p::DBGP_FEATURE_UNKNOWN)
, Success()
, TransactionId() {
}

wxEvent* t4p::DbgpFeatureSetEventClass::Clone() const {
	t4p::DbgpFeatureSetEventClass* cloned = new t4p::DbgpFeatureSetEventClass();
	cloned->Command = Command.c_str();
	cloned->Feature = Feature;
	cloned->Success = Success;
	cloned->TransactionId = TransactionId.c_str();
	return cloned;
}

bool t4p::DbgpFeatureSetEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	if (!GetNodeAttributeString(root, "command", Command, error)) {
		return false;
	}
	GetNodeAttributeBool(root, "success", Success);
	if (!GetNodeAttributeString(root, "transaction_id", TransactionId, error)) {
		return false;
	}
	wxString featureStr;
	if (!GetNodeAttributeString(root, "feature_name", featureStr, error)) {
		return false;
	}
	Feature = FeatureFromString(featureStr);

	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpContinueEventClass::DbgpContinueEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_CONTINUE)
, Command()
, Status(t4p::DBGP_STATUS_UNKNOWN)
, Reason(t4p::DBGP_REASON_UNKNOWN)
, TransactionId() {
}

wxEvent* t4p::DbgpContinueEventClass::Clone() const {
	t4p::DbgpContinueEventClass* cloned = new t4p::DbgpContinueEventClass();
	cloned->Command = Command.c_str();
	cloned->Status = Status;
	cloned->Reason = Reason;
	cloned->TransactionId = TransactionId.c_str();
	return cloned;
}

bool t4p::DbgpContinueEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	if (!GetNodeAttributeString(root, "command", Command, error)) {
		return false;
	}
	wxString statusStr;
	if (!GetNodeAttributeString(root, "status", statusStr, error)) {
		return false;
	}
	Status = StatusFromString(statusStr);

	wxString reasonStr;
	if (!GetNodeAttributeString(root, "reason", reasonStr, error)) {
		return false;
	}
	Reason = ReasonFromString(reasonStr);

	if (!GetNodeAttributeString(root, "transaction_id", TransactionId, error)) {
		return false;
	}
	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpBreakpointSetEventClass::DbgpBreakpointSetEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_BREAKPOINTSET)
, Command()
, TransactionId()
, Enabled()
, BreakpointId() {
}

wxEvent* t4p::DbgpBreakpointSetEventClass::Clone() const {
	t4p::DbgpBreakpointSetEventClass* cloned = new t4p::DbgpBreakpointSetEventClass();
	cloned->Command = Command.c_str();
	cloned->TransactionId = TransactionId.c_str();
	cloned->Enabled = Enabled;
	cloned->BreakpointId = BreakpointId.c_str();
	return cloned;
}

bool t4p::DbgpBreakpointSetEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	if (!GetNodeAttributeString(root, "command", Command, error)) {
		return false;
	}
	if (!GetNodeAttributeString(root, "transaction_id", TransactionId, error)) {
		return false;
	}
	wxString stateStr;
	if (!GetNodeAttributeString(root, "state", stateStr, error)) {
		return false;
	}
	Enabled = stateStr.CmpNoCase(wxT("enabled")) == 0;

	if (!GetNodeAttributeString(root, "id", BreakpointId, error)) {
		return false;
	}
	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpBreakpointGetEventClass::DbgpBreakpointGetEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_BREAKPOINTGET)
, Breakpoint() {
}

wxEvent* t4p::DbgpBreakpointGetEventClass::Clone() const {
	t4p::DbgpBreakpointGetEventClass* cloned = new t4p::DbgpBreakpointGetEventClass();
	cloned->Breakpoint = Breakpoint;
	return cloned;
}

bool t4p::DbgpBreakpointGetEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}

	wxXmlNode* root = doc.GetRoot();
	wxXmlNode* breakpointNode = GetNodeChild(root, wxT("breakpoint"), error);
	if (!breakpointNode) {
		return false;
	}
	if (!BreakpointFromXmlNode(breakpointNode, Breakpoint, error)) {
		return false;
	}
	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpBreakpointUpdateEventClass::DbgpBreakpointUpdateEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_BREAKPOINTUPDATE)
, Command()
, TransactionId() {
}

wxEvent* t4p::DbgpBreakpointUpdateEventClass::Clone() const {
	t4p::DbgpBreakpointUpdateEventClass* cloned = new t4p::DbgpBreakpointUpdateEventClass();
	cloned->Command = Command.c_str();
	cloned->TransactionId = TransactionId.c_str();
	return cloned;
}

bool t4p::DbgpBreakpointUpdateEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	if (!GetNodeAttributeString(root, "command", Command, error)) {
		return false;
	}
	if (!GetNodeAttributeString(root, "transaction_id", TransactionId, error)) {
		return false;
	}
	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpBreakpointRemoveEventClass::DbgpBreakpointRemoveEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_BREAKPOINTREMOVE)
, Command()
, TransactionId() {
}

wxEvent* t4p::DbgpBreakpointRemoveEventClass::Clone() const {
	t4p::DbgpBreakpointRemoveEventClass* cloned = new t4p::DbgpBreakpointRemoveEventClass();
	cloned->Command = Command.c_str();
	cloned->TransactionId = TransactionId.c_str();
	return cloned;
}

bool t4p::DbgpBreakpointRemoveEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	if (!GetNodeAttributeString(root, "command", Command, error)) {
		return false;
	}
	if (!GetNodeAttributeString(root, "transaction_id", TransactionId, error)) {
		return false;
	}
	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpBreakpointListEventClass::DbgpBreakpointListEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_BREAKPOINTLIST)
, Command()
, TransactionId()
, Breakpoints() {
}

wxEvent* t4p::DbgpBreakpointListEventClass::Clone() const {
	t4p::DbgpBreakpointListEventClass* cloned = new t4p::DbgpBreakpointListEventClass();
	cloned->Command = Command.c_str();
	cloned->TransactionId = TransactionId.c_str();
	cloned->Breakpoints = Breakpoints;
	return cloned;
}

bool t4p::DbgpBreakpointListEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	if (!GetNodeAttributeString(root, "command", Command, error)) {
		return false;
	}
	if (!GetNodeAttributeString(root, "transaction_id", TransactionId, error)) {
		return false;
	}
	wxXmlNode* child = root->GetChildren();
	while (child) {
		if (child->GetName().CmpNoCase("breakpoint") == 0) {
			t4p::DbgpBreakpointClass breakpoint;
			if (!BreakpointFromXmlNode(child, breakpoint, error)) {
				return false;
			}
			Breakpoints.push_back(breakpoint);
		}
		child = child->GetNext();
	}
	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpStackDepthEventClass::DbgpStackDepthEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_STACKDEPTH)
, Depth() {
}

wxEvent* t4p::DbgpStackDepthEventClass::Clone() const {
	t4p::DbgpStackDepthEventClass* cloned = new t4p::DbgpStackDepthEventClass();
	cloned->Depth = Depth;
	return cloned;
}

bool t4p::DbgpStackDepthEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	GetNodeAttributeInt(root, "depth", Depth);
	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpStackGetEventClass::DbgpStackGetEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_STACKGET)
, Stack() {
}

wxEvent* t4p::DbgpStackGetEventClass::Clone() const {
	t4p::DbgpStackGetEventClass* cloned = new t4p::DbgpStackGetEventClass();
	cloned->Stack = Stack;
	return cloned;
}

bool t4p::DbgpStackGetEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();

	wxXmlNode* child = root->GetChildren();
	while (child) {
		t4p::DbgpStackClass stack;
		if (!StackFromXmlNode(child, stack, error)) {
			return false;
		}
		Stack.push_back(stack);
		child = child->GetNext();
	}

	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpContextNamesEventClass::DbgpContextNamesEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_CONTEXTNAMES)
, Names()
, Ids() {
}

wxEvent* t4p::DbgpContextNamesEventClass::Clone() const {
	t4p::DbgpContextNamesEventClass* cloned = new t4p::DbgpContextNamesEventClass();
	cloned->Names = Names;
	cloned->Ids = Ids;
	return cloned;
}

bool t4p::DbgpContextNamesEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	wxXmlNode* child = root->GetChildren();
	while (child) {
		wxString name;
		if (!GetNodeAttributeString(child, "name", name, error)) {
			return false;
		}
		int id = 0;
		GetNodeAttributeInt(child, "id", id);

		Names.push_back(name);
		Ids.push_back(id);

		child = child->GetNext();
	}
	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpContextGetEventClass::DbgpContextGetEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_CONTEXTGET)
, Properties()
, ContextId() {
}

wxEvent* t4p::DbgpContextGetEventClass::Clone() const {
	t4p::DbgpContextGetEventClass* cloned = new t4p::DbgpContextGetEventClass();
	cloned->Properties = Properties;
	cloned->ContextId = ContextId;
	return cloned;
}

bool t4p::DbgpContextGetEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();

	wxXmlNode* child = root->GetChildren();
	while (child) {
		if (child->GetName().CmpNoCase("property") == 0) {
			t4p::DbgpPropertyClass prop;
			if (!PropertyFromXmlNode(child, prop, error)) {
				return false;
			}
			Properties.push_back(prop);
		}
		child = child->GetNext();
	}

	GetNodeAttributeInt(root, wxT("context"), ContextId);

	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpPropertyGetEventClass::DbgpPropertyGetEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_PROPERTYGET)
, Command()
, TransactionId()
, Property() {
}

wxEvent* t4p::DbgpPropertyGetEventClass::Clone() const {
	t4p::DbgpPropertyGetEventClass* cloned = new t4p::DbgpPropertyGetEventClass();
	cloned->Command = Command.c_str();
	cloned->TransactionId = TransactionId.c_str();
	cloned->Property = Property;
	return cloned;
}

bool t4p::DbgpPropertyGetEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	if (!GetNodeAttributeString(root, "command", Command, error)) {
		return false;
	}
	if (!GetNodeAttributeString(root, "transaction_id", TransactionId, error)) {
		return false;
	}
	wxXmlNode* prop = GetNodeChild(root, "property", error);
	if (!prop) {
		return false;
	}
	if (!PropertyFromXmlNode(prop, Property, error)) {
		return false;
	}

	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpPropertyValueEventClass::DbgpPropertyValueEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_PROPERTYVALUE)
, Command()
, TransactionId()
, Value() {
}

wxEvent* t4p::DbgpPropertyValueEventClass::Clone() const {
	t4p::DbgpPropertyValueEventClass* cloned = new t4p::DbgpPropertyValueEventClass();
	cloned->Command = Command.c_str();
	cloned->TransactionId = TransactionId.c_str();
	cloned->Value = Value;
	return cloned;
}

bool t4p::DbgpPropertyValueEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	if (!GetNodeAttributeString(root, "command", Command, error)) {
		return false;
	}
	if (!GetNodeAttributeString(root, "transaction_id", TransactionId, error)) {
		return false;
	}

	t4p::DbgpXmlErrors ignored;
	wxString encoding;
	GetNodeAttributeString(root, "encoding", encoding, ignored);
	bool doDecode = encoding == "base64";
	GetNodeText(root, Value, doDecode);

	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpPropertySetEventClass::DbgpPropertySetEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_PROPERTYSET)
, Command()
, TransactionId()
, Success() {
}

wxEvent* t4p::DbgpPropertySetEventClass::Clone() const {
	t4p::DbgpPropertySetEventClass* cloned = new t4p::DbgpPropertySetEventClass();
	cloned->Command = Command.c_str();
	cloned->TransactionId = TransactionId.c_str();
	cloned->Success = Success;
	return cloned;
}

bool t4p::DbgpPropertySetEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	if (!GetNodeAttributeString(root, "command", Command, error)) {
		return false;
	}
	if (!GetNodeAttributeString(root, "transaction_id", TransactionId, error)) {
		return false;
	}
	GetNodeAttributeBool(root, "success", Success);
	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpBreakEventClass::DbgpBreakEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_BREAK)
, Command()
, TransactionId()
, Success() {
}

wxEvent* t4p::DbgpBreakEventClass::Clone() const {
	t4p::DbgpBreakEventClass* cloned = new t4p::DbgpBreakEventClass();
	cloned->Command = Command.c_str();
	cloned->TransactionId = TransactionId.c_str();
	cloned->Success = Success;
	return cloned;
}

bool t4p::DbgpBreakEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	if (!GetNodeAttributeString(root, "command", Command, error)) {
		return false;
	}
	if (!GetNodeAttributeString(root, "transaction_id", TransactionId, error)) {
		return false;
	}
	GetNodeAttributeBool(root, "success", Success);
	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}

t4p::DbgpEvalEventClass::DbgpEvalEventClass()
: wxEvent(wxID_ANY, t4p::EVENT_DBGP_EVAL)
, Command()
, TransactionId()
, Property()
, Success() {
}

wxEvent* t4p::DbgpEvalEventClass::Clone() const {
	t4p::DbgpEvalEventClass* cloned = new t4p::DbgpEvalEventClass();
	cloned->Command = Command.c_str();
	cloned->TransactionId = TransactionId.c_str();
	cloned->Success = Success;
	cloned->Property = Property;
	return cloned;
}

bool t4p::DbgpEvalEventClass::FromXml(const wxString& xml, t4p::DbgpXmlErrors& error) {
	wxXmlDocument doc;
	if (!EnsureXmlRoot(doc, xml, wxT("response"), error)) {
		return false;
	}
	wxXmlNode* root = doc.GetRoot();
	if (!GetNodeAttributeString(root, "command", Command, error)) {
		return false;
	}
	if (!GetNodeAttributeString(root, "transaction_id", TransactionId, error)) {
		return false;
	}
	GetNodeAttributeBool(root, "success", Success);

	t4p::DbgpXmlErrors ignoredError;
	wxXmlNode* propNode = GetNodeChild(root, "property", ignoredError);
	if (propNode) {
		PropertyFromXmlNode(propNode, Property, ignoredError);
	}
	error = t4p::DBGP_XML_ERROR_NONE;
	return true;
}


t4p::DbgpCommandClass::DbgpCommandClass()
: TransactionId(0)
, Pid(0)
, MachineName() {
	Pid = wxGetProcessId();
	MachineName = wxT("triumph");
}

std::string t4p::DbgpCommandClass::Status() {
	return Build(
		"status",
		""
	);
}

std::string t4p::DbgpCommandClass::FeatureGet(const wxString& featureName) {
	return Build(
		"feature_get",
		"-n " + EscapeArg(featureName)
	);
}

std::string t4p::DbgpCommandClass::FeatureSet(const wxString& featureName, const wxString& value) {
	return Build(
		"feature_set",
		"-n " + EscapeArg(featureName) + " " +
		"-v " + EscapeArg(value)
	);
}

std::string t4p::DbgpCommandClass::Run() {
	return Build(
		"run",
		""
	);
}

std::string t4p::DbgpCommandClass::StepInto() {
	return Build(
		"step_into",
		""
	);
}

std::string t4p::DbgpCommandClass::StepOver() {
	return Build(
		"step_over",
		""
	);
}

std::string t4p::DbgpCommandClass::StepOut() {
	return Build(
		"step_out",
		""
	);
}

std::string t4p::DbgpCommandClass::Stop() {
	return Build(
		"stop",
		""
	);
}

std::string t4p::DbgpCommandClass::BreakpointFile(const wxString& filename, int lineNumber, bool enabled, int hitValue, const wxString& hitCondition) {
	wxString args;
	args += wxT("-t line");
	args += wxT(" -f ") + EscapeArg(filename);
	args += wxString::Format(" -n %d", lineNumber);
	if (enabled) {
		args += wxT(" -s enabled");
	} else {
		args += wxT(" -s disabled");
	}
	if (hitValue) {
		args += wxString::Format(" -h %d", hitValue);
	}
	if (!hitCondition.empty()) {
		args += " -o " + hitCondition;
	}
	return Build(
		"breakpoint_set",
		args
	);
}

std::string t4p::DbgpCommandClass::BreakpointCall(const wxString& function, bool enabled, int hitValue, const wxString& hitCondition) {
	wxString args;
	args += wxT("-t call");
	args += wxT(" -m ") + EscapeArg(function);
	if (enabled) {
		args += wxT(" -s enabled");
	} else {
		args += wxT(" -s disabled");
	}
	if (hitValue) {
		args += wxString::Format(" -h %d", hitValue);
	}
	if (!hitCondition.empty()) {
		args += " -o " + hitCondition;
	}

	return Build(
		"breakpoint_set",
		args
	);
}

std::string t4p::DbgpCommandClass::BreakpointReturn(const wxString& function, bool enabled, int hitValue, const wxString& hitCondition) {
	wxString args;
	args += wxT("-t return");
	args += wxT(" -m ") + EscapeArg(function);
	if (enabled) {
		args += wxT(" -s enabled");
	} else {
		args += wxT(" -s disabled");
	}
	if (hitValue) {
		args += wxString::Format(" -h %d", hitValue);
	}
	if (!hitCondition.empty()) {
		args += " -o " + hitCondition;
	}

	return Build(
		"breakpoint_set",
		args
	);
}

std::string t4p::DbgpCommandClass::BreakpointException(const wxString& exception, bool enabled) {
	wxString args;
	args += wxT("-t exception");
	args += wxT(" -x ") + EscapeArg(exception);
	if (enabled) {
		args += wxT(" -s enabled");
	} else {
		args += wxT(" -s disabled");
	}

	return Build(
		"breakpoint_set",
		args
	);
}

std::string t4p::DbgpCommandClass::BreakpointConditional(const wxString& filename, int lineNumber, const wxString& expression, bool enabled) {
	wxString args;
	args += wxT("-t conditional");
	args += wxT(" -f ") + EscapeArg(filename);
	args += wxString::Format(" -n %d", lineNumber);
	if (enabled) {
		args += wxT(" -s enabled");
	} else {
		args += wxT(" -s disabled");
	}

	return Build(
		"breakpoint_set",
		args,
		expression
	);
}

std::string t4p::DbgpCommandClass::BreakpointWatch(const wxString& expression, bool enabled) {
	wxString args;
	args += wxT("-t watch");
	if (enabled) {
		args += wxT(" -s enabled");
	} else {
		args += wxT(" -s disabled");
	}
	return Build(
		"breakpoint_set",
		args,
		expression
	);
}

std::string t4p::DbgpCommandClass::BreakpointRunToCursor(const wxString& filename, int lineNumber) {
	wxString args;
	args += wxT("-t file");
	args += wxT(" -f ") + EscapeArg(filename);
	args += wxString::Format(" -n %d", lineNumber);
	args += wxT(" -s enabled");
	args += wxT(" -r 1");

	return Build(
		"breakpoint_set",
		args
	);
}

std::string t4p::DbgpCommandClass::BreakpointGet(const wxString& breakpointId) {
	return Build(
		"breakpoint_get",
		"-d " + EscapeArg(breakpointId)
	);
}

std::string t4p::DbgpCommandClass::BreakpointDisable(const wxString& breakpointId) {
	return Build(
		"breakpoint_update",
		"-d " + EscapeArg(breakpointId) + " " +
		"-s disabled"
	);
}

std::string t4p::DbgpCommandClass::BreakpointEnable(const wxString& breakpointId) {
	return Build(
		"breakpoint_update",
		"-d " + EscapeArg(breakpointId) + " " +
		"-s enabled"
	);
}

std::string t4p::DbgpCommandClass::BreakpointRemove(const wxString& breakpointId) {
	return Build(
		"breakpoint_remove",
		"-d " + EscapeArg(breakpointId)
	);
}

std::string t4p::DbgpCommandClass::BreakpointList() {
	return Build(
		"breakpoint_list",
		""
	);
}

std::string t4p::DbgpCommandClass::StackDepth() {
	return Build(
		"stack_depth",
		""
	);
}

std::string t4p::DbgpCommandClass::StackGet(int stackDepth) {
	wxString args = wxString::Format("-d %d", stackDepth);
	return Build(
		"stack_get",
		args
	);
}

std::string t4p::DbgpCommandClass::ContextNames(int stackDepth) {
	wxString args = wxString::Format("-d %d", stackDepth);
	return Build(
		"context_names",
		args
	);
}

std::string t4p::DbgpCommandClass::ContextGet(int stackDepth, int contextId) {
	wxString args;
	args += wxString::Format("-d %d", stackDepth);
	args += wxString::Format(" -c %d", contextId);

	return Build(
		"context_get",
		args
	);
}

std::string t4p::DbgpCommandClass::PropertyGet(int stackDepth, int contextId, const wxString& propertyFullName, const wxString& propertyKey) {
	wxString args;
	args += wxT("-n ") + EscapeArg(propertyFullName);
	args += wxString::Format(" -d %d", stackDepth);
	args += wxString::Format(" -c %d", contextId);
	if (!propertyKey.empty()) {
		args += wxT(" -k ") + EscapeArg(propertyKey);
	}

	return Build(
		"property_get",
		args
	);
}

std::string t4p::DbgpCommandClass::PropertySet(int stackDepth, int contextId, const wxString& propertyFullName, const wxString& dataType, const wxString& value) {
	wxString args;
	args += wxT("-n ") + EscapeArg(propertyFullName);
	args += wxString::Format(" -d %d", stackDepth);
	args += wxString::Format(" -c %d", contextId);
	args += wxT(" -t ") + EscapeArg(dataType);

	return Build(
		"property_set",
		args,
		value
	);
}

std::string t4p::DbgpCommandClass::PropertyValue(int stackDepth, int contextId, const wxString& propertyFullName, const wxString& propertyKey) {
	wxString args;
	args += wxT("-n ") + EscapeArg(propertyFullName);
	args += wxString::Format(" -d %d", stackDepth);
	args += wxString::Format(" -c %d", contextId);
	if (!propertyKey.empty()) {
		args += wxT(" -k ") + EscapeArg(propertyKey);
	}

	return Build(
		"property_value",
		args
	);
}

std::string t4p::DbgpCommandClass::Break() {
	return Build(
		"break",
		""
	);
}

std::string t4p::DbgpCommandClass::Eval(const wxString& expression) {
	return Build(
		"eval",
		"",
		expression
	);
}

std::string t4p::DbgpCommandClass::Build(const std::string& cmd, const wxString& args, const wxString& data) {
	// command [SPACE] [arguments] [SPACE] -- base64(data) [NULL]
	// transaction id is always included
	std::string line = cmd;
	line += " ";
	line += "-i " + CurrentTransactionId();
	if (!args.empty()) {
		line += " ";
		line += t4p::WxToChar(args);
	}
	if (!data.empty()) {
		line += " ";
		line += "-- ";
		wxString encoded = wxBase64Encode(data.data(), data.Length());
		line += t4p::WxToChar(encoded);
	}
	TransactionId++;
	return line;
}

std::string t4p::DbgpCommandClass::CurrentTransactionId() const {
	return  t4p::WxToChar(MachineName + wxString::Format("-%d-%d", Pid, TransactionId));
}

wxString t4p::DbgpCommandClass::EscapeArg(const wxString& arg) {
	wxString escaped = arg;
	if (arg.Contains(wxT("\"")) || arg.Contains(wxT(" ")) || arg.Contains(wxT("-"))) {
		escaped.Replace("\"", "\\\"");

		escaped = wxT("\"") + escaped + wxT("\"");
	}
	return escaped;
}
