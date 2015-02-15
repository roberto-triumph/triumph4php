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
#include <features/TemplateFilesFeatureClass.h>
#include <widgets/TreeItemDataStringClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <globals/TemplateFileTagClass.h>
#include <actions/CallStackActionClass.h>
#include <actions/TemplateFileTagsDetectorActionClass.h>
#include <actions/SequenceClass.h>
#include <Triumph.h>
#include <wx/artprov.h>

static const int ID_TEMPLATE_FILES_PANEL = wxNewId();

t4p::TemplateFilesFeatureClass::TemplateFilesFeatureClass(t4p::AppClass& app) 
	: FeatureClass(app) {
}

void t4p::TemplateFilesFeatureClass::AddViewMenuItems(wxMenu* viewMenu) {
	viewMenu->Append(t4p::MENU_TEMPLATE_FILES + 0, _("PHP Template Files"), 
		_("Shows the view (template) files for the currently selected URL"), wxITEM_NORMAL);
}

void t4p::TemplateFilesFeatureClass::OnTemplateFilesMenu(wxCommandEvent& event) {
	std::vector<wxFileName> sourceDirs = App.Globals.AllEnabledSourceDirectories();
	if (App.Globals.UrlTagFinder.Count(sourceDirs) > 0) {
		ShowPanel();
	}
	else {
		t4p::EditorLogWarningFix("Template Feature", 
			_("Could not determine template files because no URLs were detected. Template files feature depends on the URL detectors feature."));
	}
}

void t4p::TemplateFilesFeatureClass::ShowPanel() {
	wxWindow* window = FindOutlineWindow(ID_TEMPLATE_FILES_PANEL);
	t4p::TemplateFilesPanelClass* templateFilesPanel = NULL;
	if (window) {
		templateFilesPanel = wxDynamicCast(window, t4p::TemplateFilesPanelClass);
		SetFocusToOutlineWindow(templateFilesPanel);
	}
	else {
		templateFilesPanel = new t4p::TemplateFilesPanelClass(GetOutlineNotebook(), ID_TEMPLATE_FILES_PANEL, *this);
		wxBitmap templateFileBitmap = t4p::BitmapImageAsset(wxT("template-files"));
		AddOutlineWindow(templateFilesPanel, _("Templates"), templateFileBitmap);
	}
	templateFilesPanel->UpdateControllers();
}

wxString t4p::TemplateFilesFeatureClass::CurrentFile() {
	t4p::CodeControlClass* ctrl = GetCurrentCodeControl();
	wxString fileName;
	if (ctrl) {
		fileName = ctrl->GetFileName();
	}
	return fileName;
}

void t4p::TemplateFilesFeatureClass::StartDetection() {	
	
	// start the chain reaction
	if (App.Sequences.Running()) {
		wxMessageBox(_("Please wait for the current background task to finish"));
		return;
	}
	std::vector<t4p::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	t4p::CallStackActionClass* callStackAction =  new t4p::CallStackActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_CALL_STACK);
	t4p::UrlTagClass urlTag = App.Globals.CurrentUrl;
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

t4p::UrlTagFinderClass& t4p::TemplateFilesFeatureClass::Urls() {
	return App.Globals.UrlTagFinder;
}

void t4p::TemplateFilesFeatureClass::OpenFile(wxString file) {
	wxFileName fileName(file);
	if (fileName.IsOk()) {
		t4p::OpenFileCommandEventClass openEvent(fileName.GetFullPath());
		App.EventSink.Publish(openEvent);
	}
}

void t4p::TemplateFilesFeatureClass::SetCurrentUrl(t4p::UrlTagClass url) {
	App.Globals.CurrentUrl = url;
}

void t4p::TemplateFilesFeatureClass::OnTemplateDetectionComplete(t4p::ActionEventClass& event) {
	wxWindow* window = FindOutlineWindow(ID_TEMPLATE_FILES_PANEL);
	t4p::TemplateFilesPanelClass* templateFilesPanel = NULL;
	if (window) {
		templateFilesPanel = (t4p::TemplateFilesPanelClass*) window;
	}
	else {
		templateFilesPanel = new t4p::TemplateFilesPanelClass(GetOutlineNotebook(), ID_TEMPLATE_FILES_PANEL, *this);
		AddOutlineWindow(templateFilesPanel, _("Templates"));
	}
	templateFilesPanel->UpdateResults();
}

t4p::TemplateFilesPanelClass::TemplateFilesPanelClass(wxWindow* parent, int id, t4p::TemplateFilesFeatureClass& feature)
	: TemplateFilesPanelGeneratedClass(parent, id)
	, Feature(feature) 
	, ImageList(16, 16) {
	StatusLabel->SetLabel(_(""));
	HelpButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_HELP, 
		wxART_TOOLBAR, wxSize(16, 16))));
	ImageList.Add(t4p::BitmapImageAsset(wxT("folder-horizontal")));
	ImageList.Add(t4p::BitmapImageAsset(wxT("folder-horizontal-open")));
	ImageList.Add(t4p::BitmapImageAsset(wxT("template-files")));
	ImageList.Add(t4p::BitmapImageAsset(wxT("variable-template")));
	
	// this class will own the imagelist
	FileTree->SetImageList(&ImageList);
	TemplateVariablesTree->SetImageList(&ImageList);
}

void t4p::TemplateFilesPanelClass::UpdateControllers() {
	wxArrayString wxControllers;
	std::vector<wxFileName> sourceDirs = Feature.App.Globals.AllEnabledSourceDirectories();
	std::vector<wxString> controllers = Feature.App.Globals.UrlTagFinder.AllControllerNames(sourceDirs);
	for (size_t i = 0; i < controllers.size(); ++i) {
		wxControllers.Add(controllers[i]);
	}
	wxControllers.Sort();
	Controller->Clear();
	Controller->Append(wxControllers);
}

void t4p::TemplateFilesPanelClass::UpdateResults() {		
	std::vector<t4p::TemplateFileTagClass> currentTemplates = Feature.App.Globals.CurrentTemplates();
	StatusLabel->SetLabel(wxString::Format(_("Found %ld view files"), currentTemplates.size()));
	FileTree->DeleteAllItems();

	wxTreeItemId parent = FileTree->AddRoot(_("Templates"), IMAGE_TEMPLATE_FOLDER);
	FileTree->SetItemImage(parent, IMAGE_TEMPLATE_FOLDER_OPEN, wxTreeItemIcon_Expanded);
	for (size_t i = 0; i < currentTemplates.size(); ++i) {
		wxString templateFile = currentTemplates[i].FullPath;

		// remove the project root so that the dialog is not too 'wordy'
		wxString projectLabel;
		wxString text = Feature.App.Globals.RelativeFileName(templateFile, projectLabel);
		if (!wxFileName::FileExists(templateFile)) {

			// show that the view file is missing
			text = wxT("[X] ") + text;
		}
		t4p::TreeItemDataStringClass* data = new t4p::TreeItemDataStringClass(currentTemplates[i].FullPath);
		FileTree->AppendItem(parent, text, IMAGE_TEMPLATE_FILE, -1, data);
	}
	FileTree->ExpandAll();
	
	int variableCount = 0;
	for (size_t i = 0; i < currentTemplates.size(); ++i) {
		variableCount += currentTemplates[i].Variables.size();
	}
	TemplateVariablesLabel->SetLabel(wxString::Format(_("Found %d template variables"), variableCount));
	TemplateVariablesTree->DeleteAllItems();
	parent = TemplateVariablesTree->AddRoot(_("Template Variables"), IMAGE_TEMPLATE_FOLDER);
	FileTree->SetItemImage(parent, IMAGE_TEMPLATE_FOLDER_OPEN, wxTreeItemIcon_Expanded);
	for (size_t i = 0; i < currentTemplates.size(); ++i) {
		t4p::TemplateFileTagClass templateFile = currentTemplates[i];
		wxString text = templateFile.FullPath;
		wxString projectLabel;
		
		// remove the project root so that the dialog is not too 'wordy'
		text = Feature.App.Globals.RelativeFileName(text, projectLabel);
		wxTreeItemId sub = TemplateVariablesTree->AppendItem(parent, text, IMAGE_TEMPLATE_FILE);
		for (size_t j = 0; j < templateFile.Variables.size(); ++j) {
			TemplateVariablesTree->AppendItem(sub, templateFile.Variables[j], IMAGE_TEMPLATE_VARIABLE);
		}
	}
	TemplateVariablesTree->ExpandAll();
}

void t4p::TemplateFilesPanelClass::ClearResults() {
	StatusLabel->SetLabel(_(""));
	Action->Clear();
	FileTree->DeleteAllItems();
}

void t4p::TemplateFilesPanelClass::OnActionChoice(wxCommandEvent& event) {
	wxString controller = Controller->GetStringSelection();
	wxString action = Action->GetStringSelection();

	std::vector<wxFileName> sourceDirs = Feature.App.Globals.AllEnabledSourceDirectories();
	t4p::UrlTagClass url;
	std::vector<t4p::UrlTagClass>::iterator it;
	bool found = Feature.App.Globals.UrlTagFinder.FindByClassMethod(controller, action, sourceDirs, url);
	if (found) {
		StatusLabel->SetLabel(_("Detecting"));
		Feature.SetCurrentUrl(url);
		Feature.StartDetection();
	}
}

void t4p::TemplateFilesPanelClass::OnControllerChoice(wxCommandEvent &event) {
	ClearResults();
	wxString controller = event.GetString();
	std::vector<wxFileName> sourceDirs = Feature.App.Globals.AllEnabledSourceDirectories();
	std::vector<wxString> methodNames = Feature.App.Globals.UrlTagFinder.AllMethodNames(controller, sourceDirs);
	for (size_t i = 0; i < methodNames.size(); ++i) {
		Action->AppendString(methodNames[i]);
	}
}

void t4p::TemplateFilesPanelClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = wxString::FromAscii(
		"The template files outline lets you see the template (\"view\" files) for a "
		"specific controller. Choose a controller, then an action, and "
		"the pane will be populated with all of the templates that the "
		"selected action uses.  If a template file has an [X] it means "
		"that the template file was not found."
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("View Files Help"), wxOK, this);
}

void t4p::TemplateFilesPanelClass::OnCurrentButton(wxCommandEvent &event) {
	ClearResults();
	UpdateControllers();

	wxString current = Feature.CurrentFile();
	wxFileName currentFileName(current);
	std::vector<wxFileName> sourceDirs = Feature.App.Globals.AllEnabledSourceDirectories();
	std::vector<t4p::UrlTagClass> urls;
	Feature.App.Globals.UrlTagFinder.FilterByFullPath(current, sourceDirs, urls);
	std::vector<t4p::UrlTagClass>::iterator it;
	wxArrayString controllers;
	wxArrayString methods;
	for (it = urls.begin(); it != urls.end(); ++it) {
		controllers.Add(it->ClassName);
		methods.Add(it->MethodName);
	}
	controllers.Sort();
	methods.Sort();
	Controller->Append(controllers);
	Action->Append(methods);
}

void t4p::TemplateFilesPanelClass::OnTreeItemActivated(wxTreeEvent& event) {
	wxTreeItemId item = event.GetItem();
	wxString file = FileTree->GetItemText(item);
	if (!file.IsEmpty() && file.Find(wxT("[X]")) != 0 && item != FileTree->GetRootItem()) {
		t4p::TreeItemDataStringClass* data = (t4p::TreeItemDataStringClass*)FileTree->GetItemData(item);
		if (data) {
			file =  data->Str;
			Feature.OpenFile(file);
		}
	}
}

BEGIN_EVENT_TABLE(t4p::TemplateFilesFeatureClass, wxEvtHandler) 
	EVT_MENU(t4p::MENU_TEMPLATE_FILES + 0, t4p::TemplateFilesFeatureClass::OnTemplateFilesMenu)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_TEMPLATE_FILE_TAG_DETECTOR, t4p::TemplateFilesFeatureClass::OnTemplateDetectionComplete)
END_EVENT_TABLE()
