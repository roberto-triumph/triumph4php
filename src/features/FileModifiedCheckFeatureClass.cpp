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
#include <wx/choicdlg.h>

static int ID_FILE_MODIFIED_CHECK = wxNewId();

mvceditor::FileModifiedCheckFeatureClass::FileModifiedCheckFeatureClass(mvceditor::AppClass& app)
: FeatureClass(app)
, Timer(this, ID_FILE_MODIFIED_CHECK) {
}

void mvceditor::FileModifiedCheckFeatureClass::OnAppReady(wxCommandEvent& event) {
	Timer.Start(1000, wxTIMER_CONTINUOUS);
}

void mvceditor::FileModifiedCheckFeatureClass::OnAppExit(wxCommandEvent& event) {
	Timer.Stop();
}

void mvceditor::FileModifiedCheckFeatureClass::OnTimer(wxTimerEvent& event) {
	mvceditor::NotebookClass* notebook = GetNotebook();
	size_t size = notebook->GetPageCount();
	if (0 == size) {
		return;
	}
	std::vector<mvceditor::FileModifiedTimeClass> fileMods;

	// loop through all of the opened files to get the files to
	// be checked
	// no need to check new files as they are not yet in the file system
	for (size_t i = 0; i < size; ++i) {
		mvceditor::CodeControlClass* ctrl = notebook->GetCodeControl(i);
		if (ctrl && !ctrl->IsNew()) {
			mvceditor::FileModifiedTimeClass fileMod;
			fileMod.FileName.Assign(ctrl->GetFileName());
			fileMod.ModifiedTime = ctrl->GetFileOpenedDateTime();
			fileMods.push_back(fileMod);
		}
	}
	if (!fileMods.empty()) {
		
		// stop the timer that way we dont check files again until the user answers some questions
		Timer.Stop();
		mvceditor::FileModifiedCheckActionClass* action = 
				new mvceditor::FileModifiedCheckActionClass(App.RunningThreads, ID_FILE_MODIFIED_CHECK);
		action->SetFiles(fileMods);
		App.RunningThreads.Queue(action);
	}
}

void mvceditor::FileModifiedCheckFeatureClass::OnFilesCheckComplete(mvceditor::FilesModifiedEventClass& event) {
	mvceditor::NotebookClass* notebook = GetNotebook();
	size_t size = notebook->GetPageCount();
	if (size > 0 && (event.Modified.size() + event.Deleted.size()) > 0) {
		if (!event.Modified.empty()) {
			OnFilesModified(event);
		}
		if (!event.Deleted.empty()) {
			OnFilesDeleted(event);
		}
	}
	Timer.Start(3000, wxTIMER_CONTINUOUS);
}

void mvceditor::FileModifiedCheckFeatureClass::OnFilesModified(mvceditor::FilesModifiedEventClass& event) {
	mvceditor::NotebookClass* notebook = GetNotebook();
	size_t pageCount = notebook->GetPageCount();

	// get all opened files we need to modify all controls that don't have new files
	std::vector<wxString> allOpenedFiles;
	for (size_t i = 0; i < pageCount; ++i) {
		mvceditor::CodeControlClass* ctrl = notebook->GetCodeControl(i);
		if (ctrl && !ctrl->IsNew()) {
			allOpenedFiles.push_back(ctrl->GetFileName());
		}
	}

	wxString msg;
	if (event.Modified.size() == 1) {
		msg = _("1 File have been modified externally. Reload file and lose any changes?\n");
		msg += _("If checked, the file will be reloaded. If left unchecked, the file will not be reloaded, allowing you to overwrite the file.");
	}
	else {
		msg = _("Files have been modified externally. Reload files and lose any changes?\n");
		msg += _("The checked files will be reloaded. Unchecked files will not be reloaded, allowing you to overwrite the files.");
	}
	wxArrayString choices;
	for (std::vector<wxFileName>::const_iterator file = event.Modified.begin(); file != event.Modified.end(); ++ file) {
		choices.Add(file->GetFullName() + wxT(" - ") + file->GetFullPath());
	}

	wxMultiChoiceDialog  dialog(GetMainWindow(), msg, _("Files Externally Modified"), 
		choices, wxOK | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	dialog.ShowModal();
	wxArrayInt selections = dialog.GetSelections();

	// the code below relies on there being only one code control for each file
	for (size_t i = 0; i < selections.size(); ++i) {
		wxFileName fileName = event.Modified[selections[i]];

		// find the control for the file and revert the contents
		for (size_t i = 0; i < pageCount; ++i) {
			mvceditor::CodeControlClass* ctrl = notebook->GetCodeControl(i);
			if (ctrl && ctrl->GetFileName() == fileName.GetFullPath()) {
				ctrl->Revert();
				break;
			}
		}

		// signal the control as handled
		std::vector<wxString>::iterator it = 
			std::find(allOpenedFiles.begin(), allOpenedFiles.end(), fileName.GetFullPath());
		if (it != allOpenedFiles.end()) {
			allOpenedFiles.erase(it);
		}
	}

	// now handle the files that the user did NOT want to reload
	std::vector<wxString>::iterator it;
	for (it = allOpenedFiles.begin(); it != allOpenedFiles.end(); ++it) {

		// find the date time for the file. dont use wxFileName, instead use the
		// time that was already fetched in the background thread
		// this is because getting the modification time may take a while
		// for shared files (mapped drives in windows)
		wxDateTime newModifiedTime;
		for (size_t j = 0; j < event.Modified.size(); ++j) {
			if (event.Modified[j] == *it) {
				newModifiedTime = event.ModifiedTimes[j];
				break;
			}
		}
		
		// if a file was not externally modified we wont find a date time 
		// then just skip this file and dont update the opened date time
		if (newModifiedTime.IsValid()) {

			// find the control for the file
			for (size_t i = 0; i < pageCount; ++i) {
				mvceditor::CodeControlClass* ctrl = notebook->GetCodeControl(i);
				if (ctrl && ctrl->GetFileName() == *it) {
					ctrl->UpdateOpenedDateTime(newModifiedTime);
					break;
				}
			}
		}
	}
}

void mvceditor::FileModifiedCheckFeatureClass::OnFilesDeleted(mvceditor::FilesModifiedEventClass& event) {
	mvceditor::NotebookClass* notebook = GetNotebook();
	size_t pageCount = notebook->GetPageCount();

	// get all opened files we need to modify all controls that don't have new files
	std::vector<wxString> allOpenedFiles;
	for (size_t i = 0; i < pageCount; ++i) {
		mvceditor::CodeControlClass* ctrl = notebook->GetCodeControl(i);
		if (ctrl && !ctrl->IsNew()) {
			allOpenedFiles.push_back(ctrl->GetFileName());
		}
	}
	std::vector<wxFileName>::const_iterator file;
	wxString files;
	for (file = event.Deleted.begin(); file != event.Deleted.end(); ++ file) {
		files += file->GetFullPath() + wxT("\n");
	
		// find the control for the file
		for (size_t i = 0; i < pageCount; ++i) {
			mvceditor::CodeControlClass* ctrl = notebook->GetCodeControl(i);
			if (ctrl && ctrl->GetFileName() == file->GetFullPath()) {
				ctrl->TreatAsNew();
				break;
			}
		}
	}
	wxString message;
	message += _("The following files have been deleted externally.\n");
	message += _("You will need to save the file to store the contents.\n\n");
	message += files;
	int opts = wxICON_QUESTION;
	wxMessageBox(message, _("Warning"), opts, GetMainWindow());
}

BEGIN_EVENT_TABLE(mvceditor::FileModifiedCheckFeatureClass, mvceditor::FeatureClass)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_READY, mvceditor::FileModifiedCheckFeatureClass::OnAppReady)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_EXIT, mvceditor::FileModifiedCheckFeatureClass::OnAppExit)
	EVT_TIMER(ID_FILE_MODIFIED_CHECK, mvceditor::FileModifiedCheckFeatureClass::OnTimer)
	EVT_FILES_EXTERNALLY_MODIFIED_COMPLETE(ID_FILE_MODIFIED_CHECK, mvceditor::FileModifiedCheckFeatureClass::OnFilesCheckComplete)
END_EVENT_TABLE()