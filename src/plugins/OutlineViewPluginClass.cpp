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
#include <plugins/OutlineViewPluginClass.h>
#include <language/SymbolTableClass.h>
#include <windows/StringHelperClass.h>
#include <MvcEditor.h>
#include <unicode/regex.h>
#include <wx/artprov.h>
#include <vector>
#include <algorithm>


static int ID_CONTEXT_MENU_SHOW_OUTLINE_OTHER = wxNewId();
static int ID_WINDOW_OUTLINE = wxNewId();
static int ID_CONTEXT_MENU_SHOW_OUTLINE_CURRENT = wxNewId();
static int ID_WINDOW_OUTLINE_CURRENT = wxNewId();

mvceditor::ResourceFinderBackgroundThreadClass::ResourceFinderBackgroundThreadClass(wxEvtHandler& handler)
	: ThreadWithHeartbeatClass(handler) 
	, Resources()
	, ResourceFinder() 
	, FileName() {

	// need this so that the resource finder parsers the file
	ResourceFinder.FileFilters.push_back(wxT("*.*"));
}

bool mvceditor::ResourceFinderBackgroundThreadClass::Start(const wxString& fileName) {
	FileName = fileName;
	wxThreadError error = CreateSingleInstance();
	bool created = wxTHREAD_NO_ERROR == error;
	if (created) {
		GetThread()->Run();
	}
	return created;
}

void* mvceditor::ResourceFinderBackgroundThreadClass::Entry() {
	if (wxFileName::FileExists(FileName)) {
		ResourceFinder.Clear();

		// need this call so that resources are actually parsed
		ResourceFinder.Prepare(wxT("fakeclass"));
		ResourceFinder.Walk(FileName);
		ResourceFinder.EnsureSorted();
		Resources = ResourceFinder.All();
		SignalEnd();
	}
	return 0;
}

mvceditor::OutlineViewPluginClass::OutlineViewPluginClass()
	: PluginClass()
	, ResourceFinderBackground(*this) {
	
}

void mvceditor::OutlineViewPluginClass::AddToolsMenuItems(wxMenu* toolsMenu) {
	toolsMenu->Append(mvceditor::MENU_OUTLINE, _("Outline Current File"),  _("Opens an outline view of the currently viewed file"), wxITEM_NORMAL);
}

void mvceditor::OutlineViewPluginClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_OUTLINE + 0] = wxT("Outline-Outline Current File");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::OutlineViewPluginClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {
	menu->Append(ID_CONTEXT_MENU_SHOW_OUTLINE_CURRENT, _("Outline Current File"),  _("Opens an outline view of the currently viewed file"), wxITEM_NORMAL);
	menu->Append(ID_CONTEXT_MENU_SHOW_OUTLINE_OTHER, _("Show In Outline"),  _("Search for the selected resource and opens an outline view"), wxITEM_NORMAL);
}

void mvceditor::OutlineViewPluginClass::BuildOutlineCurrentCodeControl() {
	CodeControlClass* code = GetCurrentCodeControl();
	if (code != NULL) {
		if (ResourceFinderBackground.Start(code->GetFileName())) {
			wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_OUTLINE, GetOutlineNotebook());
			if (window != NULL) {
				OutlineViewPluginPanelClass* outlineViewPanel = (OutlineViewPluginPanelClass*)window;
				SetFocusToOutlineWindow(outlineViewPanel);
				outlineViewPanel->SetStatus(_("Parsing ..."));
			}
		}
	}
}

void mvceditor::OutlineViewPluginClass::BuildOutline(const wxString& className) {
	ResourceFinderBackground.Resources.clear();
	
	mvceditor::ResourceCacheClass* resourceCache =  GetResourceCache();
	std::vector<mvceditor::ResourceClass>  matches; 

	// get the class resource itself
	if (resourceCache->PrepareAll(className)) {
		resourceCache->CollectFullyQualifiedResourceFromAll();
		matches = resourceCache->Matches();
		for (size_t i = 0; i < matches.size(); ++i) {
			ResourceFinderBackground.Resources.push_back(matches[i]);
		}
	}

	// make the resource finder match on all methods / properties
	wxString lookup;
	lookup += className;
	lookup += wxT("::");
	matches = GetResourceCache()->PrepareAndCollectNearMatchResourcesFromAll(lookup);
	for (size_t i = 0; i < matches.size(); ++i) {
		ResourceFinderBackground.Resources.push_back(matches[i]);
	}
}

void mvceditor::OutlineViewPluginClass::JumpToResource(const wxString& resource) {
	mvceditor::ResourceCacheClass* resourceCache = GetResourceCache();
	if (resourceCache->PrepareAll(resource)) {
		if (resourceCache->CollectFullyQualifiedResourceFromAll()) {
			std::vector<mvceditor::ResourceClass> matches = resourceCache->Matches();
			mvceditor::ResourceClass resource = matches[0];
			GetNotebook()->LoadPage(resource.GetFullPath());
			CodeControlClass* codeControl = GetCurrentCodeControl();
			if (codeControl) {
				int32_t position, 
					length;
				bool found = mvceditor::ResourceFinderClass::GetResourceMatchPosition(resource, codeControl->GetSafeText(), position, length);
				if (found) {
					codeControl->SetSelectionAndEnsureVisible(position, position + length);
				}
				// else the index is out of date....
			}
		}
	}	
}

void mvceditor::OutlineViewPluginClass::OnContextMenuOutline(wxCommandEvent& event) {
	CodeControlClass* code = GetCurrentCodeControl();
	bool modified = false;
	if (event.GetId() == ID_CONTEXT_MENU_SHOW_OUTLINE_CURRENT || event.GetId() == mvceditor::MENU_OUTLINE) {
		BuildOutlineCurrentCodeControl();
		modified = true;
	}
	else if (event.GetId() == ID_CONTEXT_MENU_SHOW_OUTLINE_OTHER && code && !code->GetSelectedText().IsEmpty()) { 
		BuildOutline(code->GetSelectedText()); 
		modified = true; 
	} 
	if (modified) {
		
		// create / open the outline window
		wxWindow* window = FindOutlineWindow(ID_WINDOW_OUTLINE);
		OutlineViewPluginPanelClass* outlineViewPanel = NULL;
		if (window != NULL) {
			outlineViewPanel = (OutlineViewPluginPanelClass*)window;
			SetFocusToOutlineWindow(outlineViewPanel);
			outlineViewPanel->RefreshOutlines();
		}
		else {
			mvceditor::NotebookClass* notebook = GetNotebook();
			if (notebook != NULL) {
				outlineViewPanel = new OutlineViewPluginPanelClass(GetOutlineNotebook(), ID_WINDOW_OUTLINE, this, notebook);
				if (AddOutlineWindow(outlineViewPanel, wxT("Outline"))) {
					outlineViewPanel->SetClasses(App->ResourceCache.AllNonNativeClassesGlobal());
					outlineViewPanel->RefreshOutlines();
				}
			}
		}	
	}
	else {
		event.Skip();
	}
}

void mvceditor::OutlineViewPluginClass::OnContentNotebookPageChanged(wxAuiNotebookEvent& event) {
	wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_OUTLINE, GetOutlineNotebook());

	// only change the outline if the user is looking at the outline.  otherwise, it gets 
	// annoying if the user is looking at run output, switches PHP files, and the outline
	// gets changed.
	if (window != NULL && IsOutlineWindowSelected(ID_WINDOW_OUTLINE)) {
		OutlineViewPluginPanelClass* outlineViewPanel = (OutlineViewPluginPanelClass*)window;
		SetFocusToOutlineWindow(outlineViewPanel);
		BuildOutlineCurrentCodeControl();
		outlineViewPanel->RefreshOutlines();
	}
	event.Skip();
}

void mvceditor::OutlineViewPluginClass::OnWorkComplete(wxCommandEvent &event) {
	wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_OUTLINE, GetOutlineNotebook());
	if (window != NULL) {
		OutlineViewPluginPanelClass* outlineViewPanel = (OutlineViewPluginPanelClass*)window;
		SetFocusToOutlineWindow(outlineViewPanel);
		outlineViewPanel->RefreshOutlines();
	}
}

mvceditor::OutlineViewPluginPanelClass::OutlineViewPluginPanelClass(wxWindow* parent, int windowId, OutlineViewPluginClass* plugin, 
		NotebookClass* notebook)
	: OutlineViewPluginGeneratedPanelClass(parent, windowId)
	, Plugin(plugin)
	, Notebook(notebook) {
	HelpButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_HELP, 
		wxART_TOOLBAR, wxSize(16, 16))));
	
}

void mvceditor::OutlineViewPluginPanelClass::SetStatus(const wxString& status) {
	StatusLabel->SetLabel(status);
}

void mvceditor::OutlineViewPluginPanelClass::SetClasses(std::vector<mvceditor::ResourceClass>& classes) {
	Choice->Clear();
	for (size_t i = 0; i < classes.size(); ++i) {
		Choice->AppendString(mvceditor::StringHelperClass::IcuToWx(classes[i].Resource));
	}
}

void mvceditor::OutlineViewPluginPanelClass::RefreshOutlines() {
	Tree->DeleteAllItems();
	wxTreeItemId rootId = Tree->AddRoot(_("Outline"));
	StatusLabel->SetLabel(_(""));
	std::vector<mvceditor::ResourceClass> resources = Plugin->ResourceFinderBackground.Resources;
	for (size_t i = 0; i < resources.size(); ++i) {

		// for now never show dynamic resources since there is no way we can know where the source for them is.
		int type = resources[i].Type;
		if (mvceditor::ResourceClass::DEFINE == type && !resources[i].IsDynamic) {
			UnicodeString res = resources[i].Resource;
			wxString label = mvceditor::StringHelperClass::IcuToWx(res);
			label = _("[D] ") + label;
			Tree->AppendItem(rootId, label);
		}
		else if (mvceditor::ResourceClass::CLASS == type && !resources[i].IsDynamic) {
			UnicodeString res = resources[i].Resource;
			wxString label = mvceditor::StringHelperClass::IcuToWx(res);
			label = _("[C] ") + label;
			wxTreeItemId classId = Tree->AppendItem(rootId, label);

			// for now just loop again through the resources
			// for the class we are going to add
			for (size_t j = 0; j < resources.size(); ++j) {
				if (resources[j].Resource.indexOf(resources[i].Resource) == 0  && !resources[j].IsDynamic) {
					UnicodeString res = resources[j].Identifier;
					wxString label = mvceditor::StringHelperClass::IcuToWx(res);
					if (mvceditor::ResourceClass::MEMBER == resources[j].Type) {
						label = _("[P] ") + label;
						if (!resources[j].ReturnType.isEmpty()) {
							wxString returnType = mvceditor::StringHelperClass::IcuToWx(resources[j].ReturnType);
							label = label + wxT(" [") + returnType + wxT("]");
						}
						Tree->AppendItem(classId, label);
					}
					else if (mvceditor::ResourceClass::METHOD == resources[j].Type) {
						label = _("[M] ") + label;

						// add the function signature to the label
						int32_t sigIndex = resources[j].Signature.indexOf(UNICODE_STRING_SIMPLE(" function ")); 
						if (sigIndex > 0) {
							UnicodeString sig(resources[j].Signature, sigIndex + 10);
							label = _("[M] ") + mvceditor::StringHelperClass::IcuToWx(sig);
						}
						if (!resources[j].ReturnType.isEmpty()) {
							wxString returnType = mvceditor::StringHelperClass::IcuToWx(resources[j].ReturnType);
							label += wxT(" [") + returnType + wxT("]");
						}
						Tree->AppendItem(classId, label);
					}
					else if (mvceditor::ResourceClass::CLASS_CONSTANT == resources[j].Type) {
						label = _("[O] ") + label;
						Tree->AppendItem(classId, label);
					}
				}
			}
		}
		else if (mvceditor::ResourceClass::FUNCTION == type && !resources[i].IsDynamic) {
			UnicodeString res = resources[i].Resource;
			wxString label = mvceditor::StringHelperClass::IcuToWx(res);
			label = _("[F] ") + label;

			// add the function signature to the label
			int32_t sigIndex = resources[i].Signature.indexOf(UNICODE_STRING_SIMPLE("function ")); 
			if (sigIndex >= 0) {
				UnicodeString sig(resources[i].Signature, sigIndex + 9);
				label = _("[F] ") + mvceditor::StringHelperClass::IcuToWx(sig);
			}
			if (!resources[i].ReturnType.isEmpty()) {
				wxString returnType = mvceditor::StringHelperClass::IcuToWx(resources[i].ReturnType);
				label += wxT(" [") + returnType + wxT("]");
			}
			Tree->AppendItem(rootId, label);
		}
	}
	Tree->ExpandAll();
}

void mvceditor::OutlineViewPluginPanelClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = wxString::FromAscii(
		"The outline tab allows you to 	quickly browse through your project's resources.\n"
		"1. The leftmost pane lists all of the resources of the file being viewed.\n"
		"   Changing the contents of the middle outline is done with the Lookup button "
		"   or the 'Show In Outline' context menu.\n"
		"The 'Sync Outline' button will 'reset' the outline view with the outline of the file that is currently being viewed."
		"\n"
		""
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Help"));
}

void mvceditor::OutlineViewPluginPanelClass::OnChoice(wxCommandEvent& event) {
	wxString lookup = event.GetString();
	if (!lookup.IsEmpty()) {
		Plugin->BuildOutline(lookup);
		RefreshOutlines();
	}
}

void mvceditor::OutlineViewPluginPanelClass::OnSyncButton(wxCommandEvent& event) {
	Plugin->BuildOutlineCurrentCodeControl();
}

void mvceditor::OutlineViewPluginPanelClass::OnTreeItemActivated(wxTreeEvent& event) {

	// the method name is the leaf node, the class name is the parent of the activated node
	wxTreeItemId item = event.GetItem();
	wxString methodSig = Tree->GetItemText(item);
	wxTreeItemId parentItem = Tree->GetItemParent(item);
	if (!item.IsOk() || !parentItem.IsOk()) {

		// dont want to handle a double-click on the root element
		event.Skip();
		return;
	}
	wxString classNameSig = Tree->GetItemText(parentItem);
	classNameSig = classNameSig.Mid(4); // 4 = length of '[C] '

	wxString resource = classNameSig + wxT("::");
	methodSig = methodSig.Mid(4); // 4= length of '[M] '
	
	// extract just the name from the label (function call args or property type)
	int index = methodSig.Index(wxT('('));
	if (wxNOT_FOUND == index) {
		index = methodSig.Index(wxT('['));
	}
	if (wxNOT_FOUND != index) {
		resource += methodSig.Mid(0, index);
	}
	else {

		// sig is not of a function, and prop does not have a type
		resource += methodSig;
	}
	if ( Tree->GetItemText(parentItem) != wxT("Outline") && !resource.IsEmpty()) {
		Plugin->JumpToResource(resource);
	}
	else {
		event.Skip();
	}
}

BEGIN_EVENT_TABLE(mvceditor::OutlineViewPluginClass, wxEvtHandler)
	EVT_MENU(mvceditor::MENU_OUTLINE, mvceditor::OutlineViewPluginClass::OnContextMenuOutline)
	EVT_MENU(ID_CONTEXT_MENU_SHOW_OUTLINE_CURRENT, mvceditor::OutlineViewPluginClass::OnContextMenuOutline)
	EVT_MENU(ID_CONTEXT_MENU_SHOW_OUTLINE_OTHER, mvceditor::OutlineViewPluginClass::OnContextMenuOutline)
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, mvceditor::OutlineViewPluginClass::OnContentNotebookPageChanged)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::OutlineViewPluginClass::OnWorkComplete)
END_EVENT_TABLE()