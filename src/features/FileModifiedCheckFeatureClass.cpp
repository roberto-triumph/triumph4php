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
#include <actions/FileModifiedCheckActionClass.h>
#include <Triumph.h>
#include <globals/Errors.h>
#include <globals/Events.h>
#include <wx/choicdlg.h>
#include <algorithm>
#include <map>

static int ID_FILE_MODIFIED_ACTION = wxNewId();

/**
 * @return map of the code controls that have opened files (as opposed to new files being edited)
 *         key is the full path of the file being edited, value is the code control itself
 */
static std::map<wxString, t4p::CodeControlClass*> OpenedFiles(t4p::NotebookClass* notebook) {
	std::map<wxString, t4p::CodeControlClass*> openedFiles;
	if (!notebook) {
		return openedFiles;
	}
	size_t size = notebook->GetPageCount();
	if (size > 0) {

		// loop through all of the opened files to get the files to
		// be checked
		// no need to check new files as they are not yet in the file system
		for (size_t i = 0; i < size; ++i) {
			t4p::CodeControlClass* ctrl = notebook->GetCodeControl(i);
			if (ctrl && !ctrl->IsNew()) {
				openedFiles[ctrl->GetFileName()] = ctrl;
			}
		}
	}
	return openedFiles;
}

t4p::FileModifiedCheckFeatureClass::FileModifiedCheckFeatureClass(t4p::AppClass& app)
: FeatureClass(app)
, JustReactivated(false) {
}

void t4p::FileModifiedCheckFeatureClass::FilesModifiedPrompt(std::map<wxString, t4p::CodeControlClass*>& filesToPrompt) {
	if (filesToPrompt.empty()) {
		return;
	}
 	wxArrayString choices;
	std::map<wxString, t4p::CodeControlClass*>::iterator it;
	for (it = filesToPrompt.begin(); it != filesToPrompt.end(); ++it) {
		choices.Add(it->first);
	}
	wxString msg;
	if (filesToPrompt.size() == 1) {
		msg = _("1 File has been modified externally. Reload file and lose any changes?\n");
		msg += _("If checked, the file will be reloaded. If left unchecked, the file will not be reloaded, allowing you to overwrite the file.");
	}
	else {
		msg = _("Files have been modified externally. Reload files and lose any changes?\n");
		msg += _("The checked files will be reloaded. Unchecked files will not be reloaded, allowing you to overwrite the files.");
	}
	wxMultiChoiceDialog dialog(GetMainWindow(), msg, _("Files Externally Modified"), 
		choices, wxOK | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCENTRE);
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
			t4p::CodeControlEventClass revertEvt(t4p::EVENT_APP_FILE_REVERTED, filesToPrompt[fileName]);
			App.EventSink.Publish(revertEvt);
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

void t4p::FileModifiedCheckFeatureClass::FilesDeletedPrompt(std::map<wxString, t4p::CodeControlClass*>& openedFiles, 
																  std::map<wxString, int>& deletedFiles) {
	std::map<wxString, int>::const_iterator file;
	wxString files;
	bool deletingOpened = false;
	for (file = deletedFiles.begin(); file != deletedFiles.end(); ++ file) {
		wxString fullPath = file->first;

		// find the control for the file
		if (openedFiles.end() != openedFiles.find(fullPath)) {
			t4p::CodeControlClass* ctrl = openedFiles[fullPath];
			ctrl->TreatAsNew();
			files += fullPath + wxT("\n");
			deletingOpened = true;
		}
		// send the deleted file event to the app
		wxCommandEvent deleteEvt(t4p::EVENT_APP_FILE_DELETED);
		deleteEvt.SetString(fullPath);
		App.EventSink.Publish(deleteEvt);
	}

	// only show a message if a file that is being edited was deleted
	if (deletingOpened) {
		wxString message;
		message += _("The following files have been deleted externally.\n");
		message += _("You will need to save the file to store the contents.\n\n");
		message += files;
		int opts = wxICON_QUESTION | wxCENTRE;
		wxMessageBox(message, _("Warning"), opts, GetMainWindow());
	}
}

void t4p::FileModifiedCheckFeatureClass::FilesRenamedPrompt(std::map<wxString, t4p::CodeControlClass*>& openedFiles, std::map<wxString, wxString>& pathsRenamed) {
	
	std::map<wxString, wxString>::iterator renamed;
	std::map<wxString, t4p::CodeControlClass*> openedFilesRenamed;
	for (renamed = pathsRenamed.begin(); renamed != pathsRenamed.end(); ++renamed) {
		wxString renamedFrom = renamed->first;
		if (openedFiles.find(renamedFrom) != openedFiles.end()) {
			openedFilesRenamed[renamedFrom] = openedFiles[renamedFrom];
		}
		
		// check for renames, but check the new paths
		// if a file was renamed and the new name is an opened file, treat it as modified
		/*wxString renamedTo = renamed->second;
		if (openedFiles.find(renamedTo) != openedFiles.end()) {
			openedFilesRenamed[renamedTo] = openedFiles[renamedTo];
		}*/
	}

	// ask the user whether they want to
	// 1. open the new file and close the old one
	// 2. open the new file and keep the old one open
	// 3. don't open the new one and close the old one 
	std::map<wxString, t4p::CodeControlClass*>::iterator renamedCtrl;
	for (renamedCtrl = openedFilesRenamed.begin(); renamedCtrl != openedFilesRenamed.end(); ++renamedCtrl) {
		wxArrayString choices;
		choices.Add(_("Open the new file and close the old one"));
		choices.Add(_("Open the new file and keep the old one open"));
		choices.Add(_("Don't open the new one and close the old file"));
		
		wxString newFile = pathsRenamed[renamedCtrl->first];
		wxSingleChoiceDialog choiceDialog(GetMainWindow(), 
			renamedCtrl->first + 
			_("\nhas been renamed to \n") +
			newFile +
			_("\nWhat would you like to do?"),
			_("File Rename"), 
			choices
		);
		choiceDialog.SetWindowStyle(wxCENTER | wxOK);
		choiceDialog.SetSize(choiceDialog.GetSize().GetWidth(), choiceDialog.GetSize().GetHeight() + 40);
		if (wxID_OK == choiceDialog.ShowModal()) {
			int sel = choiceDialog.GetSelection();
			t4p::CodeControlClass* ctrl = renamedCtrl->second;
			t4p::NotebookClass* notebook = GetNotebook();
			if (0 == sel || 2 == sel) {
				notebook->DeletePage(notebook->GetPageIndex(ctrl));
			}
			if (0 == sel || 1 == sel) {
				t4p::OpenFileCommandEventClass openCmd(newFile);
				App.EventSink.Publish(openCmd);
			}
			if (1 == sel) {
				ctrl->TreatAsNew();
			}
		}
	}
}

void t4p::FileModifiedCheckFeatureClass::OpenedCodeControlCheck() {
	t4p::NotebookClass* notebook = GetNotebook();
	if (notebook->GetPageCount() == 0) {
		JustReactivated = false;
		return;
	}
	
	// loop through all of the opened files to get the files to
	// be checked
	// be careful to skip new buffers since they are not yet
	// in the file system
	std::vector<t4p::FileModifiedTimeClass> filesToPoll;
	for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
		t4p::CodeControlClass* ctrl = notebook->GetCodeControl(i);
		if (!ctrl->IsNew()) {
			wxString ctrlFileName = ctrl->GetFileName();		
			t4p::FileModifiedTimeClass modTime;
			modTime.FileName.Assign(ctrlFileName);
			modTime.ModifiedTime = ctrl->GetFileOpenedDateTime();
			filesToPoll.push_back(modTime);
		}
	}
	if (!filesToPoll.empty()) {
		t4p::FileModifiedCheckActionClass* action = new t4p::FileModifiedCheckActionClass(App.RunningThreads,
			ID_FILE_MODIFIED_ACTION);
		action->SetFiles(filesToPoll);
		App.RunningThreads.Queue(action);
	}
	else {
		JustReactivated = false;
	}
}

void t4p::FileModifiedCheckFeatureClass::OnFileModifiedPollComplete(t4p::FilesModifiedEventClass& event) {
	if (event.Modified.empty() && event.Deleted.empty()) {
		JustReactivated = false;
		return;
	}

	// find the code control object for the modified/delete file
	std::map<wxString, t4p::CodeControlClass*> filesModifiedToPrompt;
	std::map<wxString, t4p::CodeControlClass*> filesDeletedToPrompt;
	std::map<wxString, int> filesDeleted;

	t4p::NotebookClass* notebook = GetNotebook();
	for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
		t4p::CodeControlClass* ctrl = notebook->GetCodeControl(i);
		wxFileName ctrlFileName(ctrl->GetFileName());
		if (std::find(event.Modified.begin(), event.Modified.end(), ctrl->GetFileName()) != event.Modified.end()) {
			filesModifiedToPrompt[ctrl->GetFileName()] = ctrl;
		}
		else if (std::find(event.Deleted.begin(), event.Deleted.end(), ctrl->GetFileName()) != event.Deleted.end()) {
			filesDeletedToPrompt[ctrl->GetFileName()] = ctrl;
			filesDeleted[ctrl->GetFileName()] = 1;
		}
	}	
	if (!filesModifiedToPrompt.empty()) {
		FilesModifiedPrompt(filesModifiedToPrompt);
	}
	if (!filesDeletedToPrompt.empty()) {
		FilesDeletedPrompt(filesDeletedToPrompt, filesDeleted);
	}
	JustReactivated = false;
}

void t4p::FileModifiedCheckFeatureClass::OnActivateApp(wxCommandEvent& event) {
	if (!JustReactivated) {
		JustReactivated = true;
		OpenedCodeControlCheck();
	}
}

BEGIN_EVENT_TABLE(t4p::FileModifiedCheckFeatureClass, t4p::FeatureClass)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_ACTIVATED, t4p::FileModifiedCheckFeatureClass::OnActivateApp)
	EVT_FILES_EXTERNALLY_MODIFIED_COMPLETE(ID_FILE_MODIFIED_ACTION, t4p::FileModifiedCheckFeatureClass::OnFileModifiedPollComplete)
END_EVENT_TABLE()
