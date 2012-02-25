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
#include <plugins/ViewFilePluginClass.h>
#include <MvcEditor.h>
#include <MvcEditorErrors.h>
#include <environment/UrlResourceClass.h>
#include <windows/StringHelperClass.h>

static const int ID_VIEW_FILE_PANEL = wxNewId();

mvceditor::CallStackThreadClass::CallStackThreadClass(wxEvtHandler& handler)
	: ThreadWithHeartbeatClass(handler) 
	, CallStack(NULL)
	, LastError(mvceditor::CallStackClass::NONE)
	, StackFile() {
		
}

void mvceditor::CallStackThreadClass::InitCallStack(mvceditor::ResourceCacheClass& resourceCache) {
	if (!CallStack.get()) {
		CallStack.reset(new mvceditor::CallStackClass(resourceCache));
	}
}

bool mvceditor::CallStackThreadClass::InitThread(const wxFileName& startFileName, const UnicodeString& className, const UnicodeString& methodName) {
	StackFile.Clear();
	WriteError = false;
	bool ret = false;
	wxThreadError threadError = CreateSingleInstance();
	if (threadError == wxTHREAD_NO_RESOURCE) {
		mvceditor::EditorLogError(mvceditor::LOW_RESOURCES);
	}
	else if (threadError == wxTHREAD_RUNNING) {
		mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER, _("Call stack generator is already running. Please wait for it to finish."));
	}
	else {
		StartFileName = startFileName;
		ClassName = className;
		MethodName = methodName;
		GetThread()->Run();
		ret = true;
		SignalStart();
	}
	return ret;
}

void* mvceditor::CallStackThreadClass::Entry() {
	
	// build the call stack then save it to a temp file
	if (CallStack->Build(StartFileName, ClassName, MethodName, LastError)) {
		StackFile.AssignTempFileName(wxT("call_stack"));
		if (!StackFile.IsOk()) {
			mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER, _("Could not create call stack file in ") + StackFile.GetFullPath());
			WriteError = true;
		}
		else if (!CallStack->Persist(StackFile)) {
			mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER, _("Could not persist call stack file in ") + StackFile.GetFullPath());
			WriteError = true;
		}
		SignalEnd();
	}
	return 0;
}

mvceditor::ViewFilePluginClass::ViewFilePluginClass() 
	: PluginClass() 
	, FrameworkDetector(NULL) 
	, CallStackThread(*this) 
	, CurrentViewFiles() {
}

void mvceditor::ViewFilePluginClass::AddToolsMenuItems(wxMenu* toolsMenu) {
	toolsMenu->Append(mvceditor::MENU_VIEW_FILES + 0, _("URL View (template) files"), 
		_("Shows the view (template) files for the currently selected URL"), wxITEM_NORMAL);
}

void mvceditor::ViewFilePluginClass::OnViewFilesMenu(wxCommandEvent& event) {
	
	wxWindow* window = FindToolsWindow(ID_VIEW_FILE_PANEL);
	mvceditor::ViewFilePanelClass* viewPanel = NULL;
	if (window) {
		viewPanel = (mvceditor::ViewFilePanelClass*)window;
		viewPanel->UpdateLabels();
		SetFocusToToolsWindow(viewPanel);
	}
	else {
		viewPanel = new mvceditor::ViewFilePanelClass(GetToolsNotebook(), ID_VIEW_FILE_PANEL, *this);
		AddToolsWindow(viewPanel, _("View (template) Files"));
		SetFocusToToolsWindow(viewPanel);
	}
	viewPanel->UpdateTitle(App->UrlResourceFinder.ChosenUrl);

	// start the chain reaction
	wxString url = App->UrlResourceFinder.ChosenUrl.Url;
	if (!url.IsEmpty()) {
		
		// TODO need another detector to go from URL -> class/method
		// also need to go from URL -> file
		wxFileName fileName = App->UrlResourceFinder.ChosenUrl.FileName;
		if (fileName.IsOk()) {
			UnicodeString className = mvceditor::StringHelperClass::wxToIcu(App->UrlResourceFinder.ChosenUrl.ClassName);
			UnicodeString methodName =  mvceditor::StringHelperClass::wxToIcu(App->UrlResourceFinder.ChosenUrl.MethodName);
			CallStackThread.InitCallStack(App->ResourceCache);
			if (!CallStackThread.InitThread(fileName, className, methodName)) {
				mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, _("Call stack file creation failed"));
			}
			else {
				CurrentViewFiles.clear();
			}				
		}
	}
}

void mvceditor::ViewFilePluginClass::OnWorkComplete(wxCommandEvent& event) {
	if (!FrameworkDetector.get()) {
		FrameworkDetector.reset(new mvceditor::PhpFrameworkDetectorClass(*this, *GetEnvironment()));
	}
	FrameworkDetector->Identifiers = PhPFrameworks().Identifiers;
	if (!FrameworkDetector->InitViewFilesDetector(GetProject()->GetRootPath(), App->UrlResourceFinder.ChosenUrl.Url, CallStackThread.StackFile)) {
		mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, _("Could not start viewFiles detector"));
	}
}
	
void mvceditor::ViewFilePluginClass::OnViewFilesDetectionComplete(mvceditor::ViewFilesDetectedEventClass& event) {
	wxWindow* window = FindToolsWindow(ID_VIEW_FILE_PANEL);
	CurrentViewFiles = event.ViewFiles;
	ViewFilePanelClass* viewPanel = NULL;
	if (window) {
		viewPanel = (ViewFilePanelClass*)window;
		viewPanel->UpdateLabels();
		SetFocusToToolsWindow(viewPanel);
	}
}

void mvceditor::ViewFilePluginClass::OnViewFilesDetectionFailed(wxCommandEvent& event) {
	mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, event.GetString());
}

mvceditor::ViewFilePanelClass::ViewFilePanelClass(wxWindow* parent, int id, mvceditor::ViewFilePluginClass& plugin)
	: LintResultsGeneratedPanelClass(parent, id)
	, Plugin(plugin) {
		Label->SetLabel(_("Template Files:"));
}

void mvceditor::ViewFilePanelClass::UpdateLabels() {
	if (Plugin.CallStackThread.LastError != mvceditor::CallStackClass::NONE && Plugin.CallStackThread.LastError != mvceditor::CallStackClass::RESOLUTION_ERROR) {
		ErrorsList->AppendString(wxString::Format(_("Call stack creation error: %d"), Plugin.CallStackThread.LastError));
	}
	else if (Plugin.CallStackThread.WriteError) {
		ErrorsList->AppendString(_("Could not Write Call stack file to file system"));
	}
	else {
		ErrorsList->Clear();
		Label->SetLabel(wxString::Format(_("Found %d view files"), Plugin.CurrentViewFiles.size()));
		for (size_t i = 0; i < Plugin.CurrentViewFiles.size(); ++i) {
			ErrorsList->AppendString(Plugin.CurrentViewFiles[i].GetFullPath());
		}
	}
}

void mvceditor::ViewFilePanelClass::UpdateTitle(const mvceditor::UrlResourceClass& chosenUrl) {
	Label->SetLabel(_("Template Files For:") + chosenUrl.FileName.GetFullPath() + 
		wxT(" (") + chosenUrl.ClassName + wxT("::") + chosenUrl.MethodName + wxT(")"));

}

BEGIN_EVENT_TABLE(mvceditor::ViewFilePluginClass, wxEvtHandler) 
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::ViewFilePluginClass::OnWorkComplete)
	EVT_FRAMEWORK_VIEW_FILES_COMPLETE(mvceditor::ViewFilePluginClass::OnViewFilesDetectionComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FRAMEWORK_VIEW_FILES_FAILED, mvceditor::ViewFilePluginClass::OnViewFilesDetectionFailed)
	EVT_MENU(mvceditor::MENU_VIEW_FILES + 0, mvceditor::ViewFilePluginClass::OnViewFilesMenu)
END_EVENT_TABLE()