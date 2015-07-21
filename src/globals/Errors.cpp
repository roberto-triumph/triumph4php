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
#include "globals/Errors.h"
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/string.h>

static wxString FIX_SEPARATOR = wxT("\n\nPossible Fixes\n\n");

wxString MessageFromError(t4p::Errors error, const wxString& extra) {
    wxString msg;
    switch (error) {
    case t4p::ERR_BAD_EXPLORER_EXCUTABLE:
        msg = t4p::MessageWithFix(_("Could not run explorer. ") + extra,
                                  _("Go To Edit ... Preferences, choose the Project tab to change the location of the explorer binary."));
        break;
    case t4p::ERR_PHP_EXECUTABLE_NONE:
        msg = t4p::MessageWithFix(_("PHP executable location not set. ") + extra,
                                  _("Go To Edit ... Preferences, choose the PHP Executable tab to change the location of the PHP executable binary."));
        break;
    case t4p::ERR_BAD_PHP_EXECUTABLE:
        msg = t4p::MessageWithFix(_("PHP binary not found. ") + extra,
                                  _("Go To Project ... Environment to choose the location of the PHP binary."));
        break;
    case t4p::ERR_MISSING_PHP_EXTENSIONS:
        msg = t4p::MessageWithFix(_("Missing required extensions. ") + extra,
                                  _("Enable the PDO and pdo_sqlite extensions in your PHP installation."));
        break;
    case t4p::ERR_LOW_RESOURCES:
        msg = t4p::MessageWithFix(_("Your system is way too busy. Please try again later. ") + extra,
                                  _("Try closing some programs non-essential program or restarting Triumph."));
        break;
    case t4p::ERR_PROJECT_DETECTION:
        msg = t4p::MessageWithFix(_("Error detecting PHP framework. ") + extra,
                                  _("Are you developing a new framework detection script? Is the script correct?"));
        break;
    case t4p::ERR_DATABASE_DETECTION:
        msg = t4p::MessageWithFix(_("Error detecting database info. ") + extra,
                                  _("Are you developing a new framework detection script? Is the script correct?"));
        break;
    case t4p::ERR_ROGUE_PROCESS:
        msg = t4p::MessageWithFix(_("Could not stop background process. ") + extra,
                                  _("Restart Triumph"));
        break;
    case t4p::ERR_CHARSET_DETECTION:
        msg = t4p::MessageWithFix(_("Could not open file due to a character set detection error. ") + extra,
                                  _("This seems to be a binary, encrypted, or compressed file.  Triumph cannot open binary, encrypted, or compressed files."));
        break;
    case t4p::ERR_BAD_SQL_CONNECTION:
        msg = t4p::MessageWithFix(_("Error connecting to DB. ") + extra,
                                  _("Start your database server\nOR ") +
                                  _("Go To SQL ... Connections and correct the DB info. \nOR Go to your project's DB config file and correct the settings."));
        break;
    case t4p::ERR_BAD_SQL_QUERY:
        msg = t4p::MessageWithFix(_("Error executing SQL query. ") + extra,
                                  _("Query has a syntax error\nOR database database server is not running?"));
        break;
    case t4p::ERR_MISSING_KEYBOARD_SHORTCUT:
        msg = t4p::MessageWithFix(_("Could not find menu item for shortcut. ") + extra,
                                  _("All other menu shortcuts were loaded successfully and are functional. ") +
                                  _("If you want to get rid of this message; Go to Edit ... Preferences, then Click OK to re-write the config file."));
        break;
    case t4p::ERR_CORRUPT_KEYBOARD_SHORTCUT:
        msg = t4p::MessageWithFix(_("Could not load menu shortcuts from config file. ") + extra,
                                  _("All other menu shortcuts were loaded successfully and are functional. ") +
                                  _("If you want to get rid of this message; Go to Edit ... Preferences, then Click OK to re-write the config file."));
        break;
    case t4p::ERR_INVALID_FILE:
        msg = t4p::MessageWithFix(_("Could not load file: ") + extra,
                                  _("Does file exist?\nDo you have access rights?"));
        break;
    case t4p::ERR_INVALID_DIRECTORY:
        msg = t4p::MessageWithFix(_("Could not open directory: ") + extra,
                                  _("Does the directory exist?\nDo you have access rights?\n") +
                                  _("Restore the directory \nOR go to File ... Defined Projects to remove the nonexistant source."));
        break;
    case t4p::ERR_FILE_TOO_LARGE:
        msg = t4p::MessageWithFix(_("File is too large: ") + extra,
                                  _("Triumph cannot handle files larger than 10 MB"));
        break;
    case t4p::ERR_INVALID_SETTINGS_DIRECTORY:
        msg = t4p::MessageWithFix(_("Settings directory does not exist:") + extra,
                                  _("Does the directory exist?\nDo you have access rights?\nRestore the directory \n") +
                                  _("OR go to Edit ... Preferences and choose a different settings directory."));
        break;
    case t4p::ERR_TAG_READ:
        msg = t4p::MessageWithFix(_("Could not read tag sqlite file:") + extra,
                                  _("Does the file exist?\nDo you have access rights?\nRestore the settings directory \n") +
                                  _("OR go to Edit ... Preferences and choose a different settings directory."));
        break;
    default:
        break;
    }
    return msg;
}

void t4p::EditorLogError(t4p::Errors error, wxString extra) {
    wxLogError(MessageFromError(error, extra));
}

void t4p::EditorLogErrorFix(wxString error, wxString fixes) {
    wxASSERT_MSG(!fixes.empty(), "fixes must be given");
    wxLogError(t4p::MessageWithFix(error, fixes));
}

void t4p::EditorLogWarning(t4p::Errors error, wxString extra) {
    wxLogWarning(MessageFromError(error, extra));
}

void t4p::EditorLogWarningFix(wxString warning, wxString fixes) {
    wxASSERT_MSG(!fixes.empty(), "fixes must be given");
    wxLogWarning(t4p::MessageWithFix(warning, fixes));
}

wxString t4p::MessageWithFix(wxString message, wxString fix) {
    return message + FIX_SEPARATOR + fix;
}

void t4p::EditorErrorFix(const wxString& errorString, wxString& error, wxString& fix) {
    int index = errorString.Find(FIX_SEPARATOR);
    if (index != wxNOT_FOUND) {
        error = errorString.Mid(0, index);
        fix = errorString.Mid(index + FIX_SEPARATOR.Length());
    } else {
        error = errorString;
    }
}
