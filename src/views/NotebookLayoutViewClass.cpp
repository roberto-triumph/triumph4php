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
	subMenu->Append(t4p::MENU_NOTEBOOK_PANE + 0, _("Split Horizontally"), _("Create a new code notebook"), wxITEM_NORMAL);
	subMenu->Append(t4p::MENU_NOTEBOOK_PANE + 1, _("Split Vertically"), _("Create a new code notebook"), wxITEM_NORMAL);

	viewMenu->AppendSubMenu(subMenu, _("Notebook Panes"), _("Create Notebooks"));
}

void t4p::NotebookLayoutViewClass::OnNotebookMenu(wxCommandEvent& event) {
	t4p::NotebookClass* currentNotebook = NULL;
	t4p::CodeControlClass* codeCtrl = NULL;
	int currentNotebookWidth = 400;
	int row = 0;
	int position = 0;
	int layer = 0;
	int insertLevel = wxAUI_INSERT_PANE;
	if (GetCurrentCodeControlWithNotebook(&codeCtrl, &currentNotebook)) {
		currentNotebookWidth = currentNotebook->GetSize().GetWidth();
		wxAuiPaneInfo currentNotebookInfo = AuiManager->GetPane(currentNotebook);
		row = currentNotebookInfo.dock_row;
		position = currentNotebookInfo.dock_pos;
		layer = currentNotebookInfo.dock_layer;
		if (event.GetId() == (t4p::MENU_NOTEBOOK_PANE + 0)) {
			position++;
		}
		else if (event.GetId() == (t4p::MENU_NOTEBOOK_PANE + 1)) {
			row++;
			insertLevel = wxAUI_INSERT_ROW;
		}
	}

	t4p::NotebookClass* newNotebook = new t4p::NotebookClass(
		GetMainWindow(),
		wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_SCROLL_BUTTONS |
		wxAUI_NB_TAB_MOVE | wxAUI_NB_WINDOWLIST_BUTTON | wxAUI_BUTTON_CLOSE,

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
	newNotebook->AddTriumphPage(t4p::FILE_TYPE_PHP);
	AuiManager->InsertPane(newNotebook,
		wxAuiPaneInfo()
			.Center().Layer(layer).Row(row).Position(position)
			.Gripper(false).Resizable(true).Floatable(false)
			.Resizable(true).PaneBorder(true),
		insertLevel
	);

	AuiManager->Update();
}

BEGIN_EVENT_TABLE(t4p::NotebookLayoutViewClass, t4p::FeatureViewClass)
	EVT_MENU_RANGE(t4p::MENU_NOTEBOOK_PANE + 0, t4p::MENU_NOTEBOOK_PANE + 10, t4p::NotebookLayoutViewClass::OnNotebookMenu)
END_EVENT_TABLE()