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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <features/EditorBehaviorFeatureClass.h>
#include <globals/Assets.h>
#include <Triumph.h>

t4p::EditorBehaviorFeatureClass::EditorBehaviorFeatureClass(t4p::AppClass& app)
: FeatureClass(app) {
}

void t4p::EditorBehaviorFeatureClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	toolBar->AddSeparator();
	
	toolBar->AddTool(t4p::MENU_BEHAVIOR + 0, _("Rectangular"), 
		t4p::BitmapImageAsset(wxT("rectangular-selection")), _("Enable / disable rectangular selection"), wxITEM_NORMAL
	);
	toolBar->AddTool(t4p::MENU_BEHAVIOR + 1, _("Word Wrap"), 
		t4p::BitmapImageAsset(wxT("wrap")), _("Enable / disable word wrap"), wxITEM_NORMAL
	);
	toolBar->AddTool(t4p::MENU_BEHAVIOR + 2, _("Indentation"), 
		t4p::BitmapImageAsset(wxT("indentation")), _("Enable / disable identation guides"), wxITEM_NORMAL
	);
	toolBar->AddTool(t4p::MENU_BEHAVIOR + 3, _("Whitespace"), 
		t4p::BitmapImageAsset(wxT("whitespace")), _("Enable / disable showing whitespace"), wxITEM_NORMAL
	);
}

void t4p::EditorBehaviorFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[t4p::MENU_BEHAVIOR + 4] = wxT("Editor-Zoom In");
	menuItemIds[t4p::MENU_BEHAVIOR + 5] = wxT("Editor-Zoom Out");
	menuItemIds[t4p::MENU_BEHAVIOR + 6] = wxT("Editor-Reset Zoom");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void t4p::EditorBehaviorFeatureClass::AddViewMenuItems(wxMenu* menu) {
	menu->Append(t4p::MENU_BEHAVIOR + 4, _("Zoom In"), _("Zoom In"));
	menu->Append(t4p::MENU_BEHAVIOR + 5, _("Zoom Out"), _("Zoom Out"));
	menu->Append(t4p::MENU_BEHAVIOR + 6, _("Reset Zoom\tCTRL+0"), _("Reset Zoom"));
}

void t4p::EditorBehaviorFeatureClass::OnToggleRectangularSelection(wxCommandEvent& event) {
	App.Preferences.CodeControlOptions.EnableRectangularSelection = !App.Preferences.CodeControlOptions.EnableRectangularSelection;
	
	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
	
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorFeatureClass::OnToggleWordWrap(wxCommandEvent& event) {
	App.Preferences.CodeControlOptions.EnableWordWrap = !App.Preferences.CodeControlOptions.EnableWordWrap;
	
	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
	
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorFeatureClass::OnToggleIndentationGuides(wxCommandEvent& event) {
	App.Preferences.CodeControlOptions.EnableIndentationGuides = !App.Preferences.CodeControlOptions.EnableIndentationGuides;
	
	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
	
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorFeatureClass::OnToggleWhitespace(wxCommandEvent& event) {
	App.Preferences.CodeControlOptions.ShowWhitespace = !App.Preferences.CodeControlOptions.ShowWhitespace;
	
	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
	
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorFeatureClass::OnZoomIn(wxCommandEvent& event) {
	App.Preferences.CodeControlOptions.Zoom++;
	if (App.Preferences.CodeControlOptions.Zoom > 50) {
		App.Preferences.CodeControlOptions.Zoom = 50;
	}
	
	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
	
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorFeatureClass::OnZoomOut(wxCommandEvent& event) {
	App.Preferences.CodeControlOptions.Zoom--;
	if (App.Preferences.CodeControlOptions.Zoom < -35) {
		App.Preferences.CodeControlOptions.Zoom = -35;
	}
	
	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
	
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorFeatureClass::OnZoomReset(wxCommandEvent& event) {
	App.Preferences.CodeControlOptions.Zoom = 0;
	
	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
	
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorFeatureClass::SetFeaturesOnNotebook() {
	t4p::NotebookClass* notebook = GetNotebook();
	t4p::CodeControlOptionsClass options = App.Preferences.CodeControlOptions;
	for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
		SetFeatures(options, notebook->GetCodeControl(i));
	}
}

void t4p::EditorBehaviorFeatureClass::SetFeatures(const t4p::CodeControlOptionsClass& options, wxStyledTextCtrl* codeCtrl) {
	if (options.IndentUsingTabs) {
		codeCtrl->SetUseTabs(true);
		codeCtrl->SetTabWidth(options.TabWidth);
		codeCtrl->SetIndent(0);
		codeCtrl->SetTabIndents(true);
		codeCtrl->SetBackSpaceUnIndents(true);
	}
	else {
		codeCtrl->SetUseTabs(false);
		codeCtrl->SetTabWidth(options.SpacesPerIndent);
		codeCtrl->SetIndent(options.SpacesPerIndent);
		codeCtrl->SetTabIndents(false);
		codeCtrl->SetBackSpaceUnIndents(false);
	}
	if (options.RightMargin > 0) {
		codeCtrl->SetEdgeMode(wxSTC_EDGE_LINE);
		codeCtrl->SetEdgeColumn(options.RightMargin);
	}
	else {
		codeCtrl->SetEdgeMode(wxSTC_EDGE_NONE);
	}
	codeCtrl->SetIndentationGuides(options.EnableIndentationGuides);
	codeCtrl->SetEOLMode(options.LineEndingMode);
	codeCtrl->SetViewEOL(options.EnableLineEndings);
	
	if (options.EnableWordWrap) {
		codeCtrl->SetWrapMode(wxSTC_WRAP_WORD);
		codeCtrl->SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_START);
	}
	else {
		codeCtrl->SetWrapMode(wxSTC_WRAP_NONE);
	}
	
	codeCtrl->SetMultipleSelection(options.EnableMultipleSelection);
	codeCtrl->SetAdditionalSelectionTyping(options.EnableMultipleSelection);
	if (options.EnableMultipleSelection) {
		codeCtrl->SetMultiPaste(wxSTC_MULTIPASTE_EACH);
		
	}
	else {
		codeCtrl->SetMultiPaste(wxSTC_MULTIPASTE_ONCE);
	}
	
	int virtualSpaceOpts = wxSTC_SCVS_NONE;
	if (options.EnableVirtualSpace) {
		virtualSpaceOpts |= wxSTC_SCVS_USERACCESSIBLE;
	}
	if (options.EnableRectangularSelection) {
		virtualSpaceOpts |= wxSTC_SCVS_RECTANGULARSELECTION;
	}
	codeCtrl->SetVirtualSpaceOptions(virtualSpaceOpts);
	
	int selectionMode = wxSTC_SEL_STREAM;
	if (options.EnableRectangularSelection) {
		selectionMode = wxSTC_SEL_RECTANGLE;
	}
	codeCtrl->SetSelectionMode(selectionMode);
	int whitespaceMode = wxSTC_WS_INVISIBLE;
	if (options.ShowWhitespace) {
		whitespaceMode = wxSTC_WS_VISIBLEALWAYS;
	}
	codeCtrl->SetViewWhiteSpace(whitespaceMode);
	codeCtrl->SetZoom(options.Zoom);
}

void t4p::EditorBehaviorFeatureClass::OnAppFileOpened(t4p::CodeControlEventClass& event) {
	SetFeatures(App.Preferences.CodeControlOptions, event.GetCodeControl());
}

BEGIN_EVENT_TABLE(t4p::EditorBehaviorFeatureClass, t4p::FeatureClass)
	EVT_APP_FILE_OPEN(t4p::EditorBehaviorFeatureClass::OnAppFileOpened)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::EditorBehaviorFeatureClass::OnPreferencesSaved)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED, t4p::EditorBehaviorFeatureClass::OnPreferencesSaved)
	EVT_MENU(t4p::MENU_BEHAVIOR + 0, t4p::EditorBehaviorFeatureClass::OnToggleRectangularSelection)
	EVT_MENU(t4p::MENU_BEHAVIOR + 1, t4p::EditorBehaviorFeatureClass::OnToggleWordWrap)
	EVT_MENU(t4p::MENU_BEHAVIOR + 2, t4p::EditorBehaviorFeatureClass::OnToggleIndentationGuides)
	EVT_MENU(t4p::MENU_BEHAVIOR + 3, t4p::EditorBehaviorFeatureClass::OnToggleWhitespace)
	EVT_MENU(t4p::MENU_BEHAVIOR + 4, t4p::EditorBehaviorFeatureClass::OnZoomIn)
	EVT_MENU(t4p::MENU_BEHAVIOR + 5, t4p::EditorBehaviorFeatureClass::OnZoomOut)
	EVT_MENU(t4p::MENU_BEHAVIOR + 6, t4p::EditorBehaviorFeatureClass::OnZoomReset)
END_EVENT_TABLE()


