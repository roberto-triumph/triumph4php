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
#ifndef __MVCEDITORERRORS_H__
#define __MVCEDITORERRORS_H__

#include <wx/string.h>

namespace mvceditor {

/**
 * These are errors from the editor itself; not the app that the editor is
 * modifying.
 */
enum Errors {
	PROJECT_DETECTION,
	DATABASE_DETECTION,
	LOW_RESOURCES,
	BAD_PHP_EXECUTABLE,
	BAD_EXPLORER_EXCUTABLE,
	ROGUE_PROCESS,
	CHARSET_DETECTION,
	BAD_SQL
};

/**
 * Wrapper around wxLogError; will format a message specific to the error. The error message will
 * show up in the "Editor messages" window. Additionally, an appropriate "fix" message will also show up
 * letting the user know how to avoid the error.

 * Use this when processing is not completed due to the error.
 */
void EditorLogError(Errors error, wxString extra = wxEmptyString);

/**
 * Wrapper around wxLogWarning; will format a message specific to the error. The error message will
 * show up in the "Editor messages" window. Additionally, an appropriate "fix" message will also show up
 * letting the user know how to avoid the error.
 * Use this when the error, while important, does not stop the processing.
 */
void EditorLogWarning(Errors error, wxString extra = wxEmptyString);

/**
 * Builds a log message that also contains a fix message. When the editor logger sees this; it will
 * treat the fix message in a special way.
 * This function allows the caller to build custom fix messages even when there is no appropriate
 * Errors enum (caller would just use wxLogError() with the contents of this returned string).
 */
wxString MessageWithFix(wxString message, wxString fix);

/**
 * Get the fix string (a message detailing how to correct an error)
 * from an error string. This is the reverse of MessageWithFix() function
 *
 * @param wxString one of the strings generated by the EditorLogError, EditorLogWarning functions
 * @param wxString the error message will be parsed out and written to this variable
 * @param wxString the fix message will be parsed out and written to this variable
 *
 * Ideally this function would take in an Errors enum, but the wxWidgets log functionality does
 * not allow passing of int values; we must look at logged message and compare using string comparisons;
 * the fix string is determined by comparing logged text vs. what the string an Errors enum would generate.
 */
void EditorErrorFix(const wxString& errorString, wxString& error, wxString& fix);

};

#endif