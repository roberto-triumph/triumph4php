/**
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
 *
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <actions/GlobalsChangeHandlerClass.h>
#include <globals/DatabaseTagClass.h>
#include <actions/GlobalActionClass.h>
#include <globals/Errors.h>
#include <soci/sqlite3/soci-sqlite3.h>

mvceditor::GlobalsChangeHandlerClass::GlobalsChangeHandlerClass(mvceditor::GlobalsClass& globals) 
	: wxEvtHandler()
	, Globals(globals) {
}

void mvceditor::GlobalsChangeHandlerClass::OnSqlMetaDataComplete(mvceditor::SqlMetaDataEventClass& event) {
	Globals.SqlResourceFinder.Copy(event.NewResources);
	std::vector<UnicodeString> errors = event.Errors;
	for (size_t i = 0; i < errors.size(); ++i) {
		wxString wxError = mvceditor::IcuToWx(errors[i]);
		mvceditor::EditorLogError(mvceditor::ERR_BAD_SQL_CONNECTION, wxError);
	}
}

void mvceditor::GlobalsChangeHandlerClass::OnDatabaseTagsComplete(wxCommandEvent& event) {
	
	// first remove all detected connections that were previously detected
	std::vector<mvceditor::DatabaseTagClass>::iterator info;
	info = Globals.DatabaseTags.begin();
	while(info != Globals.DatabaseTags.end()) {
		if (info->IsDetected) {
			info = Globals.DatabaseTags.erase(info);
		}
		else {
			info++;
		}
	}

	mvceditor::DatabaseTagFinderClass finder;
	soci::session session(*soci::factory_sqlite3(), mvceditor::WxToChar(Globals.DetectorCacheDbFileName.GetFullPath()));
	finder.InitSession(&session);

	std::vector<mvceditor::DatabaseTagClass> detected = finder.All();
	std::vector<mvceditor::DatabaseTagClass>::const_iterator tag;
	for (tag = detected.begin(); tag != detected.end(); ++tag) {
		if (!tag->Host.isEmpty() && !tag->Schema.isEmpty()) {
			Globals.DatabaseTags.push_back(*tag);
		}
	}
}


BEGIN_EVENT_TABLE(mvceditor::GlobalsChangeHandlerClass, wxEvtHandler)
	EVT_SQL_META_DATA_COMPLETE(mvceditor::ID_EVENT_ACTION_SQL_METADATA, mvceditor::GlobalsChangeHandlerClass::OnSqlMetaDataComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR, mvceditor::EVENT_WORK_COMPLETE, mvceditor::GlobalsChangeHandlerClass::OnDatabaseTagsComplete)
END_EVENT_TABLE()


