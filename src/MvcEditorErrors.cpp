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
#include <MvcEditorErrors.h>
#include <wx/string.h>
#include <wx/log.h>
#include <wx/intl.h>

static wxString FIX_SEPARATOR = wxT("\n\nFix\n\n");

wxString MessageFromError(mvceditor::Errors error, const wxString& extra) {
	wxString msg;
	switch (error) {
		case mvceditor::BAD_EXPLORER_EXCUTABLE:
			msg = mvceditor::MessageWithFix(_("Could not run explorer. ") + extra, 
				_("Go To Edit ... Preferences, choose the Project tab to change the location of the explorer binary."));
			break;
		case mvceditor::BAD_PHP_EXECUTABLE:
			msg = mvceditor::MessageWithFix(_("PHP binary not found. ") + extra,
				_("Go To Project ... Environment to choose the location of the PHP binary."));
			break;
		case mvceditor::LOW_RESOURCES:
			msg = mvceditor::MessageWithFix(_("Your system is way too busy. Please try again later. ") + extra,
				_("Try closing some programs non-essential program or restarting MVC Editor."));
			break;
		case mvceditor::PROJECT_DETECTION:
			msg = mvceditor::MessageWithFix(_("Error detecting PHP framework. ") + extra,
				_("Are you developing a new framework detection script? Is the script correct?"));
			break;
		case mvceditor::DATABASE_DETECTION:
			msg = mvceditor::MessageWithFix(_("Error detecting database info. ") + extra,
				_("Are you developing a new framework detection script? Is the script correct?"));
			break;
		case mvceditor::ROGUE_PROCESS:
			msg = mvceditor::MessageWithFix(_("Could not stop background process. ") + extra,
				_("Restart MVC Editor"));
			break;
		case mvceditor::CHARSET_DETECTION:
			msg = mvceditor::MessageWithFix(_("Could not open file due to a character set detection error. ") + extra,
				_("This seems to be a binary, encrypted, or compressed file.  MVC Editor cannot open binary, encrypted, or compressed files."));
			break;
		case mvceditor::BAD_SQL:
			msg = mvceditor::MessageWithFix(_("Error executing SQL MetaData query. ") + extra,
				_("Is the database server up and running?"));
			break;
		case mvceditor::MISSING_KEYBOARD_SHORTCUT:
			msg = mvceditor::MessageWithFix(_("Could not find menu item for shortcut. ") + extra,
				_("All other menu shortcuts were loaded successfully and are functional. If you want to get rid of this message; Go to Edit ... Preferences, then Click OK to re-write the config file."));
			break;
		case mvceditor::CORRUPT_KEYBOARD_SHORTCUT:
			msg = mvceditor::MessageWithFix(_("Could not load menu shortcuts from config file. ") + extra,
				_("All other menu shortcuts were loaded successfully and are functional. If you want to get rid of this message; Go to Edit ... Preferences, then Click OK to re-write the config file."));
			break;
		default:
			break;
	}
	return msg;
}

void mvceditor::EditorLogError(mvceditor::Errors error, wxString extra) {
	wxLogError(MessageFromError(error, extra));
}

void mvceditor::EditorLogWarning(mvceditor::Errors error, wxString extra) {
	wxLogWarning(MessageFromError(error, extra));
}

wxString mvceditor::MessageWithFix(wxString message, wxString fix) {
	return message + FIX_SEPARATOR + fix;
}

void mvceditor::EditorErrorFix(const wxString& errorString, wxString& error, wxString& fix) {
	int index = errorString.Find(FIX_SEPARATOR);
	if (index != wxNOT_FOUND) {
		error = errorString.Mid(0, index);
		fix = errorString.Mid(index + FIX_SEPARATOR.Length());
	}
	else {
		error = errorString;
	}
}