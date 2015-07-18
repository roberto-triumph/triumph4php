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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <actions/TagWipeActionClass.h>
#include <language_php/TagParserClass.h>
#include <language_php/DetectorDbClass.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <globals/FileName.h>

t4p::TagWipeActionClass::TagWipeActionClass(t4p::RunningThreadsClass& runningThreads, int eventId)
	: GlobalActionClass(runningThreads, eventId)
	, ResourceDbFileName()
	, DetectorDbFileName() {
}

bool t4p::TagWipeActionClass::Init(t4p::GlobalsClass& globals) {
	SetStatus(_("Tag Cache Wipe"));

	// not sure if wxFileName assignment is a complete clone, so use Assign() just in case
	// since we will access the filenames from multiple threads
	ResourceDbFileName.Assign(globals.TagCacheDbFileName.GetFullPath());
	DetectorDbFileName.Assign(globals.DetectorCacheDbFileName.GetFullPath());

	return ResourceDbFileName.FileExists() && DetectorDbFileName.FileExists();
}

void t4p::TagWipeActionClass::BackgroundWork() {
	SetStatus(_("Tag Cache Wipe"));

	// initialize the sqlite db
	soci::session session;
	soci::session detectorSession;
	try {
		session.open(*soci::factory_sqlite3(), t4p::WxToChar(ResourceDbFileName.GetFullPath()));
		t4p::TagParserClass tagParser;
		tagParser.Init(&session);
		tagParser.WipeAll();

		detectorSession.open(*soci::factory_sqlite3(), t4p::WxToChar(DetectorDbFileName.GetFullPath()));
		t4p::DetectorDbClass detectorDb;
		detectorDb.Init(&detectorSession);
		detectorDb.Wipe();
	} catch(std::exception const& e) {
		session.close();
		wxString msg = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
}

wxString t4p::TagWipeActionClass::GetLabel() const {
	return wxT("Tag Cache Wipe");
}

t4p::TagDeleteSourceActionClass::TagDeleteSourceActionClass(t4p::RunningThreadsClass& runningThreads, int eventId,
													  const std::vector<wxFileName>& sourceDirsToDelete)
	: GlobalActionClass(runningThreads, eventId)
	, ResourceDbFileName()
	, DetectorDbFileName()
	, SourceDirsToDelete() {
	SourceDirsToDelete = t4p::DeepCopyFileNames(sourceDirsToDelete);
}

bool t4p::TagDeleteSourceActionClass::Init(t4p::GlobalsClass& globals) {
	SetStatus(_("Tag Cache Delete Source"));

	// wxFileName assignment is not a complete clone, so use Assign() just in case
	// since we will access the filenames from multiple threads
	// not sure if wxFileName assignment is a complete clone, so use Assign() just in case
	// since we will access the filenames from multiple threads
	ResourceDbFileName.Assign(globals.TagCacheDbFileName.GetFullPath());
	DetectorDbFileName.Assign(globals.DetectorCacheDbFileName.GetFullPath());

	return ResourceDbFileName.FileExists() && DetectorDbFileName.FileExists();
}

void t4p::TagDeleteSourceActionClass::BackgroundWork() {
	SetStatus(_("Tag Cache Delete"));

	// initialize the sqlite db
	soci::session session;
	soci::session detectorSession;
	try {
		session.open(*soci::factory_sqlite3(), t4p::WxToChar(ResourceDbFileName.GetFullPath()));
		t4p::TagParserClass tagParser;
		tagParser.Init(&session);

		detectorSession.open(*soci::factory_sqlite3(), t4p::WxToChar(DetectorDbFileName.GetFullPath()));
		t4p::DetectorDbClass detectorDb;
		detectorDb.Init(&detectorSession);

		for (size_t i = 0; i < SourceDirsToDelete.size(); ++i) {
			tagParser.DeleteSource(SourceDirsToDelete[i]);
			detectorDb.DeleteSource(SourceDirsToDelete[i]);
		}
	} catch(std::exception const& e) {
		session.close();
		wxString msg = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
}

wxString t4p::TagDeleteSourceActionClass::GetLabel() const {
	return wxT("Tag Cache Delete Source");
}

t4p::TagDeleteDirectoryActionClass::TagDeleteDirectoryActionClass(t4p::RunningThreadsClass& runningThreads, int eventId,
													  const std::vector<wxFileName>& dirsToDelete)
	: GlobalActionClass(runningThreads, eventId)
	, DirsToDelete() {
	DirsToDelete = t4p::DeepCopyFileNames(dirsToDelete);
}

bool t4p::TagDeleteDirectoryActionClass::Init(t4p::GlobalsClass& globals) {
	SetStatus(_("Tag Cache Delete Directory"));

	// wxFileName assignment is not a complete clone, so use Assign() just in case
	// since we will access the filenames from multiple threads
	ResourceDbFileNames.push_back(wxFileName(globals.TagCacheDbFileName.GetFullPath()));
	return !ResourceDbFileNames.empty();
}

void t4p::TagDeleteDirectoryActionClass::BackgroundWork() {
	std::vector<wxFileName>::iterator it;
	for (it = ResourceDbFileNames.begin(); it != ResourceDbFileNames.end() && !IsCancelled(); ++it) {
		SetStatus(_("Tag Cache Delete / ") + it->GetName());
		// initialize the sqlite db
		soci::session session;
		try {
			session.open(*soci::factory_sqlite3(), t4p::WxToChar(it->GetFullPath()));
			t4p::TagParserClass tagParser;
			tagParser.Init(&session);
			tagParser.DeleteDirectories(DirsToDelete);
		} catch(std::exception const& e) {
			session.close();
			wxString msg = t4p::CharToWx(e.what());
			wxASSERT_MSG(false, msg);
		}
	}
}

wxString t4p::TagDeleteDirectoryActionClass::GetLabel() const {
	return wxT("Tag Cache Delete Directory");
}

t4p::TagDeleteFileActionClass::TagDeleteFileActionClass(t4p::RunningThreadsClass& runningThreads, int eventId,
													  const std::vector<wxFileName>& filesToDelete)
	: GlobalActionClass(runningThreads, eventId)
	, FilesToDelete() {
	FilesToDelete = t4p::DeepCopyFileNames(filesToDelete);
}

bool t4p::TagDeleteFileActionClass::Init(t4p::GlobalsClass& globals) {
	SetStatus(_("Tag Cache Delete File"));

	// not sure if wxFileName assignment is a complete clone, so use Assign() just in case
	// since we will access the filenames from multiple threads
	ResourceDbFileNames.push_back(wxFileName(globals.TagCacheDbFileName.GetFullPath()));
	return !ResourceDbFileNames.empty();
}

void t4p::TagDeleteFileActionClass::BackgroundWork() {
	std::vector<wxFileName>::iterator it;
	for (it = ResourceDbFileNames.begin(); it != ResourceDbFileNames.end() && !IsCancelled(); ++it) {
		SetStatus(_("Tag Cache Delete File/ ") + it->GetName());
		// initialize the sqlite db
		soci::session session;
		try {
			session.open(*soci::factory_sqlite3(), t4p::WxToChar(it->GetFullPath()));
			t4p::TagParserClass tagParser;
			tagParser.Init(&session);
			for (size_t i = 0; i < FilesToDelete.size(); ++i) {
				tagParser.DeleteFromFile(FilesToDelete[i].GetFullPath());
			}
		} catch(std::exception const& e) {
			session.close();
			wxString msg = t4p::CharToWx(e.what());
			wxASSERT_MSG(false, msg);
		}
	}
}

wxString t4p::TagDeleteFileActionClass::GetLabel() const {
	return wxT("Tag Cache Delete File");
}
