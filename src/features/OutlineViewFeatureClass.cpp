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
#include <vector>
#include <algorithm>


static int ID_WINDOW_OUTLINE = wxNewId();
static int ID_RESOURCE_FINDER_BACKGROUND = wxNewId();
static int ID_OUTLINE_MENU_DELETE = wxNewId();

const wxEventType mvceditor::EVENT_RESOURCE_FINDER_COMPLETE = wxNewEventType();

mvceditor::ResourceFinderCompleteEventClass::ResourceFinderCompleteEventClass(int eventId, 
																			  const std::vector<mvceditor::TagClass>& resources,
																			  const wxString& fullPath)
	: wxEvent(eventId, mvceditor::EVENT_RESOURCE_FINDER_COMPLETE)
	, Resources(resources) 

	// fully clone a string, cause we want the event to be thread safe
	, FullPath(fullPath.c_str()){
		
}

wxEvent* mvceditor::ResourceFinderCompleteEventClass::Clone() const {
	return new mvceditor::ResourceFinderCompleteEventClass(GetId(), Resources, FullPath);
}

mvceditor::ResourceFinderBackgroundThreadClass::ResourceFinderBackgroundThreadClass(
		mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ThreadWithHeartbeatClass(runningThreads, eventId) 
	, Mutex()
	, FileName()
	, PhpVersion(pelet::PHP_53){
}

void mvceditor::ResourceFinderBackgroundThreadClass::Start(const wxString& fileName, pelet::Versions phpVersion) {

	// here, set the file name to be parsed. on the next loop background work, it will be parsed
	// make sure to deep copy the string
	wxMutexLocker locker(Mutex);
	FileName = fileName.c_str();
	PhpVersion = phpVersion;
}

void mvceditor::ResourceFinderBackgroundThreadClass::BackgroundWork() {
	while (!TestDestroy()) {
		wxString fileName;
		pelet::Versions version;
		{
			// make sure synchronize access and deep copy the wxString as wxString is 
			// not thread safe
			wxMutexLocker locker(Mutex);
			fileName = FileName.c_str();
			version = PhpVersion;
			
			// clear so that for the next loop we dont reparse the file
			FileName.Clear();
		}
		if (!fileName.IsEmpty() && wxFileName::FileExists(fileName)) {
			

			// need this call so that resources are actually parsed
			// need this so that the tag finder parsers the file
			mvceditor::TagParserClass tagParser;
			mvceditor::ParsedTagFinderClass tagFinder;
			try {
				soci::session session(*soci::factory_sqlite3(), ":memory:");
				wxString error;
				if (!mvceditor::SqliteSqlScript(mvceditor::ResourceSqlSchemaAsset(), session, error)) {
					wxASSERT_MSG(false, error);
				}
				tagParser.Init(&session);
				tagParser.SetVersion(version);
				tagParser.PhpFileExtensions.push_back(wxT("*.*"));
				tagParser.Walk(fileName);

				tagFinder.Init(&session);
				std::vector<mvceditor::TagClass> resources = tagFinder.All();
				if (!TestDestroy()) {
					mvceditor::ResourceFinderCompleteEventClass evt(ID_RESOURCE_FINDER_BACKGROUND, resources, fileName);
					PostEvent(evt);
				}
			}
			catch (std::exception& e) {
				wxString wxMsg = wxString::FromAscii(e.what());
				mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER, wxMsg);
			}
		}
		if (!TestDestroy()) {
			wxSleep(1);
		}
	}
}

mvceditor::OutlineViewFeatureClass::OutlineViewFeatureClass(mvceditor::AppClass& app)
	: FeatureClass(app) 
	, ResourceFinderBackgroundThread(NULL) {
}

void mvceditor::OutlineViewFeatureClass::AddViewMenuItems(wxMenu* viewMenu) {
	viewMenu->Append(mvceditor::MENU_OUTLINE, _("Outline Current File\tSHIFT+F2"),  _("Opens an outline view of the currently viewed file"), wxITEM_NORMAL);
}

void mvceditor::OutlineViewFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_OUTLINE + 0] = wxT("Outline-Outline Current File");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::OutlineViewFeatureClass::BuildOutlineCurrentCodeControl() {
	CodeControlClass* code = GetCurrentCodeControl();
	if (code != NULL) {

		// this pointer will delete itself when the thread terminates
		if (!ResourceFinderBackgroundThread) {
			ResourceFinderBackgroundThread = 
				new mvceditor::ResourceFinderBackgroundThreadClass(App.RunningThreads, ID_RESOURCE_FINDER_BACKGROUND);
			wxThreadIdType threadId;
			wxThreadError err = ResourceFinderBackgroundThread->CreateSingleInstance(threadId);
			if (err != wxTHREAD_NO_ERROR) {
				delete ResourceFinderBackgroundThread;
				ResourceFinderBackgroundThread = NULL;
			}
		}
		
		if (ResourceFinderBackgroundThread) {
			wxString fileName = code->GetFileName();
			OutlineViewPanelClass* outlineViewPanel = NULL;
			wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_OUTLINE, GetOutlineNotebook());
			if (window) {
				outlineViewPanel = (OutlineViewPanelClass*)window;
			}
			bool fileExists = wxFileName::FileExists(fileName);
			if (fileExists) {
				ResourceFinderBackgroundThread->Start(fileName, GetEnvironment()->Php.Version);
				if (outlineViewPanel) {					
					SetFocusToOutlineWindow(outlineViewPanel);
					outlineViewPanel->SetStatus(_("Parsing ..."));
				}				
			}
			else if (code->IsNew()) {

				// don't show this error when the outline tab is working on a completely new file
				// ATTN: completely new files are not currently being outlined; we should probably
				// do so 
				if (outlineViewPanel) {	
					outlineViewPanel->SetStatus(_(""));
					std::vector<mvceditor::TagClass> tags;
					outlineViewPanel->AddFileToOutline(tags, fileName);
				}
			} 
			else {

				// show the error on the outline tab and the regular error mechanism
				if (outlineViewPanel) {	
					outlineViewPanel->SetStatus(_("Invalid File"));
				}
				mvceditor::EditorLogError(mvceditor::ERR_INVALID_FILE, fileName);
			}
		}
	}
}

std::vector<mvceditor::TagClass> mvceditor::OutlineViewFeatureClass::BuildOutline(const wxString& className) {	
	std::vector<mvceditor::TagClass> allMatches;
	std::vector<mvceditor::TagClass> matches; 

	// get the class tag itself
	matches = App.Globals.TagCache.CollectFullyQualifiedResourceFromAll(mvceditor::WxToIcu(className));
	allMatches.insert(allMatches.end(), matches.begin(), matches.end());

	// make the tag finder match on all methods / properties
	UnicodeString lookup;
	lookup += mvceditor::WxToIcu(className);
	lookup += UNICODE_STRING_SIMPLE("::");
	matches = App.Globals.TagCache.CollectNearMatchResourcesFromAll(lookup);
	allMatches.insert(allMatches.end(), matches.begin(), matches.end());
	return allMatches;
}

void mvceditor::OutlineViewFeatureClass::JumpToResource(const wxString& tag) {
	std::vector<mvceditor::TagClass> matches = App.Globals.TagCache.CollectFullyQualifiedResourceFromAll(mvceditor::WxToIcu(tag));
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

void mvceditor::OutlineViewFeatureClass::OnOutlineMenu(wxCommandEvent& event) {
	BuildOutlineCurrentCodeControl();
		
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
}

void mvceditor::OutlineViewFeatureClass::OnContentNotebookPageChanged(wxAuiNotebookEvent& event) {
	wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_OUTLINE, GetOutlineNotebook());

	// only change the outline if the user is looking at the outline.  otherwise, it gets 
	// annoying if the user is looking at run output, switches PHP files, and the outline
	// gets changed.
	if (window != NULL && IsOutlineWindowSelected(ID_WINDOW_OUTLINE)) {
		OutlineViewPanelClass* outlineViewPanel = (OutlineViewPanelClass*)window;
		SetFocusToOutlineWindow(outlineViewPanel);
		BuildOutlineCurrentCodeControl();
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

void mvceditor::OutlineViewFeatureClass::OnResourceFinderComplete(mvceditor::ResourceFinderCompleteEventClass& event) {
	wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_OUTLINE, GetOutlineNotebook());
	if (window != NULL) {
		OutlineViewPanelClass* outlineViewPanel = (OutlineViewPanelClass*)window;
		outlineViewPanel->AddFileToOutline(event.Resources, event.FullPath);
	}
}

void mvceditor::OutlineViewFeatureClass::OnFileSaved(mvceditor::FileSavedEventClass& event) {
	wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_OUTLINE, GetOutlineNotebook());

	// if the outline tab is showing, lets update the contents (by notifying the background thread to reparse the file)
	if (window != NULL && ResourceFinderBackgroundThread) {
		ResourceFinderBackgroundThread->Start(event.GetCodeControl()->GetFileName(), GetEnvironment()->Php.Version);
	}
}

mvceditor::OutlineViewPanelClass::OutlineViewPanelClass(wxWindow* parent, int windowId, OutlineViewFeatureClass* feature, 
		NotebookClass* notebook)
	: OutlineViewGeneratedPanelClass(parent, windowId)
	, ImageList(16, 16)
	, Feature(feature)
	, Notebook(notebook) {
	HelpButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_HELP, 
		wxART_TOOLBAR, wxSize(16, 16))));
	AddButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("outline-add")));
	SetStatus(_(""));

	ImageList.Add(mvceditor::IconImageAsset(wxT("outline")));
	ImageList.Add(mvceditor::IconImageAsset(wxT("document-php")));
	ImageList.Add(mvceditor::IconImageAsset(wxT("class")));
	ImageList.Add(mvceditor::IconImageAsset(wxT("method-public")));
	ImageList.Add(mvceditor::IconImageAsset(wxT("method-protected")));
	ImageList.Add(mvceditor::IconImageAsset(wxT("method-private")));
	ImageList.Add(mvceditor::IconImageAsset(wxT("method-inherited")));
	ImageList.Add(mvceditor::IconImageAsset(wxT("property-public")));
	ImageList.Add(mvceditor::IconImageAsset(wxT("property-protected")));
	ImageList.Add(mvceditor::IconImageAsset(wxT("property-private")));
	ImageList.Add(mvceditor::IconImageAsset(wxT("property-inherited")));

	ImageList.Add(mvceditor::IconImageAsset(wxT("define")));
	ImageList.Add(mvceditor::IconImageAsset(wxT("class-constant")));
	ImageList.Add(mvceditor::IconImageAsset(wxT("namespace")));
	ImageList.Add(mvceditor::IconImageAsset(wxT("function")));

	// this class will own the ImageList
	Tree->SetImageList(&ImageList);
}

void mvceditor::OutlineViewPanelClass::SetStatus(const wxString& status) {
	StatusLabel->SetLabel(status);
}

void mvceditor::OutlineViewPanelClass::AddFileToOutline(const std::vector<mvceditor::TagClass>& resources, const wxString& fullPath) {
	Tree->Freeze();
	wxTreeItemId rootId = Tree->GetRootItem();
	if (!rootId.IsOk()) {
		rootId = Tree->AddRoot(_("Outline"), IMAGE_OUTLINE_ROOT);
	}
	StatusLabel->SetLabel(_(""));

	// look for the file in the tree
	// files are in the first level
	wxTreeItemId fileId = FindFileNode(fullPath);
	if (fileId.IsOk()) {
		Tree->DeleteChildren(fileId);
	}
	else {
		fileId = Tree->AppendItem(rootId, wxFileName(fullPath).GetFullName(), IMAGE_OUTLINE_FILE, -1, new mvceditor::TreeItemDataStringClass(fullPath)); 
	}

	std::vector<mvceditor::TagClass>::const_iterator tag;
	std::vector<UnicodeString> classes;
	for (tag = resources.begin(); tag != resources.end(); ++tag) {
		
		// display all of the non-class tags first
		if (tag->Type != mvceditor::TagClass::CLASS && 
			tag->Type != mvceditor::TagClass::CLASS_CONSTANT && 
			tag->Type != mvceditor::TagClass::MEMBER &&
			tag->Type != mvceditor::TagClass::METHOD) {
			TagToNode(*tag, fileId);
		}
		else if (tag->Type == mvceditor::TagClass::CLASS) {
			classes.push_back(tag->ClassName);
		}
	}
	std::vector<UnicodeString>::const_iterator className;
	for (className = classes.begin(); className != classes.end(); ++className) {
		wxTreeItemId classId = Tree->AppendItem(fileId, mvceditor::IcuToWx(*className), IMAGE_OUTLINE_CLASS);

		// display all tags for this class
		for (tag = resources.begin(); tag != resources.end(); ++tag) {
			if ((tag->Type == mvceditor::TagClass::MEMBER || 
				tag->Type == mvceditor::TagClass::CLASS_CONSTANT ||
				tag->Type == mvceditor::TagClass::METHOD) &&
				className->caseCompare(tag->ClassName, 0) == 0) {
				TagToNode(*tag, classId);
			}
		}
	}
	Tree->ExpandAll();
	Tree->SelectItem(fileId);
	Tree->Thaw();
}

void mvceditor::OutlineViewPanelClass::TagToNode(const mvceditor::TagClass& tag, wxTreeItemId& treeId) {

	// for now never show dynamic resources since there is no way we can know where the source for them is.
	int type = tag.Type;
	UnicodeString className = mvceditor::WxToIcu(Tree->GetItemText(treeId));
	wxString label = mvceditor::IcuToWx(tag.Identifier);
	if (mvceditor::TagClass::DEFINE == type && !tag.IsDynamic) {
		Tree->AppendItem(treeId, label, IMAGE_OUTLINE_DEFINE);
	}
	else if (mvceditor::TagClass::MEMBER == tag.Type) {
		if (!tag.ReturnType.isEmpty()) {
			wxString returnType = mvceditor::IcuToWx(tag.ReturnType);
			label = label + wxT(" [") + returnType + wxT("]");
		}
		int image = IMAGE_OUTLINE_PROPERTY_PUBLIC;
		if (tag.ClassName != className) {
			image = IMAGE_OUTLINE_PROPERTY_INHERITED;
		}
		else if (tag.IsProtected) {
			image = IMAGE_OUTLINE_PROPERTY_PROTECTED;
		}
		else if (tag.IsPrivate) {
			image = IMAGE_OUTLINE_PROPERTY_PRIVATE;
		}
		Tree->AppendItem(treeId, label, image);
	}
	else if (mvceditor::TagClass::METHOD == tag.Type) {

		// add the function signature to the label
		int32_t sigIndex = tag.Signature.indexOf(UNICODE_STRING_SIMPLE(" function ")); 
		if (sigIndex > 0) {
			UnicodeString sig(tag.Signature, sigIndex + 10);
			label = mvceditor::IcuToWx(sig);
		}
		if (!tag.ReturnType.isEmpty()) {
			wxString returnType = mvceditor::IcuToWx(tag.ReturnType);
			label += wxT(" [") + returnType + wxT("]");
		}
		int image = IMAGE_OUTLINE_METHOD_PUBLIC;
		if (tag.ClassName != className) {
			image = IMAGE_OUTLINE_METHOD_INHERITED;
		}
		else if (tag.IsProtected) {
			image = IMAGE_OUTLINE_METHOD_PROTECTED;
		}
		else if (tag.IsPrivate) {
			image = IMAGE_OUTLINE_METHOD_PRIVATE;
		}
		Tree->AppendItem(treeId, label, image);
	}
	else if (mvceditor::TagClass::CLASS_CONSTANT == tag.Type) {
		Tree->AppendItem(treeId, label, IMAGE_OUTLINE_CLASS_CONSTANT);
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
		Tree->AppendItem(treeId, label, IMAGE_OUTLINE_FUNCTION);
	}
}

void mvceditor::OutlineViewPanelClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = wxString::FromAscii(
		"The outline tab allows you to quickly browse through your project's classes.\n"
		"1. The tree pane lists all of the resources of the file being viewed.\n"
		"2. The drop down shows you all of the classes from all projects. You can "
		"choose a class and the properties / methods for that class will be shown in "
		"the tree pane.\n\n"
		"The 'Sync With Editor' button will 'reset' the outline view with the outline "
		"of the file that is currently being viewed."
		"\n"
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Outline Help"), wxOK, this);
}

void mvceditor::OutlineViewPanelClass::OnAddButton(wxCommandEvent& event) {
	std::vector<mvceditor::TagClass> tags;
	mvceditor::FileSearchDialogClass dialog(this->GetParent(), *Feature, tags);
	if (dialog.ShowModal() == wxOK) {
		AddTagsToOutline(tags);
	}
}

void mvceditor::OutlineViewPanelClass::OnSyncButton(wxCommandEvent& event) {
	Feature->BuildOutlineCurrentCodeControl();
}

void mvceditor::OutlineViewPanelClass::OnTreeItemActivated(wxTreeEvent& event) {

	// the method name is the leaf node, the class name is the parent of the activated node
	wxTreeItemId item = event.GetItem();
	wxString methodSig = Tree->GetItemText(item);
	wxTreeItemId parentItem = Tree->GetItemParent(item);
	if (!item.IsOk() || Tree->GetChildrenCount(item) > 0) {

		// dont want to handle a non-leaf (a class)
		event.Skip();
		return;
	}
	wxString tag;
	if (Tree->GetItemImage(item) == IMAGE_OUTLINE_PROPERTY_PRIVATE || 
		Tree->GetItemImage(item) == IMAGE_OUTLINE_PROPERTY_PROTECTED ||
		Tree->GetItemImage(item) == IMAGE_OUTLINE_PROPERTY_PUBLIC ||
		Tree->GetItemImage(item) == IMAGE_OUTLINE_PROPERTY_INHERITED ||
		Tree->GetItemImage(item) == IMAGE_OUTLINE_METHOD_PUBLIC ||
		Tree->GetItemImage(item) == IMAGE_OUTLINE_METHOD_PROTECTED ||
		Tree->GetItemImage(item) == IMAGE_OUTLINE_METHOD_PRIVATE || 
		Tree->GetItemImage(item) == IMAGE_OUTLINE_METHOD_INHERITED) {
		wxString classNameSig = Tree->GetItemText(parentItem);
		tag = classNameSig + wxT("::");
		
		// extract just the name from the label (function call args or property type)
		int index = methodSig.Index(wxT('('));
		if (wxNOT_FOUND == index) {
			index = methodSig.Index(wxT('['));
		}
		if (wxNOT_FOUND != index) {
			tag += methodSig.Mid(0, index);
		}
		else {

			// sig is not of a function, and prop does not have a type
			tag += methodSig;
		}
	}
	else if (Tree->GetItemImage(item) == IMAGE_OUTLINE_DEFINE || Tree->GetItemImage(item) == IMAGE_OUTLINE_FUNCTION) {
		
		// extract just the name from the label (omit the return type)
		int index = methodSig.Index(wxT('('));
		if (wxNOT_FOUND == index) {
			index = methodSig.Index(wxT('['));
		}
		if (wxNOT_FOUND != index) {
			tag += methodSig.Mid(0, index);
		}
		else {
			tag += methodSig;
		}
	}
	if (!tag.IsEmpty()) {
		Feature->JumpToResource(tag);
	}
	else {
		event.Skip();
	}
}

void mvceditor::OutlineViewPanelClass::AddTagsToOutline(const std::vector<mvceditor::TagClass>& tags) {
	wxTreeItemId root = Tree->GetRootItem();
	if (!root.IsOk()) {
		root = Tree->AddRoot(_("Outline"), IMAGE_OUTLINE_ROOT);
	}
	Tree->Freeze();

	// each tag could be a file or a class tag. 
	//if its a class tag, get all of members for the class
	std::vector<mvceditor::TagClass>::const_iterator chosenTag;
	for (chosenTag = tags.begin(); chosenTag != tags.end(); ++chosenTag) {
		if (chosenTag->Identifier.indexOf(UNICODE_STRING_SIMPLE(".")) >= 0) {
			
			// user chose a file: get all classes / functions for that file
			std::vector<mvceditor::TagClass> fileTags = 
				Feature->App.Globals.TagCache.CollectAllTagsInFile(chosenTag->FullPath);
			AddFileToOutline(fileTags, chosenTag->FullPath);
		}
		else {

			// user chose a class; add the class memeber to the outline
			wxTreeItemId classRoot = Tree->AppendItem(root, mvceditor::IcuToWx(chosenTag->Identifier), IMAGE_OUTLINE_CLASS);
			AddClassToOutline(chosenTag->Identifier, classRoot);
		}
	}
	Tree->ExpandAllChildren(Tree->GetRootItem());
	Tree->Thaw();
}

void mvceditor::OutlineViewPanelClass::AddClassToOutline(const UnicodeString& className, wxTreeItemId& classRoot) {	
	std::vector<mvceditor::TagClass> outlineTags;
	outlineTags = Feature->App.Globals.TagCache.CollectAllMemberTags(className);
	std::vector<mvceditor::TagClass>::iterator tag;
	for (tag = outlineTags.begin(); tag != outlineTags.end(); ++tag) {
		TagToNode(*tag, classRoot);
	}
}

void mvceditor::OutlineViewPanelClass::OnTreeItemRightClick(wxTreeEvent& event) {
	
	// show the delete menu only on the first level items
	wxTreeItemId itemId = event.GetItem();
	wxTreeItemId rootId = Tree->GetRootItem();
	if (itemId.IsOk() && rootId == Tree->GetItemParent(itemId)) {
		wxMenu menu;
		menu.Append(ID_OUTLINE_MENU_DELETE, _("Delete"), _("Delete the item from the tree"));
		wxPoint pos = event.GetPoint();
		Tree->PopupMenu(&menu, pos);
	}
	event.Skip();
}

void mvceditor::OutlineViewPanelClass::OnTreeMenuDelete(wxCommandEvent& event) {
	wxTreeItemId itemId = Tree->GetSelection();
	if (itemId.IsOk()) {
		Tree->Delete(itemId);
	}
}

mvceditor::FileSearchDialogClass::FileSearchDialogClass(wxWindow *parent, mvceditor::OutlineViewFeatureClass& feature, std::vector<mvceditor::TagClass>& chosenTags)
	: FileSearchDialogGeneratedClass(parent)
	, Feature(feature)
	, MatchingTags()
	, ChosenTags(chosenTags) {
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
		MatchingTags = Feature.App.Globals.TagCache.CollectFullyQualifiedClassOrFileFromAll(mvceditor::WxToIcu(search));
	}
	else {
		MatchingTags = Feature.App.Globals.TagCache.CollectNearMatchClassesOrFilesFromAll(mvceditor::WxToIcu(search));
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
		Tree->Delete(fileId);
	}
}

wxTreeItemId mvceditor::OutlineViewPanelClass::FindFileNode(const wxString& fullPath) {
	wxTreeItemId treeIndex, fileId;
	wxTreeItemId rootId = Tree->GetRootItem();
	if (rootId.IsOk()) {
		wxTreeItemIdValue treeCookie;
		treeIndex = Tree->GetFirstChild(rootId, treeCookie);
		bool found = false;
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
	
}

BEGIN_EVENT_TABLE(mvceditor::OutlineViewFeatureClass, wxEvtHandler)
	EVT_MENU(mvceditor::MENU_OUTLINE, mvceditor::OutlineViewFeatureClass::OnOutlineMenu)
	EVT_AUINOTEBOOK_PAGE_CHANGED(mvceditor::ID_CODE_NOTEBOOK, mvceditor::OutlineViewFeatureClass::OnContentNotebookPageChanged)
	EVT_RESOURCE_FINDER_COMPLETE(ID_RESOURCE_FINDER_BACKGROUND, mvceditor::OutlineViewFeatureClass::OnResourceFinderComplete)
	EVT_FEATURE_FILE_SAVED(mvceditor::OutlineViewFeatureClass::OnFileSaved)
	EVT_AUINOTEBOOK_PAGE_CLOSE(mvceditor::ID_CODE_NOTEBOOK, 
		mvceditor::OutlineViewFeatureClass::OnContentNotebookPageClosed)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::OutlineViewPanelClass, OutlineViewGeneratedPanelClass)
	EVT_MENU(ID_OUTLINE_MENU_DELETE, mvceditor::OutlineViewPanelClass::OnTreeMenuDelete)
END_EVENT_TABLE()
