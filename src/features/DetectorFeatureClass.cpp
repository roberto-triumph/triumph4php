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
#include <MvcEditor.h>
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


mvceditor::DetectorClass::DetectorClass() {

}

mvceditor::DetectorClass::~DetectorClass() {

}

mvceditor::UrlTagDetectorClass::UrlTagDetectorClass()  {

}

bool mvceditor::UrlTagDetectorClass::CanTest(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project) {
	mvceditor::SourceClass source = project.Sources[0];
	wxString rootUrl = globals.Environment.Apache.GetUrl(source.RootDirectory.GetPath());
	if (rootUrl.IsEmpty()) {
		wxMessageBox(_("Cannot determine the root URL of the selected project. Please add a virtual host mapping for ") + source.RootDirectory.GetPath() +
			_(" under Edit -> Preferences -> Apache"));
		return false;
	}
	return true;
}

wxString mvceditor::UrlTagDetectorClass::TestCommandLine(const mvceditor::GlobalsClass& globals, 
													  const mvceditor::ProjectClass& project,
													  const wxString& detectorScriptFullPath) {
	mvceditor::SourceClass source = project.Sources[0];
	wxString rootUrl = globals.Environment.Apache.GetUrl(source.RootDirectory.GetPath());

	mvceditor::UrlTagDetectorParamsClass params;
	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = mvceditor::PhpDetectorsBaseAsset();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	params.RootUrl = rootUrl;
	params.ResourceDbFileName = globals.TagCacheDbFileName;
	return params.BuildCmdLine();
}

wxFileName mvceditor::UrlTagDetectorClass::LocalRootDir() {
	return mvceditor::UrlTagDetectorsLocalAsset();
}

wxFileName mvceditor::UrlTagDetectorClass::GlobalRootDir() {
	return mvceditor::UrlTagDetectorsGlobalAsset();
}

wxFileName mvceditor::UrlTagDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = mvceditor::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("UrlDetector.skeleton.php"));
	return skeletonFile;
}

wxString mvceditor::UrlTagDetectorClass::Label() {
	return _("URL Detectors");
}

wxString mvceditor::UrlTagDetectorClass::HelpMessage() {
	wxString help = wxString::FromAscii(
		"URL Detectors are PHP scripts that MVC Editor uses to find out "
		"all of the valid URL routes for your projects.  \n"
		"MVC Editor can detect routes for CodeIgniter projects.\n"
	);
	help = wxGetTranslation(help);
	return help;
}

mvceditor::TemplateFileTagsDetectorClass::TemplateFileTagsDetectorClass() 
	: DetectorClass() {

}

bool  mvceditor::TemplateFileTagsDetectorClass::CanTest(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project) {
	return true;
}

wxString mvceditor::TemplateFileTagsDetectorClass::TestCommandLine(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project,
																const wxString& detectorScriptFullPath) {
	mvceditor::TemplateFileTagsDetectorParamsClass params;
	mvceditor::SourceClass source = project.Sources[0];

	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = mvceditor::PhpDetectorsBaseAsset().GetFullPath();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	params.DetectorDbFileName = globals.DetectorCacheDbFileName;
	params.OutputDbFileName = wxT("");
	return params.BuildCmdLine();
}

wxFileName mvceditor::TemplateFileTagsDetectorClass::LocalRootDir() {
	return mvceditor::TemplateFileTagsDetectorsLocalAsset();
}

wxFileName mvceditor::TemplateFileTagsDetectorClass::GlobalRootDir() {
	return mvceditor::TemplateFilesDetectorsGlobalAsset();
}

wxFileName mvceditor::TemplateFileTagsDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = mvceditor::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("TemplateFilesDetector.skeleton.php"));
	return skeletonFile;
}

wxString mvceditor::TemplateFileTagsDetectorClass::Label() {
	return _("Template File Detectors");
}

wxString mvceditor::TemplateFileTagsDetectorClass::HelpMessage() {
	wxString help = wxString::FromAscii(
		"Template files detectors are PHP scripts that MVC Editor uses to find out "
		"all of the 'view' files for your projects.  \n"
		"MVC Editor can detect view files for CodeIgniter projects.\n"
	);
	help = wxGetTranslation(help);
	return help;
}

mvceditor::TagDetectorClass::TagDetectorClass()  {

}

bool mvceditor::TagDetectorClass::CanTest(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project) {
	return true;
}

wxString mvceditor::TagDetectorClass::TestCommandLine(const mvceditor::GlobalsClass& globals, 
													  const mvceditor::ProjectClass& project,
													  const wxString& detectorScriptFullPath) {
	mvceditor::SourceClass source = project.Sources[0];
	wxString rootUrl = globals.Environment.Apache.GetUrl(source.RootDirectory.GetPath());

	mvceditor::TagDetectorParamsClass params;
	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = mvceditor::PhpDetectorsBaseAsset();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	return params.BuildCmdLine();
}

wxFileName mvceditor::TagDetectorClass::LocalRootDir() {
	return mvceditor::TagDetectorsLocalAsset();
}

wxFileName mvceditor::TagDetectorClass::GlobalRootDir() {
	return mvceditor::TagDetectorsGlobalAsset();
}

wxFileName mvceditor::TagDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = mvceditor::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("TagDetector.skeleton.php"));
	return skeletonFile;
}

wxString mvceditor::TagDetectorClass::Label() {
	return _("Tag Detectors");
}

wxString mvceditor::TagDetectorClass::HelpMessage() {
	wxString help = wxString::FromAscii(
		"Tag Detectors are PHP scripts that MVC Editor uses to find out "
		"any tags (methods, properties, or classes) that PHP frameworks dynamically create.  \n"
		"MVC Editor can detect tags for CodeIgniter projects.\n"
	);
	help = wxGetTranslation(help);
	return help;
}

mvceditor::DatabaseTagDetectorClass::DatabaseTagDetectorClass()  {

}

bool mvceditor::DatabaseTagDetectorClass::CanTest(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project) {
	return true;
}

wxString mvceditor::DatabaseTagDetectorClass::TestCommandLine(const mvceditor::GlobalsClass& globals, 
													  const mvceditor::ProjectClass& project,
													  const wxString& detectorScriptFullPath) {
	mvceditor::SourceClass source = project.Sources[0];

	mvceditor::DatabaseTagDetectorParamsClass params;
	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = mvceditor::PhpDetectorsBaseAsset();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	return params.BuildCmdLine();
}

wxFileName mvceditor::DatabaseTagDetectorClass::LocalRootDir() {
	return mvceditor::DatabaseTagDetectorsLocalAsset();
}

wxFileName mvceditor::DatabaseTagDetectorClass::GlobalRootDir() {
	return mvceditor::DatabaseTagDetectorsGlobalAsset();
}

wxFileName mvceditor::DatabaseTagDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = mvceditor::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("DatabaseDetector.skeleton.php"));
	return skeletonFile;
}

wxString mvceditor::DatabaseTagDetectorClass::Label() {
	return _("Database Detectors");
}

wxString mvceditor::DatabaseTagDetectorClass::HelpMessage() {
	wxString help = wxString::FromAscii(
		"Database Detectors are PHP scripts that MVC Editor uses to find out "
		"any database connections that PHP frameworks dynamically create.  \n"
		"MVC Editor can detect database connections for CodeIgniter projects.\n"
	);
	help = wxGetTranslation(help);
	return help;
}

mvceditor::ConfigTagDetectorClass::ConfigTagDetectorClass()  {

}

bool mvceditor::ConfigTagDetectorClass::CanTest(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project) {
	return true;
}

wxString mvceditor::ConfigTagDetectorClass::TestCommandLine(const mvceditor::GlobalsClass& globals, 
													  const mvceditor::ProjectClass& project,
													  const wxString& detectorScriptFullPath) {
	
	mvceditor::SourceClass source = project.Sources[0];														  
	
	mvceditor::ConfigTagDetectorParamsClass params;
	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = mvceditor::PhpDetectorsBaseAsset();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	return params.BuildCmdLine();
}

wxFileName mvceditor::ConfigTagDetectorClass::LocalRootDir() {
	return mvceditor::ConfigTagDetectorsLocalAsset();
}

wxFileName mvceditor::ConfigTagDetectorClass::GlobalRootDir() {
	return mvceditor::ConfigTagDetectorsGlobalAsset();
}

wxFileName mvceditor::ConfigTagDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = mvceditor::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("ConfigDetector.skeleton.php"));
	return skeletonFile;
}

wxString mvceditor::ConfigTagDetectorClass::Label() {
	return _("Config Detectors");
}

wxString mvceditor::ConfigTagDetectorClass::HelpMessage() {
	wxString help = wxString::FromAscii(
		"Config Detectors are PHP scripts that MVC Editor uses to find out "
		"any config files that PHP frameworks use.  \n"
		"MVC Editor can detect config files for CodeIgniter projects.\n"
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
															  mvceditor::EventSinkClass& eventSink,
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
	TestButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnTestButton), NULL, this);
	AddButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnAddButton), NULL, this);
	HelpButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnHelpButton), NULL, this);
	DetectorTree->Connect(wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemDelete), NULL, this);
	DetectorTree->Connect(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemBeginLabelEdit), NULL, this);
	DetectorTree->Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemEndLabelEdit), NULL, this);
	DetectorTree->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemActivated), NULL, this);
	DetectorTree->Connect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemRightClick), NULL, this);

	ImageList = new wxImageList(16, 16);
	ImageList->Add(rootImage, wxNullBitmap);
	ImageList->Add(mvceditor::IconImageAsset(wxT("folder-horizontal")), wxNullBitmap);
	ImageList->Add(mvceditor::IconImageAsset(wxT("folder-horizontal-open")), wxNullBitmap);
	ImageList->Add(mvceditor::IconImageAsset(wxT("document-php")), wxNullBitmap);

	// this pointer will be managed by the tree control, since the tree control
	// may use the pointer in the destructor.
	DetectorTree->SetImageList(ImageList);
}

mvceditor::DetectorTreeHandlerClass::~DetectorTreeHandlerClass() {
	// Connect Events
	TestButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnTestButton), NULL, this);
	AddButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnAddButton), NULL, this);
	HelpButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DetectorTreeHandlerClass::OnHelpButton), NULL, this);
	DetectorTree->Disconnect(wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemDelete), NULL, this);
	DetectorTree->Disconnect(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemBeginLabelEdit), NULL, this);
	DetectorTree->Disconnect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemEndLabelEdit), NULL, this);
	DetectorTree->Disconnect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemActivated), NULL, this);
	DetectorTree->Disconnect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(DetectorTreeHandlerClass::OnTreeItemRightClick), NULL, this);
}

void mvceditor::DetectorTreeHandlerClass::Init() {
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

void mvceditor::DetectorTreeHandlerClass::UpdateProjects() {
	wxArrayString projectLabels;
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	std::vector<mvceditor::ProjectClass> enabledProjects = Globals.AllEnabledProjects();
	for (project = enabledProjects.begin(); project != enabledProjects.end(); ++project) {
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
		wxFFile skeletonFile(urlDetectorSkeleton.GetFullPath(), wxT("rb"));
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
		DetectorTree->AppendItem(localLabelTreeItemId, name, IMAGE_SCRIPT, IMAGE_SCRIPT, treeItemData);
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
	mvceditor::ProjectClass project = Globals.AllEnabledProjects()[projectChoiceIndex];
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

void mvceditor::DetectorTreeHandlerClass::OnTreeItemBeginLabelEdit(wxTreeEvent& event) {

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

void mvceditor::DetectorTreeHandlerClass::OnTreeItemEndLabelEdit(wxTreeEvent& event) {
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

mvceditor::UrlTagDetectorPanelClass::UrlTagDetectorPanelClass(wxWindow* parent, int id, mvceditor::GlobalsClass& globals,
														mvceditor::EventSinkClass& eventSink)
	: UrlDetectorPanelGeneratedClass(parent, id) 
	, Detector() 
	, Handler(DetectorTree, TestButton, AddButton, HelpButton, ProjectChoice, &Detector, globals, eventSink, mvceditor::IconImageAsset(wxT("url-detectors"))) {
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

mvceditor::UrlTagDetectorPanelClass::~UrlTagDetectorPanelClass() {
	Disconnect(ID_DETECTOR_TREE_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuOpenDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_RENAME, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuRenameDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_DELETE, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuDeleteDetector), NULL, &Handler);
}

void mvceditor::UrlTagDetectorPanelClass::Init() {
	Handler.Init();
}

void mvceditor::UrlTagDetectorPanelClass::UpdateProjects() {
	Handler.UpdateProjects();
}

mvceditor::TemplateFileTagsDetectorPanelClass::TemplateFileTagsDetectorPanelClass(wxWindow* parent, int id, mvceditor::GlobalsClass& globals,
														mvceditor::EventSinkClass& eventSink,
														mvceditor::RunningThreadsClass& runningThreads)
	: TemplateFilesDetectorPanelGeneratedClass(parent, id) 
	, Detector() 
	, Handler(DetectorTree, TestButton, AddButton, HelpButton, ProjectChoice, &Detector, globals, eventSink, mvceditor::IconImageAsset(wxT("template-file-detectors")))
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

mvceditor::TemplateFileTagsDetectorPanelClass::~TemplateFileTagsDetectorPanelClass() {
	Disconnect(ID_DETECTOR_TREE_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuOpenDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_RENAME, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuRenameDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_DELETE, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuDeleteDetector), NULL, &Handler);
}

void mvceditor::TemplateFileTagsDetectorPanelClass::Init() {
	Handler.Init();
}

void mvceditor::TemplateFileTagsDetectorPanelClass::UpdateProjects() {
	Handler.UpdateProjects();
}

void mvceditor::TemplateFileTagsDetectorPanelClass::OnChooseUrlButton(wxCommandEvent& event) {
	TestUrl.Reset();
	mvceditor::ChooseUrlDialogClass dialog(this, Globals.UrlTagFinder, Globals.Projects, TestUrl);
	if (dialog.ShowModal() == wxOK) {
		UrlToTest->SetValue(TestUrl.Url.BuildURI());
		mvceditor::CallStackActionClass* action = new mvceditor::CallStackActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_CALL_STACK);
		action->SetCallStackStart(TestUrl.FileName,
			mvceditor::WxToIcu(TestUrl.ClassName),
			mvceditor::WxToIcu(TestUrl.MethodName),

			// the selection index is the index of the enabled projects
			Globals.DetectorCacheDbFileName
		);
		action->Init(Globals);
		RunningThreads.Queue(action);
	}
}

mvceditor::TagDetectorPanelClass::TagDetectorPanelClass(wxWindow* parent, int id, mvceditor::GlobalsClass& globals,
														mvceditor::EventSinkClass& eventSink)
	: TagDetectorPanelGeneratedClass(parent, id) 
	, Detector() 
	, Handler(DetectorTree, TestButton, AddButton, HelpButton, ProjectChoice, &Detector, globals, eventSink, mvceditor::IconImageAsset(wxT("tag-detectors"))) {
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

mvceditor::TagDetectorPanelClass::~TagDetectorPanelClass() {
	Disconnect(ID_DETECTOR_TREE_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuOpenDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_RENAME, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuRenameDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_DELETE, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuDeleteDetector), NULL, &Handler);
}

void mvceditor::TagDetectorPanelClass::Init() {
	Handler.Init();
}

void mvceditor::TagDetectorPanelClass::UpdateProjects() {
	Handler.UpdateProjects();
}

mvceditor::DatabaseTagDetectorPanelClass::DatabaseTagDetectorPanelClass(wxWindow* parent, int id, mvceditor::GlobalsClass& globals,
														mvceditor::EventSinkClass& eventSink)
	: DatabaseDetectorPanelGeneratedClass(parent, id) 
	, Detector() 
	, Handler(DetectorTree, TestButton, AddButton, HelpButton, ProjectChoice, &Detector, globals, eventSink, mvceditor::IconImageAsset(wxT("database-detectors"))) {
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

mvceditor::DatabaseTagDetectorPanelClass::~DatabaseTagDetectorPanelClass() {
	Disconnect(ID_DETECTOR_TREE_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuOpenDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_RENAME, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuRenameDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_DELETE, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuDeleteDetector), NULL, &Handler);
}

void mvceditor::DatabaseTagDetectorPanelClass::Init() {
	Handler.Init();
}

void mvceditor::DatabaseTagDetectorPanelClass::UpdateProjects() {
	Handler.UpdateProjects();
}

mvceditor::ConfigTagDetectorPanelClass::ConfigTagDetectorPanelClass(wxWindow* parent, int id, mvceditor::GlobalsClass& globals,
														mvceditor::EventSinkClass& eventSink)
	: ConfigDetectorPanelGeneratedClass(parent, id) 
	, Detector() 
	, Handler(DetectorTree, TestButton, AddButton, HelpButton, ProjectChoice, &Detector, globals, eventSink, mvceditor::IconImageAsset(wxT("config-detectors"))) {
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

mvceditor::ConfigTagDetectorPanelClass::~ConfigTagDetectorPanelClass() {
	Disconnect(ID_DETECTOR_TREE_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuOpenDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_RENAME, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuRenameDetector), NULL, &Handler);
	Disconnect(ID_DETECTOR_TREE_DELETE, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(DetectorTreeHandlerClass::OnMenuDeleteDetector), NULL, &Handler);
}

void mvceditor::ConfigTagDetectorPanelClass::Init() {
	Handler.Init();
}

void mvceditor::ConfigTagDetectorPanelClass::UpdateProjects() {
	Handler.UpdateProjects();
}

mvceditor::DetectorFeatureClass::DetectorFeatureClass(mvceditor::AppClass &app)
	: FeatureClass(app) {

}

void mvceditor::DetectorFeatureClass::AddViewMenuItems(wxMenu* menu) {
	menu->Append(mvceditor::MENU_DETECTORS + 0, _("View URL Detectors"), _("View the URL Detectors"), wxITEM_NORMAL);
	menu->Append(mvceditor::MENU_DETECTORS + 1, _("View Template File Detectors"), _("View the Template File Detectors"), wxITEM_NORMAL);
	menu->Append(mvceditor::MENU_DETECTORS + 2, _("View Tag Detectors"), _("View the Tag Detectors"), wxITEM_NORMAL);
	menu->Append(mvceditor::MENU_DETECTORS + 3, _("View Database Detectors"), _("View the Database Detectors"), wxITEM_NORMAL);
	menu->Append(mvceditor::MENU_DETECTORS + 4, _("View Config Detectors"), _("View the config Detectors"), wxITEM_NORMAL);
}

void mvceditor::DetectorFeatureClass::AddNewMenu(wxMenuBar* menuBar) {
	wxMenu* menu = new wxMenu(0);
	menu->Append(mvceditor::MENU_DETECTORS + 5, _("Run URL Detection"), _("Run the URL Detectors against the current projects"), wxITEM_NORMAL);
	menu->Append(mvceditor::MENU_DETECTORS + 6, _("Run Template File Detection"), _("Run the Template file Detectors against the current projects"), wxITEM_NORMAL);
	menu->Append(mvceditor::MENU_DETECTORS + 7, _("Run Tag Detection"), _("Run the Tag Detectors against the current projects"), wxITEM_NORMAL);
	menu->Append(mvceditor::MENU_DETECTORS + 8, _("Run Database Detection"), _("Run the Database Detectors against the current projects"), wxITEM_NORMAL);
	menu->Append(mvceditor::MENU_DETECTORS + 9, _("Run Config Detection"), _("Run the Config Detectors against the current projects"), wxITEM_NORMAL);
	menuBar->Append(menu, _("Detectors"));
}

void mvceditor::DetectorFeatureClass::OnViewUrlDetectors(wxCommandEvent& event) {
	wxWindow* window = FindOutlineWindow(ID_URL_DETECTOR_PANEL);
	if (window) {
		SetFocusToOutlineWindow(window);
	}
	else {
		mvceditor::UrlTagDetectorPanelClass* panel = new mvceditor::UrlTagDetectorPanelClass(GetOutlineNotebook(), ID_URL_DETECTOR_PANEL, 
			App.Globals, App.EventSink);
		wxBitmap urlBitmap = mvceditor::IconImageAsset(wxT("url-detectors"));
		if (AddOutlineWindow(panel, _("URL Detectors"), urlBitmap)) {
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
		mvceditor::TemplateFileTagsDetectorPanelClass* panel = new mvceditor::TemplateFileTagsDetectorPanelClass(GetOutlineNotebook(), ID_TEMPLATE_FILES_DETECTOR_PANEL, 
			App.Globals, App.EventSink, App.RunningThreads);
		wxBitmap templateFilesBitmap = mvceditor::IconImageAsset(wxT("template-file-detectors"));
		if (AddOutlineWindow(panel, _("Template Files Detectors"), templateFilesBitmap)) {
			panel->Init();
			panel->UpdateProjects();
		}
	}
}

void mvceditor::DetectorFeatureClass::OnViewTagDetectors(wxCommandEvent& event) {
	wxWindow* window = FindOutlineWindow(ID_TAG_DETECTOR_PANEL);
	if (window) {
		SetFocusToOutlineWindow(window);
	}
	else {
		mvceditor::TagDetectorPanelClass* panel = new mvceditor::TagDetectorPanelClass(GetOutlineNotebook(), ID_TAG_DETECTOR_PANEL, 
			App.Globals, App.EventSink);
		wxBitmap tagBitmap = mvceditor::IconImageAsset(wxT("tag-detectors"));
		if (AddOutlineWindow(panel, _("Tag Detectors"), tagBitmap)) {
			panel->Init();
			panel->UpdateProjects();
		}
	}
}

void mvceditor::DetectorFeatureClass::OnViewDatabaseDetectors(wxCommandEvent& event) {
	wxWindow* window = FindOutlineWindow(ID_DATABASE_DETECTOR_PANEL);
	if (window) {
		SetFocusToOutlineWindow(window);
	}
	else {
		mvceditor::DatabaseTagDetectorPanelClass* panel = new mvceditor::DatabaseTagDetectorPanelClass(GetOutlineNotebook(), ID_DATABASE_DETECTOR_PANEL, 
			App.Globals, App.EventSink);
		wxBitmap databaseBitmap = mvceditor::IconImageAsset(wxT("database-detectors"));
		if (AddOutlineWindow(panel, _("Database Detectors"), databaseBitmap)) {
			panel->Init();
			panel->UpdateProjects();
		}
	}
}

void mvceditor::DetectorFeatureClass::OnViewConfigDetectors(wxCommandEvent& event) {
	wxWindow* window = FindOutlineWindow(ID_CONFIG_DETECTOR_PANEL);
	if (window) {
		SetFocusToOutlineWindow(window);
	}
	else {
		mvceditor::ConfigTagDetectorPanelClass* panel = new mvceditor::ConfigTagDetectorPanelClass(GetOutlineNotebook(), ID_CONFIG_DETECTOR_PANEL, 
			App.Globals, App.EventSink);
		wxBitmap configBitmap = mvceditor::IconImageAsset(wxT("config-detectors"));
		if (AddOutlineWindow(panel, _("Config Detectors"), configBitmap)) {
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
	std::vector<mvceditor::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	actions.push_back(
		new mvceditor::UrlTagDetectorActionClass(App.RunningThreads, mvceditor::ID_EVENT_ACTION_URL_TAG_DETECTOR)	
	);
	App.Sequences.Build(actions);
}

void mvceditor::DetectorFeatureClass::OnRunTemplateFileDetectors(wxCommandEvent& event) {
	if (App.Sequences.Running()) {
		wxMessageBox(_("Please wait for the current background task to finish"));
		return;
	}
	std::vector<mvceditor::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	mvceditor::CallStackActionClass* callStackAction =  new mvceditor::CallStackActionClass(App.RunningThreads, mvceditor::ID_EVENT_ACTION_CALL_STACK);
	mvceditor::UrlTagClass urlTag = App.Globals.CurrentUrl;
	
	if (!urlTag.Url.GetServer().IsEmpty() && urlTag.FileName.IsOk()
		&& !urlTag.ClassName.IsEmpty() && !urlTag.MethodName.IsEmpty()) {
		callStackAction->SetCallStackStart(urlTag.FileName,
			mvceditor::WxToIcu(urlTag.ClassName),
			mvceditor::WxToIcu(urlTag.MethodName),
			App.Globals.DetectorCacheDbFileName);
		actions.push_back(callStackAction);
		actions.push_back(
			new mvceditor::TemplateFileTagsDetectorActionClass(App.RunningThreads, mvceditor::ID_EVENT_ACTION_TEMPLATE_FILE_TAG_DETECTOR)
		);
		App.Sequences.Build(actions);
	}
	else {
		mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER, 
			_("Need to choose a URL to detect templates for. Template files feature depends on the URL detectors feature."));
	}
}

void mvceditor::DetectorFeatureClass::OnRunTagDetectors(wxCommandEvent& event) {
	std::vector<mvceditor::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	actions.push_back(
		new mvceditor::TagDetectorActionClass(App.RunningThreads, mvceditor::ID_EVENT_ACTION_TAG_DETECTOR)
	);
	App.Sequences.Build(actions);
}

void mvceditor::DetectorFeatureClass::OnRunDatabaseDetectors(wxCommandEvent& event) {
	std::vector<mvceditor::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	actions.push_back(
		new mvceditor::DatabaseTagDetectorActionClass(App.RunningThreads, mvceditor::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR)
	);
	App.Sequences.Build(actions);
}

void mvceditor::DetectorFeatureClass::OnRunConfigDetectors(wxCommandEvent& event) {
	std::vector<mvceditor::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	actions.push_back(
		new mvceditor::ConfigTagDetectorActionClass(App.RunningThreads, mvceditor::ID_EVENT_ACTION_CONFIG_TAG_DETECTOR)
	);
	App.Sequences.Build(actions);
}

void mvceditor::DetectorFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	wxWindow* window = FindOutlineWindow(ID_URL_DETECTOR_PANEL);
	if (window) {
		mvceditor::UrlTagDetectorPanelClass* panel = (mvceditor::UrlTagDetectorPanelClass*) window;
		panel->UpdateProjects();
	}
	window = FindOutlineWindow(ID_TEMPLATE_FILES_DETECTOR_PANEL);
	if (window) {
		mvceditor::TemplateFileTagsDetectorPanelClass* panel = (mvceditor::TemplateFileTagsDetectorPanelClass*) window;
		panel->UpdateProjects();
	}
	window = FindOutlineWindow(ID_TAG_DETECTOR_PANEL);
	if (window) {
		mvceditor::TagDetectorPanelClass* panel = (mvceditor::TagDetectorPanelClass*) window;
		panel->UpdateProjects();
	}
	window = FindOutlineWindow(ID_DATABASE_DETECTOR_PANEL);
	if (window) {
		mvceditor::DatabaseTagDetectorPanelClass* panel = (mvceditor::DatabaseTagDetectorPanelClass*) window;
		panel->UpdateProjects();
	}
	window = FindOutlineWindow(ID_CONFIG_DETECTOR_PANEL);
	if (window) {
		mvceditor::ConfigTagDetectorPanelClass* panel = (mvceditor::ConfigTagDetectorPanelClass*) window;
		panel->UpdateProjects();
	}
}

BEGIN_EVENT_TABLE(mvceditor::DetectorFeatureClass, mvceditor::FeatureClass)
	EVT_MENU(mvceditor::MENU_DETECTORS + 0, mvceditor::DetectorFeatureClass::OnViewUrlDetectors)
	EVT_MENU(mvceditor::MENU_DETECTORS + 1, mvceditor::DetectorFeatureClass::OnViewTemplateFileDetectors)
	EVT_MENU(mvceditor::MENU_DETECTORS + 2, mvceditor::DetectorFeatureClass::OnViewTagDetectors)
	EVT_MENU(mvceditor::MENU_DETECTORS + 3, mvceditor::DetectorFeatureClass::OnViewDatabaseDetectors)
	EVT_MENU(mvceditor::MENU_DETECTORS + 4, mvceditor::DetectorFeatureClass::OnViewConfigDetectors)

	EVT_MENU(mvceditor::MENU_DETECTORS + 5, mvceditor::DetectorFeatureClass::OnRunUrlDetectors)
	EVT_MENU(mvceditor::MENU_DETECTORS + 6, mvceditor::DetectorFeatureClass::OnRunTemplateFileDetectors)
	EVT_MENU(mvceditor::MENU_DETECTORS + 7, mvceditor::DetectorFeatureClass::OnRunTagDetectors)
	EVT_MENU(mvceditor::MENU_DETECTORS + 8, mvceditor::DetectorFeatureClass::OnRunDatabaseDetectors)
	EVT_MENU(mvceditor::MENU_DETECTORS + 9, mvceditor::DetectorFeatureClass::OnRunConfigDetectors)

	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PREFERENCES_SAVED, mvceditor::DetectorFeatureClass::OnPreferencesSaved)
END_EVENT_TABLE()
