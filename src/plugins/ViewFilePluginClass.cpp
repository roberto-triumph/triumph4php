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
#include <environment/UrlResourceClass.h>
#include <MvcEditor.h>
#include <MvcEditorAssets.h>
#include <MvcEditorErrors.h>
#include <MvcEditorString.h>
#include <wx/artprov.h>

static const int ID_VIEW_FILE_PANEL = wxNewId();
static const int ID_CALL_STACK_THREAD = wxNewId();

const wxEventType mvceditor::EVENT_CALL_STACK_COMPLETE = wxNewEventType();

mvceditor::CallStackCompleteEventClass::CallStackCompleteEventClass(int eventId, mvceditor::CallStackClass::Errors error, bool writeError)
	: wxEvent(eventId, mvceditor::EVENT_CALL_STACK_COMPLETE)
	, LastError(error)
	, WriteError(writeError) {
}

wxEvent* mvceditor::CallStackCompleteEventClass::Clone() const {
	return new mvceditor::CallStackCompleteEventClass(GetId(), LastError, WriteError);
}

mvceditor::CallStackThreadClass::CallStackThreadClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ThreadWithHeartbeatClass(runningThreads, eventId) 
	, CallStack(NULL)
	, PersistFile() {
		
}

void mvceditor::CallStackThreadClass::InitCallStack(mvceditor::ResourceCacheClass& resourceCache) {
	if (!CallStack.get()) {
		CallStack.reset(new mvceditor::CallStackClass(resourceCache));
	}
}

bool mvceditor::CallStackThreadClass::InitThread(const wxFileName& startFileName, const UnicodeString& className, const UnicodeString& methodName,
												 pelet::Versions version, const wxFileName& persistFile, wxThreadIdType& threadId) {
	bool ret = false;

	// make sure to set these BEFORE calling CreateSingleInstance
	// in order to prevent Entry from reading them while we write to them
	StartFileName = startFileName;
	ClassName = className;
	MethodName = methodName;
	Version = version;
	PersistFile = persistFile;
	wxThreadError threadError = CreateSingleInstance(threadId);
	if (threadError == wxTHREAD_NO_RESOURCE) {
		mvceditor::EditorLogError(mvceditor::LOW_RESOURCES);
	}
	else if (threadError == wxTHREAD_RUNNING) {
		mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER, _("Call stack generator is already running. Please wait for it to finish."));
	}
	else {	
		ret = true;
	}
	return ret;
}

void mvceditor::CallStackThreadClass::BackgroundWork() {
	mvceditor::CallStackClass::Errors lastError = mvceditor::CallStackClass::NONE;
	bool writeError = false;
	
	// build the call stack then save it to a temp file
	if (CallStack->Build(StartFileName, ClassName, MethodName, Version, lastError)) {
		if (!PersistFile.IsOk()) {
			mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER, _("Could not create call stack file in ") + PersistFile.GetFullPath());
			writeError = true;
		}
		else if (!CallStack->Persist(PersistFile)) {
			mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER, _("Could not persist call stack file in ") + PersistFile.GetFullPath());
			writeError = true;
		}
	}
	if (!TestDestroy()) {

		// PostEvent() will set the proper EventId
		mvceditor::CallStackCompleteEventClass evt(wxID_ANY, lastError, writeError);
		PostEvent(evt);
	}
}

mvceditor::ViewFilePluginClass::ViewFilePluginClass(mvceditor::AppClass& app) 
	: PluginClass(app) 
	, FrameworkDetector(*this, app.RunningThreads, app.Structs.Environment) 
	, CallStackPersistFile() 
	, LastError(mvceditor::CallStackClass::NONE)
	, WriteError(false) {
	App.RunningThreads.AddEventHandler(this);
}

void mvceditor::ViewFilePluginClass::AddViewMenuItems(wxMenu* viewMenu) {
	viewMenu->Append(mvceditor::MENU_VIEW_FILES + 0, _("Views"), 
		_("Shows the view (template) files for the currently selected URL"), wxITEM_NORMAL);
}

void mvceditor::ViewFilePluginClass::OnViewInfosMenu(wxCommandEvent& event) {
	if (!App.Structs.UrlResourceFinder.Urls.empty()) {
		ShowPanel();
	}
	else {
		
		// we need the resource cache; the resource cache is needed to figure out the URLs
		// and templates for each controller.
		// we will trigger the project indexing, then once the project has been indexed 
		// we will show the panel.
		State = INDEXING;
		wxCommandEvent indexEvent(mvceditor::EVENT_CMD_PROJECT_URLS);
		App.EventSink.Publish(indexEvent);
	}
}

void mvceditor::ViewFilePluginClass::OnProjectUrls(wxCommandEvent& event) {
	
	// if we triggered the indexing
	if (INDEXING == State) {
		ShowPanel();	
	}
	State = FREE;
}

void mvceditor::ViewFilePluginClass::ShowPanel() {
	wxWindow* window = FindOutlineWindow(ID_VIEW_FILE_PANEL);
	mvceditor::ViewFilePanelClass* viewPanel = NULL;
	if (window) {
		viewPanel = wxDynamicCast(window, mvceditor::ViewFilePanelClass);
		SetFocusToOutlineWindow(viewPanel);
	}
	else {
		viewPanel = new mvceditor::ViewFilePanelClass(GetOutlineNotebook(), ID_VIEW_FILE_PANEL, *this);
		AddOutlineWindow(viewPanel, _("Views"));
	}
	viewPanel->UpdateControllers();
}

wxString mvceditor::ViewFilePluginClass::CurrentFile() {
	mvceditor::CodeControlClass* ctrl = GetCurrentCodeControl();
	wxString fileName;
	if (ctrl) {
		fileName = ctrl->GetFileName();
	}
	return fileName;
}

void mvceditor::ViewFilePluginClass::StartDetection() {	
	
	// start the chain reaction
	wxString url = App.Structs.CurrentUrl.Url.BuildURI();
	if (!url.IsEmpty()) {
		wxFileName fileName = App.Structs.CurrentUrl.FileName;
		if (fileName.IsOk()) {
			UnicodeString className = mvceditor::WxToIcu(App.Structs.CurrentUrl.ClassName);
			UnicodeString methodName =  mvceditor::WxToIcu(App.Structs.CurrentUrl.MethodName);
			CallStackPersistFile.AssignTempFileName(mvceditor::TempDirAsset().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) + wxT("call_stack"));
			
			// TODO: this isn't good, resource cache is not meant to be read/written to from multiple threads
			// this pointer will delete itself when the thread terminates
			mvceditor::CallStackThreadClass* thread = 
				new mvceditor::CallStackThreadClass(App.RunningThreads, ID_CALL_STACK_THREAD);
			thread->InitCallStack(*GetResourceCache());
			wxThreadIdType threadId;
			if (!thread->InitThread(fileName, className, methodName, GetEnvironment()->Php.Version, CallStackPersistFile, threadId)) {
				mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, _("Call stack file creation failed"));
				delete thread;
			}
			else {
				App.Structs.CurrentViewInfos.clear();
			}				
		}
	}
}

void mvceditor::ViewFilePluginClass::OnCallStackComplete(mvceditor::CallStackCompleteEventClass& event) {
	if (!App.Structs.Frameworks.empty()) {
		if (!FrameworkDetector.InitViewInfosDetector(App.Structs.Frameworks, App.Structs.UrlResourceFinder.ChosenUrl.Url.BuildURI(), CallStackPersistFile)) {
			mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, _("Could not start ViewInfos detector"));
		}
	}
	LastError = event.LastError;
	WriteError = event.WriteError;
}
	
void mvceditor::ViewFilePluginClass::OnViewInfosDetectionComplete(mvceditor::ViewInfosDetectedEventClass& event) {
	wxWindow* window = FindOutlineWindow(ID_VIEW_FILE_PANEL);
	App.Structs.CurrentViewInfos = event.GetViewInfos();
	ViewFilePanelClass* viewPanel = NULL;
	if (window) {
		viewPanel = (ViewFilePanelClass*)window;
		viewPanel->UpdateResults();
		SetFocusToToolsWindow(viewPanel);
	}
	wxRemoveFile(CallStackPersistFile.GetFullPath());
}

void mvceditor::ViewFilePluginClass::OnViewInfosDetectionFailed(wxCommandEvent& event) {
	mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, event.GetString());
	wxRemoveFile(CallStackPersistFile.GetFullPath());
}

mvceditor::UrlResourceFinderClass& mvceditor::ViewFilePluginClass::Urls() {
	return App.Structs.UrlResourceFinder;
}

void mvceditor::ViewFilePluginClass::OpenFile(wxString file) {
	wxFileName fileName(file);
	if (fileName.IsOk()) {
		GetNotebook()->LoadPage(fileName.GetFullPath());
	}
}

void mvceditor::ViewFilePluginClass::SetCurrentUrl(mvceditor::UrlResourceClass url) {
	App.Structs.CurrentUrl = url;
}

std::vector<mvceditor::ViewInfoClass> mvceditor::ViewFilePluginClass::CurrentViewInfos() {
	return App.Structs.CurrentViewInfos;
}

mvceditor::ViewFilePanelClass::ViewFilePanelClass(wxWindow* parent, int id, mvceditor::ViewFilePluginClass& plugin)
	: ViewFilePanelGeneratedClass(parent, id)
	, Plugin(plugin) {
	StatusLabel->SetLabel(_(""));
	HelpButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_HELP, 
		wxART_TOOLBAR, wxSize(16, 16))));
}

void mvceditor::ViewFilePanelClass::UpdateControllers() {
	wxArrayString controllers;
	mvceditor::UrlResourceFinderClass& urls = Plugin.Urls();
	std::vector<mvceditor::UrlResourceClass>::iterator it;
	for (it = urls.Urls.begin(); it != urls.Urls.end(); ++it) {
		if (wxNOT_FOUND == controllers.Index(it->ClassName)) {
			controllers.Add(it->ClassName);
		}
	}
	controllers.Sort();
	
	Controller->Clear();
	Controller->Append(controllers);
}

void mvceditor::ViewFilePanelClass::UpdateResults() {
	if (Plugin.LastError == mvceditor::CallStackClass::NONE || Plugin.LastError == mvceditor::CallStackClass::RESOLUTION_ERROR) {
		
		std::vector<mvceditor::ViewInfoClass> currentViewInfos = Plugin.CurrentViewInfos();
		StatusLabel->SetLabel(wxString::Format(_("Found %d view files"), currentViewInfos.size()));
		FileTree->DeleteAllItems();

		wxTreeItemId parent = FileTree->AddRoot(_("Views"));
		for (size_t i = 0; i < currentViewInfos.size(); ++i) {
			wxString viewFile = currentViewInfos[i].FileName;

			// remove the project root so that the dialog is not too 'wordy'
			wxString projectLabel;
			wxString text = Plugin.App.Structs.RelativeFileName(viewFile, projectLabel);
			if (!wxFileName::FileExists(viewFile)) {

				// show that the view file is missing
				text = wxT("[X] ") + text;
			}
			mvceditor::StringTreeItemDataClass* data = new mvceditor::StringTreeItemDataClass(currentViewInfos[i].FileName);
			FileTree->AppendItem(parent, text, -1, -1, data);
		}
		FileTree->ExpandAll();
		
		int variableCount = 0;
		for (size_t i = 0; i < currentViewInfos.size(); ++i) {
			variableCount += currentViewInfos[i].TemplateVariables.size();
		}
		TemplateVariablesLabel->SetLabel(wxString::Format(_("Found %d template variables"),variableCount));
		TemplateVariablesTree->DeleteAllItems();
		parent = TemplateVariablesTree->AddRoot(_("Template Variables"));
		for (size_t i = 0; i < currentViewInfos.size(); ++i) {
			mvceditor::ViewInfoClass viewInfo = currentViewInfos[i];
			wxString text = viewInfo.FileName;
			wxString projectLabel;
			
			// remove the project root so that the dialog is not too 'wordy'
			text = Plugin.App.Structs.RelativeFileName(text, projectLabel);
			wxTreeItemId sub = TemplateVariablesTree->AppendItem(parent, text);
			for (size_t j = 0; j < viewInfo.TemplateVariables.size(); ++j) {
				TemplateVariablesTree->AppendItem(sub, viewInfo.TemplateVariables[j]);
			}
		}
		TemplateVariablesTree->ExpandAll();
	}
	else if (Plugin.WriteError) {
		StatusLabel->SetLabel(_("Error"));
		mvceditor::EditorLogError(mvceditor::WARNING_OTHER, 
			_("Could not Write Call stack file to file system"));
	}
	else {
		switch (Plugin.LastError) {
			case mvceditor::CallStackClass::PARSE_ERR0R:
				StatusLabel->SetLabel(_("The controller file has a syntax error."));
				break;
			case mvceditor::CallStackClass::RESOURCE_NOT_FOUND:
				StatusLabel->SetLabel(_("The controller file has a syntax error."));
				break;
			case mvceditor::CallStackClass::EMPTY_CACHE:
				StatusLabel->SetLabel(_("No URLs have been detected. Index your project."));
				break;
			case mvceditor::CallStackClass::STACK_LIMIT:
				StatusLabel->SetLabel(_("The editor hit a stack recursion limit."));
				break;
			default:
			
				// NONE or RESOLUTION_ERROR are not errors
				break;
		}
	}
}

void mvceditor::ViewFilePanelClass::ClearResults() {
	StatusLabel->SetLabel(_(""));
	Action->Clear();
	FileTree->DeleteAllItems();
}

void mvceditor::ViewFilePanelClass::OnActionChoice(wxCommandEvent& event) {
	wxString controller = Controller->GetStringSelection();
	wxString action = Action->GetStringSelection();

	mvceditor::UrlResourceFinderClass& urls = Plugin.Urls();
	mvceditor::UrlResourceClass url;
	std::vector<mvceditor::UrlResourceClass>::iterator it;
	for (it = urls.Urls.begin(); it != urls.Urls.end(); ++it) {
		if (it->ClassName.CmpNoCase(controller) == 0 && it->MethodName.CmpNoCase(action) == 0) {
			url = *it;
			break;
		}
	}
	if (!url.Url.BuildURI().IsEmpty()) {
		StatusLabel->SetLabel(_("Detecting"));
		Plugin.SetCurrentUrl(url);
		Plugin.StartDetection();
	}
}

void mvceditor::ViewFilePanelClass::OnControllerChoice(wxCommandEvent &event) {
	ClearResults();

	wxString controller = event.GetString();
	mvceditor::UrlResourceFinderClass& urls = Plugin.Urls();
	std::vector<mvceditor::UrlResourceClass>::iterator it;
	for (it = urls.Urls.begin(); it != urls.Urls.end(); ++it) {
		if (it->ClassName.CmpNoCase(controller) == 0) {
			Action->AppendString(it->MethodName);
		}
	}
}

void mvceditor::ViewFilePanelClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = wxString::FromAscii(
		"The views outline lets you see the views (templates) for a "
		"specific controller. Choose a controller, then an action, and "
		"the pane will be populated with all of the templates that the "
		"selected action uses.  If a template file has an [X] it means "
		"that the template file was not found."
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("View Files Help"), wxOK, this);
}

void mvceditor::ViewFilePanelClass::OnCurrentButton(wxCommandEvent &event) {
	ClearResults();
	UpdateControllers();

	wxString current = Plugin.CurrentFile();
	wxFileName currentFileName(current);
	mvceditor::UrlResourceFinderClass& urls = Plugin.Urls();
	std::vector<mvceditor::UrlResourceClass>::iterator it;
	for (it = urls.Urls.begin(); it != urls.Urls.end(); ++it) {
		if (it->FileName == currentFileName) {
			
			// select the controller that is located inside of the 
			// currently opened file
			for (size_t i = 0; i < Controller->GetCount(); ++i) {
				if (Controller->GetString(i) == it->ClassName) {
					Controller->SetSelection(i);

					std::vector<mvceditor::UrlResourceClass>::iterator itAction;
					for (itAction = urls.Urls.begin(); itAction != urls.Urls.end(); ++itAction) {
						if (itAction ->ClassName.CmpNoCase(it->ClassName) == 0) {
							Action->AppendString(itAction ->MethodName);
						}
					}
					break;
				}
			}
			break;
		}
	}
}

void mvceditor::ViewFilePanelClass::OnTreeItemActivated(wxTreeEvent& event) {
	wxTreeItemId item = event.GetItem();
	wxString file = FileTree->GetItemText(item);
	if (!file.IsEmpty() && !file.Find(wxT("[X]")) == 0 && item != FileTree->GetRootItem()) {
		mvceditor::StringTreeItemDataClass* data = (mvceditor::StringTreeItemDataClass*)FileTree->GetItemData(item);
		if (data) {
			file =  data->Str;
			Plugin.OpenFile(file);
		}
	}
}

mvceditor::StringTreeItemDataClass::StringTreeItemDataClass(const wxString& str)
	: wxTreeItemData()
	, Str(str) {

}

BEGIN_EVENT_TABLE(mvceditor::ViewFilePluginClass, wxEvtHandler) 
	EVT_CALL_STACK_COMPLETE(ID_CALL_STACK_THREAD, mvceditor::ViewFilePluginClass::OnCallStackComplete)
	EVT_FRAMEWORK_VIEW_INFOS_COMPLETE(mvceditor::ViewFilePluginClass::OnViewInfosDetectionComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FRAMEWORK_VIEW_FILES_FAILED, mvceditor::ViewFilePluginClass::OnViewInfosDetectionFailed)
	EVT_MENU(mvceditor::MENU_VIEW_FILES + 0, mvceditor::ViewFilePluginClass::OnViewInfosMenu)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PROJECT_URLS, mvceditor::ViewFilePluginClass::OnProjectUrls)
END_EVENT_TABLE()
