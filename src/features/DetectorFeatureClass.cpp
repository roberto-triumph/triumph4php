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
#include <actions/UrlTagDetectorActionClass.h>
#include <actions/TagDetectorActionClass.h>
#include <actions/CallStackActionClass.h>
#include <actions/TemplateFileTagsDetectorActionClass.h>
#include <actions/DatabaseTagDetectorActionClass.h>
#include <actions/ConfigTagDetectorActionClass.h>
#include <widgets/TreeItemDataStringClass.h>
#include <widgets/ChooseUrlDialogClass.h>
#include <globals/Assets.h>
#include <globals/Errors.h>
#include <Triumph.h>
#include <wx/artprov.h>
#include <wx/file.h>
#include <wx/ffile.h>

static const int ID_URL_DETECTOR_PANEL = wxNewId();
static const int ID_TEMPLATE_FILES_DETECTOR_PANEL = wxNewId();
static const int ID_TAG_DETECTOR_PANEL = wxNewId();
static const int ID_DATABASE_DETECTOR_PANEL = wxNewId();
static const int ID_CONFIG_DETECTOR_PANEL = wxNewId();

static const int ID_DETECTOR_TREE_OPEN = wxNewId();
static const int ID_DETECTOR_TREE_RENAME = wxNewId();
static const int ID_DETECTOR_TREE_DELETE = wxNewId();


t4p::DetectorClass::DetectorClass() {

}

t4p::DetectorClass::~DetectorClass() {

}

t4p::UrlTagDetectorClass::UrlTagDetectorClass()  {

}

bool t4p::UrlTagDetectorClass::CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project) {
	t4p::SourceClass source = project.Sources[0];
	wxString rootUrl = globals.Environment.Apache.GetUrl(source.RootDirectory.GetPath());
	if (rootUrl.IsEmpty()) {
		wxMessageBox(_("Cannot determine the root URL of the selected project. Please add a virtual host mapping for ") + source.RootDirectory.GetPath() +
			_(" under Edit -> Preferences -> Apache"));
		return false;
	}
	return true;
}

wxString t4p::UrlTagDetectorClass::TestCommandLine(const t4p::GlobalsClass& globals, 
													  const t4p::ProjectClass& project,
													  const wxString& detectorScriptFullPath) {
	t4p::SourceClass source = project.Sources[0];
	wxString rootUrl = globals.Environment.Apache.GetUrl(source.RootDirectory.GetPath());

	t4p::UrlTagDetectorParamsClass params;
	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = t4p::PhpDetectorsBaseAsset();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	params.RootUrl = rootUrl;
	params.ResourceDbFileName = globals.TagCacheDbFileName;
	return params.BuildCmdLine();
}

wxFileName t4p::UrlTagDetectorClass::LocalRootDir() {
	return t4p::UrlTagDetectorsLocalAsset();
}

wxFileName t4p::UrlTagDetectorClass::GlobalRootDir() {
	return t4p::UrlTagDetectorsGlobalAsset();
}

wxFileName t4p::UrlTagDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = t4p::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("UrlDetector.skeleton.php"));
	return skeletonFile;
}

wxString t4p::UrlTagDetectorClass::Label() {
	return _("URL Detectors");
}

void t4p::UrlTagDetectorClass::HelpMessage() {
	UrlDetectorHelpDialogGeneratedClass dialog(NULL);
	dialog.ShowModal();
}

t4p::TemplateFileTagsDetectorClass::TemplateFileTagsDetectorClass() 
	: DetectorClass() {

}

bool  t4p::TemplateFileTagsDetectorClass::CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project) {
	return true;
}

wxString t4p::TemplateFileTagsDetectorClass::TestCommandLine(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project,
																const wxString& detectorScriptFullPath) {
	t4p::TemplateFileTagsDetectorParamsClass params;
	t4p::SourceClass source = project.Sources[0];

	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = t4p::PhpDetectorsBaseAsset().GetFullPath();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	params.DetectorDbFileName = globals.DetectorCacheDbFileName;
	params.OutputDbFileName = wxT("");
	return params.BuildCmdLine();
}

wxFileName t4p::TemplateFileTagsDetectorClass::LocalRootDir() {
	return t4p::TemplateFileTagsDetectorsLocalAsset();
}

wxFileName t4p::TemplateFileTagsDetectorClass::GlobalRootDir() {
	return t4p::TemplateFilesDetectorsGlobalAsset();
}

wxFileName t4p::TemplateFileTagsDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = t4p::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("TemplateFilesDetector.skeleton.php"));
	return skeletonFile;
}

wxString t4p::TemplateFileTagsDetectorClass::Label() {
	return _("Template File Detectors");
}

void t4p::TemplateFileTagsDetectorClass::HelpMessage() {
	TemplateFilesDetectorHelpGeneratedClass dialog(NULL);
	dialog.ShowModal();
}

t4p::TagDetectorClass::TagDetectorClass()  {

}

bool t4p::TagDetectorClass::CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project) {
	return true;
}

wxString t4p::TagDetectorClass::TestCommandLine(const t4p::GlobalsClass& globals, 
													  const t4p::ProjectClass& project,
													  const wxString& detectorScriptFullPath) {
	t4p::SourceClass source = project.Sources[0];
	wxString rootUrl = globals.Environment.Apache.GetUrl(source.RootDirectory.GetPath());

	t4p::TagDetectorParamsClass params;
	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = t4p::PhpDetectorsBaseAsset();
	params.ScriptName = detectorScriptFullPath;
	params.ResourceDbFileName = globals.TagCacheDbFileName;
	params.SourceDir = source.RootDirectory;
	return params.BuildCmdLine();
}

wxFileName t4p::TagDetectorClass::LocalRootDir() {
	return t4p::TagDetectorsLocalAsset();
}

wxFileName t4p::TagDetectorClass::GlobalRootDir() {
	return t4p::TagDetectorsGlobalAsset();
}

wxFileName t4p::TagDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = t4p::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("TagDetector.skeleton.php"));
	return skeletonFile;
}

wxString t4p::TagDetectorClass::Label() {
	return _("Tag Detectors");
}

void t4p::TagDetectorClass::HelpMessage() {
	wxString help = wxString::FromAscii(
		"Tag Detectors are PHP scripts that Triumph uses to find out "
		"any tags (methods, properties, or classes) that PHP frameworks dynamically create.  \n"
		"Triumph can detect tags for CodeIgniter projects.\n"
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Help"), wxOK);
}

t4p::DatabaseTagDetectorClass::DatabaseTagDetectorClass()  {

}

bool t4p::DatabaseTagDetectorClass::CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project) {
	return true;
}

wxString t4p::DatabaseTagDetectorClass::TestCommandLine(const t4p::GlobalsClass& globals, 
													  const t4p::ProjectClass& project,
													  const wxString& detectorScriptFullPath) {
	t4p::SourceClass source = project.Sources[0];

	t4p::DatabaseTagDetectorParamsClass params;
	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = t4p::PhpDetectorsBaseAsset();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	return params.BuildCmdLine();
}

wxFileName t4p::DatabaseTagDetectorClass::LocalRootDir() {
	return t4p::DatabaseTagDetectorsLocalAsset();
}

wxFileName t4p::DatabaseTagDetectorClass::GlobalRootDir() {
	return t4p::DatabaseTagDetectorsGlobalAsset();
}

wxFileName t4p::DatabaseTagDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = t4p::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("DatabaseDetector.skeleton.php"));
	return skeletonFile;
}

wxString t4p::DatabaseTagDetectorClass::Label() {
	return _("Database Detectors");
}

void t4p::DatabaseTagDetectorClass::HelpMessage() {
	DatabaseDetectorHelpDialogGenereatedClass dialog(NULL);
	dialog.ShowModal();
}

t4p::ConfigTagDetectorClass::ConfigTagDetectorClass()  {

}

bool t4p::ConfigTagDetectorClass::CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project) {
	return true;
}

wxString t4p::ConfigTagDetectorClass::TestCommandLine(const t4p::GlobalsClass& globals, 
													  const t4p::ProjectClass& project,
													  const wxString& detectorScriptFullPath) {
	
	t4p::SourceClass source = project.Sources[0];														  
	
	t4p::ConfigTagDetectorParamsClass params;
	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = t4p::PhpDetectorsBaseAsset();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	return params.BuildCmdLine();
}

wxFileName t4p::ConfigTagDetectorClass::LocalRootDir() {
	return t4p::ConfigTagDetectorsLocalAsset();
}

wxFileName t4p::ConfigTagDetectorClass::GlobalRootDir() {
	return t4p::ConfigTagDetectorsGlobalAsset();
}

wxFileName t4p::ConfigTagDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = t4p::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("ConfigDetector.skeleton.php"));
	return skeletonFile;
}

wxString t4p::ConfigTagDetectorClass::Label() {
	return _("Config Detectors");
}

void t4p::ConfigTagDetectorClass::HelpMessage() {
	ConfigDetectorsHelpDialogGeneratedClass dialog(NULL);
	dialog.ShowModal();
}

t4p::DetectorTreeHandlerClass::DetectorTreeHandlerClass(wxTreeCtrl* detectorTree,
															  wxButton* testButton,
															  wxButton* addButton,
															  wxButton* helpButton,
															  wxChoice* projectChoice,
															  t4p::DetectorClass* detector,
															  t4p::GlobalsClass& globals, 
															  t4p::EventSinkClass& eventSink,
															  const wxBitmap& rootImage)
	: wxEvtHandler()
	, ImageList(NULL)
	, DetectorTree(detectorTree)
	, TestButton(testButton)
	, AddButton(addButton)
	, HelpButton(helpButton)
	, ProjectChoice(projectChoice)
	, Detector(detector) 
	, Globals(globals) 
	, EventSink(eventSink) {

	// Connect Events
	if (TestButton) {
		TestButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnTestButton), NULL, this);
	}
	AddButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnAddButton), NULL, this);
	HelpButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnHelpButton), NULL, this);
	DetectorTree->Connect(wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemDelete), NULL, this);
	DetectorTree->Connect(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemBeginLabelEdit), NULL, this);
	DetectorTree->Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemEndLabelEdit), NULL, this);
	DetectorTree->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemActivated), NULL, this);
	DetectorTree->Connect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemRightClick), NULL, this);

	ImageList = new wxImageList(16, 16);
	ImageList->Add(rootImage, wxNullBitmap);
	ImageList->Add(t4p::BitmapImageAsset(wxT("folder-horizontal")), wxNullBitmap);
	ImageList->Add(t4p::BitmapImageAsset(wxT("folder-horizontal-open")), wxNullBitmap);
	ImageList->Add(t4p::BitmapImageAsset(wxT("document-php")), wxNullBitmap);

	// this pointer will be managed by the tree control, since the tree control
	// may use the pointer in the destructor.
	DetectorTree->SetImageList(ImageList);
}

t4p::DetectorTreeHandlerClass::~DetectorTreeHandlerClass() {
	// Connect Events
	if (TestButton) {
		TestButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnTestButton), NULL, this);	
	}
	AddButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnAddButton), NULL, this);
	HelpButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnHelpButton), NULL, this);
	DetectorTree->Disconnect(wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemDelete), NULL, this);
	DetectorTree->Disconnect(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemBeginLabelEdit), NULL, this);
	DetectorTree->Disconnect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemEndLabelEdit), NULL, this);
	DetectorTree->Disconnect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemActivated), NULL, this);
	DetectorTree->Disconnect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemRightClick), NULL, this);
}

void t4p::DetectorTreeHandlerClass::Init() {
	wxString globalRootDir = Detector->GlobalRootDir().GetFullPath();
	wxString localRootDir = Detector->LocalRootDir().GetFullPath();
	
	DetectorTree->Freeze();
	DetectorTree->DeleteAllItems();
	DetectorTree->AddRoot(Detector->Label(), IMAGE_ROOT_DETECTOR);
	wxTreeItemId globalItemId = DetectorTree->AppendItem(DetectorTree->GetRootItem(), _("Global"), IMAGE_FOLDER, IMAGE_FOLDER);
	DetectorTree->SetItemImage(globalItemId, IMAGE_FOLDER_OPEN, wxTreeItemIcon_Expanded);
	wxTreeItemId localItemId = DetectorTree->AppendItem(DetectorTree->GetRootItem(), _("Local"), IMAGE_FOLDER, IMAGE_FOLDER);
	DetectorTree->SetItemImage(localItemId, IMAGE_FOLDER_OPEN, wxTreeItemIcon_Expanded);

	FillSubTree(globalRootDir, globalItemId);
	FillSubTree(localRootDir, localItemId);

	DetectorTree->ExpandAllChildren(DetectorTree->GetRootItem());
	DetectorTree->Thaw();
}

void t4p::DetectorTreeHandlerClass::UpdateProjects() {
	wxArrayString projectLabels;
	std::vector<t4p::ProjectClass>::const_iterator project;
	std::vector<t4p::ProjectClass> enabledProjects = Globals.AllEnabledProjects();
	for (project = enabledProjects.begin(); project != enabledProjects.end(); ++project) {
		projectLabels.Add(project->Label);
	}
	ProjectChoice->Clear();
	if (!projectLabels.IsEmpty()) {
		ProjectChoice->Append(projectLabels);
		ProjectChoice->Select(0);
	}
}

void t4p::DetectorTreeHandlerClass::FillSubTree(const wxString& detectorRootDir, wxTreeItemId treeItemDir) {
	wxDir dir;
	if (dir.Open(detectorRootDir)) {
		wxString fileName;
		if (dir.GetFirst(&fileName, wxEmptyString, wxDIR_DIRS)) {
			do {
				wxTreeItemId subRoot = DetectorTree->AppendItem(treeItemDir, fileName, IMAGE_FOLDER, IMAGE_FOLDER);
				DetectorTree->SetItemImage(subRoot, IMAGE_FOLDER_OPEN, wxTreeItemIcon_Expanded);

				FillSubTree(detectorRootDir + wxFileName::GetPathSeparator() + fileName, subRoot);
			} while (dir.GetNext(&fileName));
		}
		if (dir.GetFirst(&fileName, wxEmptyString, wxDIR_FILES)) {
			do {
				wxFileName fullPath(detectorRootDir, fileName);
				TreeItemDataStringClass* treeItemData = new TreeItemDataStringClass(fullPath.GetFullPath());
				DetectorTree->AppendItem(treeItemDir, fileName, IMAGE_SCRIPT, IMAGE_SCRIPT, treeItemData);
			} while (dir.GetNext(&fileName));

		}
	}
}

void t4p::DetectorTreeHandlerClass::OnTreeItemActivated(wxTreeEvent& event) {
	wxTreeItemId id = event.GetItem();
	TreeItemDataStringClass* treeItemData = (TreeItemDataStringClass*)DetectorTree->GetItemData(id);
	if (treeItemData) {
		t4p::OpenFileCommandEventClass openEvent(treeItemData->Str);
		EventSink.Publish(openEvent);
	}
}

void t4p::DetectorTreeHandlerClass::OnHelpButton(wxCommandEvent& event) {
	Detector->HelpMessage();
}

void t4p::DetectorTreeHandlerClass::OnAddButton(wxCommandEvent& event) {
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
		wxFFile skeletonFile(urlDetectorSkeleton.GetFullPath(), wxT("rb"));
		wxASSERT_MSG(skeletonFile.IsOpened(), _("Skeleton file not found."));
		wxString contents;
		
		skeletonFile.ReadAll(&contents);
		file.Write(contents);

		// close file so that it can be opened later on by the app
		file.Close();
		t4p::OpenFileCommandEventClass openEvent(localDetectorScript.GetFullPath());
		EventSink.Publish(openEvent);

		// add the tree node also
		TreeItemDataStringClass* treeItemData = new TreeItemDataStringClass(localDetectorScript.GetFullPath());
		wxTreeItemId localLabelTreeItemId = DetectorTree->GetLastChild(DetectorTree->GetRootItem());
		DetectorTree->AppendItem(localLabelTreeItemId, name, IMAGE_SCRIPT, IMAGE_SCRIPT, treeItemData);
		DetectorTree->SortChildren(localLabelTreeItemId);
		DetectorTree->ExpandAllChildren(localLabelTreeItemId);
	}
	else {
		wxMessageBox(_("File could not be created:") + localDetectorScript.GetFullPath());
	}
}

void t4p::DetectorTreeHandlerClass::OnTestButton(wxCommandEvent& event) {
	if (Globals.Environment.Php.NotInstalled()) {
		t4p::EditorLogError(t4p::ERR_PHP_EXECUTABLE_NONE);
		return;
	}
	
	// create the command to test the selected detector on the selected
	// project
	int projectChoiceIndex = ProjectChoice->GetSelection();
	if (ProjectChoice->IsEmpty() || projectChoiceIndex >= (int)Globals.Projects.size()) {
		wxMessageBox(_("Please choose a project to test the detector on."));
		return;
	}
	t4p::ProjectClass project = Globals.AllEnabledProjects()[projectChoiceIndex];
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
		wxCommandEvent runEvent(t4p::EVENT_CMD_RUN_COMMAND);
		runEvent.SetString(cmdLine);
		EventSink.Publish(runEvent);
	}
}

void t4p::DetectorTreeHandlerClass::OnTreeItemRightClick(wxTreeEvent& event) {
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

void t4p::DetectorTreeHandlerClass::OnTreeItemDelete(wxTreeEvent& event) {
	wxTreeItemId treeItemId = event.GetItem();
	TreeItemDataStringClass* data = (TreeItemDataStringClass*)DetectorTree->GetItemData(treeItemId);
	if (data) {
		wxRemoveFile(data->Str);
	}
	event.Skip();
}

void t4p::DetectorTreeHandlerClass::OnMenuOpenDetector(wxCommandEvent& event) {
	wxTreeItemId id = DetectorTree->GetSelection();
	TreeItemDataStringClass* treeItemData = (TreeItemDataStringClass*)DetectorTree->GetItemData(id);
	if (treeItemData) {
		t4p::OpenFileCommandEventClass openEvent(treeItemData->Str);
		EventSink.Publish(openEvent);
	}
}

void t4p::DetectorTreeHandlerClass::OnMenuRenameDetector(wxCommandEvent& event) {
	wxTreeItemId id = DetectorTree->GetSelection();
	TreeItemDataStringClass* treeItemData = (TreeItemDataStringClass*)DetectorTree->GetItemData(id);
	if (treeItemData) {
		DetectorTree->EditLabel(id);	
	}
}

void t4p::DetectorTreeHandlerClass::OnMenuDeleteDetector(wxCommandEvent& event) {
	wxTreeItemId treeItemId = DetectorTree->GetSelection();
	TreeItemDataStringClass* data = (TreeItemDataStringClass*)DetectorTree->GetItemData(treeItemId);
	if (data) {
		int res = wxMessageBox(_("Delete this detector? The operation cannot be undone.\n") + data->Str, _("Delete URL detector"), wxYES_NO);
		if (wxYES == res) {
			DetectorTree->Delete(treeItemId);
		}
	}
}

void t4p::DetectorTreeHandlerClass::OnTreeItemBeginLabelEdit(wxTreeEvent& event) {

	// only allow edited when detector is a "local" detector
	wxTreeItemId i = event.GetItem();
	bool isLocalDetector = false;
	while (i != DetectorTree->GetRootItem()) {
		wxString label = DetectorTree->GetItemText(i);
		if (_("Local") == label) {
			isLocalDetector = true;
			break;
		}
		i = DetectorTree->GetItemParent(i);
	}
	if (isLocalDetector) {
		event.Skip();
	}
	else {
		event.Veto();
	}
}

void t4p::DetectorTreeHandlerClass::OnTreeItemEndLabelEdit(wxTreeEvent& event) {
	wxTreeItemId treeItemId = event.GetItem();
	wxString newName = event.GetLabel();
	if (newName.find_first_of(wxFileName::GetForbiddenChars(), 0) != std::string::npos) {
		wxMessageBox(_("Filename contains invalid characters."));
		event.Veto();
	}
	else if (newName.IsEmpty()) {
		wxMessageBox(_("Filename cannot be empty."));
		event.Veto();
	}
	else {

		// rename the file and set the tree item data; the label itself will be set 
		// by the next event handler
		TreeItemDataStringClass* data = (TreeItemDataStringClass*)DetectorTree->GetItemData(treeItemId);
		wxASSERT(data);
		wxFileName oldFileName(data->Str);
		wxFileName newFileName(oldFileName.GetPath(), newName);
		if (oldFileName == newFileName) {
			
			// no name change== dont try to move files
			return;
		}
		else if (!newFileName.FileExists()) {
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

t4p::UrlTagDetectorPanelClass::UrlTagDetectorPanelClass(wxWindow* parent, int id, t4p::GlobalsClass& globals,
														t4p::EventSinkClass& eventSink)
	: UrlDetectorPanelGeneratedClass(parent, id) 
	, Detector() 
	, Handler(DetectorTree, TestButton, AddButton, HelpButton, ProjectChoice, &Detector, globals, eventSink, t4p::BitmapImageAsset(wxT("url-detectors"))) {
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

t4p::UrlTagDetectorPanelClass::~UrlTagDetectorPanelClass() {
	Disconnect(ID_DETECTOR_TREE_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuOpenDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_RENAME, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuRenameDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_DELETE, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuDeleteDetector), NULL, &Handler);
}

void t4p::UrlTagDetectorPanelClass::Init() {
	Handler.Init();
}

void t4p::UrlTagDetectorPanelClass::UpdateProjects() {
	Handler.UpdateProjects();
}

t4p::TemplateFileTagsDetectorPanelClass::TemplateFileTagsDetectorPanelClass(wxWindow* parent, int id, t4p::GlobalsClass& globals,
														t4p::EventSinkClass& eventSink,
														t4p::RunningThreadsClass& runningThreads)
	: TemplateFilesDetectorPanelGeneratedClass(parent, id) 
	, Detector() 
	// null == we will handle the test button in this class
	, Handler(DetectorTree, NULL, AddButton, HelpButton, ProjectChoice, &Detector, globals, eventSink, t4p::BitmapImageAsset(wxT("template-file-detectors")))
	, TestUrl()
	, Globals(globals) 
	, RunningThreads(runningThreads) 
	, EventSink(eventSink) {
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
	
	RunningThreads.AddEventHandler(this->GetEventHandler());
	this->GetEventHandler()->Connect(t4p::ID_EVENT_ACTION_CALL_STACK, t4p::EVENT_ACTION_COMPLETE, 
		wxCommandEventHandler(TemplateFileTagsDetectorPanelClass::OnCallStackComplete),
		NULL, this
	);
}

t4p::TemplateFileTagsDetectorPanelClass::~TemplateFileTagsDetectorPanelClass() {
	Disconnect(ID_DETECTOR_TREE_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuOpenDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_RENAME, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuRenameDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_DELETE, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuDeleteDetector), NULL, &Handler);
		
	RunningThreads.RemoveEventHandler(this->GetEventHandler());
	Disconnect(t4p::ID_EVENT_ACTION_CALL_STACK, t4p::EVENT_ACTION_COMPLETE, 
		wxCommandEventHandler(TemplateFileTagsDetectorPanelClass::OnCallStackComplete)
	);
}

void t4p::TemplateFileTagsDetectorPanelClass::Init() {
	Handler.Init();
}

void t4p::TemplateFileTagsDetectorPanelClass::OnChooseUrlButton(wxCommandEvent& event) {
	TestUrl.Reset();
	t4p::ChooseUrlDialogClass dialog(this, Globals.UrlTagFinder, Globals.Projects, TestUrl);
	if (dialog.ShowModal() == wxOK) {
		UrlToTest->SetValue(TestUrl.Url.BuildURI());
		t4p::CallStackActionClass* action = new t4p::CallStackActionClass(RunningThreads, t4p::ID_EVENT_ACTION_CALL_STACK);
			action->SetCallStackStart(TestUrl.FileName,
			t4p::WxToIcu(TestUrl.ClassName),
			t4p::WxToIcu(TestUrl.MethodName),
			
			// the selection index is the index of the enabled projects
			Globals.DetectorCacheDbFileName
		);
		action->Init(Globals);
		RunningThreads.Queue(action);
	}
}

void t4p::TemplateFileTagsDetectorPanelClass::UpdateProjects() {
	Handler.UpdateProjects();
}

void t4p::TemplateFileTagsDetectorPanelClass::OnTestButton(wxCommandEvent& event) {
	if (Globals.Environment.Php.NotInstalled()) {
		t4p::EditorLogError(t4p::ERR_PHP_EXECUTABLE_NONE);
		return;
	}
	
	// create the command to test the selected detector on the selected
	// project
	int projectChoiceIndex = ProjectChoice->GetSelection();
	if (ProjectChoice->IsEmpty() || projectChoiceIndex >= (int)Globals.Projects.size()) {
		wxMessageBox(_("Please choose a project to test the detector on."));
		return;
	}
	t4p::ProjectClass project = Globals.AllEnabledProjects()[projectChoiceIndex];
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
	
	wxString fileName;
	
	// to get the templates, we first need to build the call stack
	if (!TestUrl.ClassName.IsEmpty() &&
		!TestUrl.MethodName.IsEmpty()) {
		
		t4p::CallStackActionClass* callStackAction = new t4p::CallStackActionClass(RunningThreads, t4p::ID_EVENT_ACTION_CALL_STACK);
		callStackAction->SetCallStackStart(fileName,
			t4p::WxToIcu(TestUrl.ClassName),
			t4p::WxToIcu(TestUrl.MethodName),
			Globals.DetectorCacheDbFileName);
		RunningThreads.Queue(callStackAction);
	}
}

void t4p::TemplateFileTagsDetectorPanelClass::OnCallStackComplete(wxCommandEvent& event) {
	wxTreeItemId itemId = DetectorTree->GetSelection();
	TreeItemDataStringClass* treeItemData = itemId.IsOk() ? 
		(TreeItemDataStringClass*)DetectorTree->GetItemData(itemId)
		: NULL;
	if (!treeItemData) {
		return;
	}
	int projectChoiceIndex = ProjectChoice->GetSelection();
	if (ProjectChoice->IsEmpty() || projectChoiceIndex >= (int)Globals.Projects.size()) {
		return;
	}
	t4p::ProjectClass project = Globals.AllEnabledProjects()[projectChoiceIndex];
	
	wxString detectorScriptFullPath = treeItemData->Str;
	if (Detector.CanTest(Globals, project)) {	
		wxString cmdLine = Detector.TestCommandLine(Globals, project, detectorScriptFullPath);

		// send the command line to a new app command event to start a process
		wxCommandEvent runEvent(t4p::EVENT_CMD_RUN_COMMAND);
		runEvent.SetString(cmdLine);
		EventSink.Publish(runEvent);
	}
}

t4p::TagDetectorPanelClass::TagDetectorPanelClass(wxWindow* parent, int id, t4p::GlobalsClass& globals,
														t4p::EventSinkClass& eventSink)
	: TagDetectorPanelGeneratedClass(parent, id) 
	, Detector() 
	, Handler(DetectorTree, TestButton, AddButton, HelpButton, ProjectChoice, &Detector, globals, eventSink, t4p::BitmapImageAsset(wxT("tag-detectors"))) {
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

t4p::TagDetectorPanelClass::~TagDetectorPanelClass() {
	Disconnect(ID_DETECTOR_TREE_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuOpenDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_RENAME, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuRenameDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_DELETE, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuDeleteDetector), NULL, &Handler);
}

void t4p::TagDetectorPanelClass::Init() {
	Handler.Init();
}

void t4p::TagDetectorPanelClass::UpdateProjects() {
	Handler.UpdateProjects();
}

t4p::DatabaseTagDetectorPanelClass::DatabaseTagDetectorPanelClass(wxWindow* parent, int id, t4p::GlobalsClass& globals,
														t4p::EventSinkClass& eventSink)
	: DatabaseDetectorPanelGeneratedClass(parent, id) 
	, Detector() 
	, Handler(DetectorTree, TestButton, AddButton, HelpButton, ProjectChoice, &Detector, globals, eventSink, t4p::BitmapImageAsset(wxT("database-detectors"))) {
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

t4p::DatabaseTagDetectorPanelClass::~DatabaseTagDetectorPanelClass() {
	Disconnect(ID_DETECTOR_TREE_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuOpenDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_RENAME, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuRenameDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_DELETE, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuDeleteDetector), NULL, &Handler);
}

void t4p::DatabaseTagDetectorPanelClass::Init() {
	Handler.Init();
}

void t4p::DatabaseTagDetectorPanelClass::UpdateProjects() {
	Handler.UpdateProjects();
}

t4p::ConfigTagDetectorPanelClass::ConfigTagDetectorPanelClass(wxWindow* parent, int id, t4p::GlobalsClass& globals,
														t4p::EventSinkClass& eventSink)
	: ConfigDetectorPanelGeneratedClass(parent, id) 
	, Detector() 
	, Handler(DetectorTree, TestButton, AddButton, HelpButton, ProjectChoice, &Detector, globals, eventSink, t4p::BitmapImageAsset(wxT("config-detectors"))) {
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

t4p::ConfigTagDetectorPanelClass::~ConfigTagDetectorPanelClass() {
	Disconnect(ID_DETECTOR_TREE_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuOpenDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_RENAME, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuRenameDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_DELETE, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuDeleteDetector), NULL, &Handler);
}

void t4p::ConfigTagDetectorPanelClass::Init() {
	Handler.Init();
}

void t4p::ConfigTagDetectorPanelClass::UpdateProjects() {
	Handler.UpdateProjects();
}

t4p::DetectorFeatureClass::DetectorFeatureClass(t4p::AppClass &app)
	: FeatureClass(app) {

}

void t4p::DetectorFeatureClass::AddViewMenuItems(wxMenu* menu) {
	menu->Append(t4p::MENU_DETECTORS + 0, _("View URL Detectors"), _("View the URL Detectors"), wxITEM_NORMAL);
	menu->Append(t4p::MENU_DETECTORS + 1, _("View Template File Detectors"), _("View the Template File Detectors"), wxITEM_NORMAL);
	menu->Append(t4p::MENU_DETECTORS + 2, _("View Tag Detectors"), _("View the Tag Detectors"), wxITEM_NORMAL);
	menu->Append(t4p::MENU_DETECTORS + 3, _("View Database Detectors"), _("View the Database Detectors"), wxITEM_NORMAL);
	menu->Append(t4p::MENU_DETECTORS + 4, _("View Config Detectors"), _("View the config Detectors"), wxITEM_NORMAL);
}

void t4p::DetectorFeatureClass::AddNewMenu(wxMenuBar* menuBar) {
	wxMenu* menu = new wxMenu();
	menu->Append(t4p::MENU_DETECTORS + 5, _("Run URL Detection"), _("Run the URL Detectors against the current projects"), wxITEM_NORMAL);
	menu->Append(t4p::MENU_DETECTORS + 6, _("Run Template File Detection"), _("Run the Template file Detectors against the current projects"), wxITEM_NORMAL);
	menu->Append(t4p::MENU_DETECTORS + 7, _("Run Tag Detection"), _("Run the Tag Detectors against the current projects"), wxITEM_NORMAL);
	menu->Append(t4p::MENU_DETECTORS + 8, _("Run Database Detection"), _("Run the Database Detectors against the current projects"), wxITEM_NORMAL);
	menu->Append(t4p::MENU_DETECTORS + 9, _("Run Config Detection"), _("Run the Config Detectors against the current projects"), wxITEM_NORMAL);
	menuBar->Append(menu, _("Detectors"));
}

void t4p::DetectorFeatureClass::OnViewUrlDetectors(wxCommandEvent& event) {
	wxWindow* window = FindOutlineWindow(ID_URL_DETECTOR_PANEL);
	if (window) {
		SetFocusToOutlineWindow(window);
	}
	else {
		t4p::UrlTagDetectorPanelClass* panel = new t4p::UrlTagDetectorPanelClass(GetOutlineNotebook(), ID_URL_DETECTOR_PANEL, 
			App.Globals, App.EventSink);
		wxBitmap urlBitmap = t4p::BitmapImageAsset(wxT("url-detectors"));
		if (AddOutlineWindow(panel, _("URL Detectors"), urlBitmap)) {
			panel->Init();
			panel->UpdateProjects();
		}
	}
}

void t4p::DetectorFeatureClass::OnViewTemplateFileDetectors(wxCommandEvent& event) {
	wxWindow* window = FindOutlineWindow(ID_TEMPLATE_FILES_DETECTOR_PANEL);
	if (window) {
		SetFocusToOutlineWindow(window);
	}
	else {
		t4p::TemplateFileTagsDetectorPanelClass* panel = new t4p::TemplateFileTagsDetectorPanelClass(
			GetOutlineNotebook(), ID_TEMPLATE_FILES_DETECTOR_PANEL, 
			App.Globals, App.EventSink, App.RunningThreads);
		wxBitmap templateFilesBitmap = t4p::BitmapImageAsset(wxT("template-file-detectors"));
		if (AddOutlineWindow(panel, _("Template Files Detectors"), templateFilesBitmap)) {
			panel->Init();
			panel->UpdateProjects();
		}
	}
}

void t4p::DetectorFeatureClass::OnViewTagDetectors(wxCommandEvent& event) {
	wxWindow* window = FindOutlineWindow(ID_TAG_DETECTOR_PANEL);
	if (window) {
		SetFocusToOutlineWindow(window);
	}
	else {
		t4p::TagDetectorPanelClass* panel = new t4p::TagDetectorPanelClass(GetOutlineNotebook(), ID_TAG_DETECTOR_PANEL, 
			App.Globals, App.EventSink);
		wxBitmap tagBitmap = t4p::BitmapImageAsset(wxT("tag-detectors"));
		if (AddOutlineWindow(panel, _("Tag Detectors"), tagBitmap)) {
			panel->Init();
			panel->UpdateProjects();
		}
	}
}

void t4p::DetectorFeatureClass::OnViewDatabaseDetectors(wxCommandEvent& event) {
	wxWindow* window = FindOutlineWindow(ID_DATABASE_DETECTOR_PANEL);
	if (window) {
		SetFocusToOutlineWindow(window);
	}
	else {
		t4p::DatabaseTagDetectorPanelClass* panel = new t4p::DatabaseTagDetectorPanelClass(GetOutlineNotebook(), ID_DATABASE_DETECTOR_PANEL, 
			App.Globals, App.EventSink);
		wxBitmap databaseBitmap = t4p::BitmapImageAsset(wxT("database-detectors"));
		if (AddOutlineWindow(panel, _("Database Detectors"), databaseBitmap)) {
			panel->Init();
			panel->UpdateProjects();
		}
	}
}

void t4p::DetectorFeatureClass::OnViewConfigDetectors(wxCommandEvent& event) {
	wxWindow* window = FindOutlineWindow(ID_CONFIG_DETECTOR_PANEL);
	if (window) {
		SetFocusToOutlineWindow(window);
	}
	else {
		t4p::ConfigTagDetectorPanelClass* panel = new t4p::ConfigTagDetectorPanelClass(GetOutlineNotebook(), ID_CONFIG_DETECTOR_PANEL, 
			App.Globals, App.EventSink);
		wxBitmap configBitmap = t4p::BitmapImageAsset(wxT("config-detectors"));
		if (AddOutlineWindow(panel, _("Config Detectors"), configBitmap)) {
			panel->Init();
			panel->UpdateProjects();
		}
	}
}

void t4p::DetectorFeatureClass::OnRunUrlDetectors(wxCommandEvent& event) {
	if (App.Sequences.Running()) {
		wxMessageBox(_("Please wait for the current background task to finish"));
		return;
	}
	std::vector<t4p::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	actions.push_back(
		new t4p::UrlTagDetectorActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_URL_TAG_DETECTOR)	
	);
	App.Sequences.Build(actions);
}

void t4p::DetectorFeatureClass::OnRunTemplateFileDetectors(wxCommandEvent& event) {
	if (App.Sequences.Running()) {
		wxMessageBox(_("Please wait for the current background task to finish"));
		return;
	}
	std::vector<t4p::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	t4p::CallStackActionClass* callStackAction =  new t4p::CallStackActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_CALL_STACK);
	t4p::UrlTagClass urlTag = App.Globals.CurrentUrl;
	
	if (!urlTag.Url.GetServer().IsEmpty() && urlTag.FileName.IsOk()
		&& !urlTag.ClassName.IsEmpty() && !urlTag.MethodName.IsEmpty()) {
		callStackAction->SetCallStackStart(urlTag.FileName,
			t4p::WxToIcu(urlTag.ClassName),
			t4p::WxToIcu(urlTag.MethodName),
			App.Globals.DetectorCacheDbFileName);
		actions.push_back(callStackAction);
		actions.push_back(
			new t4p::TemplateFileTagsDetectorActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_TEMPLATE_FILE_TAG_DETECTOR)
		);
		App.Sequences.Build(actions);
	}
	else {
		t4p::EditorLogWarningFix("Template Error", 
			_("Need to choose a URL to detect templates for. Template files feature depends on the URL detectors feature."));
	}
}

void t4p::DetectorFeatureClass::OnRunTagDetectors(wxCommandEvent& event) {
	std::vector<t4p::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	actions.push_back(
		new t4p::TagDetectorActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_TAG_DETECTOR)
	);
	App.Sequences.Build(actions);
}

void t4p::DetectorFeatureClass::OnRunDatabaseDetectors(wxCommandEvent& event) {
	std::vector<t4p::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	actions.push_back(
		new t4p::DatabaseTagDetectorActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR)
	);
	App.Sequences.Build(actions);
}

void t4p::DetectorFeatureClass::OnRunConfigDetectors(wxCommandEvent& event) {
	std::vector<t4p::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	actions.push_back(
		new t4p::ConfigTagDetectorActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_CONFIG_TAG_DETECTOR)
	);
	App.Sequences.Build(actions);
}

void t4p::DetectorFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	wxWindow* window = FindOutlineWindow(ID_URL_DETECTOR_PANEL);
	if (window) {
		t4p::UrlTagDetectorPanelClass* panel = (t4p::UrlTagDetectorPanelClass*) window;
		panel->UpdateProjects();
	}
	window = FindOutlineWindow(ID_TEMPLATE_FILES_DETECTOR_PANEL);
	if (window) {
		t4p::TemplateFileTagsDetectorPanelClass* panel = (t4p::TemplateFileTagsDetectorPanelClass*) window;
		panel->UpdateProjects();
	}
	window = FindOutlineWindow(ID_TAG_DETECTOR_PANEL);
	if (window) {
		t4p::TagDetectorPanelClass* panel = (t4p::TagDetectorPanelClass*) window;
		panel->UpdateProjects();
	}
	window = FindOutlineWindow(ID_DATABASE_DETECTOR_PANEL);
	if (window) {
		t4p::DatabaseTagDetectorPanelClass* panel = (t4p::DatabaseTagDetectorPanelClass*) window;
		panel->UpdateProjects();
	}
	window = FindOutlineWindow(ID_CONFIG_DETECTOR_PANEL);
	if (window) {
		t4p::ConfigTagDetectorPanelClass* panel = (t4p::ConfigTagDetectorPanelClass*) window;
		panel->UpdateProjects();
	}
}

BEGIN_EVENT_TABLE(t4p::DetectorFeatureClass, t4p::FeatureClass)
	EVT_MENU(t4p::MENU_DETECTORS + 0, t4p::DetectorFeatureClass::OnViewUrlDetectors)
	EVT_MENU(t4p::MENU_DETECTORS + 1, t4p::DetectorFeatureClass::OnViewTemplateFileDetectors)
	EVT_MENU(t4p::MENU_DETECTORS + 2, t4p::DetectorFeatureClass::OnViewTagDetectors)
	EVT_MENU(t4p::MENU_DETECTORS + 3, t4p::DetectorFeatureClass::OnViewDatabaseDetectors)
	EVT_MENU(t4p::MENU_DETECTORS + 4, t4p::DetectorFeatureClass::OnViewConfigDetectors)

	EVT_MENU(t4p::MENU_DETECTORS + 5, t4p::DetectorFeatureClass::OnRunUrlDetectors)
	EVT_MENU(t4p::MENU_DETECTORS + 6, t4p::DetectorFeatureClass::OnRunTemplateFileDetectors)
	EVT_MENU(t4p::MENU_DETECTORS + 7, t4p::DetectorFeatureClass::OnRunTagDetectors)
	EVT_MENU(t4p::MENU_DETECTORS + 8, t4p::DetectorFeatureClass::OnRunDatabaseDetectors)
	EVT_MENU(t4p::MENU_DETECTORS + 9, t4p::DetectorFeatureClass::OnRunConfigDetectors)

	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::DetectorFeatureClass::OnPreferencesSaved)
END_EVENT_TABLE()
