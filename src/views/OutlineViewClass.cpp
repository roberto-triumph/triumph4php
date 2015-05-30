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
#include <features/OutlineFeatureClass.h>
#include <views/OutlineViewClass.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <globals/Errors.h>
#include <globals/TagList.h>
#include <code_control/CodeControlClass.h>
#include <language/TagParserClass.h>
#include <globals/Sqlite.h>
#include <globals/Number.h>
#include <widgets/TreeItemDataStringClass.h>
#include <widgets/Buttons.h>
#include <Triumph.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <unicode/regex.h>
#include <wx/artprov.h>
#include <wx/tokenzr.h>
#include <vector>
#include <algorithm>


static int ID_WINDOW_OUTLINE = wxNewId();
static int ID_OUTLINE_MENU_DELETE = wxNewId();
static int ID_OUTLINE_MENU_COLLAPSE = wxNewId();
static int ID_OUTLINE_MENU_COLLAPSE_ALL = wxNewId();
static int ID_OUTLINE_MENU_EXPAND_ALL = wxNewId();
static int ID_OUTLINE_MENU_TOGGLE_METHODS = wxNewId();
static int ID_OUTLINE_MENU_TOGGLE_PROPERTIES = wxNewId();
static int ID_OUTLINE_MENU_TOGGLE_CONSTANTS = wxNewId();
static int ID_OUTLINE_MENU_TOGGLE_PUBLIC_ONLY = wxNewId();
static int ID_OUTLINE_MENU_TOGGLE_FUNCTION_ARGS = wxNewId();
static int ID_OUTLINE_MENU_TOGGLE_INHERITED = wxNewId();
static int ID_OUTLINE_MENU_SORT_BY_NAME = wxNewId();
static int ID_OUTLINE_MENU_SORT_BY_TYPE = wxNewId();

static bool SortTagsByName(const t4p::TagClass& a, const t4p::TagClass& b) {
	return a.Identifier.caseCompare(b.Identifier, 0) < 0;
}

static bool SortTagsByTypeAndName(const t4p::TagClass& a, const t4p::TagClass& b) {
	if (a.Type < b.Type) {
		return true;
	}
	else if (a.Type > b.Type) {
		return false;
	}
	return a.Identifier.caseCompare(b.Identifier, 0) < 0;
}

namespace t4p {

/**
 * class to hold a tag ID for each item in the outline tree.
 */
class IdTreeItemDataClass  : public wxTreeItemData {

public:

	int Id;

	IdTreeItemDataClass(int id) 
		: wxTreeItemData()
		, Id(id) {
		
	}

};
}

t4p::OutlineViewClass::OutlineViewClass(t4p::OutlineFeatureClass& feature)
	: FeatureViewClass() 
	, Feature(feature) {
}

void t4p::OutlineViewClass::AddViewMenuItems(wxMenu* viewMenu) {
	viewMenu->Append(t4p::MENU_OUTLINE, _("Outline Current File\tCTRL+SHIFT+O"),  _("Opens an outline view of the currently viewed file"), wxITEM_NORMAL);
}

void t4p::OutlineViewClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[t4p::MENU_OUTLINE + 0] = wxT("Outline-Outline Current File");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void t4p::OutlineViewClass::JumpToResource(int tagId) {
	t4p::TagClass tag;
	bool found = Feature.App.Globals.TagCache.FindById(tagId, tag);
	if (found) {
		LoadCodeControl(tag.GetFullPath());
		CodeControlClass* codeControl = GetCurrentCodeControl();
		if (codeControl) {
			int32_t position, 
				length;
			bool found = t4p::ParsedTagFinderClass::GetResourceMatchPosition(tag, codeControl->GetSafeText(), position, length);
			if (found) {
				codeControl->SetSelectionAndEnsureVisible(position, position + length);
			}
			// else the index is out of date....
		}
	}
}

void t4p::OutlineViewClass::StartTagSearch(const std::vector<UnicodeString>& searchStrings) {
	t4p::OutlineTagCacheSearchActionClass* action = new t4p::OutlineTagCacheSearchActionClass(Feature.App.RunningThreads, wxID_ANY);
	action->SetSearch(searchStrings, Feature.App.Globals);
	Feature.App.RunningThreads.Queue(action);
}

void t4p::OutlineViewClass::OnOutlineMenu(wxCommandEvent& event) {
	
	// create / open the outline window
	wxWindow* window = FindOutlineWindow(ID_WINDOW_OUTLINE);
	OutlineViewPanelClass* outlineViewPanel = NULL;
	if (window != NULL) {
		outlineViewPanel = (OutlineViewPanelClass*)window;
		SetFocusToOutlineWindow(outlineViewPanel);

	}
	else {
		outlineViewPanel = new OutlineViewPanelClass(GetOutlineNotebook(), ID_WINDOW_OUTLINE, Feature, *this);
		wxBitmap outlineBitmap = t4p::BitmapImageAsset(wxT("outline"));
		AddOutlineWindow(outlineViewPanel, wxT("Outline"), outlineBitmap); 
	}

	// get all classes / functions for the active file
	t4p::CodeControlClass* codeCtrl = GetCurrentCodeControl();
	if (codeCtrl && !codeCtrl->GetFileName().IsEmpty()) {
		std::vector<UnicodeString> searchStrings;
		searchStrings.push_back(t4p::WxToIcu(codeCtrl->GetFileName()));
		StartTagSearch(searchStrings);
	}
}

void t4p::OutlineViewClass::OnAppFilePageChanged(t4p::CodeControlEventClass& event) {
	wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_OUTLINE, GetOutlineNotebook());

	// only change the outline if the user is looking at the outline.  otherwise, it gets 
	// annoying if the user is looking at run output, switches PHP files, and the outline
	// gets changed.
	if (window != NULL && IsOutlineWindowSelected(ID_WINDOW_OUTLINE)) {
		OutlineViewPanelClass* outlineViewPanel = (OutlineViewPanelClass*)window;
		SetFocusToOutlineWindow(outlineViewPanel);
		
		t4p::CodeControlClass* codeCtrl = event.GetCodeControl();
		if (codeCtrl && !codeCtrl->GetFileName().IsEmpty()) {
			std::vector<UnicodeString> searchStrings;
			searchStrings.push_back(t4p::WxToIcu(codeCtrl->GetFileName()));
			StartTagSearch(searchStrings);
		}
	}
	event.Skip();
}

void t4p::OutlineViewClass::OnAppFileClosed(t4p::CodeControlEventClass& event) {
	wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_OUTLINE, GetOutlineNotebook());
	if (window != NULL) {
		OutlineViewPanelClass* outlineViewPanel = (OutlineViewPanelClass*)window;
		CodeControlClass* codeCtrl = event.GetCodeControl();
		if (codeCtrl) {
			outlineViewPanel->RemoveFileFromOutline(codeCtrl->GetFileName());
		}
	}
	event.Skip();
}

void t4p::OutlineViewClass::OnAppFileOpened(t4p::CodeControlEventClass& event) {
	
	//if the outline window is open, update the file that was parsed
	wxWindow* window = FindOutlineWindow(ID_WINDOW_OUTLINE);
	if (window != NULL) {
		wxString fileName = event.GetCodeControl()->GetFileName();
		std::vector<UnicodeString> searchStrings;
		searchStrings.push_back(t4p::WxToIcu(fileName));
		StartTagSearch(searchStrings);
	}
}

void t4p::OutlineViewClass::OnTagSearchComplete(t4p::OutlineSearchCompleteEventClass& event) {

	//if the outline window is open, update the tree
	wxWindow* window = FindOutlineWindow(ID_WINDOW_OUTLINE);
	OutlineViewPanelClass* outlineViewPanel = NULL;
	if (window != NULL) {
		outlineViewPanel = (OutlineViewPanelClass*)window;
		outlineViewPanel->AddTagsToOutline(event.Tags);
	}
}

t4p::OutlineViewPanelClass::OutlineViewPanelClass(wxWindow* parent, int windowId, OutlineFeatureClass& feature, 
		OutlineViewClass& view)
	: OutlineViewGeneratedPanelClass(parent, windowId)
	, OutlinedTags()
	, ImageList(NULL)
	, Feature(feature)
	, View(view)
	, ShowMethods(true)
	, ShowProperties(true)
	, ShowConstants(true) 
	, ShowInherited(false)
	, ShowPublicOnly(false) 
	, ShowFunctionArgs(false)
	, SortByName(true)
	, SortByType(false) {
	SyncButton->SetBitmapLabel(t4p::BitmapImageButtonPrepAsset(wxT("outline-refresh")));
	AddButton->SetBitmapLabel(t4p::BitmapImageButtonPrepAsset(wxT("outline-add")));
	FilterButton->SetBitmapLabel(t4p::BitmapImageButtonPrepAsset(wxT("filter")));
	SortButton->SetBitmapLabel(t4p::BitmapImageButtonPrepAsset(wxT("sort")));
	HelpButtonIcon(HelpButton);

	SetStatus(_(""));
	
	ImageList = new wxImageList(16, 16);
	ImageList->Add(t4p::BitmapImageAsset(wxT("outline")));
	ImageList->Add(t4p::BitmapImageAsset(wxT("document-php")));
	ImageList->Add(t4p::BitmapImageAsset(wxT("class")));
	ImageList->Add(t4p::BitmapImageAsset(wxT("method-public")));
	ImageList->Add(t4p::BitmapImageAsset(wxT("method-protected")));
	ImageList->Add(t4p::BitmapImageAsset(wxT("method-private")));
	ImageList->Add(t4p::BitmapImageAsset(wxT("method-inherited")));
	ImageList->Add(t4p::BitmapImageAsset(wxT("property-public")));
	ImageList->Add(t4p::BitmapImageAsset(wxT("property-protected")));
	ImageList->Add(t4p::BitmapImageAsset(wxT("property-private")));
	ImageList->Add(t4p::BitmapImageAsset(wxT("property-inherited")));

	ImageList->Add(t4p::BitmapImageAsset(wxT("define")));
	ImageList->Add(t4p::BitmapImageAsset(wxT("class-constant")));
	ImageList->Add(t4p::BitmapImageAsset(wxT("namespace")));
	ImageList->Add(t4p::BitmapImageAsset(wxT("function")));
	ImageList->Add(t4p::BitmapImageAsset(wxT("variable-template")));

	// let the tree control managet the image list
	// since it may need to use it in the destructor
	Tree->AssignImageList(ImageList);
	
	Tree->SetIndent(10);
}

void t4p::OutlineViewPanelClass::SetStatus(const wxString& status) {
	StatusLabel->SetLabel(status);
}

void t4p::OutlineViewPanelClass::AddTagsToOutline(const std::vector<t4p::OutlineSearchCompleteClass>& tags) {
	size_t oldSize = OutlinedTags.size();
	OutlinedTags.insert(OutlinedTags.end(), tags.begin(), tags.end());
	std::vector<t4p::OutlineSearchCompleteClass>::iterator searchTag;
	Tree->Freeze();
	wxTreeItemId rootId = Tree->GetRootItem();
	if (!rootId.IsOk()) {
		rootId = Tree->AddRoot(_("Outline"), IMAGE_OUTLINE_ROOT);
	}
	StatusLabel->SetLabel(_(""));
	for (searchTag = OutlinedTags.begin() + oldSize; searchTag != OutlinedTags.end(); ++searchTag) {
		wxString label = searchTag->Label;

		// look for the file in the tree
		// files are in the first level
		wxTreeItemId fileId = FindFileNode(label);

		// when adding, add the new item to the top, that way the newly added item is
		// fully visible
		// when search is a filename, the label is a full path to a file
		if (fileId.IsOk() && searchTag->IsLabelFileName()) {
			Tree->Delete(fileId);
			fileId = Tree->PrependItem(rootId, wxFileName(label).GetFullName(), IMAGE_OUTLINE_FILE, -1, new t4p::TreeItemDataStringClass(label)); 
		}
		else if (searchTag->IsLabelFileName()) {
			fileId = Tree->PrependItem(rootId, wxFileName(label).GetFullName(), IMAGE_OUTLINE_FILE, -1, new t4p::TreeItemDataStringClass(label)); 
		}
		else if (fileId.IsOk() && !searchTag->IsLabelFileName()) {
			Tree->Delete(fileId);
			fileId = Tree->PrependItem(rootId, label, IMAGE_OUTLINE_CLASS, -1, 0); 
		}
		else {
			fileId = Tree->PrependItem(rootId, label, IMAGE_OUTLINE_CLASS, -1, 0); 
		}

		std::map<wxString, std::vector<t4p::TagClass> >::iterator mapTag; 		
		std::vector<t4p::TagClass>::const_iterator memberTag;
		for (mapTag = searchTag->Tags.begin(); mapTag != searchTag->Tags.end(); ++mapTag) {
			wxTreeItemId parent;
			if (mapTag->first.IsEmpty()) {
				parent = fileId;
			}
			else {
				parent = Tree->AppendItem(fileId, mapTag->first, IMAGE_OUTLINE_CLASS); 
			}
			if (SortByName) {
				std::sort(mapTag->second.begin(), mapTag->second.end(),  SortTagsByName);
			}
			else if (SortByType) {
				std::sort(mapTag->second.begin(), mapTag->second.end(),  SortTagsByTypeAndName);
			}
			
			// display all tags for this class or the class's base classes
			for (memberTag = mapTag->second.begin(); memberTag !=  mapTag->second.end(); ++memberTag) {					
				TagToNode(*memberTag, parent, t4p::WxToIcu(mapTag->first));
			}
		}
		Tree->SelectItem(fileId);
	}
	Tree->ExpandAll();
	Tree->Thaw();
}

void t4p::OutlineViewPanelClass::TagToNode(const t4p::TagClass& tag, wxTreeItemId& treeId, UnicodeString classNameNode) {

	// for now never show dynamic resources since there is no way we can know where the source for them is.
	int type = tag.Type;
	UnicodeString className = t4p::WxToIcu(Tree->GetItemText(treeId));
	wxString label = t4p::IcuToWx(tag.Identifier);

	// if the flag to show only public members is on, then do not show private or protected tags
	bool passesAccessCheck = !ShowPublicOnly || (!tag.IsPrivate && !tag.IsProtected);

	// compare clas nodes in the same way that they come from the OutlineTagCacheSearchActionClass
	UnicodeString tagClassName = tag.ClassName;
	if (!tag.NamespaceName.isEmpty()) {
		tagClassName += UNICODE_STRING_SIMPLE(": ") + tag.NamespaceName;
	}
	bool isInheritedTag = tagClassName.caseCompare(classNameNode, 0) != 0;
	if (!classNameNode.isEmpty() && !ShowInherited) {
		
		// make sure that the inheritance check passes too
		passesAccessCheck = passesAccessCheck && !isInheritedTag;
	}
	if (t4p::TagClass::DEFINE == type && !tag.IsDynamic) {
		Tree->AppendItem(treeId, label, IMAGE_OUTLINE_DEFINE, -1, new t4p::IdTreeItemDataClass(tag.Id));
	}
	else if (t4p::TagClass::MEMBER == tag.Type && ShowProperties && passesAccessCheck) {
		label = t4p::IcuToWx(tag.Identifier);
		if (isInheritedTag) {
			label = t4p::IcuToWx(tag.ClassName) + wxT("::") + label;
		}
		if (!tag.ReturnType.isEmpty()) {
			wxString returnType = t4p::IcuToWx(tag.ReturnType);
			label = label + wxT(" [") + returnType + wxT("]");
		}
		int image = IMAGE_OUTLINE_PROPERTY_PUBLIC;
		if (isInheritedTag) {
			image = IMAGE_OUTLINE_PROPERTY_INHERITED;
		}
		else if (tag.IsProtected) {
			image = IMAGE_OUTLINE_PROPERTY_PROTECTED;
		}
		else if (tag.IsPrivate) {
			image = IMAGE_OUTLINE_PROPERTY_PRIVATE;
		}
		Tree->AppendItem(treeId, label, image, -1, new t4p::IdTreeItemDataClass(tag.Id));
	}
	else if (t4p::TagClass::METHOD == tag.Type && ShowMethods && passesAccessCheck) {
		label = t4p::IcuToWx(tag.Identifier);
		if (isInheritedTag) {
			label = t4p::IcuToWx(tag.ClassName) + wxT("::") + label;
		}

		// check to see if we have args. if so, add an ellipsis to show that there are things hidden
		// that can be seen
		if (tag.Signature.indexOf(UNICODE_STRING_SIMPLE("()")) > 0) {
			label += wxT("()");
		}
		else {
			label += wxT("(...)");
		}
		if (!tag.ReturnType.isEmpty()) {
			wxString returnType = t4p::IcuToWx(tag.ReturnType);
			label += wxT(" [") + returnType + wxT("]");
		}
		int image = IMAGE_OUTLINE_METHOD_PUBLIC;
		if (isInheritedTag) {
			image = IMAGE_OUTLINE_METHOD_INHERITED;
		}
		else if (tag.IsProtected) {
			image = IMAGE_OUTLINE_METHOD_PROTECTED;
		}
		else if (tag.IsPrivate) {
			image = IMAGE_OUTLINE_METHOD_PRIVATE;
		}
		wxTreeItemId funcId = Tree->AppendItem(treeId, label, image, -1, new t4p::IdTreeItemDataClass(tag.Id));
		if (ShowFunctionArgs) {

			// add the function args under the method name
			int32_t argsStart = tag.Signature.indexOf(UNICODE_STRING_SIMPLE("(")); 
			int32_t argsEnd = tag.Signature.indexOf(UNICODE_STRING_SIMPLE(")")); 
			if (argsStart > 0 && argsEnd > 0) {
				UnicodeString sig(tag.Signature, argsStart + 1, argsEnd - argsStart - 1);
				
				wxStringTokenizer tok(t4p::IcuToWx(sig), wxT(","));
				while (tok.HasMoreTokens()) {
					Tree->AppendItem(funcId, tok.NextToken(), IMAGE_OUTLINE_ARGUMENT, -1, new t4p::IdTreeItemDataClass(tag.Id));
				}
			}
		}
	}
	else if (t4p::TagClass::CLASS_CONSTANT == tag.Type && ShowConstants && passesAccessCheck) {
		if (tag.ClassName != className) {
			label = t4p::IcuToWx(tag.ClassName) + wxT("::") + label;
		}
		Tree->AppendItem(treeId, label, IMAGE_OUTLINE_CLASS_CONSTANT, -1, new t4p::IdTreeItemDataClass(tag.Id));
	}
	else if (t4p::TagClass::FUNCTION == type && !tag.IsDynamic) {
		UnicodeString res = tag.Identifier;
		wxString label = t4p::IcuToWx(res);

		// add the function signature to the label
		int32_t sigIndex = tag.Signature.indexOf(UNICODE_STRING_SIMPLE("function ")); 
		if (sigIndex >= 0) {
			UnicodeString sig(tag.Signature, sigIndex + 9);
			label = t4p::IcuToWx(sig);
		}
		if (!tag.ReturnType.isEmpty()) {
			wxString returnType = t4p::IcuToWx(tag.ReturnType);
			label += wxT(" [") + returnType + wxT("]");
		}
		Tree->AppendItem(treeId, label, IMAGE_OUTLINE_FUNCTION, -1, new t4p::IdTreeItemDataClass(tag.Id));
	}
}

void t4p::OutlineViewPanelClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = wxString::FromAscii(
		"The outline tab allows you to see a skeleton of a file or members of a class.\n\n"
		"1. The tree pane lists all of the files that are currently being edited.\n"
		"2. Items in the tree are added / removed as you open or close files.\n"
		"3. At any point, you can add any arbitrary class or file from any\n"
		"   of your projects by clicking on the Add button\n"
		"4. You can remove an existing item from the tree by right-clicking\n"
		"   then choosing Delete\n"
		"5. The 'Sync With Editor' button will 'reset' the outline view with the\n"
		"   outline all of the files that are currently being edited.\n"
		"\n"
		"The outline view shows functions, defines, classes, methods, members,\n"
		"class constants. When a class is shown, all of its inherited members,\n"
		"and inherited traits are shown as well."
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Outline Help"), wxOK, this);
}

void t4p::OutlineViewPanelClass::OnAddButton(wxCommandEvent& event) {
	std::vector<t4p::TagClass> tags;
	t4p::FileSearchDialogClass dialog(this->GetParent(), Feature, tags);
	if (dialog.ShowModal() == wxOK) {
		SearchTagsToOutline(tags);
	}
}

void t4p::OutlineViewPanelClass::OnSyncButton(wxCommandEvent& event) {
	OutlinedTags.clear();
	Tree->DeleteAllItems();
	std::vector<UnicodeString> searchStrings;
	std::vector<t4p::CodeControlClass*> codeCtrls = View.AllCodeControls();
	for (size_t i = 0; i < codeCtrls.size(); i++) {
		t4p::CodeControlClass* codeCtrl = codeCtrls[i];
		if (codeCtrl && !codeCtrl->GetFileName().IsEmpty()) {
			searchStrings.push_back(t4p::WxToIcu(codeCtrl->GetFileName()));
		}
	}
	View.StartTagSearch(searchStrings);
}

void t4p::OutlineViewPanelClass::OnTreeItemActivated(wxTreeEvent& event) {

	// the method name is the leaf node, the class name is the parent of the activated node
	wxTreeItemId item = event.GetItem();
	wxString methodSig = Tree->GetItemText(item);
	if (!item.IsOk() || Tree->GetChildrenCount(item) > 0) {

		// dont want to handle a non-leaf (a class)
		event.Skip();
		return;
	}
	t4p::IdTreeItemDataClass* idItemData = (t4p::IdTreeItemDataClass*)Tree->GetItemData(item);
	if (!idItemData) {
		event.Skip();
		return;
	}
	View.JumpToResource(idItemData->Id);
}

void t4p::OutlineViewPanelClass::SearchTagsToOutline(const std::vector<t4p::TagClass>& tags) {

	// each tag could be a file or a class tag. 
	//if its a class tag, get all of members for the class
	std::vector<t4p::TagClass>::const_iterator chosenTag;
	std::vector<UnicodeString> searchStrings;
		
	for (chosenTag = tags.begin(); chosenTag != tags.end(); ++chosenTag) {
		if (chosenTag->Identifier.indexOf(UNICODE_STRING_SIMPLE(".")) >= 0) {
			
			// user chose a file: get all classes / functions for that file
			searchStrings.push_back(t4p::WxToIcu(chosenTag->FullPath));		
		}
		else {

			// user chose a class; add the class member to the outline
			searchStrings.push_back(chosenTag->FullyQualifiedClassName());
		}
	}
	View.StartTagSearch(searchStrings);
}

void t4p::OutlineViewPanelClass::OnTreeItemRightClick(wxTreeEvent& event) {
	
	// show the delete menu only on the first level items
	wxTreeItemId itemId = event.GetItem();
	wxTreeItemId rootId = Tree->GetRootItem();
	
	// set the node that was clicked on, that way the context
	// menu handlers know to work on the item that was clicked , which may
	// not be the tree selected item 
	Tree->SelectItem(itemId);
	wxMenu menu;
	if (itemId.IsOk() && rootId == Tree->GetItemParent(itemId)) {		
		menu.Append(ID_OUTLINE_MENU_DELETE, _("Delete"), _("Delete the item from the tree"));
	}
	if (itemId.IsOk() && Tree->HasChildren(itemId)) {
		menu.Append(ID_OUTLINE_MENU_COLLAPSE, _("Collapse"), _("Collapse this item"));
	}
	menu.Append(ID_OUTLINE_MENU_COLLAPSE_ALL, _("Collapse All"), _("Collapse all items in the tree"));
	menu.Append(ID_OUTLINE_MENU_EXPAND_ALL, _("Expand All"), _("Expand all items in the tree"));
	wxMenuItem* item = menu.AppendCheckItem(ID_OUTLINE_MENU_TOGGLE_METHODS, _("Show Methods"), _("Toggle showing methods in the outline"));
	item->Check(ShowMethods);

	item = menu.AppendCheckItem(ID_OUTLINE_MENU_TOGGLE_PROPERTIES, _("Show Properties"), _("Toggle showing properties in the outline"));
	item->Check(ShowProperties);
	
	item = menu.AppendCheckItem(ID_OUTLINE_MENU_TOGGLE_CONSTANTS, _("Show Class Constants"), _("Toggle showing class constants in the outline"));
	item->Check(ShowConstants);

	item = menu.AppendCheckItem(ID_OUTLINE_MENU_TOGGLE_INHERITED, _("Show Inherited Members"), _("Toggle showing inherited tags in the outline"));
	item->Check(ShowInherited);

	item = menu.AppendCheckItem(ID_OUTLINE_MENU_TOGGLE_PUBLIC_ONLY, _("Show Public Tags Only"), _("Toggle showing public tags in the outline"));
	item->Check(ShowPublicOnly);

	item = menu.AppendCheckItem(ID_OUTLINE_MENU_TOGGLE_FUNCTION_ARGS, _("Show Function Arguments"), _("Toggle showing functionn arguments in the outline"));
	item->Check(ShowFunctionArgs);

	menu.AppendSeparator();
	item = menu.AppendCheckItem(ID_OUTLINE_MENU_SORT_BY_NAME, _("Sort By Name"), _("Sort tags by name"));
	item->Check(SortByName);

	item = menu.AppendCheckItem(ID_OUTLINE_MENU_SORT_BY_TYPE, _("Sort By Type"), _("Sort tags by type"));
	item->Check(SortByType);

	wxPoint pos = event.GetPoint();
	Tree->PopupMenu(&menu, pos);
	event.Skip();
}


void t4p::OutlineViewPanelClass::OnFilterLeftDown(wxMouseEvent& event) {
	wxPoint pos = event.GetPosition();
	wxHitTest test = FilterButton->HitTest(pos);
	if (test== wxHT_WINDOW_INSIDE) {
		wxMenu menu;
		menu.Append(ID_OUTLINE_MENU_COLLAPSE_ALL, _("Collapse All"), _("Collapse all items in the tree"));
		menu.Append(ID_OUTLINE_MENU_EXPAND_ALL, _("Expand All"), _("Expand all items in the tree"));
		wxMenuItem* item = menu.AppendCheckItem(ID_OUTLINE_MENU_TOGGLE_METHODS, _("Show Methods"), _("Toggle showing methods in the outline"));
		item->Check(ShowMethods);

		item = menu.AppendCheckItem(ID_OUTLINE_MENU_TOGGLE_PROPERTIES, _("Show Properties"), _("Toggle showing properties in the outline"));
		item->Check(ShowProperties);
		
		item = menu.AppendCheckItem(ID_OUTLINE_MENU_TOGGLE_CONSTANTS, _("Show Class Constants"), _("Toggle showing class constants in the outline"));
		item->Check(ShowConstants);

		item = menu.AppendCheckItem(ID_OUTLINE_MENU_TOGGLE_INHERITED, _("Show Inherited Members"), _("Toggle showing inherited tags in the outline"));
		item->Check(ShowInherited);

		item = menu.AppendCheckItem(ID_OUTLINE_MENU_TOGGLE_PUBLIC_ONLY, _("Show Public Tags Only"), _("Toggle showing public tags in the outline"));
		item->Check(ShowPublicOnly);

		item = menu.AppendCheckItem(ID_OUTLINE_MENU_TOGGLE_FUNCTION_ARGS, _("Show Function Arguments"), _("Toggle showing functionn arguments in the outline"));
		item->Check(ShowFunctionArgs);

		FilterButton->PopupMenu(&menu, pos);
	}
	
	// according to docs, always allow default processing of mouse down events to take place
	//
	// The handler of this event should normally call event.Skip() to allow the default processing to take 
	// place as otherwise the window under mouse wouldn't get the focus.
	event.Skip();
}

void t4p::OutlineViewPanelClass::OnSortLeftDown(wxMouseEvent& event) {
	wxPoint pos = event.GetPosition();
	wxHitTest test = SortButton->HitTest(pos);
	if (test== wxHT_WINDOW_INSIDE) {
		wxMenu menu;
		wxMenuItem* item;

		item = menu.AppendCheckItem(ID_OUTLINE_MENU_SORT_BY_NAME, _("Sort By Name"), _("Sort tags by name"));
		item->Check(SortByName);

		item = menu.AppendCheckItem(ID_OUTLINE_MENU_SORT_BY_TYPE, _("Sort By Type"), _("Sort tags by type"));
		item->Check(SortByType);

		SortButton->PopupMenu(&menu, pos);
	}

	// according to docs, always allow default processing of mouse down events to take place
	//
	// The handler of this event should normally call event.Skip() to allow the default processing to take 
	// place as otherwise the window under mouse wouldn't get the focus.
	event.Skip();
}

void t4p::OutlineViewPanelClass::OnTreeMenuDelete(wxCommandEvent& event) {
	
	// only allow deletion on the first level items
	wxTreeItemId rootId = Tree->GetRootItem();
	wxTreeItemId itemId = Tree->GetSelection();
	if (itemId.IsOk() && rootId == Tree->GetItemParent(itemId)) {
		t4p::TreeItemDataStringClass* data = (t4p::TreeItemDataStringClass*)Tree->GetItemData(itemId);
		wxString label = Tree->GetItemText(itemId);
		if (data) {
			wxString fullPath = data->Str;

			// remove the file from the outlined tags list
			std::vector<t4p::OutlineSearchCompleteClass>::iterator it = OutlinedTags.begin();
			while(it != OutlinedTags.end()) {
				if (it->Label == label)  {
					it = OutlinedTags.erase(it);
				}
				else {
					it++;
				}
			}
			Tree->Delete(itemId);
		}
	}
}

void t4p::OutlineViewPanelClass::OnTreeMenuCollapse(wxCommandEvent& event) {
	wxTreeItemId itemId = Tree->GetSelection();
	if (itemId.IsOk()) {
		Tree->Collapse(itemId);
	}
}

void t4p::OutlineViewPanelClass::OnTreeMenuCollapseAll(wxCommandEvent& event) {
	wxTreeItemId rootId = Tree->GetRootItem();
	if (rootId.IsOk()) {
		Tree->CollapseAllChildren(rootId);
		Tree->Expand(rootId);
	}
}

void t4p::OutlineViewPanelClass::OnTreeMenuExpandAll(wxCommandEvent& event) {
	wxTreeItemId rootId = Tree->GetRootItem();
	if (rootId.IsOk()) {
		Tree->ExpandAllChildren(rootId);
	}
}

void t4p::OutlineViewPanelClass::RedrawOutline() {
	std::vector<t4p::OutlineSearchCompleteClass> tags = OutlinedTags;
	OutlinedTags.clear(); // AddFileToOutline will add the given tags to this list
	AddTagsToOutline(tags);		
}

void t4p::OutlineViewPanelClass::OnMethodsClick(wxCommandEvent& event) {
	ShowMethods = !ShowMethods;
	RedrawOutline();
}

void t4p::OutlineViewPanelClass::OnPropertiesClick(wxCommandEvent& event) {
	ShowProperties = !ShowProperties;
	RedrawOutline();
}

void t4p::OutlineViewPanelClass::OnConstantsClick(wxCommandEvent& event) {
	ShowConstants = !ShowConstants;
	RedrawOutline();
}

void t4p::OutlineViewPanelClass::OnInheritedClick(wxCommandEvent& event) {
	ShowInherited = !ShowInherited;
	RedrawOutline();
}

void t4p::OutlineViewPanelClass::OnPublicOnlyClick(wxCommandEvent& event) {
	ShowPublicOnly = !ShowPublicOnly;
	RedrawOutline();
}

void t4p::OutlineViewPanelClass::OnFunctionArgsClick(wxCommandEvent& event) {
	ShowFunctionArgs = !ShowFunctionArgs;
	RedrawOutline();
}

void t4p::OutlineViewPanelClass::OnSortByTypeClick(wxCommandEvent& event) {
	SortByType = !SortByType;
	if (SortByType) {
		SortByName = false;
	}
	RedrawOutline();
}

void t4p::OutlineViewPanelClass::OnSortByNameClick(wxCommandEvent& event) {
	SortByName = !SortByName;
	if (SortByName) {
		SortByType = false;
	}
	RedrawOutline();
}

t4p::FileSearchDialogClass::FileSearchDialogClass(wxWindow *parent, t4p::OutlineFeatureClass& feature, std::vector<t4p::TagClass>& chosenTags)
	: FileSearchDialogGeneratedClass(parent)
	, Feature(feature)
	, MatchingTags()
	, ChosenTags(chosenTags) {
	SearchText->SetFocus();
	Init();
}

void t4p::FileSearchDialogClass::Init() {
	ProjectChoice->Append(_("All Enabled Projects"), (void*)NULL);
	for (size_t i = 0; i < Feature.App.Globals.Projects.size(); ++i) {
		if (Feature.App.Globals.Projects[i].IsEnabled) {

			// should be ok to reference this vector since it wont change because this is a 
			// modal dialog
			ProjectChoice->Append(Feature.App.Globals.Projects[i].Label, &Feature.App.Globals.Projects[i]);
		}
	}
	ProjectChoice->Select(0);
}

void t4p::FileSearchDialogClass::Search() {
	wxString search = SearchText->GetValue();
	if (search.Length() < 2) {
		return;
	}
	std::vector<t4p::ProjectClass*> projects;
	bool showAllProjects = ProjectChoice->GetSelection() == 0;
	if (!showAllProjects) {
		projects.push_back((t4p::ProjectClass*)ProjectChoice->GetClientData(ProjectChoice->GetSelection()));
	}
	else {

		// the first item in the wxChoice will not have client data; the "all" option
		for (size_t i = 1; i < ProjectChoice->GetCount(); ++i) {
			projects.push_back((t4p::ProjectClass*) ProjectChoice->GetClientData(i));
		}
	}
	if (search.Length() == 2) {
		MatchingTags = Feature.App.Globals.TagCache.ExactClassOrFile(t4p::WxToIcu(search));
	}
	else {
		MatchingTags = Feature.App.Globals.TagCache.NearMatchClassesOrFiles(t4p::WxToIcu(search));
	}

	// no need to show jump to results for native functions
	t4p::TagListRemoveNativeMatches(MatchingTags);
	t4p::TagListKeepMatchesFromProjects(MatchingTags, projects);
	ShowTags(search, MatchingTags);
}

void t4p::FileSearchDialogClass::OnSearchText(wxCommandEvent& event) {
	Search();
}

void t4p::FileSearchDialogClass::OnProjectChoice(wxCommandEvent& event) {
	Search();
}

void t4p::FileSearchDialogClass::OnSearchKeyDown(wxKeyEvent& event) {
	int keyCode = event.GetKeyCode();
	int selection = MatchesList->GetSelection();
	if (keyCode == WXK_DOWN) {		
		if (!MatchesList->IsEmpty() && t4p::NumberLessThan(selection, (MatchesList->GetCount() - 1))) {
			MatchesList->SetSelection(selection + 1);
		}
		else if (!MatchesList->IsEmpty()) {

			// cycle back to the beginning
			MatchesList->SetSelection(0);
		}
		SearchText->SetFocus();
	}
	else if (keyCode == WXK_UP) {
		if (!MatchesList->IsEmpty() && selection > 0 && t4p::NumberLessThan(selection, MatchesList->GetCount())) {
			MatchesList->SetSelection(selection - 1);
		}
		else if (!MatchesList->IsEmpty() && selection == 0) {

			// cycle back to the end
			MatchesList->SetSelection(MatchesList->GetCount() - 1);
		}
		SearchText->SetFocus();
	}
	else {
		event.Skip();
	}
}

void t4p::FileSearchDialogClass::OnMatchesListDoubleClick(wxCommandEvent& event) {
	TransferDataFromWindow();
	ChosenTags.clear();
	int selection = event.GetSelection();
	if (t4p::NumberLessThan(selection, MatchesList->GetCount())) {
		ChosenTags.push_back(MatchingTags[selection]);
	}
	if (ChosenTags.empty()) {
		return;
	}
	EndModal(wxOK);
}

void t4p::FileSearchDialogClass::OnMatchesListKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_RETURN) {
		wxCommandEvent cmdEvt;
		OnSearchEnter(cmdEvt);
	}
	else {
		event.Skip();
	}
}

void t4p::OutlineViewPanelClass::RemoveFileFromOutline(const wxString& fullPath) {
	wxTreeItemId fileId = FindFileNode(fullPath);
	if (fileId.IsOk()) {
		wxString label = Tree->GetItemText(fileId);
		Tree->Delete(fileId);

		// remove the file from the outlined tags list
		std::vector<t4p::OutlineSearchCompleteClass>::iterator it = OutlinedTags.begin();
		
		while(it != OutlinedTags.end()) {
			if (it->Label == label)  {
				it = OutlinedTags.erase(it);
			}
			else {
				it++;
			}
		}
	}
}

wxTreeItemId t4p::OutlineViewPanelClass::FindFileNode(const wxString& fullPath) {
	wxTreeItemId treeIndex, fileId;
	wxTreeItemId rootId = Tree->GetRootItem();
	if (rootId.IsOk()) {
		wxTreeItemIdValue treeCookie;
		treeIndex = Tree->GetFirstChild(rootId, treeCookie);
		while (treeIndex.IsOk()) {
			t4p::TreeItemDataStringClass* data = (t4p::TreeItemDataStringClass*)Tree->GetItemData(treeIndex);
			if (data && data->Str == fullPath) {
				fileId = treeIndex;
				break;
			}
			treeIndex = Tree->GetNextChild(rootId, treeCookie);
		}
	}
	return fileId;
}

void t4p::FileSearchDialogClass::ShowTags(const wxString& finderQuery, const std::vector<t4p::TagClass>& allMatches) {
	wxArrayString files;
	for (size_t i = 0; i < allMatches.size(); ++i) {
		files.Add(allMatches[i].GetFullPath());
	}
	MatchesList->Clear();
	bool showAllProjects = ProjectChoice->GetSelection() == 0;
	t4p::ProjectClass* selectedProject = NULL;
	if (!showAllProjects) {
		selectedProject = (t4p::ProjectClass*)ProjectChoice->GetClientData(ProjectChoice->GetSelection());
	}
	
	// dont show the project path to the user
	for (size_t i = 0; i < files.GetCount(); ++i) {
		wxString projectLabel;
		wxString relativeName;
		if (showAllProjects) {
			relativeName = Feature.App.Globals.RelativeFileName(files[i], projectLabel);
		}
		else {
			relativeName = selectedProject->RelativeFileName(files[i]);
			projectLabel = selectedProject->Label;
		}
		wxString matchLabel;
		t4p::TagClass match = allMatches[i];
		if (t4p::TagClass::MEMBER == match.Type || t4p::TagClass::METHOD == match.Type ||
			t4p::TagClass::CLASS_CONSTANT == match.Type) {
			matchLabel += t4p::IcuToWx(match.ClassName);
			matchLabel += wxT("::");
			matchLabel += t4p::IcuToWx(match.Identifier);
		}
		else if (t4p::TagClass::CLASS == match.Type || t4p::TagClass::FUNCTION == match.Type
			|| t4p::TagClass::DEFINE == match.Type) {
			matchLabel += t4p::IcuToWx(match.Identifier);
		}
		else {
			matchLabel += t4p::IcuToWx(match.Identifier);
		}
		matchLabel += wxT(" - ");
		matchLabel += relativeName;
		matchLabel +=  wxT("  (") + projectLabel + wxT(")");
		MatchesList->Append(matchLabel);
	}
	if (!MatchesList->IsEmpty()) {
		MatchesList->Select(0);
	}
	MatchesLabel->SetLabel(wxString::Format(_("Found %ld files. Please choose file(s) to open."), allMatches.size()));
}

void t4p::FileSearchDialogClass::OnSearchEnter(wxCommandEvent& event) {
	if (MatchingTags.size() == 1) {

		// if there is only match, just take the user to it
		TransferDataFromWindow();
		ChosenTags.clear();
		ChosenTags.push_back(MatchingTags[0]);
		EndModal(wxOK);
	}
	else {
		wxArrayInt checks;
		for (size_t i = 0; i < MatchesList->GetCount(); ++i) {
			if (MatchesList->IsChecked(i)) {
				checks.Add(i);
			}
		}
		if (checks.Count() > 1) {
		
			// open the checked items
			for (size_t i = 0; i < checks.Count(); ++i) {
				size_t matchIndex = checks.Item(i);
				if (matchIndex < MatchingTags.size()) {
					ChosenTags.push_back(MatchingTags[matchIndex]);
				}
			}
			EndModal(wxOK);
		}
		else {
			// no checked items, take the user to the
			// selected item
			int selection = MatchesList->GetSelection();
			if (t4p::NumberLessThan(selection, MatchingTags.size())) {
				ChosenTags.push_back(MatchingTags[selection]);
				EndModal(wxOK);
			}
		}

	}
}


void t4p::FileSearchDialogClass::OnOkButton(wxCommandEvent& event) {
	OnSearchEnter(event);
}

BEGIN_EVENT_TABLE(t4p::OutlineViewClass, FeatureViewClass)
	EVT_MENU(t4p::MENU_OUTLINE, t4p::OutlineViewClass::OnOutlineMenu)
	EVT_APP_FILE_PAGE_CHANGED(t4p::OutlineViewClass::OnAppFilePageChanged)
	EVT_APP_FILE_CLOSED(t4p::OutlineViewClass::OnAppFileClosed)
	EVT_APP_FILE_OPEN(t4p::OutlineViewClass::OnAppFileOpened)
	EVENT_OUTLINE_SEARCH_COMPLETE(wxID_ANY, t4p::OutlineViewClass::OnTagSearchComplete)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::OutlineViewPanelClass, OutlineViewGeneratedPanelClass)
	EVT_MENU(ID_OUTLINE_MENU_DELETE, t4p::OutlineViewPanelClass::OnTreeMenuDelete)
	EVT_MENU(ID_OUTLINE_MENU_COLLAPSE, t4p::OutlineViewPanelClass::OnTreeMenuCollapse)
	EVT_MENU(ID_OUTLINE_MENU_COLLAPSE_ALL, t4p::OutlineViewPanelClass::OnTreeMenuCollapseAll)
	EVT_MENU(ID_OUTLINE_MENU_EXPAND_ALL, t4p::OutlineViewPanelClass::OnTreeMenuExpandAll)
	EVT_MENU(ID_OUTLINE_MENU_TOGGLE_METHODS, t4p::OutlineViewPanelClass::OnMethodsClick)
	EVT_MENU(ID_OUTLINE_MENU_TOGGLE_PROPERTIES, t4p::OutlineViewPanelClass::OnPropertiesClick)
	EVT_MENU(ID_OUTLINE_MENU_TOGGLE_CONSTANTS, t4p::OutlineViewPanelClass::OnConstantsClick)
	EVT_MENU(ID_OUTLINE_MENU_TOGGLE_INHERITED, t4p::OutlineViewPanelClass::OnInheritedClick)
	EVT_MENU(ID_OUTLINE_MENU_TOGGLE_PUBLIC_ONLY, t4p::OutlineViewPanelClass::OnPublicOnlyClick)
	EVT_MENU(ID_OUTLINE_MENU_TOGGLE_FUNCTION_ARGS, t4p::OutlineViewPanelClass::OnFunctionArgsClick)
	EVT_MENU(ID_OUTLINE_MENU_SORT_BY_NAME, t4p::OutlineViewPanelClass::OnSortByNameClick)
	EVT_MENU(ID_OUTLINE_MENU_SORT_BY_TYPE, t4p::OutlineViewPanelClass::OnSortByTypeClick)
END_EVENT_TABLE()
