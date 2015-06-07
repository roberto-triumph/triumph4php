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
#include <views/NotebookLayoutViewClass.h>
#include <widgets/NotebookClass.h>
#include <Triumph.h>
#include <globals/Assets.h>
#include <globals/Number.h>
#include <widgets/AuiManager.h>
#include <wx/wupdlock.h>

t4p::NotebookLayoutViewClass::NotebookLayoutViewClass(t4p::NotebookLayoutFeatureClass& feature)
: FeatureViewClass()
///, Feature(feature) 
{

}

void t4p::NotebookLayoutViewClass::AddViewMenuItems(wxMenu* viewMenu) {
	wxMenu* subMenu = new wxMenu();
	subMenu->Append(t4p::MENU_NOTEBOOK_PANE + 2, _("2 rows, 1 Column"), _("Create 2 Notebooks, One on top of the other"), wxITEM_NORMAL);
	subMenu->Append(t4p::MENU_NOTEBOOK_PANE + 3, _("3 rows, 1 Column"), _("Create 3 Notebooks, Each on top of the next"), wxITEM_NORMAL);
	subMenu->Append(t4p::MENU_NOTEBOOK_PANE + 4, _("1 row, 2 Columns"), _("Create 2 Notebooks, One to the right of the other"), wxITEM_NORMAL);
	subMenu->Append(t4p::MENU_NOTEBOOK_PANE + 5, _("1 row, 3 Columns"), _("Create 3 Notebooks, Each to the right of the other"), wxITEM_NORMAL);

	subMenu->Append(t4p::MENU_NOTEBOOK_PANE + 6, _("Reset"), _("Removes all extra notebooks"), wxITEM_NORMAL);

	subMenu->Append(t4p::MENU_NOTEBOOK_PANE + 0, _("Split Horizontally"), _("Create a new code notebook"), wxITEM_NORMAL);
	subMenu->Append(t4p::MENU_NOTEBOOK_PANE + 1, _("Split Vertically"), _("Create a new code notebook"), wxITEM_NORMAL);

	viewMenu->AppendSubMenu(subMenu, _("Notebook Layout"), _("Create Multiple Code Notebooks"));
}

void t4p::NotebookLayoutViewClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	toolBar->AddTool(t4p::MENU_NOTEBOOK_PANE + 4,
		_("1 row, 2 Columns"),
		t4p::BitmapImageAsset(wxT("layout-2-equal-vertical")),
		_("Create 2 Notebooks, One to the right of the other"),
		wxITEM_NORMAL
	);
	toolBar->AddTool(t4p::MENU_NOTEBOOK_PANE + 2,
		_("2 Rows, 1 Column"),
		t4p::BitmapImageAsset(wxT("layout-2-equal-horizontal")),
		_("Create 2 Notebooks, One on top of the other"),
		wxITEM_NORMAL
	);
}

void t4p::NotebookLayoutViewClass::OnNotebookMenu(wxCommandEvent& event) {
	t4p::NotebookClass* currentNotebook = NULL;
	t4p::CodeControlClass* codeCtrl = NULL;
	if (GetCurrentCodeControlWithNotebook(&codeCtrl, &currentNotebook)) {
		if (event.GetId() == (t4p::MENU_NOTEBOOK_PANE + 0)) {

			// splitting horizontally
			currentNotebook->SplitHorizontally();
		}
		else if (event.GetId() == (t4p::MENU_NOTEBOOK_PANE + 1)) {

			// splitting vertically
			currentNotebook->SplitVertically();
		}
	}
}

void t4p::NotebookLayoutViewClass::OnNotebookCreateColumns(wxCommandEvent& event) {
	
	//
	// Implementation notes:
	// rows are created by using AUI's docking and layers
	// notebooks are placed in the right dock; each
	// notebook in its own layer so that each notebook
	// takes up an entire column.
	//
	wxWindowUpdateLocker locker(GetMainWindow());
	t4p::AuiResetCodeNotebooks(*AuiManager);

	int columnCount = 1;
	if (event.GetId() == (t4p::MENU_NOTEBOOK_PANE + 4)) {
		columnCount = 2;
	}
	if (event.GetId() == (t4p::MENU_NOTEBOOK_PANE + 5)) {
		columnCount = 3;
	}

	t4p::NotebookClass* currentNotebook = NULL;
	t4p::NotebookClass* firstNotebook = NULL;
	t4p::CodeControlClass* codeCtrl = NULL;
	std::vector<t4p::NotebookClass*> notebooks = t4p::AuiAllCodeNotebooks(*AuiManager);
	if (!notebooks.empty()) {
		firstNotebook = notebooks[0];
	}
	wxSize newNotebookSize(400, 400);
	if (GetCurrentCodeControlWithNotebook(&codeCtrl, &currentNotebook)) {
		newNotebookSize = currentNotebook->GetSize();
		newNotebookSize.Scale(1.0 / columnCount, 1);
		currentNotebook->SetSize(newNotebookSize);
	}

	for (int i = 1; i < columnCount && t4p::NumberLessThan(i, notebooks.size()); i++) {
		t4p::NotebookClass* notebook = notebooks[i];
		notebook->SetSize(newNotebookSize);
		if (notebook->GetPageCount() == 0) {
			notebook->AddTriumphPage(t4p::FILE_TYPE_PHP);
		}
		wxAuiPaneInfo& info = AuiManager->GetPane(notebook);
		info.Right().Row(i - 1).Position(0).Layer(0)
			.Gripper(false).Resizable(true).Floatable(false)
			.Resizable(true).PaneBorder(false).CaptionVisible(false)
			.CloseButton(false).Show().BestSize(newNotebookSize);
		
		// when there are more than 2 notebooks, given them names so
		// that the user can tell them apart (for "moving tabs" purposes)
		if (columnCount > 2) {
			info.CaptionVisible(true).CloseButton(false);
		}
	}
	if (firstNotebook && columnCount > 2) {
		wxAuiPaneInfo& currentNotebookInfo = AuiManager->GetPane(firstNotebook);
		currentNotebookInfo.CaptionVisible(true).CloseButton(false);
	}
	AuiManager->Update();
}

void t4p::NotebookLayoutViewClass::OnNotebookCreateGrid(wxCommandEvent& event) {
}

void t4p::NotebookLayoutViewClass::OnNotebookCreateRows(wxCommandEvent& event) {
	
	//
	// Implementation notes:
	// rows are created by using AUI's docking and layers
	// notebooks are placed in the bottom dock; each
	// notebook in its own layer so that each notebook
	// takes up an entire row.
	//
	wxWindowUpdateLocker locker(GetMainWindow());
	t4p::AuiResetCodeNotebooks(*AuiManager);

	int rowCount = 1;
	if (event.GetId() == (t4p::MENU_NOTEBOOK_PANE + 2)) {
		rowCount = 2;
	}
	if (event.GetId() == (t4p::MENU_NOTEBOOK_PANE + 3)) {
		rowCount = 3;
	}

	t4p::NotebookClass* currentNotebook = NULL;
	t4p::NotebookClass* firstNotebook = NULL;
	t4p::CodeControlClass* codeCtrl = NULL;
	std::vector<t4p::NotebookClass*> notebooks = t4p::AuiAllCodeNotebooks(*AuiManager);
	if (!notebooks.empty()) {
		firstNotebook = notebooks[0];
	}
	wxSize newNotebookSize(400, 400);
	if (GetCurrentCodeControlWithNotebook(&codeCtrl, &currentNotebook)) {
		newNotebookSize = currentNotebook->GetSize();
		newNotebookSize.Scale(1, 1.0 / rowCount);
		currentNotebook->SetSize(newNotebookSize);
	}

	for (int i = 1; i < rowCount && t4p::NumberLessThan(i, notebooks.size()); i++) {
		t4p::NotebookClass* notebook = notebooks[i];
		notebook->SetSize(newNotebookSize);
		if (notebook->GetPageCount() == 0) {
			notebook->AddTriumphPage(t4p::FILE_TYPE_PHP);
		}
		wxAuiPaneInfo& info = AuiManager->GetPane(notebook);
		
		// i+2 because we want the tools notebook and the find/replace
		// panels below these notebook splits
		info.Bottom().Row(i + 2).Position(0)
			.Gripper(false).Resizable(true).Floatable(false)
			.Resizable(true).PaneBorder(false).CaptionVisible(false)
			.CloseButton(false).Show().BestSize(newNotebookSize);
		
		// when there are more than 2 notebooks, given them names so
		// that the user can tell them apart (for "moving tabs" purposes)
		if (rowCount > 2) {
			info.CaptionVisible(true).CloseButton(false);
		}
	}
	if (firstNotebook && rowCount > 2) {
		wxAuiPaneInfo& currentNotebookInfo = AuiManager->GetPane(firstNotebook);
		currentNotebookInfo.CaptionVisible(true);
	}
	AuiManager->Update();
}

void t4p::NotebookLayoutViewClass::OnNotebookReset(wxCommandEvent& event) {
	t4p::AuiResetCodeNotebooks(*AuiManager);
	AuiManager->Update();
}

BEGIN_EVENT_TABLE(t4p::NotebookLayoutViewClass, t4p::FeatureViewClass)
	EVT_MENU_RANGE(t4p::MENU_NOTEBOOK_PANE + 0, t4p::MENU_NOTEBOOK_PANE + 1, t4p::NotebookLayoutViewClass::OnNotebookMenu)
	EVT_MENU_RANGE(t4p::MENU_NOTEBOOK_PANE + 2, t4p::MENU_NOTEBOOK_PANE + 3, t4p::NotebookLayoutViewClass::OnNotebookCreateRows)
	EVT_MENU_RANGE(t4p::MENU_NOTEBOOK_PANE + 4, t4p::MENU_NOTEBOOK_PANE + 5, t4p::NotebookLayoutViewClass::OnNotebookCreateColumns)

	EVT_MENU(t4p::MENU_NOTEBOOK_PANE + 6, t4p::NotebookLayoutViewClass::OnNotebookReset)
END_EVENT_TABLE()

