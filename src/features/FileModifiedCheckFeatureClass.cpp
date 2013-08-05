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
#include <features/FileModifiedCheckFeatureClass.h>
#include <MvcEditor.h>
#include <globals/Events.h>
#include <wx/choicdlg.h>
#include <algorithm>
#include <map>

static int ID_FILE_MODIFIED_CHECK = wxNewId();

mvceditor::FileModifiedCheckFeatureClass::FileModifiedCheckFeatureClass(mvceditor::AppClass& app)
: FeatureClass(app)
, Timer(this, ID_FILE_MODIFIED_CHECK)
, FsWatcher() 
, FilesExternallyModified()
, FilesExternallyDeleted() 
, DirsExternallyCreated() 
, Ticks(0) {
	FsWatcher.SetOwner(this);
}

void mvceditor::FileModifiedCheckFeatureClass::OnAppReady(wxCommandEvent& event) {
	Timer.Start(250, wxTIMER_CONTINUOUS);
	
	// add the enabled projects to the watch list
	std::vector<mvceditor::SourceClass> sources = App.Globals.AllEnabledPhpSources();
	std::vector<mvceditor::SourceClass>::const_iterator source;
	for (source = sources.begin(); source != sources.end(); ++source) {
		int flags = wxFSW_EVENT_CREATE  | wxFSW_EVENT_DELETE  | wxFSW_EVENT_RENAME | wxFSW_EVENT_MODIFY | wxFSW_EVENT_ERROR | wxFSW_EVENT_WARNING;
		wxFileName sourceDir = source->RootDirectory;
		sourceDir.DontFollowLink();
		FsWatcher.AddTree(sourceDir, flags);
	}
}

void mvceditor::FileModifiedCheckFeatureClass::OnAppExit(wxCommandEvent& event) {

	// unregister ourselves as the event handler from watcher 
	FsWatcher.SetOwner(NULL);

	Timer.Stop();
}

void mvceditor::FileModifiedCheckFeatureClass::OnTimer(wxTimerEvent& event) {
	Timer.Stop();
	Ticks++;

	mvceditor::NotebookClass* notebook = GetNotebook();
	size_t size = notebook->GetPageCount();
	std::map<wxString, mvceditor::CodeControlClass*> openedFiles;
	if (size > 0) {

		// loop through all of the opened files to get the files to
		// be checked
		// no need to check new files as they are not yet in the file system
		for (size_t i = 0; i < size; ++i) {
			mvceditor::CodeControlClass* ctrl = notebook->GetCodeControl(i);
			if (ctrl && !ctrl->IsNew()) {
				openedFiles[ctrl->GetFileName()] = ctrl;
			}
		}
	}

	// if the file that was modified is one of the opened files, we need to prompt the user
	// to see if they want to reload the new version
	std::vector<wxFileName>::iterator f = FilesExternallyModified.begin();
	std::map<wxString, mvceditor::CodeControlClass*> filesToPrompt;
	while (f != FilesExternallyModified.end()) {
		bool isOpened = openedFiles.find(f->GetFullPath()) != openedFiles.end();
		if (!isOpened) {
			
			// file is not open. handle it later
			f++;
			
		}
		else if (f->GetModificationTime() > openedFiles[f->GetFullPath()]->GetFileOpenedDateTime()) {
			
			// file is opened, but since file modified time is newer than what the code 
			// control read in then it means that the file was modified externally
			filesToPrompt[f->GetFullPath()] = openedFiles[f->GetFullPath()];

			// remove it from the files to handle list
			f = FilesExternallyModified.erase(f);
		}
		else {

			// this file was saved by the user clicking "save" on mvc editor. do nothing
			// because the cache will be updated via the EVT_APP_FILE_SAVED handler
			f = FilesExternallyModified.erase(f);
		}
	}
	if (!filesToPrompt.empty()) {
		FilesModifiedPrompt(filesToPrompt);
	}
	if (!FilesExternallyDeleted.empty()) {
		FilesDeletedPrompt(openedFiles, FilesExternallyDeleted);
		FilesExternallyDeleted.clear();
	}
	if (Ticks % 20 == 0 && (!FilesExternallyModified.empty() || !DirsExternallyCreated.empty())) {
		HandleNonOpenedFiles(openedFiles);
		Ticks = 1;
	}
	Timer.Start(250, wxTIMER_CONTINUOUS);
}

void mvceditor::FileModifiedCheckFeatureClass::HandleNonOpenedFiles(std::map<wxString, mvceditor::CodeControlClass*>& openedFiles) {
	std::vector<wxFileName>::iterator f = FilesExternallyModified.begin();
	while (f != FilesExternallyModified.end()) {
		bool isOpened = openedFiles.find(f->GetFullPath()) != openedFiles.end();
		if (!isOpened) {
			// file is not open. notify the app that a file was externally modified
			wxCommandEvent modifiedEvt(mvceditor::EVENT_APP_FILE_EXTERNALLY_MODIFIED);
			modifiedEvt.SetString(f->GetFullPath());
			App.EventSink.Publish(modifiedEvt);
			f = FilesExternallyModified.erase(f);
		}
	}
	if (!DirsExternallyCreated.empty()) {
		for (size_t i = 0; i < DirsExternallyCreated.size(); ++i) {
			wxCommandEvent modifiedEvt(mvceditor::EVENT_APP_DIR_CREATED);
			modifiedEvt.SetString(DirsExternallyCreated[i].GetFullPath());
			App.EventSink.Publish(modifiedEvt);
		}
		DirsExternallyCreated.clear();
	}

}

void mvceditor::FileModifiedCheckFeatureClass::FilesModifiedPrompt(std::map<wxString, mvceditor::CodeControlClass*>& filesToPrompt) {
	wxArrayString choices;
	std::map<wxString, mvceditor::CodeControlClass*>::iterator it;
	for (it = filesToPrompt.begin(); it != filesToPrompt.end(); ++it) {
		choices.Add(it->first);
	}
	wxString msg;
	if (filesToPrompt.size() == 1) {
		msg = _("1 File have been modified externally. Reload file and lose any changes?\n");
		msg += _("If checked, the file will be reloaded. If left unchecked, the file will not be reloaded, allowing you to overwrite the file.");
	}
	else {
		msg = _("Files have been modified externally. Reload files and lose any changes?\n");
		msg += _("The checked files will be reloaded. Unchecked files will not be reloaded, allowing you to overwrite the files.");
	}
	wxMultiChoiceDialog dialog(GetMainWindow(), msg, _("Files Externally Modified"), 
		choices, wxOK | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	dialog.ShowModal();
	wxArrayInt selections = dialog.GetSelections();

	// the code below relies on there being only one code control for each file
	std::vector<wxString> revertedFiles;
	for (size_t i = 0; i < selections.size(); ++i) {
		wxString fileName(choices[selections[i]]);

		// find the control for the file and revert the contents
		if (filesToPrompt.find(fileName) != filesToPrompt.end()) {
			filesToPrompt[fileName]->Revert();

			// need to notify the app that the file was reloaded
			wxCommandEvent reloadEvt(mvceditor::EVENT_APP_FILE_REVERTED);
			reloadEvt.SetString(fileName);
			App.EventSink.Publish(reloadEvt);
			revertedFiles.push_back(fileName);
		}
	}

	// now handle the files that the user did NOT want to reload
	// if the user did not want to reload the file, update the  opened date time
	// so that we treat the file as up-to-date
	for (it = filesToPrompt.begin(); it != filesToPrompt.end(); ++it) {
		if (std::find(revertedFiles.begin(), revertedFiles.end(), it->first) == revertedFiles.end()) {
			wxFileName fn(it->first);
			it->second->UpdateOpenedDateTime(fn.GetModificationTime());
		}
	}
}

void mvceditor::FileModifiedCheckFeatureClass::FilesDeletedPrompt(std::map<wxString, mvceditor::CodeControlClass*>& openedFiles, std::vector<wxFileName>& deletedFiles) {
	std::vector<wxFileName>::const_iterator file;
	wxString files;
	bool deletingOpened = false;
	for (file = deletedFiles.begin(); file != deletedFiles.end(); ++ file) {
	
		// find the control for the file
		if (openedFiles.end() != openedFiles.find(file->GetFullPath())) {
			mvceditor::CodeControlClass* ctrl = openedFiles[file->GetFullPath()];
			ctrl->TreatAsNew();
			files += file->GetFullPath() + wxT("\n");
			deletingOpened = true;
		}
		// send the deleted file event to the app
		wxCommandEvent deleteEvt(mvceditor::EVENT_APP_FILE_DELETED);
		deleteEvt.SetString(file->GetFullPath());
		App.EventSink.Publish(deleteEvt);		
	}

	// only show a message if a file that is being edited was deleted
	if (deletingOpened) {
		wxString message;
		message += _("The following files have been deleted externally.\n");
		message += _("You will need to save the file to store the contents.\n\n");
		message += files;
		int opts = wxICON_QUESTION;
		wxMessageBox(message, _("Warning"), opts, GetMainWindow());
	}
}

void mvceditor::FileModifiedCheckFeatureClass::OnFsWatcher(wxFileSystemWatcherEvent& event) {
	wxFileName path = event.GetPath();
	if (wxFSW_EVENT_MODIFY == event.GetChangeType()) {
		
		// a modify event could be from a file modification or directory modification
		// skip directory modifications, as directories are modified when files added/removed from the dir
		if (path.FileExists() && std::find(FilesExternallyModified.begin(), FilesExternallyModified.end(), path) == FilesExternallyModified.end()) {
			
			// this file was modified
			FilesExternallyModified.push_back(path);
		}
	}
	else if (wxFSW_EVENT_CREATE == event.GetChangeType()) {
		if (path.FileExists() && std::find(FilesExternallyModified.begin(), FilesExternallyModified.end(), path) == FilesExternallyModified.end()) {
			
			// this file was created, same logic as file modification
			FilesExternallyModified.push_back(path);
		}
		else if (path.DirExists() && std::find(DirsExternallyCreated.begin(), DirsExternallyCreated.end(), path) == DirsExternallyCreated.end()) {
			
			// a new directory was created
			DirsExternallyCreated.push_back(path);
		}
	}
	else if (wxFSW_EVENT_DELETE == event.GetChangeType()) {
		bool isFile = App.Globals.TagCache.HasFullPath(path.GetFullPath());
		if (isFile && std::find(FilesExternallyDeleted.begin(), FilesExternallyDeleted.end(), path) == FilesExternallyDeleted.end()) {
			
			// this file was created, same logic as file modification
			FilesExternallyDeleted.push_back(path);
		}
	}
	else if (wxFSW_EVENT_WARNING == event.GetChangeType()) {
		wxASSERT_MSG(false, event.GetErrorDescription());
	}
	else if (wxFSW_EVENT_ERROR == event.GetChangeType()) {
		wxASSERT_MSG(false, event.GetErrorDescription());
	}
}

BEGIN_EVENT_TABLE(mvceditor::FileModifiedCheckFeatureClass, mvceditor::FeatureClass)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_READY, mvceditor::FileModifiedCheckFeatureClass::OnAppReady)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_EXIT, mvceditor::FileModifiedCheckFeatureClass::OnAppExit)
	EVT_TIMER(ID_FILE_MODIFIED_CHECK, mvceditor::FileModifiedCheckFeatureClass::OnTimer)
	EVT_FSWATCHER(wxID_ANY, mvceditor::FileModifiedCheckFeatureClass::OnFsWatcher)
END_EVENT_TABLE()