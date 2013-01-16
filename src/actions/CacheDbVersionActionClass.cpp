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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <actions/CacheDbVersionActionClass.h>
#include <globals/Assets.h>
#include <globals/Errors.h>
#include <soci/sqlite3/soci-sqlite3.h>

/**
 * This number must match the number on the schema_version table
 * of the tags db; if numbers do not match the db will be recreated.
 */
static const int SCHEMA_VERSION_TAGS = 1;

/**
 * This number must match the number on the schema_version table of the
 * detectors db; if numbers do not match the db will be recreated.
 */
static const int SCHEMA_VERSION_DETECTOR = 1;

mvceditor::TagCacheDbVersionActionClass::TagCacheDbVersionActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ActionClass(runningThreads, eventId)
	, TagDbs() 
	, Session() {

}

bool mvceditor::TagCacheDbVersionActionClass::Init(mvceditor::GlobalsClass& globals) {
	bool ret = false;
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	for (project = globals.Projects.begin(); project != globals.Projects.end(); ++project) {
		if (project->IsEnabled) {

			// don't think wxFileName copy constructor is a deep clone
			// we need a deep clone since we access this in the background thread
			wxFileName file(project->ResourceDbFileName.GetFullPath());
			TagDbs.push_back(file);
			ret = true;
		}
	}
	return ret;
}

void mvceditor::TagCacheDbVersionActionClass::BackgroundWork() {
	std::vector<wxFileName>::iterator filename;
	for (filename = TagDbs.begin(); filename != TagDbs.end() && !IsCancelled(); ++filename) {
		try {
			
			// if file does not exist create it
			
			std::string stdFilename = mvceditor::WxToChar(filename->GetFullPath());
			Session.open(*soci::factory_sqlite3(), stdFilename);
			int versionNumber = mvceditor::SqliteSchemaVersion(Session);
			if (versionNumber != SCHEMA_VERSION_TAGS) {
				wxString error;
				bool good = mvceditor::SqliteSqlScript(mvceditor::ResourceSqlSchemaAsset(), Session, error);
				if (!good) {
					mvceditor::EditorLogError(mvceditor::WARNING_OTHER, error);
				}
			}
			Session.close();
		} catch (std::exception& e) {
			wxString msg = wxString::FromAscii(e.what());
			wxUnusedVar(msg);
			wxASSERT_MSG(false, msg);
		}
	}
}

wxString mvceditor::TagCacheDbVersionActionClass::GetLabel() const {
	return _("Tag Cache Version Check");
}

mvceditor::DetectorCacheDbVersionActionClass::DetectorCacheDbVersionActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ActionClass(runningThreads, eventId) 
	, DetectorDbs() 
	, Session() {

}

bool mvceditor::DetectorCacheDbVersionActionClass::Init(mvceditor::GlobalsClass& globals) {
	bool ret = false;
	std::vector<mvceditor::ProjectClass>::iterator project;
	for (project = globals.Projects.begin(); project != globals.Projects.end(); ++project) {
		if (project->IsEnabled) {
			
			// don't think wxFileName copy constructor is a deep clone
			// we need a deep clone since we access this in the background thread
			wxFileName file(project->DetectorDbFileName.GetFullPath());
			DetectorDbs.push_back(file);
			ret = true;
		}
	}
	return ret;
}

void mvceditor::DetectorCacheDbVersionActionClass::BackgroundWork() {
	std::vector<wxFileName>::iterator filename;
	for (filename = DetectorDbs.begin(); filename != DetectorDbs.end() && !IsCancelled(); ++filename) {
		try {

			// if file does not exist create it
			std::string stdFilename = mvceditor::WxToChar(filename->GetFullPath());
			Session.open(*soci::factory_sqlite3(), stdFilename);
			int versionNumber = mvceditor::SqliteSchemaVersion(Session);
			if (versionNumber != SCHEMA_VERSION_DETECTOR) {
				wxString error;
				bool good = mvceditor::SqliteSqlScript(mvceditor::DetectorSqlSchemaAsset(), Session, error);
				if (!good) {
					mvceditor::EditorLogError(mvceditor::WARNING_OTHER, error);
				}
			}
			Session.close();
		} catch (std::exception& e) {
			wxString msg = wxString::FromAscii(e.what());
			wxUnusedVar(msg);
			wxASSERT_MSG(false, msg);
		}
	}
}

wxString mvceditor::DetectorCacheDbVersionActionClass::GetLabel() const {
	return _("Detector Cache Version Check");
}


