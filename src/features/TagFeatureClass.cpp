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
#include <features/TagFeatureClass.h>
#include <globals/String.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <globals/Events.h>
#include <globals/Number.h>
#include <actions/TagWipeActionClass.h>
#include <globals/TagList.h>
#include <Triumph.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/valgen.h>

t4p::TagFeatureClass::TagFeatureClass(t4p::AppClass& app)
	: FeatureClass(app)
	, JumpToText()
	, CacheState(CACHE_STALE) {
}

void t4p::TagFeatureClass::OnAppStartSequenceComplete(wxCommandEvent& event) {
	CacheState = CACHE_OK;
}

wxString t4p::TagFeatureClass::CacheStatus() {
	if (CACHE_OK == CacheState) {
		return _("OK");
	}
	return _("Stale");
}

void t4p::TagFeatureClass::OnAppFileDeleted(wxCommandEvent& event) {
	// clean up the cache in a background thread
	std::vector<wxFileName> filesToDelete;
	filesToDelete.push_back(wxFileName(event.GetString()));
	t4p::TagDeleteFileActionClass* action =  new t4p::TagDeleteFileActionClass(App.SqliteRunningThreads, wxID_ANY,
		filesToDelete);
	action->Init(App.Globals);
	App.SqliteRunningThreads.Queue(action);
}

void t4p::TagFeatureClass::OnAppFileRenamed(t4p::RenameEventClass& event) {
	t4p::ProjectTagSingleFileRenameActionClass* action = new t4p::ProjectTagSingleFileRenameActionClass(App.SqliteRunningThreads, wxID_ANY);
	action->SetPaths(event.OldPath.GetFullPath(), event.NewPath.GetFullPath());
	action->Init(App.Globals);
	App.SqliteRunningThreads.Queue(action);
}

void t4p::TagFeatureClass::OnAppFileExternallyModified(wxCommandEvent& event) {
	// the file is assumed not be opened, we don't need to build the symbol table
	// just retag it
	// see the comment for EVENT_APP_FILE_EXTERNALLY_MODIFIED in Events.h
	// if the file is from an active project, then re-tag it
	// otherwise do nothing
	wxString fileName = event.GetString();
	t4p::ProjectTagSingleFileActionClass* tagAction = new t4p::ProjectTagSingleFileActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_TAG_FINDER_LIST);
	tagAction->SetFileToParse(fileName);
	if (tagAction->Init(App.Globals)) {
		App.SqliteRunningThreads.Queue(tagAction);
	}
	else {
		delete tagAction;
	}
}

void t4p::TagFeatureClass::OnAppDirCreated(wxCommandEvent& event) {
	t4p::ProjectTagDirectoryActionClass* tagAction =  new t4p::ProjectTagDirectoryActionClass(App.SqliteRunningThreads, wxID_ANY);
	tagAction->SetDirToParse(event.GetString());
	if (tagAction->Init(App.Globals)) {
		App.SqliteRunningThreads.Queue(tagAction);
	}
	else {
		delete tagAction;
	}
}

void t4p::TagFeatureClass::OnAppDirDeleted(wxCommandEvent& event) {
	std::vector<wxFileName> dirsToDelete;
	wxFileName dir;
	dir.AssignDir(event.GetString());
	dirsToDelete.push_back(dir);
	t4p::TagDeleteDirectoryActionClass* tagAction =  new t4p::TagDeleteDirectoryActionClass(App.SqliteRunningThreads, wxID_ANY, dirsToDelete);
	if (tagAction->Init(App.Globals)) {
		App.SqliteRunningThreads.Queue(tagAction);
	}
	else {
		delete tagAction;
	}
}

void t4p::TagFeatureClass::OnAppDirRenamed(t4p::RenameEventClass& event) {
t4p::ProjectTagDirectoryRenameActionClass* action = new t4p::ProjectTagDirectoryRenameActionClass(App.SqliteRunningThreads, wxID_ANY);
	action->SetPaths(event.OldPath.GetPath(), event.NewPath.GetPath());
	action->Init(App.Globals);
	App.SqliteRunningThreads.Queue(action);
}

BEGIN_EVENT_TABLE(t4p::TagFeatureClass, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_FILE_DELETED, t4p::TagFeatureClass::OnAppFileDeleted)
	EVT_APP_FILE_RENAMED(t4p::TagFeatureClass::OnAppFileRenamed)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_DIR_CREATED,  t4p::TagFeatureClass::OnAppDirCreated)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_DIR_DELETED,  t4p::TagFeatureClass::OnAppDirDeleted)
	EVT_APP_DIR_RENAMED(t4p::TagFeatureClass::OnAppDirRenamed)

	// we will treat new exernal file and file external modified the same
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_FILE_EXTERNALLY_CREATED, t4p::TagFeatureClass::OnAppFileExternallyModified)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_FILE_EXTERNALLY_MODIFIED, t4p::TagFeatureClass::OnAppFileExternallyModified)


	EVT_COMMAND(wxID_ANY, t4p::EVENT_SEQUENCE_COMPLETE, t4p::TagFeatureClass::OnAppStartSequenceComplete)
END_EVENT_TABLE()
