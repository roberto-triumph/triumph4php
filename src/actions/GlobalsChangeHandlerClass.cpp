/**
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
 * The MIT License
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
#include "actions/GlobalsChangeHandlerClass.h"
#include <soci/sqlite3/soci-sqlite3.h>
#include <vector>
#include "actions/GlobalActionClass.h"
#include "globals/Errors.h"
#include "language_sql/DatabaseTagClass.h"

t4p::GlobalsChangeHandlerClass::GlobalsChangeHandlerClass(t4p::GlobalsClass& globals)
    : wxEvtHandler()
    , Globals(globals) {
}

void t4p::GlobalsChangeHandlerClass::OnSqlMetaDataComplete(t4p::SqlMetaDataEventClass& event) {
    std::vector<UnicodeString> errors = event.Errors;
    for (size_t i = 0; i < errors.size(); ++i) {
        wxString wxError = t4p::IcuToWx(errors[i]);
        t4p::EditorLogError(t4p::ERR_BAD_SQL_CONNECTION, wxError);
    }
}

void t4p::GlobalsChangeHandlerClass::OnDatabaseTagsComplete(t4p::ActionEventClass& event) {
    // first remove all detected connections that were previously detected
    std::vector<t4p::DatabaseTagClass>::iterator info;
    info = Globals.DatabaseTags.begin();
    while (info != Globals.DatabaseTags.end()) {
        if (info->IsDetected) {
            info = Globals.DatabaseTags.erase(info);
        } else {
            info++;
        }
    }


    soci::session session(*soci::factory_sqlite3(), t4p::WxToChar(Globals.DetectorCacheDbFileName.GetFullPath()));
    t4p::DatabaseTagFinderClass finder(session);
    std::vector<wxFileName> sourceDirectories = Globals.AllEnabledSourceDirectories();

    std::vector<t4p::DatabaseTagClass> detected = finder.All(sourceDirectories);
    std::vector<t4p::DatabaseTagClass>::const_iterator tag;
    for (tag = detected.begin(); tag != detected.end(); ++tag) {
        if (!tag->Host.isEmpty() && !tag->Schema.isEmpty()) {
            Globals.DatabaseTags.push_back(*tag);
        }
    }
}

BEGIN_EVENT_TABLE(t4p::GlobalsChangeHandlerClass, wxEvtHandler)
    EVT_SQL_META_DATA_COMPLETE(t4p::ID_EVENT_ACTION_SQL_METADATA, t4p::GlobalsChangeHandlerClass::OnSqlMetaDataComplete)
    EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR, t4p::GlobalsChangeHandlerClass::OnDatabaseTagsComplete)
END_EVENT_TABLE()


