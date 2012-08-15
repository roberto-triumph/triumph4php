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
#include <MvcEditorString.h>
#include <MvcEditor.h>
#include <unicode/regex.h>
#include <wx/artprov.h>
#include <vector>
#include <algorithm>


static int ID_CONTEXT_MENU_SHOW_OUTLINE_OTHER = wxNewId();
static int ID_WINDOW_OUTLINE = wxNewId();
static int ID_CONTEXT_MENU_SHOW_OUTLINE_CURRENT = wxNewId();

const wxEventType mvceditor::EVENT_RESOURCE_FINDER_COMPLETE = wxNewEventType();

mvceditor::ResourceFinderCompleteEventClass::ResourceFinderCompleteEventClass(const std::vector<mvceditor::ResourceClass>& resources)
	: wxEvent(wxID_ANY, mvceditor::EVENT_RESOURCE_FINDER_COMPLETE)
	, Resources(resources) {
		
}

wxEvent* mvceditor::ResourceFinderCompleteEventClass::Clone() const {
	return new mvceditor::ResourceFinderCompleteEventClass(Resources);
}

mvceditor::ResourceFinderBackgroundThreadClass::ResourceFinderBackgroundThreadClass(wxEvtHandler& handler,
		mvceditor::RunningThreadsClass& runningThreads)
	: ThreadWithHeartbeatClass(handler, runningThreads) 
	, ResourceFinder() 
	, FileName() {
}

bool mvceditor::ResourceFinderBackgroundThreadClass::Start(const wxString& fileName, const mvceditor::EnvironmentClass& environment) {
	FileName = fileName;
	
	// need this so that the resource finder parsers the file
	ResourceFinder.InitMemory();
	ResourceFinder.FileFilters.push_back(wxT("*.*"));
	
	ResourceFinder.SetVersion(environment.Php.Version);
	wxThreadError error = CreateSingleInstance();
	bool created = wxTHREAD_NO_ERROR == error;
	return created;
}

void mvceditor::ResourceFinderBackgroundThreadClass::BackgroundWork() {
	if (wxFileName::FileExists(FileName)) {

		// need this call so that resources are actually parsed
		ResourceFinder.Walk(FileName);
		std::vector<mvceditor::ResourceClass> resources = ResourceFinder.All();
		mvceditor::ResourceFinderCompleteEventClass evt(resources);
		wxPostEvent(&Handler, evt);
	}
}

mvceditor::OutlineViewPluginClass::OutlineViewPluginClass(mvceditor::AppClass& app)
	: PluginClass(app) {
	
}

void mvceditor::OutlineViewPluginClass::AddViewMenuItems(wxMenu* viewMenu) {
	viewMenu->Append(mvceditor::MENU_OUTLINE, _("Outline Current File"),  _("Opens an outline view of the currently viewed file"), wxITEM_NORMAL);
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

		// this pointer will delete itself when the thread terminates
		mvceditor::ResourceFinderBackgroundThreadClass* thread = 
			new mvceditor::ResourceFinderBackgroundThreadClass(*this, App.RunningThreads);
		if (thread->Start(code->GetFileName(), *GetEnvironment())) {
			wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_OUTLINE, GetOutlineNotebook());
			if (window != NULL) {
				OutlineViewPluginPanelClass* outlineViewPanel = (OutlineViewPluginPanelClass*)window;
				SetFocusToOutlineWindow(outlineViewPanel);
				outlineViewPanel->SetStatus(_("Parsing ..."));
			}
		}
		else {
			delete thread;
		}
	}
}

std::vector<mvceditor::ResourceClass> mvceditor::OutlineViewPluginClass::BuildOutline(const wxString& className) {	
	mvceditor::ResourceCacheClass* resourceCache =  GetResourceCache();
	std::vector<mvceditor::ResourceClass> allMatches;
	std::vector<mvceditor::ResourceClass> matches; 

	// get the class resource itself
	matches = resourceCache->CollectFullyQualifiedResourceFromAll(mvceditor::WxToIcu(className));
	allMatches.insert(allMatches.end(), matches.begin(), matches.end());

	// make the resource finder match on all methods / properties
	UnicodeString lookup;
	lookup += mvceditor::WxToIcu(className);
	lookup += UNICODE_STRING_SIMPLE("::");
	matches = GetResourceCache()->CollectNearMatchResourcesFromAll(lookup);
	allMatches.insert(allMatches.end(), matches.begin(), matches.end());
	return allMatches;
}

void mvceditor::OutlineViewPluginClass::JumpToResource(const wxString& resource) {
	mvceditor::ResourceCacheClass* resourceCache = GetResourceCache();
	std::vector<mvceditor::ResourceClass> matches = resourceCache->CollectFullyQualifiedResourceFromAll(mvceditor::WxToIcu(resource));
	if (!matches.empty()) {
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

void mvceditor::OutlineViewPluginClass::OnContextMenuOutline(wxCommandEvent& event) {
	CodeControlClass* code = GetCurrentCodeControl();
	bool modified = false;
	std::vector<mvceditor::ResourceClass> matches;
	if (event.GetId() == ID_CONTEXT_MENU_SHOW_OUTLINE_CURRENT || event.GetId() == mvceditor::MENU_OUTLINE) {
		BuildOutlineCurrentCodeControl();
		modified = true;
	}
	else if (event.GetId() == ID_CONTEXT_MENU_SHOW_OUTLINE_OTHER && code && !code->GetSelectedText().IsEmpty()) { 
		matches = BuildOutline(code->GetSelectedText()); 
		modified = true; 
	} 
	if (modified) {
		
		// create / open the outline window
		wxWindow* window = FindOutlineWindow(ID_WINDOW_OUTLINE);
		OutlineViewPluginPanelClass* outlineViewPanel = NULL;
		if (window != NULL) {
			outlineViewPanel = (OutlineViewPluginPanelClass*)window;
			SetFocusToOutlineWindow(outlineViewPanel);
			outlineViewPanel->RefreshOutlines(matches);
		}
		else {
			mvceditor::NotebookClass* notebook = GetNotebook();
			if (notebook != NULL) {
				outlineViewPanel = new OutlineViewPluginPanelClass(GetOutlineNotebook(), ID_WINDOW_OUTLINE, this, notebook);
				if (AddOutlineWindow(outlineViewPanel, wxT("Outline"))) {
					outlineViewPanel->SetClasses(GetResourceCache()->AllNonNativeClassesGlobal());
					outlineViewPanel->RefreshOutlines(matches);
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
	}
	event.Skip();
}

void mvceditor::OutlineViewPluginClass::OnResourceFinderComplete(mvceditor::ResourceFinderCompleteEventClass& event) {
	wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_OUTLINE, GetOutlineNotebook());
	if (window != NULL) {
		OutlineViewPluginPanelClass* outlineViewPanel = (OutlineViewPluginPanelClass*)window;
		SetFocusToOutlineWindow(outlineViewPanel);
		outlineViewPanel->RefreshOutlines(event.Resources);
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

void mvceditor::OutlineViewPluginPanelClass::SetClasses(const std::vector<mvceditor::ResourceClass>& classes) {
	Choice->Clear();
	for (size_t i = 0; i < classes.size(); ++i) {
		Choice->AppendString(mvceditor::IcuToWx(classes[i].Identifier));
	}
}

void mvceditor::OutlineViewPluginPanelClass::RefreshOutlines(const std::vector<mvceditor::ResourceClass>& resources) {
	Tree->Freeze();
	Tree->DeleteAllItems();
	wxTreeItemId rootId = Tree->AddRoot(_("Outline"));
	StatusLabel->SetLabel(_(""));
	std::vector<mvceditor::ResourceClass>::const_iterator resource;
	for (resource = resources.begin(); resource != resources.end(); ++resource) {

		// for now never show dynamic resources since there is no way we can know where the source for them is.
		int type = resource->Type;
		if (mvceditor::ResourceClass::DEFINE == type && !resource->IsDynamic) {
			UnicodeString res = resource->Identifier;
			wxString label = mvceditor::IcuToWx(res);
			label = _("[D] ") + label;
			Tree->AppendItem(rootId, label);
		}
		else if (mvceditor::ResourceClass::CLASS == type && !resource->IsDynamic) {
			UnicodeString res = resource->Identifier;
			wxString label = mvceditor::IcuToWx(res);
			label = _("[C] ") + label;
			wxTreeItemId classId = Tree->AppendItem(rootId, label);

			// for now just loop again through the resources
			// for the class we are going to add
			std::vector<mvceditor::ResourceClass>::const_iterator j;
			for (j = resources.begin(); j != resources.end(); ++j) {
				if (j->ClassName.caseCompare(resource->Identifier, 0) == 0  && !j->IsDynamic) {
					UnicodeString res = j->Identifier;
					wxString label = mvceditor::IcuToWx(res);
					if (mvceditor::ResourceClass::MEMBER == j->Type) {
						label = _("[P] ") + label;
						if (!j->ReturnType.isEmpty()) {
							wxString returnType = mvceditor::IcuToWx(j->ReturnType);
							label = label + wxT(" [") + returnType + wxT("]");
						}
						Tree->AppendItem(classId, label);
					}
					else if (mvceditor::ResourceClass::METHOD == j->Type) {
						label = _("[M] ") + label;

						// add the function signature to the label
						int32_t sigIndex = j->Signature.indexOf(UNICODE_STRING_SIMPLE(" function ")); 
						if (sigIndex > 0) {
							UnicodeString sig(j->Signature, sigIndex + 10);
							label = _("[M] ") + mvceditor::IcuToWx(sig);
						}
						if (!j->ReturnType.isEmpty()) {
							wxString returnType = mvceditor::IcuToWx(j->ReturnType);
							label += wxT(" [") + returnType + wxT("]");
						}
						Tree->AppendItem(classId, label);
					}
					else if (mvceditor::ResourceClass::CLASS_CONSTANT == j->Type) {
						label = _("[O] ") + label;
						Tree->AppendItem(classId, label);
					}
				}
			}
		}
		else if (mvceditor::ResourceClass::FUNCTION == type && !resource->IsDynamic) {
			UnicodeString res = resource->Identifier;
			wxString label = mvceditor::IcuToWx(res);
			label = _("[F] ") + label;

			// add the function signature to the label
			int32_t sigIndex = resource->Signature.indexOf(UNICODE_STRING_SIMPLE("function ")); 
			if (sigIndex >= 0) {
				UnicodeString sig(resource->Signature, sigIndex + 9);
				label = _("[F] ") + mvceditor::IcuToWx(sig);
			}
			if (!resource->ReturnType.isEmpty()) {
				wxString returnType = mvceditor::IcuToWx(resource->ReturnType);
				label += wxT(" [") + returnType + wxT("]");
			}
			Tree->AppendItem(rootId, label);
		}
	}
	Tree->ExpandAll();
	Tree->Thaw();
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
		std::vector<mvceditor::ResourceClass> resources = Plugin->BuildOutline(lookup);
		RefreshOutlines(resources);
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
	if (!item.IsOk() || Tree->GetChildrenCount(item) > 0) {

		// dont want to handle a non-leaf (a class)
		event.Skip();
		return;
	}
	wxString resource;
	if (methodSig.StartsWith(wxT("[P]")) || methodSig.StartsWith(wxT("[M]"))) {
		wxString classNameSig = Tree->GetItemText(parentItem);
		classNameSig = classNameSig.Mid(4); // 4 = length of '[C] '

		resource = classNameSig + wxT("::");
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
	}
	else if (methodSig.StartsWith(wxT("[D]")) || methodSig.StartsWith(wxT("[F]"))) {
		methodSig = methodSig.Mid(4); // 4= length of '[D] '
		
		// extract just the name from the label (omit the return type)
		int index = methodSig.Index(wxT('('));
		if (wxNOT_FOUND == index) {
			index = methodSig.Index(wxT('['));
		}
		if (wxNOT_FOUND != index) {
			resource += methodSig.Mid(0, index);
		}
		else {
			resource += methodSig;
		}
	}
	if (!resource.IsEmpty()) {
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
	EVT_AUINOTEBOOK_PAGE_CHANGED(mvceditor::ID_CODE_NOTEBOOK, mvceditor::OutlineViewPluginClass::OnContentNotebookPageChanged)
	EVT_RESOURCE_FINDER_COMPLETE(mvceditor::OutlineViewPluginClass::OnResourceFinderComplete)
END_EVENT_TABLE()