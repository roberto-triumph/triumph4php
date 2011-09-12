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
#include <unicode/regex.h>
#include <wx/artprov.h>
#include <vector>
#include <algorithm>


int ID_MENU_OUTLINE_CURRENT = mvceditor::PluginClass::newMenuId();
int ID_CONTEXT_MENU_SHOW_OUTLINE_OTHER = mvceditor::PluginClass::newMenuId();
int ID_WINDOW_OUTLINE = wxNewId();
int ID_CONTEXT_MENU_SHOW_OUTLINE_CURRENT = wxNewId();
int ID_WINDOW_OUTLINE_CURRENT = wxNewId();
int ID_WINDOW_OUTLINE_COMMENT = wxNewId();
int ID_MENU_OUTLINE_CURRENT_JUMP_TO = wxNewId();
int ID_MENU_OUTLINE_CURRENT_JUMP_TO_COMMENT = wxNewId();

mvceditor::OutlineViewPluginClass::OutlineViewPluginClass()
	: PluginClass()
	, CurrentOutline()
	, PhpDoc()
	, Parser() 
	, CurrentOutlineLines()
	, Connected(false) {
	Parser.SetClassObserver(this);
	Parser.SetClassMemberObserver(this);
	Parser.SetFunctionObserver(this);
}

mvceditor::OutlineViewPluginClass::~OutlineViewPluginClass() {
	NotebookClass* notebook = GetNotebook();
	if (notebook != NULL && Connected) {
		notebook->Disconnect(notebook->GetId(), wxID_ANY, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED,
			wxAuiNotebookEventHandler(OutlineViewPluginClass::OnPageChanged), NULL, this);
		notebook->Disconnect(notebook->GetId(), wxID_ANY, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, 
			wxAuiNotebookEventHandler(OutlineViewPluginClass::OnPageChanged), NULL, this);
	}
}

void mvceditor::OutlineViewPluginClass::AddToolsMenuItems(wxMenu* toolsMenu) {
	toolsMenu->Append(ID_MENU_OUTLINE_CURRENT, _("Outline Current File"),  _("Opens an outline view of the currently viewed file"), wxITEM_NORMAL);
}

void mvceditor::OutlineViewPluginClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {
	menu->Append(ID_CONTEXT_MENU_SHOW_OUTLINE_CURRENT, _("Outline Current File"),  _("Opens an outline view of the currently viewed file"), wxITEM_NORMAL);
	menu->Append(ID_CONTEXT_MENU_SHOW_OUTLINE_OTHER, _("Show In Outline"),  _("Search for the selected resource and opens an outline view"), wxITEM_NORMAL);
}

void mvceditor::OutlineViewPluginClass::BuildOutlineCurrentCodeControl() {
	CodeControlClass* code = GetCurrentCodeControl();
	CurrentOutlineLines.clear();
	if (code != NULL) {
		UnicodeString source = code->GetSafeText();
		Parser.ScanString(source);
	}
	CurrentOutline = HumanFriendlyOutline();
}

void mvceditor::OutlineViewPluginClass::BuildOutline(const wxString& line) {
	mvceditor::ResourceFinderClass* resourceFinder = NULL;
	if (GetProject()) {
		resourceFinder = GetProject()->GetResourceFinder();
	}
	if (resourceFinder != NULL && resourceFinder->Prepare(line)) {
		if (resourceFinder->CollectNearMatchResources()) {
			CurrentOutline.remove();
			for(size_t i = 0; i < resourceFinder->GetResourceMatchCount(); ++i) {
				mvceditor::ResourceClass resource = resourceFinder->GetResourceMatch(i);
				
				// take the resource
				CurrentOutline.append(resource.Resource);
				
				// take the parameters from the signature
				int32_t pos = resource.Signature.indexOf(UNICODE_STRING_SIMPLE("("));
				if (pos >= 0) {
					UnicodeString params(resource.Signature, pos);
					CurrentOutline.append(params);
				}
				
				// TODO: nationalize the words 'method' and 'property'
				if (resource.Type == mvceditor::ResourceClass::METHOD) {
					CurrentOutline.append(UNICODE_STRING_SIMPLE(" [Method]"));
				}
				else if (resource.Type == mvceditor::ResourceClass::MEMBER) {
					CurrentOutline.append(UNICODE_STRING_SIMPLE(" [Property]"));
				}
				CurrentOutline.append(UNICODE_STRING_SIMPLE("\n"));
			}
		}
	}
}
	
void mvceditor::OutlineViewPluginClass::BuildPhpDoc(const wxString& line) {
	mvceditor::ResourceFinderClass* resourceFinder = NULL;
	if (GetProject()) {
		resourceFinder = GetProject()->GetResourceFinder();
	}
	if (resourceFinder != NULL && resourceFinder->Prepare(line)) {
		if (resourceFinder->CollectFullyQualifiedResource()) {
			PhpDoc.remove();
			mvceditor::ResourceClass resource = resourceFinder->GetResourceMatch(0);
			
			// take the parameters from the signature			
			PhpDoc.append(resource.Resource);
			int32_t pos = resource.Signature.indexOf(UNICODE_STRING_SIMPLE("("));
			if (pos >= 0) {
				UnicodeString params(resource.Signature, pos);
				PhpDoc.append(params);
			}
			PhpDoc.append(UNICODE_STRING_SIMPLE("\n\n"));
			UnicodeString comment(resource.Comment);
			
			// the ending comment is not taken out by the regex below
			comment.findAndReplace(UNICODE_STRING_SIMPLE("*/"), UNICODE_STRING_SIMPLE(""));
			// make the comment pretty by re-formatting and taking out the comment *s
			UErrorCode error = U_ZERO_ERROR;
			RegexPattern* pattern = RegexPattern::compile(UNICODE_STRING_SIMPLE("(\n|^)\\s*(\\*|/\\*\\*|\\*/)\\s?"), UREGEX_MULTILINE, error);
			if (U_SUCCESS(error)) {
				RegexMatcher* matcher = pattern->matcher(comment, error);
				if (U_SUCCESS(error)) {
					UnicodeString prettyComment = matcher->replaceAll(UNICODE_STRING_SIMPLE("\n"), error);
					if (U_SUCCESS(error)) {
						PhpDoc.append(prettyComment);
					}
					delete matcher;
				}
				delete pattern;
			}
		}
		// else the index is out of date....
	}
}

void mvceditor::OutlineViewPluginClass::JumpToResource(const wxString& resource) {
	mvceditor::ResourceFinderClass* resourceFinder = NULL;
	if (GetProject()) {
		resourceFinder = GetProject()->GetResourceFinder();
	}
	if (resourceFinder != NULL && resourceFinder->Prepare(resource)) {
		if (resourceFinder->CollectFullyQualifiedResource()) {
			GetNotebook()->LoadPage(resourceFinder->GetResourceMatchFullPath(0));
			CodeControlClass* codeControl = GetCurrentCodeControl();
			if (codeControl) {
				int32_t position, 
					length;
				bool found = resourceFinder->GetResourceMatchPosition(0, codeControl->GetSafeText(), position, length);
				if (found) {
					codeControl->SetSelectionAndEnsureVisible(position, position + length);
				}
				// else the index is out of date....
			}
		}
	}	
}

UnicodeString mvceditor::OutlineViewPluginClass::HumanFriendlyOutline() {
	sort(CurrentOutlineLines.begin(), CurrentOutlineLines.end());
	UnicodeString text;
	for (std::vector<UnicodeString>::const_iterator it = CurrentOutlineLines.begin(); it != CurrentOutlineLines.end(); ++it) {
		text.append(*it);
		text.append(UNICODE_STRING_SIMPLE("\n"));
	}
	return text;
}

void mvceditor::OutlineViewPluginClass::OnContextMenuOutline(wxCommandEvent& event) {
	CodeControlClass* code = GetCurrentCodeControl();
	bool modified = false;
	if (event.GetId() == ID_CONTEXT_MENU_SHOW_OUTLINE_CURRENT || event.GetId() == ID_MENU_OUTLINE_CURRENT) {
		BuildOutlineCurrentCodeControl();
		modified = true;
	}
	else if (event.GetId() == ID_CONTEXT_MENU_SHOW_OUTLINE_OTHER && code && !code->GetSelectedText().IsEmpty()) { 
		// adding the :: makes the resource finder match on all of the class's methods 
		BuildOutline(code->GetSelectedText() + wxT("::")); 
		modified = true; 
	} 
	if (modified) {
		
		// create / open the outline window
		wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_OUTLINE, GetToolsParentWindow());
		OutlineViewPluginPanelClass* outlineViewPanel = NULL;
		if (window != NULL) {
			outlineViewPanel = (OutlineViewPluginPanelClass*)window;
			SetFocusToToolsWindow(outlineViewPanel);
			outlineViewPanel->RefreshOutlines();
		}
		else {
			mvceditor::NotebookClass* notebook = GetNotebook();
			if (notebook != NULL) {
				outlineViewPanel = new OutlineViewPluginPanelClass(GetToolsParentWindow(), ID_WINDOW_OUTLINE, this, notebook);
				if (AddToolsWindow(outlineViewPanel, wxT("Outline"))) {
					Connected = true;
					notebook->Connect(notebook->GetId(), wxID_ANY, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED,
						wxAuiNotebookEventHandler(OutlineViewPluginClass::OnPageChanged), NULL, this);
					notebook->Connect(notebook->GetId(), wxID_ANY, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, 
						wxAuiNotebookEventHandler(OutlineViewPluginClass::OnPageChanged), NULL, this);
					outlineViewPanel->RefreshOutlines();
				}
			}
		}	
	}
	else {
		event.Skip();
	}
}

void mvceditor::OutlineViewPluginClass::OnPageChanged(wxAuiNotebookEvent& event) {
	wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_OUTLINE, GetToolsParentWindow());

	// only change the outline if the user is looking at the outline.  otherwise, it gets 
	// annoying if the user is looking at run output, switches PHP files, and the outline
	// gets changed.
	if (window != NULL && IsToolsWindowSelected(ID_WINDOW_OUTLINE)) {
		OutlineViewPluginPanelClass* outlineViewPanel = NULL;
		outlineViewPanel = (OutlineViewPluginPanelClass*)window;
		SetFocusToToolsWindow(outlineViewPanel);
		BuildOutlineCurrentCodeControl();
		outlineViewPanel->RefreshOutlines();
	}
	event.Skip();
}

void mvceditor::OutlineViewPluginClass::ClassFound(const UnicodeString& className, const UnicodeString& signature, 
		const UnicodeString& comment) {
	CurrentOutlineLines.push_back(signature);
}
	
void mvceditor::OutlineViewPluginClass::DefineDeclarationFound(const UnicodeString& variableName, const UnicodeString& variableValue, 
		const UnicodeString& comment) {
	UnicodeString line;
	line.append(variableName).append(UNICODE_STRING_SIMPLE(" = ")).append(variableValue);
	CurrentOutlineLines.push_back(line);
}
	
void mvceditor::OutlineViewPluginClass::MethodFound(const UnicodeString& className, const UnicodeString& methodName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment) {
	UnicodeString line;
	
	// TODO: nationalize the word "method"
	line.append(className).append(UNICODE_STRING_SIMPLE("::")).append(signature).append(UNICODE_STRING_SIMPLE(" [method]"));
	CurrentOutlineLines.push_back(line);
}
		
void mvceditor::OutlineViewPluginClass::PropertyFound(const UnicodeString& className, const UnicodeString& propertyName, 
		const UnicodeString& propertyType, const UnicodeString& comment, bool isConst) {
	UnicodeString line;
	
	// TODO: nationalize the word "property"
	line.append(className).append(UNICODE_STRING_SIMPLE("::")).append(propertyName).append(UNICODE_STRING_SIMPLE(" [Property] "));
	if (!propertyType.isEmpty()) {
		line.append(propertyType);
	}
	CurrentOutlineLines.push_back(line);
}
		
void mvceditor::OutlineViewPluginClass::FunctionFound(const UnicodeString& functionName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment) {
	CurrentOutlineLines.push_back(signature);
}

mvceditor::OutlineViewPluginPanelClass::OutlineViewPluginPanelClass(wxWindow* parent, int windowId, OutlineViewPluginClass* plugin, 
		NotebookClass* notebook)
	: OutlineViewPluginGeneratedPanelClass(parent, windowId)
	, Outline1(NULL)
	, Outline3(NULL)
	, Plugin(plugin)
	, Notebook(notebook) {
	HelpButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_HELP, 
		wxART_TOOLBAR, wxSize(16, 16))));
	
	Outline1 = new mvceditor::CodeControlClass(this, *notebook->CodeControlOptions, plugin->GetProject(), ID_WINDOW_OUTLINE_CURRENT);
	Outline1->SetReadOnly(true);
	OutlineSizer->Add(Outline1, 2, wxALL|wxEXPAND, 5);
	
	Outline3 = new mvceditor::CodeControlClass(this, *notebook->CodeControlOptions, plugin->GetProject(), ID_WINDOW_OUTLINE_COMMENT);
	Outline3->SetWrapMode(wxSTC_WRAP_WORD);
	Outline3->SetReadOnly(true);
	OutlineSizer->Add(Outline3, 0, wxALL|wxEXPAND, 5);
	
	this->Layout();

	// listen to the context menu events directly.
	// need this for proper handling on Win32
	// not sure how to do it in a better way
	Outline1->Connect(Outline1->GetId(), wxID_ANY, wxEVT_CONTEXT_MENU,
		wxContextMenuEventHandler(OutlineViewPluginPanelClass::OnContextMenu), NULL, this);
}

void mvceditor::OutlineViewPluginPanelClass::OnClose(wxCloseEvent& event) {
	Outline1->Disconnect(Outline1->GetId(), wxID_ANY, wxEVT_CONTEXT_MENU,
		wxContextMenuEventHandler(OutlineViewPluginPanelClass::OnContextMenu), NULL, this);
}

void mvceditor::OutlineViewPluginPanelClass::RefreshOutlines() {
	wxString outline = mvceditor::StringHelperClass::IcuToWx(Plugin->CurrentOutline);
	
	// prevent the caret from moving if the content is going to be the same
	if (outline != Outline1->GetText()) {
		Outline1->SetReadOnly(false);
		Outline1->SetText(outline);
		Outline1->SetReadOnly(true);
	}
	outline = mvceditor::StringHelperClass::IcuToWx(Plugin->PhpDoc);
	if (outline != Outline3->GetText()) {
		Outline3->SetReadOnly(false);
		Outline3->SetText(outline);
		Outline3->SetReadOnly(true);
	}
}

void mvceditor::OutlineViewPluginPanelClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = wxString::FromAscii(
		"The outline tab allows you to 	quickly browse through your project's resources and see the PHPDoc "
		"comments attached to those resources.  The outline window consists of 2 panes:\n"
		"1. The leftmost pane lists all of the resources of the file being viewed.\n"
		"   Changing the contents of the middle outline is done with the Lookup button "
		"   or the 'Show In Outline' context menu.\n"
		"2. The rightmost pane shows the PHPDoc comment of any resource.  This is triggered with the 'Show PHPDoc' context menu\n\n"
		"The 'Sync Outline' button will 'reset' the outline view with the outline of the file that is currently being viewed."
		"\n"
		""
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Help"));
}

void mvceditor::OutlineViewPluginPanelClass::OnLookupButton(wxCommandEvent& event) {
	wxString lookup = Lookup->GetValue();
	if (!lookup.IsEmpty()) {
		
		// make the resource finder match on all methods / properties
		lookup += wxT("::");
		Plugin->BuildOutline(lookup);
		RefreshOutlines();
	}
}

void mvceditor::OutlineViewPluginPanelClass::OnSyncButton(wxCommandEvent& event) {
	Plugin->BuildOutlineCurrentCodeControl();
	RefreshOutlines();
}

void mvceditor::OutlineViewPluginPanelClass::OnContextMenu(wxContextMenuEvent& event) {
	wxMenu menu;
	
	// i have no idea how we can know which code control the popup menu is in.
	// for now I am assigning each code control window its own menu ID and differentiating that way
	if (event.GetEventObject() == Outline1) {
		menu.Append(ID_MENU_OUTLINE_CURRENT_JUMP_TO, _("Jump To Resource"),  _("Opens this resource in the editor"), wxITEM_NORMAL);
		menu.Append(ID_MENU_OUTLINE_CURRENT_JUMP_TO_COMMENT, _("Show Comment"),  _("Display the resource comment"), wxITEM_NORMAL);
		PopupMenu(&menu);
	}
	else {
		event.Skip();
	}
}

void mvceditor::OutlineViewPluginPanelClass::OnOutlineJumpTo(wxCommandEvent& event) {
	wxString resource = ResourceFromOutline(event);
	if (!resource.IsEmpty()) {
		Plugin->JumpToResource(resource);
	}
	else {
		event.Skip();
	}
}

void mvceditor::OutlineViewPluginPanelClass::OnOutlineJumpToComment(wxCommandEvent& event) {
	wxString resource = ResourceFromOutline(event);
	if (!resource.IsEmpty()) {
		Plugin->BuildPhpDoc(resource);
		RefreshOutlines();
	}
	else {
		event.Skip();
	}
}

wxString mvceditor::OutlineViewPluginPanelClass::ResourceFromOutline(wxCommandEvent& event) {
	wxString line;
	// i have no idea how we can know which code control the popup menu is in.
	// for now I am assigning each code control window its own menu ID and differentiating that way
	// I tried event.GetEventObject() == Outline1 and that did not work
	int caretPos = 0;
	if (event.GetId() == ID_MENU_OUTLINE_CURRENT_JUMP_TO || event.GetId() == ID_MENU_OUTLINE_CURRENT_JUMP_TO_COMMENT) {
		line = Outline1->GetCurLine(&caretPos);
	}
	
	if (!line.IsEmpty()) {
		
		// from the line, only the part of the text that will make a resource match (ie. the function name, or class::method)
		// the ' ' and  '[' come from the way the resource is made into human friendly from (in OutlineViewPluginClass)
		// the '(' comes from the resource signature (in the case of methods)
		int pos = line.find_first_of(wxT(" [("));
		if (wxNOT_FOUND != pos) {
			line = line.Mid(0, pos);
		}
		
		// remove any newlines
		line.Trim(false).Trim(true);
	}
	return line;
}

BEGIN_EVENT_TABLE(mvceditor::OutlineViewPluginClass, wxEvtHandler)
	EVT_MENU(ID_MENU_OUTLINE_CURRENT, mvceditor::OutlineViewPluginClass::OnContextMenuOutline)
	EVT_MENU(ID_CONTEXT_MENU_SHOW_OUTLINE_CURRENT, mvceditor::OutlineViewPluginClass::OnContextMenuOutline)
	EVT_MENU(ID_CONTEXT_MENU_SHOW_OUTLINE_OTHER, mvceditor::OutlineViewPluginClass::OnContextMenuOutline)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::OutlineViewPluginPanelClass, OutlineViewPluginGeneratedPanelClass)
	EVT_CLOSE(mvceditor::OutlineViewPluginPanelClass::OnClose)
	EVT_MENU(ID_MENU_OUTLINE_CURRENT_JUMP_TO, mvceditor::OutlineViewPluginPanelClass::OnOutlineJumpTo)
	EVT_MENU(ID_MENU_OUTLINE_CURRENT_JUMP_TO_COMMENT, mvceditor::OutlineViewPluginPanelClass::OnOutlineJumpToComment)
END_EVENT_TABLE()