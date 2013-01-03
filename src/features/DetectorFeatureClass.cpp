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
#include <features/DetectorFeatureClass.h>
#include <actions/UrlDetectorActionClass.h>
#include <actions/CallStackActionClass.h>
#include <actions/TemplateFilesDetectorActionClass.h>
#include <widgets/TreeItemDataStringClass.h>
#include <widgets/ChooseUrlDialogClass.h>
#include <globals/Assets.h>
#include <MvcEditor.h>
#include <wx/artprov.h>

static const int ID_URL_DETECTOR_PANEL = wxNewId();
static const int ID_TEMPLATE_FILES_DETECTOR_PANEL = wxNewId();

static const int ID_DETECTOR_TREE_OPEN = wxNewId();
static const int ID_DETECTOR_TREE_RENAME = wxNewId();
static const int ID_DETECTOR_TREE_DELETE = wxNewId();


mvceditor::DetectorClass::DetectorClass() {

}

mvceditor::DetectorClass::~DetectorClass() {

}

mvceditor::UrlDetectorClass::UrlDetectorClass()  {

}

bool mvceditor::UrlDetectorClass::CanTest(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project) {
	mvceditor::SourceClass source = project.Sources[0];
	wxString rootUrl = globals.Environment.Apache.GetUrl(source.RootDirectory.GetPath());
	if (rootUrl.IsEmpty()) {
		wxMessageBox(_("Cannot determine the root URL of the selected project. Please add a virtual host mapping for ") + source.RootDirectory.GetPath() +
			_(" under Edit -> Preferences -> Apache"));
		return false;
	}
	return true;
}

wxString mvceditor::UrlDetectorClass::TestCommandLine(const mvceditor::GlobalsClass& globals, 
													  const mvceditor::ProjectClass& project,
													  const wxString& detectorScriptFullPath) {
	mvceditor::SourceClass source = project.Sources[0];
	wxString rootUrl = globals.Environment.Apache.GetUrl(source.RootDirectory.GetPath());

	mvceditor::UrlDetectorParamsClass params;
	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = mvceditor::PhpDetectorsBaseAsset();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	params.ResourceDbFileName = project.ResourceDbFileName;
	params.RootUrl = rootUrl;
	return params.BuildCmdLine();
}

wxFileName mvceditor::UrlDetectorClass::LocalRootDir() {
	return mvceditor::UrlDetectorsLocalAsset();
}

wxFileName mvceditor::UrlDetectorClass::GlobalRootDir() {
	return mvceditor::UrlDetectorsGlobalAsset();
}

wxFileName mvceditor::UrlDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = mvceditor::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("UrlDetector.skeleton.php"));
	return skeletonFile;
}

wxString mvceditor::UrlDetectorClass::Label() {
	return _("URL Detectors");
}

wxString mvceditor::UrlDetectorClass::HelpMessage() {
	wxString help = wxString::FromAscii(
		"URL Detectors are PHP scripts that MVC Editor uses to find out "
		"all of the valid URL routes for your projects.  \n"
		"MVC Editor can detect routes for CodeIgniter projects.\n"
	);
	help = wxGetTranslation(help);
	return help;
}

mvceditor::TemplateFilesDetectorClass::TemplateFilesDetectorClass() 
	: DetectorClass() {

}

bool  mvceditor::TemplateFilesDetectorClass::CanTest(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project) {
	return true;
}

wxString mvceditor::TemplateFilesDetectorClass::TestCommandLine(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project,
																const wxString& detectorScriptFullPath) {
	mvceditor::TemplateFilesDetectorParamsClass params;
	mvceditor::SourceClass source = project.Sources[0];

	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = mvceditor::PhpDetectorsBaseAsset().GetFullPath();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	params.DetectorDbFileName = project.DetectorDbFileName;
	params.OutputDbFileName = wxT("");
	return params.BuildCmdLine();
}

wxFileName mvceditor::TemplateFilesDetectorClass::LocalRootDir() {
	return mvceditor::TemplateFilesDetectorsLocalAsset();
}

wxFileName mvceditor::TemplateFilesDetectorClass::GlobalRootDir() {
	return mvceditor::TemplateFilesDetectorsGlobalAsset();
}

wxFileName mvceditor::TemplateFilesDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = mvceditor::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("TemplateFilesDetector.skeleton.php"));
	return skeletonFile;
}

wxString mvceditor::TemplateFilesDetectorClass::Label() {
	return _("Template File Detectors");
}

wxString mvceditor::TemplateFilesDetectorClass::HelpMessage() {
	wxString help = wxString::FromAscii(
		"Template files detectors are PHP scripts that MVC Editor uses to find out "
		"all of the 'view' files for your projects.  \n"
		"MVC Editor can detect view files for CodeIgniter projects.\n"
	);
	help = wxGetTranslation(help);
	return help;
}

mvceditor::DetectorTreeHandlerClass::DetectorTreeHandlerClass(wxTreeCtrl* detectorTree,
															  wxButton* testButton,
															  wxButton* addButton,
															  wxButton* helpButton,
															  wxChoice* projectChoice,
															  mvceditor::DetectorClass* detector,
															  mvceditor::GlobalsClass& globals, 
															  mvceditor::EventSinkClass& eventSink)
	: wxEvtHandler()
	, DetectorTree(detectorTree)
	, TestButton(testButton)
	, AddButton(addButton)
	, HelpButton(helpButton)
	, ProjectChoice(projectChoice)
	, Detector(detector) 
	, Globals(globals) 
	, EventSink(eventSink) {

	// Connect Events
	TestButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnTestButton), NULL, this);
	AddButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnAddButton), NULL, this);
	HelpButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnHelpButton), NULL, this);
	DetectorTree->Connect(wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemDelete), NULL, this);
	DetectorTree->Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemEndLabelEdit), NULL, this);
	DetectorTree->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemActivated), NULL, this);
	DetectorTree->Connect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemRightClick), NULL, this);
}

mvceditor::DetectorTreeHandlerClass::~DetectorTreeHandlerClass() {
	// Connect Events
	TestButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnTestButton), NULL, this);
	AddButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnAddButton), NULL, this);
	HelpButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnHelpButton), NULL, this);
	DetectorTree->Disconnect(wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemDelete), NULL, this);
	DetectorTree->Disconnect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemEndLabelEdit), NULL, this);
	DetectorTree->Disconnect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemActivated), NULL, this);
	DetectorTree->Disconnect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemRightClick), NULL, this);
}

void mvceditor::DetectorTreeHandlerClass::Init() {
	wxString globalRootDir = Detector->GlobalRootDir().GetFullPath();
	wxString localRootDir = Detector->LocalRootDir().GetFullPath();
	
	DetectorTree->Freeze();
	DetectorTree->DeleteAllItems();
	DetectorTree->AddRoot(Detector->Label());
	wxTreeItemId globalItemId = DetectorTree->AppendItem(DetectorTree->GetRootItem(), _("Global"));
	wxTreeItemId localItemId = DetectorTree->AppendItem(DetectorTree->GetRootItem(), _("Local"));
	
	FillSubTree(globalRootDir, globalItemId);
	FillSubTree(localRootDir, localItemId);

	DetectorTree->ExpandAllChildren(DetectorTree->GetRootItem());
	DetectorTree->Thaw();
}

void mvceditor::DetectorTreeHandlerClass::UpdateProjects() {
	wxArrayString projectLabels;
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	for (project = Globals.Projects.begin(); project != Globals.Projects.end(); ++project) {
		projectLabels.Add(project->Label);
	}
	ProjectChoice->Clear();
	if (!projectLabels.IsEmpty()) {
		ProjectChoice->Append(projectLabels);
		ProjectChoice->Select(0);
	}
}

void mvceditor::DetectorTreeHandlerClass::FillSubTree(const wxString& detectorRootDir, wxTreeItemId treeItemDir) {
	wxDir dir;
	if (dir.Open(detectorRootDir)) {
		wxString fileName;
		if (dir.GetFirst(&fileName, wxEmptyString, wxDIR_DIRS)) {
			do {
				wxTreeItemId subRoot = DetectorTree->AppendItem(treeItemDir, fileName);
				FillSubTree(detectorRootDir + wxFileName::GetPathSeparator() + fileName, subRoot);
			} while (dir.GetNext(&fileName));
		}
		if (dir.GetFirst(&fileName, wxEmptyString, wxDIR_FILES)) {
			do {
				wxFileName fullPath(detectorRootDir, fileName);
				TreeItemDataStringClass* treeItemData = new TreeItemDataStringClass(fullPath.GetFullPath());
				DetectorTree->AppendItem(treeItemDir, fileName, -1, -1, treeItemData);
			} while (dir.GetNext(&fileName));

		}
	}
}

void mvceditor::DetectorTreeHandlerClass::OnTreeItemActivated(wxTreeEvent& event) {
	wxTreeItemId id = event.GetItem();
	TreeItemDataStringClass* treeItemData = (TreeItemDataStringClass*)DetectorTree->GetItemData(id);
	if (treeItemData) {
		wxCommandEvent openEvent(mvceditor::EVENT_CMD_FILE_OPEN);
		openEvent.SetString(treeItemData->Str);
		EventSink.Publish(openEvent);
	}
}

void mvceditor::DetectorTreeHandlerClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = Detector->HelpMessage();
	wxMessageBox(help, _("Help"), wxOK);
}

void mvceditor::DetectorTreeHandlerClass::OnAddButton(wxCommandEvent& event) {
	wxString name = ::wxGetTextFromUser(_("Please enter a file name (no extension)"), _("Create a URL Detector"), wxEmptyString);
	wxString forbidden = wxFileName::GetForbiddenChars();
	if (name.find_first_of(forbidden, 0) != std::string::npos) {
		wxMessageBox(_("Please enter valid a file name"));
		return;
	}
	if (name.IsEmpty()) {
		wxMessageBox(_("Please enter a file name"));
		return;
	}
	if (!name.EndsWith(wxT(".php"))) {
		name += wxT(".php");
	}
	wxFileName localDetectorScript(Detector->LocalRootDir().GetPath(), name);
	if (localDetectorScript.FileExists()) {
		wxMessageBox(_("File name already exists. Please enter another name."));
		return;
	}

	wxFile file;
	if (file.Create(localDetectorScript.GetFullPath())) {
		
		// populate the file with some starter code to guide the user
		wxFileName urlDetectorSkeleton = Detector->SkeletonFile();
		wxFFile skeletonFile(urlDetectorSkeleton.GetFullPath().fn_str(), wxT("r"));
		wxASSERT_MSG(skeletonFile.IsOpened(), _("Skeleton file not found."));
		wxString contents;
		
		skeletonFile.ReadAll(&contents);
		file.Write(contents);

		// close file so that it can be opened later on by the app
		file.Close();
		wxCommandEvent openEvent(mvceditor::EVENT_CMD_FILE_OPEN);
		openEvent.SetString(localDetectorScript.GetFullPath());
		EventSink.Publish(openEvent);

		// add the tree node also
		TreeItemDataStringClass* treeItemData = new TreeItemDataStringClass(localDetectorScript.GetFullPath());
		wxTreeItemId localLabelTreeItemId = DetectorTree->GetLastChild(DetectorTree->GetRootItem());
		DetectorTree->AppendItem(localLabelTreeItemId, name, -1,-1, treeItemData);
		DetectorTree->SortChildren(localLabelTreeItemId);
		DetectorTree->ExpandAllChildren(localLabelTreeItemId);
	}
	else {
		wxMessageBox(_("File could not be created:") + localDetectorScript.GetFullPath());
	}
}

void mvceditor::DetectorTreeHandlerClass::OnTestButton(wxCommandEvent& event) {
	
	// create the command to test the selected detector on the selected
	// project
	int projectChoiceIndex = ProjectChoice->GetSelection();
	if (ProjectChoice->IsEmpty() || projectChoiceIndex >= (int)Globals.Projects.size()) {
		wxMessageBox(_("Please choose a project to test the detector on."));
		return;
	}

	mvceditor::ProjectClass project = Globals.Projects[projectChoiceIndex];
	if (project.Sources.empty()) {
		wxMessageBox(_("Selected project does not have any source directories. Please choose another project"));
		return;
	}	
	
	// make sure that item selected is an actual detector and not a label
	wxTreeItemId itemId = DetectorTree->GetSelection();
	TreeItemDataStringClass* treeItemData = itemId.IsOk() ? 
		(TreeItemDataStringClass*)DetectorTree->GetItemData(itemId)
		: NULL;
	if (!treeItemData) {
		wxMessageBox(_("Please choose a detector to test."));
		return;
	}
	
	wxString detectorScriptFullPath = treeItemData->Str;
	if (Detector->CanTest(Globals, project)) {	
		wxString cmdLine = Detector->TestCommandLine(Globals, project, detectorScriptFullPath);

		// send the command line to a new app command event to start a process
		wxCommandEvent runEvent(mvceditor::EVENT_CMD_RUN_COMMAND);
		runEvent.SetString(cmdLine);
		EventSink.Publish(runEvent);
	}
}

void mvceditor::DetectorTreeHandlerClass::OnTreeItemRightClick(wxTreeEvent& event) {
	wxTreeItemId treeItemId = event.GetItem();
	TreeItemDataStringClass* data = (TreeItemDataStringClass*)DetectorTree->GetItemData(treeItemId);
	if (data) {
		DetectorTree->SelectItem(treeItemId, true);

		// only show the menu on the filenames not the "local" or "global" labels
		wxTreeItemId i = treeItemId;
		bool isLocalDetector = false;
		while (i != DetectorTree->GetRootItem()) {
			wxString label = DetectorTree->GetItemText(i);
			if (_("Local") == label) {
				isLocalDetector = true;
				break;
			}
			i = DetectorTree->GetItemParent(i);
		}

		wxMenu menu;
		menu.Append(ID_DETECTOR_TREE_OPEN, _("Open"), _("Open the selected URL Detector"));	
		menu.Append(ID_DETECTOR_TREE_RENAME, _("Rename"), _("Rename the selected URL Detector"));
		menu.Append(ID_DETECTOR_TREE_DELETE, _("Delete"), _("Delete the selected URL Detector"));
	
		// only show the destructive menu items on the "local" detectors
		menu.Enable(ID_DETECTOR_TREE_RENAME, isLocalDetector);
		menu.Enable(ID_DETECTOR_TREE_DELETE, isLocalDetector);

		wxPoint pos = event.GetPoint();
		DetectorTree->PopupMenu(&menu, pos);
	}
	event.Skip();
}

void mvceditor::DetectorTreeHandlerClass::OnTreeItemDelete(wxTreeEvent& event) {
	wxTreeItemId treeItemId = event.GetItem();
	TreeItemDataStringClass* data = (TreeItemDataStringClass*)DetectorTree->GetItemData(treeItemId);
	if (data) {
		wxRemoveFile(data->Str);
	}
	event.Skip();
}

void mvceditor::DetectorTreeHandlerClass::OnMenuOpenDetector(wxCommandEvent& event) {
	wxTreeItemId id = DetectorTree->GetSelection();
	TreeItemDataStringClass* treeItemData = (TreeItemDataStringClass*)DetectorTree->GetItemData(id);
	if (treeItemData) {
		wxCommandEvent openEvent(mvceditor::EVENT_CMD_FILE_OPEN);
		openEvent.SetString(treeItemData->Str);
		EventSink.Publish(openEvent);
	}
}

void mvceditor::DetectorTreeHandlerClass::OnMenuRenameDetector(wxCommandEvent& event) {
	wxTreeItemId id = DetectorTree->GetSelection();
	TreeItemDataStringClass* treeItemData = (TreeItemDataStringClass*)DetectorTree->GetItemData(id);
	if (treeItemData) {
		DetectorTree->EditLabel(id);	
	}
}

void mvceditor::DetectorTreeHandlerClass::OnMenuDeleteDetector(wxCommandEvent& event) {
	wxTreeItemId treeItemId = DetectorTree->GetSelection();
	TreeItemDataStringClass* data = (TreeItemDataStringClass*)DetectorTree->GetItemData(treeItemId);
	if (data) {
		int res = wxMessageBox(_("Delete this detector? The operation cannot be undone.\n") + data->Str, _("Delete URL detector"), wxYES_NO);
		if (wxYES == res) {
			DetectorTree->Delete(treeItemId);
		}
	}
}

void mvceditor::DetectorTreeHandlerClass::OnTreeItemEndLabelEdit(wxTreeEvent& event) {
	wxTreeItemId treeItemId = event.GetItem();
	wxString newName = event.GetLabel();
	if (newName.find_first_of(wxFileName::GetForbiddenChars(), 0) != std::string::npos) {
		wxMessageBox(_("Filename contains invalid characters."));
		event.Veto();
	}
	else {

		// rename the file and set the tree item data; the label itself will be set 
		// by the next event handler
		TreeItemDataStringClass* data = (TreeItemDataStringClass*)DetectorTree->GetItemData(treeItemId);
		wxASSERT(data);
		wxFileName oldFileName(data->Str);
		wxFileName newFileName(oldFileName.GetPath(), newName);
		if (!newFileName.FileExists()) {
			data->Str = newFileName.GetFullPath();
			wxRenameFile(oldFileName.GetFullPath(), newFileName.GetFullPath());
			event.Skip();
		}
		else {
			wxMessageBox(_("File name already exists. Please choose a different name."));
			event.Veto();
		}
	}
}

mvceditor::UrlDetectorPanelClass::UrlDetectorPanelClass(wxWindow* parent, int id, mvceditor::GlobalsClass& globals,
														mvceditor::EventSinkClass& eventSink)
	: UrlDetectorPanelGeneratedClass(parent, id) 
	, Detector() 
	, Handler(DetectorTree, TestButton, AddButton, HelpButton, ProjectChoice, &Detector, globals, eventSink) {
	HelpButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_HELP, 
		wxART_TOOLBAR, wxSize(16, 16))));

	// propagate the menu events to the handler since the handler is not connected to the 
	// GUI it will not get them by default
	Connect(ID_DETECTOR_TREE_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuOpenDetector), NULL, &Handler);
	Connect(ID_DETECTOR_TREE_RENAME, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuRenameDetector), NULL, &Handler);
	Connect(ID_DETECTOR_TREE_DELETE, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuDeleteDetector), NULL, &Handler);
}

mvceditor::UrlDetectorPanelClass::~UrlDetectorPanelClass() {
	Disconnect(ID_DETECTOR_TREE_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuOpenDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_RENAME, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuRenameDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_DELETE, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuDeleteDetector), NULL, &Handler);
}

void mvceditor::UrlDetectorPanelClass::Init() {
	Handler.Init();
}

void mvceditor::UrlDetectorPanelClass::UpdateProjects() {
	Handler.UpdateProjects();
}

mvceditor::TemplateFilesDetectorPanelClass::TemplateFilesDetectorPanelClass(wxWindow* parent, int id, mvceditor::GlobalsClass& globals,
														mvceditor::EventSinkClass& eventSink,
														mvceditor::RunningThreadsClass& runningThreads)
	: TemplateFilesDetectorPanelGeneratedClass(parent, id) 
	, Detector() 
	, Handler(DetectorTree, TestButton, AddButton, HelpButton, ProjectChoice, &Detector, globals, eventSink)
	, TestUrl()
	, Globals(globals) 
	, RunningThreads(runningThreads) {
	HelpButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_HELP, 
		wxART_TOOLBAR, wxSize(16, 16))));

	// propagate the menu events to the handler since the handler is not connected to the 
	// GUI it will not get them by default
	Connect(ID_DETECTOR_TREE_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuOpenDetector), NULL, &Handler);
	Connect(ID_DETECTOR_TREE_RENAME, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuRenameDetector), NULL, &Handler);
	Connect(ID_DETECTOR_TREE_DELETE, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuDeleteDetector), NULL, &Handler);
}

mvceditor::TemplateFilesDetectorPanelClass::~TemplateFilesDetectorPanelClass() {
	Disconnect(ID_DETECTOR_TREE_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuOpenDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_RENAME, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuRenameDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_DELETE, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuDeleteDetector), NULL, &Handler);
}

void mvceditor::TemplateFilesDetectorPanelClass::Init() {
	Handler.Init();
}

void mvceditor::TemplateFilesDetectorPanelClass::UpdateProjects() {
	Handler.UpdateProjects();
}

void mvceditor::TemplateFilesDetectorPanelClass::OnChooseUrlButton(wxCommandEvent& event) {
	TestUrl.Reset();
	mvceditor::ChooseUrlDialogClass dialog(this, Globals.UrlResourceFinder, TestUrl);
	if (dialog.ShowModal() == wxOK) {
		UrlToTest->SetValue(TestUrl.Url.BuildURI());
		mvceditor::CallStackActionClass* action = new mvceditor::CallStackActionClass(RunningThreads, wxID_ANY);
		action->SetCallStackStart(TestUrl.FileName,
			mvceditor::WxToIcu(TestUrl.ClassName),
			mvceditor::WxToIcu(TestUrl.MethodName),
			Globals.Projects[ProjectChoice->GetSelection()].DetectorDbFileName
		);
		wxThreadIdType threadId;
		action->Init(Globals);
		action->CreateSingleInstance(threadId);
	}
}

mvceditor::DetectorFeatureClass::DetectorFeatureClass(mvceditor::AppClass &app)
	: FeatureClass(app) {

}

void mvceditor::DetectorFeatureClass::AddViewMenuItems(wxMenu* menu) {
	menu->Append(mvceditor::MENU_DETECTORS + 0, _("View URL Detectors"), _("View the URL Detectors"), wxITEM_NORMAL);
	menu->Append(mvceditor::MENU_DETECTORS + 1, _("View Template File Detectors"), _("View the Template File Detectors"), wxITEM_NORMAL);
}

void mvceditor::DetectorFeatureClass::AddNewMenu(wxMenuBar* menuBar) {
	wxMenu* menu = new wxMenu(0);
	menu->Append(mvceditor::MENU_DETECTORS + 2, _("Run URL Detection"), _("Run the URL Detectors against the current projects"), wxITEM_NORMAL);
	menu->Append(mvceditor::MENU_DETECTORS + 3, _("Run Template File Detection"), _("Run the Template file Detectors against the current projects"), wxITEM_NORMAL);
	menuBar->Append(menu, _("Detectors"));
}

void mvceditor::DetectorFeatureClass::OnViewUrlDetectors(wxCommandEvent& event) {
	wxWindow* window = FindOutlineWindow(ID_URL_DETECTOR_PANEL);
	if (window) {
		SetFocusToOutlineWindow(window);
	}
	else {
		mvceditor::UrlDetectorPanelClass* panel = new mvceditor::UrlDetectorPanelClass(GetOutlineNotebook(), ID_URL_DETECTOR_PANEL, 
			App.Globals, App.EventSink);
		if (AddOutlineWindow(panel, _("URL Detectors"))) {
			panel->Init();
			panel->UpdateProjects();
		}
	}
}

void mvceditor::DetectorFeatureClass::OnViewTemplateFileDetectors(wxCommandEvent& event) {
	wxWindow* window = FindOutlineWindow(ID_TEMPLATE_FILES_DETECTOR_PANEL);
	if (window) {
		SetFocusToOutlineWindow(window);
	}
	else {
		mvceditor::TemplateFilesDetectorPanelClass* panel = new mvceditor::TemplateFilesDetectorPanelClass(GetOutlineNotebook(), ID_TEMPLATE_FILES_DETECTOR_PANEL, 
			App.Globals, App.EventSink, App.RunningThreads);
		if (AddOutlineWindow(panel, _("Template Files Detectors"))) {
			panel->Init();
			panel->UpdateProjects();
		}
	}
}

void mvceditor::DetectorFeatureClass::OnRunUrlDetectors(wxCommandEvent& event) {
	if (App.Sequences.Running()) {
		wxMessageBox(_("Please wait for the current background task to finish"));
		return;
	}
	std::vector<mvceditor::ActionClass*> actions;

	// the sequence class will own this pointer
	actions.push_back(
		new mvceditor::UrlDetectorActionClass(App.RunningThreads, mvceditor::ID_EVENT_ACTION_URL_DETECTOR)	
	);
	App.Sequences.Build(actions);
}

void mvceditor::DetectorFeatureClass::OnRunTemplateFileDetectors(wxCommandEvent& event) {
	/*
	if (App.Sequences.Running()) {
		wxMessageBox(_("Please wait for the current background task to finish"));
		return;
	}
	std::vector<mvceditor::ActionClass*> actions;

	// the sequence class will own this pointer
	actions.push_back(
		new mvceditor::UrlDetectorActionClass(App.RunningThreads, mvceditor::ID_EVENT_ACTION_URL_DETECTOR)	
	);
	App.Sequences.Build(actions);
	*/
}

BEGIN_EVENT_TABLE(mvceditor::DetectorFeatureClass, mvceditor::FeatureClass)
	EVT_MENU(mvceditor::MENU_DETECTORS + 0, mvceditor::DetectorFeatureClass::OnViewUrlDetectors)
	EVT_MENU(mvceditor::MENU_DETECTORS + 1, mvceditor::DetectorFeatureClass::OnViewTemplateFileDetectors)
	EVT_MENU(mvceditor::MENU_DETECTORS + 2, mvceditor::DetectorFeatureClass::OnRunUrlDetectors)
	EVT_MENU(mvceditor::MENU_DETECTORS + 3, mvceditor::DetectorFeatureClass::OnRunTemplateFileDetectors)
END_EVENT_TABLE()
