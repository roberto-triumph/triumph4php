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


t4p::NotebookLayoutViewClass::NotebookLayoutViewClass(t4p::NotebookLayoutFeatureClass& feature)
: FeatureViewClass()
, Feature(feature) {

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

	viewMenu->AppendSubMenu(subMenu, _("Notebook Panes"), _("Create Notebooks"));
}

void t4p::NotebookLayoutViewClass::OnNotebookMenu(wxCommandEvent& event) {
	t4p::NotebookClass* currentNotebook = NULL;
	t4p::CodeControlClass* codeCtrl = NULL;
	wxSize newNotebookSize(400, 400);
	int row = 0;
	int position = 0;
	int layer = 0;
	int insertLevel = wxAUI_INSERT_PANE;
	int direction = wxAUI_DOCK_CENTER;
	if (GetCurrentCodeControlWithNotebook(&codeCtrl, &currentNotebook)) {
		newNotebookSize = currentNotebook->GetSize();

		wxAuiPaneInfo currentNotebookInfo = AuiManager->GetPane(currentNotebook);
		row = currentNotebookInfo.dock_row;
		position = currentNotebookInfo.dock_pos;
		layer = currentNotebookInfo.dock_layer;
		direction =currentNotebookInfo.dock_direction;
		if (event.GetId() == (t4p::MENU_NOTEBOOK_PANE + 0)) {

			// splitting horizontally
			// if we are splitting the center notebook, then
			// we split horizontally by putting the new notebook
			// in a new dock position (bottom)
			if (currentNotebookInfo.dock_direction == wxAUI_DOCK_CENTER) {
				direction = wxAUI_DOCK_BOTTOM;
			}
			else {
				position++;
			}
			newNotebookSize.Scale(1, 0.5);
		}
		else if (event.GetId() == (t4p::MENU_NOTEBOOK_PANE + 1)) {

			// splitting vertically
			// if we are splitting the center notebook, then
			// we split vertically by putting the new notebook
			// in a new dock position (right)
			if (currentNotebookInfo.dock_direction == wxAUI_DOCK_CENTER) {
				direction = wxAUI_DOCK_RIGHT;
			}
			else {
				row++;
				insertLevel = wxAUI_INSERT_ROW;
			}
			newNotebookSize.Scale(0.5, 1);
		}
	}

	t4p::NotebookClass* newNotebook = NewNotebook();
	newNotebook->SetSize(newNotebookSize);
	newNotebook->AddTriumphPage(t4p::FILE_TYPE_PHP);
	AuiManager->InsertPane(newNotebook,
		wxAuiPaneInfo()
			.Layer(layer).Row(row).Position(position)
			.Direction(direction)
			.Gripper(false).Resizable(true).Floatable(false)
			.Resizable(true).PaneBorder(false).CaptionVisible(false)
			.CloseButton(false),
		insertLevel
	);

	AuiManager->Update();
}

void t4p::NotebookLayoutViewClass::OnNotebookCreateColumns(wxCommandEvent& event) {
	int columnCount = 1;
	if (event.GetId() == (t4p::MENU_NOTEBOOK_PANE + 4)) {
		columnCount = 2;
	}
	if (event.GetId() == (t4p::MENU_NOTEBOOK_PANE + 5)) {
		columnCount = 3;
	}

	t4p::NotebookClass* currentNotebook = NULL;
	t4p::CodeControlClass* codeCtrl = NULL;
	wxSize newNotebookSize(400, 400);
	if (GetCurrentCodeControlWithNotebook(&codeCtrl, &currentNotebook)) {
		newNotebookSize = currentNotebook->GetSize();
		wxAuiPaneInfo currentNotebookInfo = AuiManager->GetPane(currentNotebook);
		newNotebookSize.Scale(1.0 / columnCount, 1);
	}

	for (int i = 1; i < columnCount; i++) {
		t4p::NotebookClass* newNotebook = NewNotebook();
		newNotebook->SetSize(newNotebookSize);
		newNotebook->AddTriumphPage(t4p::FILE_TYPE_PHP);
		wxAuiPaneInfo info;
		info.Right().Row(i - 1).Position(0)
			.Gripper(false).Resizable(true).Floatable(false)
			.Resizable(true).PaneBorder(false).CaptionVisible(false)
			.CloseButton(false);
		AuiManager->AddPane(newNotebook, info);
	}
	AuiManager->Update();
}

void t4p::NotebookLayoutViewClass::OnNotebookCreateGrid(wxCommandEvent& event) {
}

void t4p::NotebookLayoutViewClass::OnNotebookCreateRows(wxCommandEvent& event) {
	int rowCount = 1;
	if (event.GetId() == (t4p::MENU_NOTEBOOK_PANE + 2)) {
		rowCount = 2;
	}
	if (event.GetId() == (t4p::MENU_NOTEBOOK_PANE + 3)) {
		rowCount = 3;
	}

	t4p::NotebookClass* currentNotebook = NULL;
	t4p::CodeControlClass* codeCtrl = NULL;
	wxSize newNotebookSize(400, 400);
	if (GetCurrentCodeControlWithNotebook(&codeCtrl, &currentNotebook)) {
		newNotebookSize = currentNotebook->GetSize();
		wxAuiPaneInfo currentNotebookInfo = AuiManager->GetPane(currentNotebook);
		newNotebookSize.Scale(1, 1.0 / rowCount);
	}

	for (int i = 1; i < rowCount; i++) {
		t4p::NotebookClass* newNotebook = NewNotebook();
		newNotebook->SetSize(newNotebookSize);
		newNotebook->AddTriumphPage(t4p::FILE_TYPE_PHP);
		wxAuiPaneInfo info;
		info.Bottom().Row(i - 1).Position(0)
			.Gripper(false).Resizable(true).Floatable(false)
			.Resizable(true).PaneBorder(false).CaptionVisible(false)
			.CloseButton(false);
		AuiManager->AddPane(newNotebook, info);
	}
	AuiManager->Update();
}

void t4p::NotebookLayoutViewClass::OnNotebookReset(wxCommandEvent& event) {
	std::vector<t4p::NotebookClass*> notebooks = t4p::CodeNotebooks(GetMainWindow());
	if (notebooks.size() == 1) {
		return;
	}
	t4p::NotebookClass* firstNotebook = notebooks[0];

	for (size_t i = 1; i < notebooks.size(); i++) {
		t4p::NotebookClass* notebook = notebooks[i];
		AuiManager->DetachPane(notebook);

		// move the code controls for this notebook to the
		// first notebook
		while (notebook->GetPageCount() > 0) {
			t4p::CodeControlClass* codeCtrl = notebook->GetCodeControl(0);
			firstNotebook->Adopt(codeCtrl, notebook);
		}
		notebook->Destroy();
	}
	AuiManager->Update();
}

t4p::NotebookClass* t4p::NotebookLayoutViewClass::NewNotebook() {
		t4p::NotebookClass* newNotebook = new t4p::NotebookClass(
		GetMainWindow(),
		wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_SCROLL_BUTTONS |
		wxAUI_NB_TAB_MOVE | wxAUI_NB_WINDOWLIST_BUTTON,

		// very important that the name be given, this is how we find the
		// notebooks when iterating through the frame's children
		wxT("NotebookClass")
	);
	newNotebook->InitApp(
		&Feature.App.Preferences.CodeControlOptions,
		&Feature.App.Preferences,
		&Feature.App.Globals,
		&Feature.App.EventSink
	);
	return newNotebook;
}

BEGIN_EVENT_TABLE(t4p::NotebookLayoutViewClass, t4p::FeatureViewClass)
	EVT_MENU_RANGE(t4p::MENU_NOTEBOOK_PANE + 0, t4p::MENU_NOTEBOOK_PANE + 1, t4p::NotebookLayoutViewClass::OnNotebookMenu)
	EVT_MENU_RANGE(t4p::MENU_NOTEBOOK_PANE + 2, t4p::MENU_NOTEBOOK_PANE + 3, t4p::NotebookLayoutViewClass::OnNotebookCreateRows)
	EVT_MENU_RANGE(t4p::MENU_NOTEBOOK_PANE + 4, t4p::MENU_NOTEBOOK_PANE + 5, t4p::NotebookLayoutViewClass::OnNotebookCreateColumns)

	EVT_MENU(t4p::MENU_NOTEBOOK_PANE + 6, t4p::NotebookLayoutViewClass::OnNotebookReset)
END_EVENT_TABLE()

