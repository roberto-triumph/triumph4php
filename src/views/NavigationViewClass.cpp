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
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <views/NavigationViewClass.h>
#include <features/FeatureClass.h>
#include <widgets/NotebookClass.h>
#include <widgets/AuiManager.h>
#include <widgets/ListWidget.h>
#include <widgets/FileTypeImageList.h>
#include <globals/Number.h>

t4p::NavigationViewClass::NavigationViewClass() 
: FeatureViewClass() {
}

void t4p::NavigationViewClass::AddEditMenuItems(wxMenu* menu) {
	menu->Append(t4p::MENU_NAVIGATION + 0, _("Select Tab\tCTRL+E"), _("Select Tab"), wxITEM_NORMAL);
}

void t4p::NavigationViewClass::AddKeyboardShortcuts(std::vector<t4p::DynamicCmdClass>& cmds) {
	std::map<int, wxString> shortcuts;
	shortcuts[t4p::MENU_NAVIGATION + 0] = "Edit - Select Tab";
	AddDynamicCmd(shortcuts, cmds);
}


void t4p::NavigationViewClass::OnEditNavigatePane(wxCommandEvent& event) {
	std::vector<t4p::CodeControlClass*>codeCtrls = AllCodeControls();
	wxAuiNotebook* tools = GetToolsNotebook();
	wxAuiNotebook* outline = GetOutlineNotebook();
	if (!codeCtrls.empty() ||( tools && tools->GetPageCount() > 0) 
			|| (outline && outline->GetPageCount() > 0)) {
		t4p::NavigationChoiceClass choice;
		t4p::NavigationViewDialogClass dialog(GetMainWindow(), 
			*this, *AuiManager,
			t4p::AuiVisibleCodeNotebooks(*AuiManager), outline,
			tools, choice
		);
		if (dialog.ShowModal() == wxOK && choice.Notebook) {
			if (choice.PageIndex != wxNOT_FOUND &&
					t4p::NumberLessThan(choice.PageIndex, choice.Notebook->GetPageCount())) {
				choice.Notebook->SetSelection(choice.PageIndex);
				wxWindow* window = choice.Notebook->GetPage(choice.PageIndex);
				if (window && (choice.Notebook == tools || choice.Notebook == outline)) {
					window->SetFocus();
				}
			}
		}
	}
}

t4p::NavigationChoiceClass::NavigationChoiceClass()
: Notebook(NULL)
, PageIndex(wxNOT_FOUND) {
	
}

t4p::NavigationViewDialogClass::NavigationViewDialogClass(wxWindow* parent, 
	t4p::NavigationViewClass& view,
	wxAuiManager& auiManager,
	std::vector<t4p::NotebookClass*> codeNotebooks,
	wxAuiNotebook* outlineNotebook, 
	wxAuiNotebook* toolsNotebook,
	t4p::NavigationChoiceClass& choice)
: NavigationViewDialogGeneratedClass(parent)
, CodeNotebooks(codeNotebooks)
, OutlineNotebook(outlineNotebook)
, ToolsNotebook(toolsNotebook) 
, Choice(choice) {
	wxImageList* fileImageList = new wxImageList(16, 16);
	t4p::FileTypeImageList(*fileImageList);
	FilesList->AssignImageList(fileImageList, wxIMAGE_LIST_SMALL);
	
	wxArrayString files;
	int newRowNumber = 0;
	bool doShowNotebookName =  CodeNotebooks.size() > 1;
	for (size_t i = 0; i < CodeNotebooks.size(); i++) {
		for (size_t p = 0; p < CodeNotebooks[i]->GetPageCount(); p++) {
			wxString value = CodeNotebooks[i]->GetPageText(p);
			if (doShowNotebookName) {
				value += " (" + auiManager.GetPane(CodeNotebooks[i]).name + ")";
			}
			wxListItem column1;
			column1.SetColumn(0);
			column1.SetId(newRowNumber);
			column1.SetText(value);
			column1.SetImage(t4p::FileTypeImageIdFromType(
				CodeNotebooks[i]->GetCodeControl(p)->GetFileType()
			));
			FilesList->InsertItem(column1);
			
			newRowNumber++;
		}
	}
	
	newRowNumber = 0;
	for (size_t i = 0; i < OutlineNotebook->GetPageCount(); i++) {
		wxString value = OutlineNotebook->GetPageText(i);
			wxListItem column1;
			column1.SetColumn(0);
			column1.SetId(newRowNumber);
			column1.SetText(value);
			PanelsList->InsertItem(column1);
			
			newRowNumber++;
	}
	for (size_t i = 0; i < ToolsNotebook->GetPageCount(); i++) {
		wxString value = ToolsNotebook->GetPageText(i);
			wxListItem column1;
			column1.SetColumn(0);
			column1.SetId(newRowNumber);
			column1.SetText(value);
			PanelsList->InsertItem(column1);
			
			newRowNumber++;
	}
	
	if (PanelsList->GetItemCount() == 0) {
		BodySizer->Remove(PanelsSizer);
		BodySizer->SetCols(1);
	}
	if (FilesList->GetItemCount() == 0) {
		BodySizer->Remove(FilesSizer);
		BodySizer->SetCols(1);
	}
	
	if (FilesList->GetItemCount() > 0) {
		FilesList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		FilesList->SetFocus();
	}
	else if (PanelsList->GetItemCount() > 0) {
		PanelsList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		PanelsList->SetFocus();
	}
	
	BodySizer->Layout();
}


void t4p::NavigationViewDialogClass::OnFilesListKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_ESCAPE) {
		EndModal(wxID_CANCEL);
		return;
	}
	if (event.GetKeyCode() == WXK_LEFT || event.GetKeyCode() == WXK_RIGHT) {
		int selected = t4p::ListCtrlSelected(FilesList);
		if (selected != wxNOT_FOUND && selected < PanelsList->GetItemCount()) {
			FilesList->SetItemState(selected, 0, wxLIST_STATE_SELECTED);
			PanelsList->SetItemState(selected, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
			PanelsList->SetFocus();
		}
		else if (PanelsList->GetItemCount() > 0) {
			FilesList->SetItemState(0, 0, wxLIST_STATE_SELECTED);
			PanelsList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
			PanelsList->SetFocus();
		}
		return;
	}
	event.Skip();
}

void t4p::NavigationViewDialogClass::OnPanelsListKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_ESCAPE) {
		EndModal(wxID_CANCEL);
		return;
	}
	if (event.GetKeyCode() == WXK_LEFT || event.GetKeyCode() == WXK_RIGHT) {
		int selected = t4p::ListCtrlSelected(PanelsList);
		if (selected != wxNOT_FOUND && selected < FilesList->GetItemCount()) {
			PanelsList->SetItemState(selected, 0, wxLIST_STATE_SELECTED);
			FilesList->SetItemState(selected, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
			FilesList->SetFocus();
		}
		else if (FilesList->GetItemCount() > 0) {
			PanelsList->SetItemState(0, 0, wxLIST_STATE_SELECTED);
			FilesList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
			FilesList->SetFocus();
		}
		return;
	}
	event.Skip();
}

void t4p::NavigationViewDialogClass::OnFileItemActivated(wxListEvent& event) {
	int selected = event.GetIndex();
	for (size_t i = 0; i < CodeNotebooks.size() && !Choice.Notebook; i++) {
		for (size_t p = 0; p < CodeNotebooks[i]->GetPageCount()  && !Choice.Notebook; p++) {
			if (selected == (i + p)) {
				Choice.Notebook = CodeNotebooks[i];
				Choice.PageIndex = p;
				break;
			}
		}
	}
	EndModal(wxOK);
}

void t4p::NavigationViewDialogClass::OnPanelItemActivated(wxListEvent& event) {
	int selected = event.GetIndex();
	int outlineMin = 0;
	int outlineMax = OutlineNotebook->GetPageCount();
	int toolsMin = OutlineNotebook->GetPageCount();
	int toolsMax = OutlineNotebook->GetPageCount() + ToolsNotebook->GetPageCount();
	if (selected >= outlineMin && selected < outlineMax) {
		for (size_t i = 0; i < OutlineNotebook->GetPageCount(); i++) {
			if (((int)selected) == i) {
				Choice.Notebook = OutlineNotebook;
				Choice.PageIndex = i;
				break;
			}
		}
	}
	if (selected >= toolsMin && selected < toolsMax) {
		for (size_t i = 0; i < ToolsNotebook->GetPageCount(); i++) {
			if (selected == (i + OutlineNotebook->GetPageCount())) {
				Choice.Notebook = ToolsNotebook;
				Choice.PageIndex = i;
				break;
			}
		}
	}
	EndModal(wxOK);
}

BEGIN_EVENT_TABLE(t4p::NavigationViewClass, t4p::FeatureViewClass)
	EVT_MENU(t4p::MENU_NAVIGATION + 0, t4p::NavigationViewClass::OnEditNavigatePane)
END_EVENT_TABLE()
