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
static const int SCHEMA_VERSION_TAGS = 10;

/**
 * This number must match the number on the schema_version table of the
 * detectors db; if numbers do not match the db will be recreated.
 */
static const int SCHEMA_VERSION_DETECTOR = 9;

t4p::TagCacheDbVersionActionClass::TagCacheDbVersionActionClass(t4p::RunningThreadsClass& runningThreads, int eventId)
	: GlobalActionClass(runningThreads, eventId)
	, TagDbs()
	, Session() {
}

bool t4p::TagCacheDbVersionActionClass::Init(t4p::GlobalsClass& globals) {
	SetStatus(_("Tag Cache Check"));

	// don't think wxFileName copy constructor is a deep clone
	// we need a deep clone since we access this in the background thread
	wxFileName file(globals.TagCacheDbFileName.GetFullPath());
	TagDbs.push_back(file);
	return true;
}

void t4p::TagCacheDbVersionActionClass::BackgroundWork() {
	std::vector<wxFileName>::iterator filename;
	for (filename = TagDbs.begin(); filename != TagDbs.end() && !IsCancelled(); ++filename) {
		try {
			// if file does not exist create it
			// if the directory that the sqlite file should be in does not exist
			// then dont create the dirs.
			if (wxFileName::DirExists(filename->GetPath())) {
				std::string stdFilename = t4p::WxToChar(filename->GetFullPath());
				Session.open(*soci::factory_sqlite3(), stdFilename);
				int versionNumber = t4p::SqliteSchemaVersion(Session);
				if (versionNumber != SCHEMA_VERSION_TAGS) {
					wxString error;
					bool good = t4p::SqliteSqlScript(t4p::ResourceSqlSchemaAsset(), Session, error);
					if (!good) {
						t4p::EditorLogError(t4p::ERR_TAG_READ, error);
					}
				}
				Session.close();
			}
		} catch (std::exception& e) {
			wxString msg = wxString::FromAscii(e.what());
			wxUnusedVar(msg);
			wxASSERT_MSG(false, msg);
		}
	}
}

wxString t4p::TagCacheDbVersionActionClass::GetLabel() const {
	return _("Tag Cache Version Check");
}

t4p::DetectorCacheDbVersionActionClass::DetectorCacheDbVersionActionClass(t4p::RunningThreadsClass& runningThreads, int eventId)
	: GlobalActionClass(runningThreads, eventId)
	, DetectorDbs()
	, Session() {
}

bool t4p::DetectorCacheDbVersionActionClass::Init(t4p::GlobalsClass& globals) {
	SetStatus(_("Detector Cache Check"));

	// don't think wxFileName copy constructor is a deep clone
	// we need a deep clone since we access this in the background thread
	wxFileName file(globals.DetectorCacheDbFileName.GetFullPath());
	DetectorDbs.push_back(file);
	return true;
}

void t4p::DetectorCacheDbVersionActionClass::BackgroundWork() {
	std::vector<wxFileName>::iterator filename;
	for (filename = DetectorDbs.begin(); filename != DetectorDbs.end() && !IsCancelled(); ++filename) {
		try {
			// if file does not exist create it
			// if the directory that the sqlite file should be in does not exist
			// then dont create the dirs.
			if (wxFileName::DirExists(filename->GetPath())) {
				std::string stdFilename = t4p::WxToChar(filename->GetFullPath());
				Session.open(*soci::factory_sqlite3(), stdFilename);
				int versionNumber = t4p::SqliteSchemaVersion(Session);
				if (versionNumber != SCHEMA_VERSION_DETECTOR) {
					wxString error;
					bool good = t4p::SqliteSqlScript(t4p::DetectorSqlSchemaAsset(), Session, error);
					if (!good) {
						t4p::EditorLogError(t4p::ERR_TAG_READ, error);
					}
				}
				Session.close();
			}
		} catch (std::exception& e) {
			wxString msg = wxString::FromAscii(e.what());
			wxUnusedVar(msg);
			wxASSERT_MSG(false, msg);
		}
	}
}

wxString t4p::DetectorCacheDbVersionActionClass::GetLabel() const {
	return _("Detector Cache Version Check");
}


