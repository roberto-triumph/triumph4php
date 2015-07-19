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
#include <UnitTest++.h>
#include <TriumphChecks.h>
#include <language_php/DbgpEventClass.h>
#include <wx/string.h>
#include <wx/log.h>

SUITE(DbgpEventClassTest) {
TEST(BreakpointResponse) {
	// 5 breakpoints
	// 1 line breakpoint
	// 1 call breakpoint
	// 1 exception breakpoint
	// 1 conditional breakpoint
	// 1 watch breakpoint
	wxString xml = wxString::FromAscii(
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<response command=\"breakpoint_list\" \n"
		"	transaction_id=\"transaction_id\" >\n"
		"<breakpoint id=\"BREAKPOINT_123\" \n"
		"        type=\"line\" \n"
		"        state=\"enabled\" \n"
		"        filename=\"/var/www/site/index.php\" \n"
		"        lineno=\"34\" \n"
		"        hit_value=\"1\" \n"
		"        hit_condition=\">=\" \n"
		"        hit_count=\"1\"> \n"
		"</breakpoint>\n"
		"<breakpoint id=\"BREAKPOINT_124\" \n"
		"        type=\"call\" \n"
		"        state=\"disabled\" \n"
		"        function=\"printTheUser\" \n"
		"        hit_value=\"1\" \n"
		"        hit_condition=\">=\" \n"
		"        hit_count=\"1\"> \n"
		"</breakpoint>\n"
		"<breakpoint id=\"BREAKPOINT_125\" \n"
		"        type=\"exception\" \n"
		"        state=\"enabled\" \n"
		"        exception=\"Exception\" \n"
		"        hit_value=\"1\" \n"
		"        hit_condition=\">=\" \n"
		"        hit_count=\"1\"> \n"
		"</breakpoint>\n"
		"<breakpoint id=\"BREAKPOINT_126\" \n"
		"        type=\"conditional\" \n"
		"        state=\"enabled\" \n"
		"        filename=\"/var/www/site/index3.php\" \n"
		"        lineno=\"34\" \n"
		"        hit_value=\"1\" \n"
		"        hit_condition=\">=\" \n"
		"        hit_count=\"1\"> \n"
		"<expression>Y291bnQoJG5hbWVzKSA9PSAw</expression> \n" // base64 encoded count($names) == 0
		"</breakpoint>\n"
		"<breakpoint id=\"BREAKPOINT_127\" \n"
		"        type=\"watch\" \n"
		"        state=\"enabled\" \n"
		"        hit_value=\"1\" \n"
		"        hit_condition=\">=\" \n"
		"        hit_count=\"1\"> \n"
		"<expression>JG5hbWVz</expression> \n" // base64 encoded $names
		"</breakpoint>\n"
		"</response>\n"
	);
	t4p::DbgpBreakpointListEventClass breakpointEvt;
	t4p::DbgpXmlErrors error = t4p::DBGP_XML_ERROR_NONE;
	bool success = breakpointEvt.FromXml(xml, error);

	CHECK(success);
	CHECK_VECTOR_SIZE(5, breakpointEvt.Breakpoints);

	CHECK_EQUAL(wxT("BREAKPOINT_123"), breakpointEvt.Breakpoints[0].BreakpointId);
	CHECK_EQUAL(wxT("line"), breakpointEvt.Breakpoints[0].BreakpointType);
	CHECK(breakpointEvt.Breakpoints[0].IsEnabled);
	CHECK_EQUAL(wxT("/var/www/site/index.php"), breakpointEvt.Breakpoints[0].Filename);
	CHECK_EQUAL(34, breakpointEvt.Breakpoints[0].LineNumber);
	CHECK_EQUAL(1, breakpointEvt.Breakpoints[0].HitValue);
	CHECK_EQUAL(wxT(">="), breakpointEvt.Breakpoints[0].HitCondition);
	CHECK_EQUAL(1, breakpointEvt.Breakpoints[0].HitCount);
	CHECK_EQUAL(wxT(""), breakpointEvt.Breakpoints[0].Expression);

	CHECK_EQUAL(wxT("BREAKPOINT_124"), breakpointEvt.Breakpoints[1].BreakpointId);
	CHECK_EQUAL(wxT("call"), breakpointEvt.Breakpoints[1].BreakpointType);
	CHECK_EQUAL(false, breakpointEvt.Breakpoints[1].IsEnabled);
	CHECK_EQUAL(wxT("printTheUser"), breakpointEvt.Breakpoints[1].Function);

	CHECK_EQUAL(wxT("BREAKPOINT_125"), breakpointEvt.Breakpoints[2].BreakpointId);
	CHECK_EQUAL(wxT("exception"), breakpointEvt.Breakpoints[2].BreakpointType);
	CHECK_EQUAL(true, breakpointEvt.Breakpoints[2].IsEnabled);
	CHECK_EQUAL(wxT("Exception"), breakpointEvt.Breakpoints[2].Exception);

	CHECK_EQUAL(wxT("BREAKPOINT_126"), breakpointEvt.Breakpoints[3].BreakpointId);
	CHECK_EQUAL(wxT("conditional"), breakpointEvt.Breakpoints[3].BreakpointType);
	CHECK_EQUAL(true, breakpointEvt.Breakpoints[3].IsEnabled);
	CHECK_EQUAL(wxT("/var/www/site/index3.php"), breakpointEvt.Breakpoints[3].Filename);
	CHECK_EQUAL(34, breakpointEvt.Breakpoints[3].LineNumber);
	CHECK_EQUAL(wxT("count($names) == 0"), breakpointEvt.Breakpoints[3].Expression);

	CHECK_EQUAL(wxT("BREAKPOINT_127"), breakpointEvt.Breakpoints[4].BreakpointId);
	CHECK_EQUAL(wxT("watch"), breakpointEvt.Breakpoints[4].BreakpointType);
	CHECK_EQUAL(true, breakpointEvt.Breakpoints[4].IsEnabled);
	CHECK_EQUAL(wxT("$names"), breakpointEvt.Breakpoints[4].Expression);
}

TEST(Stack) {
	wxString xml = wxString::FromAscii(
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<response command=\"stack_get\" \n"
		"          transaction_id=\"transaction_id\"> \n"
		"    <stack level=\"1\" \n"
		"           type=\"file\" \n"
		"           filename=\"/var/www/site/index.php\" \n"
		"           lineno=\"50\" \n"
		"           where=\"cmd\" \n"
		"           cmdbegin=\"/var/www/site/index.php:3\" \n"
		"           cmdend=\"/var/www/site/index.php:50\"/> \n"
		"    <stack level=\"0\" \n"
		"           type=\"eval\" \n"
		"           filename=\"/var/www/site/index_main.php\" \n"
		"           lineno=\"10\"> \n"
		"        <input level=\"2\" \n"
		"               type=\"file\" \n"
		"               filename=\"/var/www/site/index_main.php\" \n"
		"               lineno=\"1\"/> \n"
		"    </stack> \n"
		"</response>\n"
	);
	t4p::DbgpStackGetEventClass stackEvt;
	t4p::DbgpXmlErrors error = t4p::DBGP_XML_ERROR_NONE;

	bool success = stackEvt.FromXml(xml, error);

	CHECK(success);
	CHECK_VECTOR_SIZE(2, stackEvt.Stack);

	CHECK_EQUAL(1, stackEvt.Stack[0].Level);
	CHECK_EQUAL(wxT("file"), stackEvt.Stack[0].Type);
	CHECK_EQUAL(wxT("/var/www/site/index.php"), stackEvt.Stack[0].Filename);
	CHECK_EQUAL(50, stackEvt.Stack[0].LineNumber);
	CHECK_EQUAL(wxT("cmd"), stackEvt.Stack[0].Where);
	CHECK_EQUAL(wxT("/var/www/site/index.php:3"), stackEvt.Stack[0].CmdBegin);
	CHECK_EQUAL(wxT("/var/www/site/index.php:50"), stackEvt.Stack[0].CmdEnd);

	CHECK_EQUAL(0, stackEvt.Stack[1].Level);
	CHECK_EQUAL(wxT("eval"), stackEvt.Stack[1].Type);
	CHECK_EQUAL(wxT("/var/www/site/index_main.php"), stackEvt.Stack[1].Filename);
	CHECK_EQUAL(10, stackEvt.Stack[1].LineNumber);
	CHECK_EQUAL(wxT(""), stackEvt.Stack[1].Where);
	CHECK_EQUAL(wxT(""), stackEvt.Stack[1].CmdBegin);
	CHECK_EQUAL(wxT(""), stackEvt.Stack[1].CmdEnd);
}

TEST(DebugEngineErrorResponse) {
	wxString xml = wxString::FromAscii(
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<response command=\"stack_get\" \n"
		"          transaction_id=\"transaction_id\"> \n"
		"    <error code=\"3\" \n"
		"           apperr=\"app specific error code\"> \n"
		"       <message>invalid context name</message>" // error messages are not base64 encoded
		"    </error> \n"
		"</response>\n"
	);
	t4p::DbgpErrorEventClass errorEvt;
	t4p::DbgpXmlErrors error = t4p::DBGP_XML_ERROR_NONE;

	bool success = errorEvt.FromXml(xml, error);

	CHECK(success);
	CHECK_EQUAL(wxT("stack_get"), errorEvt.Command);
	CHECK_EQUAL(wxT("transaction_id"), errorEvt.TransactionId);
	CHECK_EQUAL(3, errorEvt.ErrorCode);
	CHECK_EQUAL(wxT("app specific error code"), errorEvt.AppErrorCode);
	CHECK_EQUAL(wxT("invalid context name"), errorEvt.Message);
}

TEST(InvalidXml) {
	wxString xml = wxString::FromAscii(
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<response command=\"stack_get\" ><> \n"  // <-- bad xml here
		"          transaction_id=\"transaction_id\"> \n"
		"    <error code=\"3\" \n"
		"           apperr=\"app specific error code\"> \n"
		"       <message>aW52YWxpZCBjb250ZXh0IG5hbWU=</message>" // base64 encoded "invalid context name"
		"    </error> \n"
		"</response>\n"
	);
	t4p::DbgpErrorEventClass errorEvt;
	t4p::DbgpXmlErrors error = t4p::DBGP_XML_ERROR_NONE;


	// disable xml error; we know its invalid
	bool oldValue = wxLog::EnableLogging(false);

	bool success = errorEvt.FromXml(xml, error);

	wxLog::EnableLogging(oldValue);

	CHECK_EQUAL(false, success);
	CHECK_EQUAL(t4p::DBGP_XML_ERROR_PARSE, error);
}

TEST(CommandGenerationWithArgs) {
	wxString filename = wxT("/var/www/site/index.php");
	int lineNumber = 40;
	bool enabled = true;
	long pid = wxGetProcessId();

	t4p::DbgpCommandClass cmd;

	std::string line = cmd.BreakpointFile(filename, lineNumber, enabled);
	std::string expected = "breakpoint_set";
	expected += " -i " + wxString::Format("triumph-%ld-%d", pid, 0);
	expected += " -t line -f /var/www/site/index.php -n 40 -s enabled";
	CHECK_EQUAL(expected, line);
}

TEST(CommandGenerationWithData) {
	wxString expression = wxT("/var/www/site/index.php");
	wxString propertyName = wxT("$x[\"a b\"]");
	wxString dataType = "string";
	int stackDepth = 1;
	int contextId = 2;

	long pid = wxGetProcessId();

	t4p::DbgpCommandClass cmd;

	std::string line = cmd.PropertySet(stackDepth, contextId, propertyName, dataType, expression);

	std::string expected = "property_set";
	expected += " -i " + wxString::Format("triumph-%ld-%d", pid, 0);
	expected += " -n \"$x[\\\"a b\\\"]\"";  // quotes must be escaped
	expected +=  wxString::Format(" -d %d", stackDepth);
	expected +=  wxString::Format(" -c %d", contextId);
	expected +=  " -t " + dataType;
	expected +=  " -- L3Zhci93d3cvc2l0ZS9pbmRleC5waHA="; // base64 encode of expression
	CHECK_EQUAL(expected, line);
}
}
