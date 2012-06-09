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
#include <wx/artprov.h>

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
	LastError = mvceditor::CallStackClass::NONE;
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
		ret = true;
		SignalStart();
	}
	return ret;
}

void mvceditor::CallStackThreadClass::Entry() {
	
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
	}
	SignalEnd();
}

mvceditor::ViewFilePluginClass::ViewFilePluginClass() 
	: PluginClass() 
	, FrameworkDetector(NULL) 
	, CallStackThread(*this) 
	, CurrentViewInfos() 
	, State(FREE) {
}

void mvceditor::ViewFilePluginClass::AddToolsMenuItems(wxMenu* toolsMenu) {
	toolsMenu->Append(mvceditor::MENU_VIEW_FILES + 0, _("Views"), 
		_("Shows the view (template) files for the currently selected URL"), wxITEM_NORMAL);
}

void mvceditor::ViewFilePluginClass::OnViewInfosMenu(wxCommandEvent& event) {
	if (!App->UrlResourceFinder.Urls.empty()) {
		ShowPanel();
	}
	else {
		// we need the resource cache; the resource cache is needed to figure out the URLs
		// and templates for each controller.
		// we will trigger the project indexing, then once the project has been indexed 
		// we will show the panel.
		State = INDEXING;
		wxCommandEvent indexEvent(mvceditor::EVENT_CMD_RE_INDEX);
		App->EventSink.Publish(indexEvent);
	}
}

void mvceditor::ViewFilePluginClass::OnProjectIndexed(wxCommandEvent& event) {
	
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
	wxString url = ChosenUrl.Url.BuildURI();
	if (!url.IsEmpty()) {
		wxFileName fileName = ChosenUrl.FileName;
		if (fileName.IsOk()) {
			UnicodeString className = mvceditor::StringHelperClass::wxToIcu(ChosenUrl.ClassName);
			UnicodeString methodName =  mvceditor::StringHelperClass::wxToIcu(ChosenUrl.MethodName);
			CallStackThread.InitCallStack(App->ResourceCache);
			if (!CallStackThread.InitThread(fileName, className, methodName)) {
				mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, _("Call stack file creation failed"));
			}
			else {
				CurrentViewInfos.clear();
			}				
		}
	}
}

void mvceditor::ViewFilePluginClass::OnWorkComplete(wxCommandEvent& event) {
	if (!FrameworkDetector.get()) {
		FrameworkDetector.reset(new mvceditor::PhpFrameworkDetectorClass(*this, *GetEnvironment()));
	}
	FrameworkDetector->Identifiers = PhPFrameworks().Identifiers;
	if (!FrameworkDetector->InitViewInfosDetector(GetProject()->GetRootPath(), App->UrlResourceFinder.ChosenUrl.Url.BuildURI(), CallStackThread.StackFile)) {
		mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, _("Could not start ViewInfos detector"));
	}
}
	
void mvceditor::ViewFilePluginClass::OnViewInfosDetectionComplete(mvceditor::ViewInfosDetectedEventClass& event) {
	wxWindow* window = FindOutlineWindow(ID_VIEW_FILE_PANEL);
	CurrentViewInfos = event.ViewInfos;
	ViewFilePanelClass* viewPanel = NULL;
	if (window) {
		viewPanel = (ViewFilePanelClass*)window;
		viewPanel->UpdateResults();
		SetFocusToToolsWindow(viewPanel);
	}
	wxRemoveFile(CallStackThread.StackFile.GetFullPath());
}

void mvceditor::ViewFilePluginClass::OnViewInfosDetectionFailed(wxCommandEvent& event) {
	mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, event.GetString());
	wxRemoveFile(CallStackThread.StackFile.GetFullPath());
}

mvceditor::UrlResourceFinderClass& mvceditor::ViewFilePluginClass::Urls() {
	return App->UrlResourceFinder;
}

void mvceditor::ViewFilePluginClass::OpenFile(wxString file) {
	wxFileName fileName(file);
	if (fileName.IsOk()) {
		GetNotebook()->LoadPage(fileName.GetFullPath());
	}
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
	if (Plugin.CallStackThread.LastError == mvceditor::CallStackClass::NONE || Plugin.CallStackThread.LastError == mvceditor::CallStackClass::RESOLUTION_ERROR) {
		
		StatusLabel->SetLabel(wxString::Format(_("Found %d view files"), Plugin.CurrentViewInfos.size()));
		FileTree->DeleteAllItems();

		wxTreeItemId parent = FileTree->AddRoot(_("Views"));
		mvceditor::ProjectClass* project = Plugin.GetProject();
		wxString root = project->GetRootPath();
		for (size_t i = 0; i < Plugin.CurrentViewInfos.size(); ++i) {
			wxString viewFile = Plugin.CurrentViewInfos[i].FileName;

			// remove the project root so that the dialog is not too 'wordy'
			wxString text = viewFile.Mid(root.Len());
			if (!wxFileName::FileExists(viewFile)) {

				// show that the view file is missing
				text = wxT("[X] ") + text;
			}
			FileTree->AppendItem(parent, text);
		}
		FileTree->ExpandAll();
		
		int variableCount = 0;
		for (size_t i = 0; i < Plugin.CurrentViewInfos.size(); ++i) {
			variableCount += Plugin.CurrentViewInfos[i].TemplateVariables.size();
		}
		TemplateVariablesLabel->SetLabel(wxString::Format(_("Found %d template variables"),variableCount));
		TemplateVariablesTree->DeleteAllItems();
		parent = TemplateVariablesTree->AddRoot(_("Template Variables"));
		for (size_t i = 0; i < Plugin.CurrentViewInfos.size(); ++i) {
			mvceditor::ViewInfoClass viewInfo = Plugin.CurrentViewInfos[i];
			wxString text = viewInfo.FileName;
			
			// remove the project root so that the dialog is not too 'wordy'
			text = text.Mid(root.Len());
			wxTreeItemId sub = TemplateVariablesTree->AppendItem(parent, text);
			for (size_t j = 0; j < viewInfo.TemplateVariables.size(); ++j) {
				TemplateVariablesTree->AppendItem(sub, viewInfo.TemplateVariables[j]);
			}
		}
		TemplateVariablesTree->ExpandAll();
	}
	else if (Plugin.CallStackThread.WriteError) {
		StatusLabel->SetLabel(_("Error"));
		mvceditor::EditorLogError(mvceditor::WARNING_OTHER, 
			_("Could not Write Call stack file to file system"));
	}
	else {
		switch (Plugin.CallStackThread.LastError) {
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
		Plugin.ChosenUrl = url;
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
	wxMessageBox(help);
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
	if (!file.IsEmpty() && !file.Find(wxT("[X]")) == 0) {
		mvceditor::ProjectClass* project = Plugin.GetProject();
		wxString root = project->GetRootPath();
		file =  root + wxFileName::GetPathSeparator() + file;
		Plugin.OpenFile(file);
	}
}

BEGIN_EVENT_TABLE(mvceditor::ViewFilePluginClass, wxEvtHandler) 
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::ViewFilePluginClass::OnWorkComplete)
	EVT_FRAMEWORK_VIEW_INFOS_COMPLETE(mvceditor::ViewFilePluginClass::OnViewInfosDetectionComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FRAMEWORK_VIEW_FILES_FAILED, mvceditor::ViewFilePluginClass::OnViewInfosDetectionFailed)
	EVT_MENU(mvceditor::MENU_VIEW_FILES + 0, mvceditor::ViewFilePluginClass::OnViewInfosMenu)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PROJECT_INDEXED, mvceditor::ViewFilePluginClass::OnProjectIndexed)
END_EVENT_TABLE()