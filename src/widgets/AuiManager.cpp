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
#include <widgets/AuiManager.h>
#include <widgets/NotebookClass.h>

const int t4p::AUI_MAX_CODE_NOTEBOOKS = 5;

void t4p::AuiAddCodeNotebook(wxAuiManager& auiManager, t4p::NotebookClass* notebook, int notebookNumber) {
	
	// very important that the name be given, this is how we find the
	// notebooks when iterating through the frame's children
	notebook->SetName("NotebookClass");
	wxAuiPaneInfo info;
	info.Name(wxString::Format("Notebook %d", notebookNumber))
		.Caption(wxString::Format("Notebook %d", notebookNumber))
		.CaptionVisible(false)
		.Center()
		.PaneBorder(true).Gripper(false).Floatable(false)
		.Resizable(true).CloseButton(false)
		.Row(notebookNumber - 1);
	if (notebookNumber > 1) {
		info.Hide().Right().CaptionVisible(false);
	}
	auiManager.AddPane(notebook, info);	
}

std::vector<t4p::NotebookClass*> t4p::AuiVisibleCodeNotebooks(wxAuiManager& auiManager) {
	std::vector<t4p::NotebookClass*> allNotebooks = t4p::AuiAllCodeNotebooks(auiManager);
	std::vector<t4p::NotebookClass*> notebooks;
	for (size_t i = 0; i < allNotebooks.size(); i++) {
		wxAuiPaneInfo info = auiManager.GetPane(allNotebooks[i]);
		if (info.IsShown()) {
			notebooks.push_back(allNotebooks[i]);
		}
	}
	return notebooks;
}

std::vector<t4p::NotebookClass*> t4p::AuiAllCodeNotebooks(wxAuiManager& auiManager) {
	std::vector<t4p::NotebookClass*> notebooks;
	for (int i = 1; i <= t4p::AUI_MAX_CODE_NOTEBOOKS; i++) {
		wxAuiPaneInfo& info = auiManager.GetPane(wxString::Format("Notebook %d", i));
		if (info.IsOk() && info.window && info.window->GetName() == wxT("NotebookClass")) {
			notebooks.push_back((t4p::NotebookClass*)info.window);
		}
	}
	return notebooks;
}

void t4p::AuiResetCodeNotebooks(wxAuiManager& auiManager) {
	std::vector<t4p::NotebookClass*> notebooks = t4p::AuiAllCodeNotebooks(auiManager);
	if (notebooks.size() == 1) {
		return;
	}
	t4p::NotebookClass* centerNotebook = NULL;
	for (size_t i = 0; i < notebooks.size(); i++) {
		t4p::NotebookClass* notebook = notebooks[i];
		wxAuiPaneInfo& info = auiManager.GetPane(notebook);
		if (info.dock_direction == wxAUI_DOCK_CENTER) {
			centerNotebook = notebooks[i];
		}
	}
	wxASSERT_MSG(centerNotebook, "there must be a center notebook");
	if (centerNotebook) {
		for (size_t i = 0; i < notebooks.size(); i++) {
			t4p::NotebookClass* notebook = notebooks[i];
			wxAuiPaneInfo& info = auiManager.GetPane(notebook);
			if (info.dock_direction != wxAUI_DOCK_CENTER) {
				
				// move the code controls for this notebook to the
				// first notebook
				info.Hide();
				info.Row(0);
			}
		}
	}
	
	// in case the caption was visible due to > 2 notebooks
	wxAuiPaneInfo& info = auiManager.GetPane(centerNotebook);
	info.CaptionVisible(false);
}

t4p::NotebookClass* t4p::AuiNextHiddenCodeNotebook(wxAuiManager& auiManager) {
	std::vector<t4p::NotebookClass*> notebooks = t4p::AuiAllCodeNotebooks(auiManager);
	t4p::NotebookClass* ret = NULL;
	for (size_t i = 0; i < notebooks.size(); i++) {
		t4p::NotebookClass* notebook = notebooks[i];
		wxAuiPaneInfo info = auiManager.GetPane(notebook);
		if (!info.IsShown()) {
			ret = notebook;
			break;
		}
	}
	return ret;
}

int t4p::AuiRowCount(wxAuiManager& auiManager, int dock) {
	int highestRow = 0;
	std::vector<t4p::NotebookClass*> notebooks = t4p::AuiVisibleCodeNotebooks(auiManager);
	for (size_t i = 0; i < notebooks.size(); i++) {
		wxAuiPaneInfo info = auiManager.GetPane(notebooks[i]);
		if (info.dock_direction == dock && info.dock_row > highestRow) {
			highestRow = info.dock_row;
		}
	}
	return highestRow;
}