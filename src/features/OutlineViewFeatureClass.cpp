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
#include <features/OutlineViewFeatureClass.h>
#include <language/SymbolTableClass.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <globals/Errors.h>
#include <globals/TagList.h>
#include <language/TagParserClass.h>
#include <globals/Sqlite.h>
#include <widgets/TreeItemDataStringClass.h>
#include <MvcEditor.h>
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

const wxEventType mvceditor::EVENT_TAG_SEARCH_COMPLETE = wxNewEventType();

static bool SortTagsByName(const mvceditor::TagClass& a, const mvceditor::TagClass& b) {
	return a.Identifier.caseCompare(b.Identifier, 0) < 0;
}

static bool SortTagsByTypeAndName(const mvceditor::TagClass& a, const mvceditor::TagClass& b) {
	int ret = 0;
	if (a.Type < b.Type) {
		return true;
	}
	else if (a.Type > b.Type) {
		return false;
	}
	return a.Identifier.caseCompare(b.Identifier, 0) < 0;
}

mvceditor::TagSearchCompleteClass::TagSearchCompleteClass()
	: Label()
	, Tags() {

}


mvceditor::TagSearchCompleteClass::TagSearchCompleteClass(const mvceditor::TagSearchCompleteClass& src)
	: Label()
	, Tags() {
	Copy(src);
}

void mvceditor::TagSearchCompleteClass::Copy(const mvceditor::TagSearchCompleteClass& src) {
	Label = src.Label;
	Tags = src.Tags;
}

bool mvceditor::TagSearchCompleteClass::IsLabelFileName() const {
	return Label.find(wxT(".")) != wxNOT_FOUND;
}

mvceditor::TagSearchCompleteEventClass::TagSearchCompleteEventClass(int eventId, 
																	const std::vector<mvceditor::TagSearchCompleteClass>& tags)
	: wxEvent(eventId, mvceditor::EVENT_TAG_SEARCH_COMPLETE)
	, Tags(tags) {

}

wxEvent* mvceditor::TagSearchCompleteEventClass::Clone() const {
	return new mvceditor::TagSearchCompleteEventClass(GetId(), Tags);
}

mvceditor::TagCacheSearchActionClass::TagCacheSearchActionClass(mvceditor::RunningThreadsClass& runningThreads,
																int eventId)
	: ActionClass(runningThreads, eventId)
	, TagCache()
	, SearchStrings() {

}

void mvceditor::TagCacheSearchActionClass::SetSearch(const std::vector<UnicodeString>& searches, mvceditor::GlobalsClass& globals) {
	SearchStrings = searches;
	mvceditor::GlobalCacheClass* cache = new mvceditor::GlobalCacheClass;
	cache->InitGlobalTag(globals.TagCacheDbFileName, globals.GetPhpFileExtensions(), globals.GetMiscFileExtensions(),
		globals.Environment.Php.Version);
	cache->InitNativeTag(mvceditor::NativeFunctionsAsset());
	cache->InitDetectorTag(globals.DetectorCacheDbFileName);
	TagCache.RegisterGlobal(cache);
}

void mvceditor::TagCacheSearchActionClass::BackgroundWork() {
	std::vector<mvceditor::TagSearchCompleteClass> topLevelTags;
	if (!IsCancelled()) {
		std::vector<UnicodeString>::const_iterator search;
		for (search = SearchStrings.begin(); !IsCancelled() && search != SearchStrings.end(); ++search) {
			std::vector<mvceditor::TagClass> tags;
			std::vector<mvceditor::TagClass>::const_iterator tag;
			TagSearchCompleteClass tagSearchComplete;
			tagSearchComplete.Label = mvceditor::IcuToWx(*search);
			if (search->indexOf(UNICODE_STRING_SIMPLE(".")) >= 0) {
				
				// searching for all tags in the file
				tags = TagCache.AllClassesFunctionsDefines(mvceditor::IcuToWx(*search));
				
				// now for each class, collect all methods/properties for any class.
				for (tag = tags.begin(); tag != tags.end(); ++tag) {
					if (tag->Type == mvceditor::TagClass::CLASS) {
						wxString classLabel = mvceditor::IcuToWx(tag->Identifier);
						if (!tag->NamespaceName.isEmpty()) {
							classLabel += wxT(": ") + mvceditor::IcuToWx(tag->NamespaceName);
						}
						tagSearchComplete.Tags[classLabel] = TagCache.AllMemberTags(tag->FullyQualifiedClassName());
					}
					else {
						tagSearchComplete.Tags[wxT("")].push_back(*tag);
					}
				}
			}
			else {
				
				// searching for all members in a class name
				tags = TagCache.ExactTags(*search);
				if (!tags.empty()) {
					tag = tags.begin();
				}
				wxString classLabel = mvceditor::IcuToWx(tag->Identifier);
				if (classLabel.Contains(wxT("\\"))) {
					classLabel = classLabel.AfterLast(wxT('\\'));
				}
				if (!tag->NamespaceName.isEmpty()) {
					classLabel += wxT(": ") + mvceditor::IcuToWx(tag->NamespaceName);
				}
				tagSearchComplete.Label = classLabel;
				tagSearchComplete.Tags[wxT("")] = TagCache.AllMemberTags(tag->FullyQualifiedClassName());
			}
			topLevelTags.push_back(tagSearchComplete);
		}
	}
	if (!IsCancelled()) {

		// PostEvent will set the correct event ID
		mvceditor::TagSearchCompleteEventClass evt(wxID_ANY, topLevelTags);
		PostEvent(evt);	
	}
}
wxString mvceditor::TagCacheSearchActionClass::GetLabel() const {
	return wxT("Tag Cache Search");
}

mvceditor::OutlineViewFeatureClass::OutlineViewFeatureClass(mvceditor::AppClass& app)
	: FeatureClass(app) {
}

void mvceditor::OutlineViewFeatureClass::AddViewMenuItems(wxMenu* viewMenu) {
	viewMenu->Append(mvceditor::MENU_OUTLINE, _("Outline Current File\tSHIFT+F2"),  _("Opens an outline view of the currently viewed file"), wxITEM_NORMAL);
}

void mvceditor::OutlineViewFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_OUTLINE + 0] = wxT("Outline-Outline Current File");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::OutlineViewFeatureClass::JumpToResource(const wxString& tag) {
	std::vector<mvceditor::TagClass> matches = App.Globals.TagCache.ExactTags(mvceditor::WxToIcu(tag));
	if (!matches.empty()) {
		mvceditor::TagClass tag = matches[0];
		GetNotebook()->LoadPage(tag.GetFullPath());
		CodeControlClass* codeControl = GetCurrentCodeControl();
		if (codeControl) {
			int32_t position, 
				length;
			bool found = mvceditor::ParsedTagFinderClass::GetResourceMatchPosition(tag, codeControl->GetSafeText(), position, length);
			if (found) {
				codeControl->SetSelectionAndEnsureVisible(position, position + length);
			}
			// else the index is out of date....
		}
	}	
}

void mvceditor::OutlineViewFeatureClass::StartTagSearch(const std::vector<UnicodeString>& searchStrings) {
	mvceditor::TagCacheSearchActionClass* action = new mvceditor::TagCacheSearchActionClass(App.RunningThreads, wxID_ANY);
	action->SetSearch(searchStrings, App.Globals);
	App.RunningThreads.Queue(action);
}

void mvceditor::OutlineViewFeatureClass::OnOutlineMenu(wxCommandEvent& event) {
	
	// create / open the outline window
	wxWindow* window = FindOutlineWindow(ID_WINDOW_OUTLINE);
	OutlineViewPanelClass* outlineViewPanel = NULL;
	if (window != NULL) {
		outlineViewPanel = (OutlineViewPanelClass*)window;
		SetFocusToOutlineWindow(outlineViewPanel);

	}
	else {
		mvceditor::NotebookClass* notebook = GetNotebook();
		if (notebook != NULL) {
			outlineViewPanel = new OutlineViewPanelClass(GetOutlineNotebook(), ID_WINDOW_OUTLINE, this, notebook);
			wxBitmap outlineBitmap = mvceditor::IconImageAsset(wxT("outline"));
			AddOutlineWindow(outlineViewPanel, wxT("Outline"), outlineBitmap);
		}
	}

	// get all classes / functions for the active file
	mvceditor::CodeControlClass* codeCtrl = GetCurrentCodeControl();
	if (codeCtrl && !codeCtrl->GetFileName().IsEmpty()) {
		std::vector<UnicodeString> searchStrings;
		searchStrings.push_back(mvceditor::WxToIcu(codeCtrl->GetFileName()));
		StartTagSearch(searchStrings);
	}
}

void mvceditor::OutlineViewFeatureClass::OnContentNotebookPageChanged(wxAuiNotebookEvent& event) {
	wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_OUTLINE, GetOutlineNotebook());

	// only change the outline if the user is looking at the outline.  otherwise, it gets 
	// annoying if the user is looking at run output, switches PHP files, and the outline
	// gets changed.
	if (window != NULL && IsOutlineWindowSelected(ID_WINDOW_OUTLINE)) {
		OutlineViewPanelClass* outlineViewPanel = (OutlineViewPanelClass*)window;
		SetFocusToOutlineWindow(outlineViewPanel);

		mvceditor::CodeControlClass* codeCtrl = GetNotebook()->GetCodeControl(event.GetSelection());
		if (codeCtrl && !codeCtrl->GetFileName().IsEmpty()) {
			std::vector<UnicodeString> searchStrings;
			searchStrings.push_back(mvceditor::WxToIcu(codeCtrl->GetFileName()));
			StartTagSearch(searchStrings);
		}
	}
	event.Skip();
}

void mvceditor::OutlineViewFeatureClass::OnContentNotebookPageClosed(wxAuiNotebookEvent& event) {
	wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_OUTLINE, GetOutlineNotebook());
	if (window != NULL) {
		OutlineViewPanelClass* outlineViewPanel = (OutlineViewPanelClass*)window;
		int currentPage = event.GetSelection();
		CodeControlClass* codeCtrl = GetNotebook()->GetCodeControl(currentPage);
		if (codeCtrl) {
			outlineViewPanel->RemoveFileFromOutline(codeCtrl->GetFileName());
		}
	}
	event.Skip();
}

void mvceditor::OutlineViewFeatureClass::OnWorkingCacheComplete(mvceditor::WorkingCacheCompleteEventClass& event) {
	
	//if the outline window is open, update the file that was parsed
	wxWindow* window = FindOutlineWindow(ID_WINDOW_OUTLINE);
	if (window != NULL) {
		wxString fileName = event.GetFileName();
		std::vector<UnicodeString> searchStrings;
		searchStrings.push_back(mvceditor::WxToIcu(fileName));
		StartTagSearch(searchStrings);
	}
}

void mvceditor::OutlineViewFeatureClass::OnTagSearchComplete(mvceditor::TagSearchCompleteEventClass& event) {

	//if the outline window is open, update the tree
	wxWindow* window = FindOutlineWindow(ID_WINDOW_OUTLINE);
	OutlineViewPanelClass* outlineViewPanel = NULL;
	if (window != NULL) {
		outlineViewPanel = (OutlineViewPanelClass*)window;
		outlineViewPanel->AddTagsToOutline(event.Tags);
	}
}

mvceditor::OutlineViewPanelClass::OutlineViewPanelClass(wxWindow* parent, int windowId, OutlineViewFeatureClass* feature, 
		NotebookClass* notebook)
	: OutlineViewGeneratedPanelClass(parent, windowId)
	, OutlinedTags()
	, ImageList(NULL)
	, Feature(feature)
	, Notebook(notebook)
	, ShowMethods(true)
	, ShowProperties(true)
	, ShowConstants(true) 
	, ShowInherited(true)
	, ShowPublicOnly(false) 
	, ShowFunctionArgs(false)
	, SortByName(true)
	, SortByType(false) {
	HelpButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_HELP, 
		wxART_TOOLBAR, wxSize(16, 16))));
	SyncButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("outline-refresh")));
	AddButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("outline-add")));
	FilterButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("filter")));
	SortButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("sort")));

	SetStatus(_(""));
	
	ImageList = new wxImageList(16, 16);
	ImageList->Add(mvceditor::IconImageAsset(wxT("outline")));
	ImageList->Add(mvceditor::IconImageAsset(wxT("document-php")));
	ImageList->Add(mvceditor::IconImageAsset(wxT("class")));
	ImageList->Add(mvceditor::IconImageAsset(wxT("method-public")));
	ImageList->Add(mvceditor::IconImageAsset(wxT("method-protected")));
	ImageList->Add(mvceditor::IconImageAsset(wxT("method-private")));
	ImageList->Add(mvceditor::IconImageAsset(wxT("method-inherited")));
	ImageList->Add(mvceditor::IconImageAsset(wxT("property-public")));
	ImageList->Add(mvceditor::IconImageAsset(wxT("property-protected")));
	ImageList->Add(mvceditor::IconImageAsset(wxT("property-private")));
	ImageList->Add(mvceditor::IconImageAsset(wxT("property-inherited")));

	ImageList->Add(mvceditor::IconImageAsset(wxT("define")));
	ImageList->Add(mvceditor::IconImageAsset(wxT("class-constant")));
	ImageList->Add(mvceditor::IconImageAsset(wxT("namespace")));
	ImageList->Add(mvceditor::IconImageAsset(wxT("function")));
	ImageList->Add(mvceditor::IconImageAsset(wxT("variable-template")));

	// let the tree control managet the image list
	// since it may need to use it in the destructor
	Tree->AssignImageList(ImageList);
}

void mvceditor::OutlineViewPanelClass::SetStatus(const wxString& status) {
	StatusLabel->SetLabel(status);
}

void mvceditor::OutlineViewPanelClass::AddTagsToOutline(const std::vector<mvceditor::TagSearchCompleteClass>& tags) {
	size_t oldSize = OutlinedTags.size();
	OutlinedTags.insert(OutlinedTags.end(), tags.begin(), tags.end());
	std::vector<mvceditor::TagSearchCompleteClass>::iterator searchTag;
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
			fileId = Tree->PrependItem(rootId, wxFileName(label).GetFullName(), IMAGE_OUTLINE_FILE, -1, new mvceditor::TreeItemDataStringClass(label)); 
		}
		else if (searchTag->IsLabelFileName()) {
			fileId = Tree->PrependItem(rootId, wxFileName(label).GetFullName(), IMAGE_OUTLINE_FILE, -1, new mvceditor::TreeItemDataStringClass(label)); 
		}
		else if (fileId.IsOk() && !searchTag->IsLabelFileName()) {
			Tree->Delete(fileId);
			fileId = Tree->PrependItem(rootId, label, IMAGE_OUTLINE_CLASS, -1, new mvceditor::TreeItemDataStringClass(label)); 
		}
		else {
			fileId = Tree->PrependItem(rootId, label, IMAGE_OUTLINE_CLASS, -1, new mvceditor::TreeItemDataStringClass(label)); 
		}

		std::map<wxString, std::vector<mvceditor::TagClass> >::iterator mapTag; 		
		std::vector<mvceditor::TagClass>::const_iterator memberTag;
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
				TagToNode(*memberTag, parent, mvceditor::WxToIcu(mapTag->first));
			}
		}
		Tree->SelectItem(fileId);
	}
	Tree->ExpandAll();
	Tree->Thaw();
}

void mvceditor::OutlineViewPanelClass::TagToNode(const mvceditor::TagClass& tag, wxTreeItemId& treeId, UnicodeString classNameNode) {

	// for now never show dynamic resources since there is no way we can know where the source for them is.
	int type = tag.Type;
	UnicodeString className = mvceditor::WxToIcu(Tree->GetItemText(treeId));
	wxString label = mvceditor::IcuToWx(tag.Identifier);

	// if the flag to show only public members is on, then do not show private or protected tags
	bool passesAccessCheck = !ShowPublicOnly || (!tag.IsPrivate && !tag.IsProtected);

	// compare clas nodes in the same way that they come from the TagCacheSearchActionClass
	UnicodeString tagClassName = tag.ClassName;
	if (!tag.NamespaceName.isEmpty()) {
		tagClassName += UNICODE_STRING_SIMPLE(": ") + tag.NamespaceName;
	}
	bool isInheritedTag = tagClassName.caseCompare(classNameNode, 0) != 0;
	if (!classNameNode.isEmpty() && !ShowInherited) {
		
		// make sure that the inheritance check passes too
		passesAccessCheck = passesAccessCheck && !isInheritedTag;
	}
	if (mvceditor::TagClass::DEFINE == type && !tag.IsDynamic) {
		Tree->AppendItem(treeId, label, IMAGE_OUTLINE_DEFINE, -1, 
			new mvceditor::TreeItemDataStringClass(mvceditor::IcuToWx(tag.Key)));
	}
	else if (mvceditor::TagClass::MEMBER == tag.Type && ShowProperties && passesAccessCheck) {
		label = mvceditor::IcuToWx(tag.Identifier);
		if (isInheritedTag) {
			label = mvceditor::IcuToWx(tag.ClassName) + wxT("::") + label;
		}
		if (!tag.ReturnType.isEmpty()) {
			wxString returnType = mvceditor::IcuToWx(tag.ReturnType);
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
		Tree->AppendItem(treeId, label, image, -1, 
			new mvceditor::TreeItemDataStringClass(mvceditor::IcuToWx(tag.Key)));
	}
	else if (mvceditor::TagClass::METHOD == tag.Type && ShowMethods && passesAccessCheck) {
		label = mvceditor::IcuToWx(tag.Identifier);
		if (isInheritedTag) {
			label = mvceditor::IcuToWx(tag.ClassName) + wxT("::") + label;
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
			wxString returnType = mvceditor::IcuToWx(tag.ReturnType);
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
		wxTreeItemId funcId = Tree->AppendItem(treeId, label, image, -1, 
			new mvceditor::TreeItemDataStringClass(mvceditor::IcuToWx(tag.Key)));
		if (ShowFunctionArgs) {

			// add the function args under the method name
			int32_t argsStart = tag.Signature.indexOf(UNICODE_STRING_SIMPLE("(")); 
			int32_t argsEnd = tag.Signature.indexOf(UNICODE_STRING_SIMPLE(")")); 
			if (argsStart > 0 && argsEnd > 0) {
				UnicodeString sig(tag.Signature, argsStart + 1, argsEnd - argsStart - 1);
				
				wxStringTokenizer tok(mvceditor::IcuToWx(sig), wxT(","));
				while (tok.HasMoreTokens()) {
					Tree->AppendItem(funcId, tok.NextToken(), IMAGE_OUTLINE_ARGUMENT, -1,
						new mvceditor::TreeItemDataStringClass(mvceditor::IcuToWx(tag.Key)));
				}
			}
		}
	}
	else if (mvceditor::TagClass::CLASS_CONSTANT == tag.Type && ShowConstants && passesAccessCheck) {
		if (tag.ClassName != className) {
			label = mvceditor::IcuToWx(tag.ClassName) + wxT("::") + label;
		}
		Tree->AppendItem(treeId, label, IMAGE_OUTLINE_CLASS_CONSTANT, -1,
			new mvceditor::TreeItemDataStringClass(mvceditor::IcuToWx(tag.Key)));
	}
	else if (mvceditor::TagClass::FUNCTION == type && !tag.IsDynamic) {
		UnicodeString res = tag.Identifier;
		wxString label = mvceditor::IcuToWx(res);

		// add the function signature to the label
		int32_t sigIndex = tag.Signature.indexOf(UNICODE_STRING_SIMPLE("function ")); 
		if (sigIndex >= 0) {
			UnicodeString sig(tag.Signature, sigIndex + 9);
			label = mvceditor::IcuToWx(sig);
		}
		if (!tag.ReturnType.isEmpty()) {
			wxString returnType = mvceditor::IcuToWx(tag.ReturnType);
			label += wxT(" [") + returnType + wxT("]");
		}
		Tree->AppendItem(treeId, label, IMAGE_OUTLINE_FUNCTION, -1, 
			new mvceditor::TreeItemDataStringClass(mvceditor::IcuToWx(tag.Key)));
	}
}

void mvceditor::OutlineViewPanelClass::OnHelpButton(wxCommandEvent& event) {
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

void mvceditor::OutlineViewPanelClass::OnAddButton(wxCommandEvent& event) {
	std::vector<mvceditor::TagClass> tags;
	mvceditor::FileSearchDialogClass dialog(this->GetParent(), *Feature, tags);
	if (dialog.ShowModal() == wxOK) {
		SearchTagsToOutline(tags);
	}
}

void mvceditor::OutlineViewPanelClass::OnSyncButton(wxCommandEvent& event) {
	OutlinedTags.clear();
	Tree->DeleteAllItems();
	std::vector<UnicodeString> searchStrings;
	for (size_t i = 0; i < Notebook->GetPageCount(); i++) {
		mvceditor::CodeControlClass* codeCtrl = Notebook->GetCodeControl(i);
		if (codeCtrl && !codeCtrl->GetFileName().IsEmpty()) {
			searchStrings.push_back(mvceditor::WxToIcu(codeCtrl->GetFileName()));
		}
	}
	Feature->StartTagSearch(searchStrings);
}

void mvceditor::OutlineViewPanelClass::OnTreeItemActivated(wxTreeEvent& event) {

	// the method name is the leaf node, the class name is the parent of the activated node
	wxTreeItemId item = event.GetItem();
	wxString methodSig = Tree->GetItemText(item);
	if (!item.IsOk() || Tree->GetChildrenCount(item) > 0) {

		// dont want to handle a non-leaf (a class)
		event.Skip();
		return;
	}
	mvceditor::TreeItemDataStringClass* data = (mvceditor::TreeItemDataStringClass*)Tree->GetItemData(item);
	if (!data || data->Str.IsEmpty()) {
		event.Skip();
		return;
	}
	wxString tag = data->Str;
	Feature->JumpToResource(tag);
}

void mvceditor::OutlineViewPanelClass::SearchTagsToOutline(const std::vector<mvceditor::TagClass>& tags) {

	// each tag could be a file or a class tag. 
	//if its a class tag, get all of members for the class
	std::vector<mvceditor::TagClass>::const_iterator chosenTag;
	std::vector<UnicodeString> searchStrings;
		
	for (chosenTag = tags.begin(); chosenTag != tags.end(); ++chosenTag) {
		if (chosenTag->Identifier.indexOf(UNICODE_STRING_SIMPLE(".")) >= 0) {
			
			// user chose a file: get all classes / functions for that file
			searchStrings.push_back(mvceditor::WxToIcu(chosenTag->FullPath));		
		}
		else {

			// user chose a class; add the class member to the outline
			searchStrings.push_back(chosenTag->FullyQualifiedClassName());
		}
	}
	Feature->StartTagSearch(searchStrings);
}

void mvceditor::OutlineViewPanelClass::OnTreeItemRightClick(wxTreeEvent& event) {
	
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


void mvceditor::OutlineViewPanelClass::OnFilterLeftDown(wxMouseEvent& event) {
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
	else {
		event.Skip();
	}
}

void mvceditor::OutlineViewPanelClass::OnSortLeftDown(wxMouseEvent& event) {
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
	else {
		event.Skip();
	}
}

void mvceditor::OutlineViewPanelClass::OnTreeMenuDelete(wxCommandEvent& event) {
	
	// only allow deletion on the first level items
	wxTreeItemId rootId = Tree->GetRootItem();
	wxTreeItemId itemId = Tree->GetSelection();
	if (itemId.IsOk() && rootId == Tree->GetItemParent(itemId)) {
		mvceditor::TreeItemDataStringClass* data = (mvceditor::TreeItemDataStringClass*)Tree->GetItemData(itemId);
		wxString label = Tree->GetItemText(itemId);
		if (data) {
			wxString fullPath = data->Str;

			// remove the file from the outlined tags list
			std::vector<mvceditor::TagSearchCompleteClass>::iterator it = OutlinedTags.begin();
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

void mvceditor::OutlineViewPanelClass::OnTreeMenuCollapse(wxCommandEvent& event) {
	wxTreeItemId itemId = Tree->GetSelection();
	if (itemId.IsOk()) {
		Tree->Collapse(itemId);
	}
}

void mvceditor::OutlineViewPanelClass::OnTreeMenuCollapseAll(wxCommandEvent& event) {
	wxTreeItemId rootId = Tree->GetRootItem();
	if (rootId.IsOk()) {
		Tree->CollapseAllChildren(rootId);
		Tree->Expand(rootId);
	}
}

void mvceditor::OutlineViewPanelClass::OnTreeMenuExpandAll(wxCommandEvent& event) {
	wxTreeItemId rootId = Tree->GetRootItem();
	if (rootId.IsOk()) {
		Tree->ExpandAllChildren(rootId);
	}
}

void mvceditor::OutlineViewPanelClass::RedrawOutline() {
	std::vector<mvceditor::TagSearchCompleteClass> tags = OutlinedTags;
	OutlinedTags.clear(); // AddFileToOutline will add the given tags to this list
	AddTagsToOutline(tags);		
}

void mvceditor::OutlineViewPanelClass::OnMethodsClick(wxCommandEvent& event) {
	ShowMethods = !ShowMethods;
	RedrawOutline();
}

void mvceditor::OutlineViewPanelClass::OnPropertiesClick(wxCommandEvent& event) {
	ShowProperties = !ShowProperties;
	RedrawOutline();
}

void mvceditor::OutlineViewPanelClass::OnConstantsClick(wxCommandEvent& event) {
	ShowConstants = !ShowConstants;
	RedrawOutline();
}

void mvceditor::OutlineViewPanelClass::OnInheritedClick(wxCommandEvent& event) {
	ShowInherited = !ShowInherited;
	RedrawOutline();
}

void mvceditor::OutlineViewPanelClass::OnPublicOnlyClick(wxCommandEvent& event) {
	ShowPublicOnly = !ShowPublicOnly;
	RedrawOutline();
}

void mvceditor::OutlineViewPanelClass::OnFunctionArgsClick(wxCommandEvent& event) {
	ShowFunctionArgs = !ShowFunctionArgs;
	RedrawOutline();
}

void mvceditor::OutlineViewPanelClass::OnSortByTypeClick(wxCommandEvent& event) {
	SortByType = !SortByType;
	if (SortByType) {
		SortByName = false;
	}
	RedrawOutline();
}

void mvceditor::OutlineViewPanelClass::OnSortByNameClick(wxCommandEvent& event) {
	SortByName = !SortByName;
	if (SortByName) {
		SortByType = false;
	}
	RedrawOutline();
}

mvceditor::FileSearchDialogClass::FileSearchDialogClass(wxWindow *parent, mvceditor::OutlineViewFeatureClass& feature, std::vector<mvceditor::TagClass>& chosenTags)
	: FileSearchDialogGeneratedClass(parent)
	, Feature(feature)
	, MatchingTags()
	, ChosenTags(chosenTags) {
	SearchText->SetFocus();
	Init();
}

void mvceditor::FileSearchDialogClass::Init() {
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

void mvceditor::FileSearchDialogClass::Search() {
	wxString search = SearchText->GetValue();
	if (search.Length() < 2) {
		return;
	}
	std::vector<mvceditor::ProjectClass*> projects;
	bool showAllProjects = ProjectChoice->GetSelection() == 0;
	if (!showAllProjects) {
		projects.push_back((mvceditor::ProjectClass*)ProjectChoice->GetClientData(ProjectChoice->GetSelection()));
	}
	else {

		// the first item in the wxChoice will not have client data; the "all" option
		for (size_t i = 1; i < ProjectChoice->GetCount(); ++i) {
			projects.push_back((mvceditor::ProjectClass*) ProjectChoice->GetClientData(i));
		}
	}
	if (search.Length() == 2) {
		MatchingTags = Feature.App.Globals.TagCache.ExactClassOrFile(mvceditor::WxToIcu(search));
	}
	else {
		MatchingTags = Feature.App.Globals.TagCache.NearMatchClassesOrFiles(mvceditor::WxToIcu(search));
	}

	// no need to show jump to results for native functions
	// TODO: CollectNearResourceMatches shows resources from files that were recently deleted
	// need to hide them / remove them
	mvceditor::TagListRemoveNativeMatches(MatchingTags);
	mvceditor::TagListKeepMatchesFromProjects(MatchingTags, projects);
	ShowTags(search, MatchingTags);
}

void mvceditor::FileSearchDialogClass::OnSearchText(wxCommandEvent& event) {
	Search();
}

void mvceditor::FileSearchDialogClass::OnProjectChoice(wxCommandEvent& event) {
	Search();
}

void mvceditor::FileSearchDialogClass::OnSearchKeyDown(wxKeyEvent& event) {
	int keyCode = event.GetKeyCode();
	size_t selection = MatchesList->GetSelection();
	if (keyCode == WXK_DOWN) {		
		if (!MatchesList->IsEmpty() && selection >= 0 && selection < (MatchesList->GetCount() - 1)) {
			MatchesList->SetSelection(selection + 1);
		}
		else if (!MatchesList->IsEmpty() && selection >= 0) {

			// cycle back to the beginning
			MatchesList->SetSelection(0);
		}
		SearchText->SetFocus();
	}
	else if (keyCode == WXK_UP) {
		if (!MatchesList->IsEmpty() && selection > 0 && selection < MatchesList->GetCount()) {
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

void mvceditor::FileSearchDialogClass::OnMatchesListDoubleClick(wxCommandEvent& event) {
	TransferDataFromWindow();
	ChosenTags.clear();
	size_t selection = event.GetSelection();
	if (selection >= 0 && selection < MatchesList->GetCount()) {
		ChosenTags.push_back(MatchingTags[selection]);
	}
	if (ChosenTags.empty()) {
		return;
	}
	EndModal(wxOK);
}

void mvceditor::FileSearchDialogClass::OnMatchesListKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_RETURN) {
		wxCommandEvent cmdEvt;
		OnSearchEnter(cmdEvt);
	}
	else {
		event.Skip();
	}
}

void mvceditor::OutlineViewPanelClass::RemoveFileFromOutline(const wxString& fullPath) {
	wxTreeItemId fileId = FindFileNode(fullPath);
	if (fileId.IsOk()) {
		wxString label = Tree->GetItemText(fileId);
		Tree->Delete(fileId);

		// remove the file from the outlined tags list
		std::vector<mvceditor::TagSearchCompleteClass>::iterator it = OutlinedTags.begin();
		
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

wxTreeItemId mvceditor::OutlineViewPanelClass::FindFileNode(const wxString& fullPath) {
	wxTreeItemId treeIndex, fileId;
	wxTreeItemId rootId = Tree->GetRootItem();
	if (rootId.IsOk()) {
		wxTreeItemIdValue treeCookie;
		treeIndex = Tree->GetFirstChild(rootId, treeCookie);
		while (treeIndex.IsOk()) {
			mvceditor::TreeItemDataStringClass* data = (mvceditor::TreeItemDataStringClass*)Tree->GetItemData(treeIndex);
			if (data && data->Str == fullPath) {
				fileId = treeIndex;
				break;
			}
			treeIndex = Tree->GetNextChild(rootId, treeCookie);
		}
	}
	return fileId;
}

void mvceditor::FileSearchDialogClass::ShowTags(const wxString& finderQuery, const std::vector<mvceditor::TagClass>& allMatches) {
	wxArrayString files;
	for (size_t i = 0; i < allMatches.size(); ++i) {
		files.Add(allMatches[i].GetFullPath());
	}
	MatchesList->Clear();
	bool showAllProjects = ProjectChoice->GetSelection() == 0;
	mvceditor::ProjectClass* selectedProject = NULL;
	if (!showAllProjects) {
		selectedProject = (mvceditor::ProjectClass*)ProjectChoice->GetClientData(ProjectChoice->GetSelection());
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
		mvceditor::TagClass match = allMatches[i];
		if (mvceditor::TagClass::MEMBER == match.Type || mvceditor::TagClass::METHOD == match.Type ||
			mvceditor::TagClass::CLASS_CONSTANT == match.Type) {
			matchLabel += mvceditor::IcuToWx(match.ClassName);
			matchLabel += wxT("::");
			matchLabel += mvceditor::IcuToWx(match.Identifier);
		}
		else if (mvceditor::TagClass::CLASS == match.Type || mvceditor::TagClass::FUNCTION == match.Type
			|| mvceditor::TagClass::DEFINE == match.Type) {
			matchLabel += mvceditor::IcuToWx(match.Identifier);
		}
		else {
			matchLabel += mvceditor::IcuToWx(match.Identifier);
		}
		matchLabel += wxT(" - ");
		matchLabel += relativeName;
		matchLabel +=  wxT("  (") + projectLabel + wxT(")");
		MatchesList->Append(matchLabel);
	}
	if (!MatchesList->IsEmpty()) {
		MatchesList->Select(0);
	}
	MatchesLabel->SetLabel(wxString::Format(_("Found %d files. Please choose file(s) to open."), allMatches.size()));
}

void mvceditor::FileSearchDialogClass::OnSearchEnter(wxCommandEvent& event) {
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
				if (matchIndex >= 0 && matchIndex < MatchingTags.size()) {
					ChosenTags.push_back(MatchingTags[matchIndex]);
				}
			}
			EndModal(wxOK);
		}
		else {
			// no checked items, take the user to the
			// selected item
			size_t selection = MatchesList->GetSelection();
			if (selection >= 0 && selection < MatchingTags.size()) {
				ChosenTags.push_back(MatchingTags[selection]);
				EndModal(wxOK);
			}
		}

	}
}


void mvceditor::FileSearchDialogClass::OnOkButton(wxCommandEvent& event) {
	OnSearchEnter(event);
}

BEGIN_EVENT_TABLE(mvceditor::OutlineViewFeatureClass, wxEvtHandler)
	EVT_MENU(mvceditor::MENU_OUTLINE, mvceditor::OutlineViewFeatureClass::OnOutlineMenu)
	EVT_AUINOTEBOOK_PAGE_CHANGED(mvceditor::ID_CODE_NOTEBOOK, mvceditor::OutlineViewFeatureClass::OnContentNotebookPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSE(mvceditor::ID_CODE_NOTEBOOK, 
		mvceditor::OutlineViewFeatureClass::OnContentNotebookPageClosed)
	EVT_WORKING_CACHE_COMPLETE(wxID_ANY, mvceditor::OutlineViewFeatureClass::OnWorkingCacheComplete)
	EVENT_TAG_SEARCH_COMPLETE(wxID_ANY, mvceditor::OutlineViewFeatureClass::OnTagSearchComplete)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::OutlineViewPanelClass, OutlineViewGeneratedPanelClass)
	EVT_MENU(ID_OUTLINE_MENU_DELETE, mvceditor::OutlineViewPanelClass::OnTreeMenuDelete)
	EVT_MENU(ID_OUTLINE_MENU_COLLAPSE, mvceditor::OutlineViewPanelClass::OnTreeMenuCollapse)
	EVT_MENU(ID_OUTLINE_MENU_COLLAPSE_ALL, mvceditor::OutlineViewPanelClass::OnTreeMenuCollapseAll)
	EVT_MENU(ID_OUTLINE_MENU_EXPAND_ALL, mvceditor::OutlineViewPanelClass::OnTreeMenuExpandAll)
	EVT_MENU(ID_OUTLINE_MENU_TOGGLE_METHODS, mvceditor::OutlineViewPanelClass::OnMethodsClick)
	EVT_MENU(ID_OUTLINE_MENU_TOGGLE_PROPERTIES, mvceditor::OutlineViewPanelClass::OnPropertiesClick)
	EVT_MENU(ID_OUTLINE_MENU_TOGGLE_CONSTANTS, mvceditor::OutlineViewPanelClass::OnConstantsClick)
	EVT_MENU(ID_OUTLINE_MENU_TOGGLE_INHERITED, mvceditor::OutlineViewPanelClass::OnInheritedClick)
	EVT_MENU(ID_OUTLINE_MENU_TOGGLE_PUBLIC_ONLY, mvceditor::OutlineViewPanelClass::OnPublicOnlyClick)
	EVT_MENU(ID_OUTLINE_MENU_TOGGLE_FUNCTION_ARGS, mvceditor::OutlineViewPanelClass::OnFunctionArgsClick)
	EVT_MENU(ID_OUTLINE_MENU_SORT_BY_NAME, mvceditor::OutlineViewPanelClass::OnSortByNameClick)
	EVT_MENU(ID_OUTLINE_MENU_SORT_BY_TYPE, mvceditor::OutlineViewPanelClass::OnSortByTypeClick)
END_EVENT_TABLE()
