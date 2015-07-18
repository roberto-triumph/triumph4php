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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <widgets/ListWidget.h>

void t4p::ListCtrlAdd(wxListCtrl* list, const wxString& column1Value, const wxString& column2Value) {
	int newRowNumber = list->GetItemCount();

	// list ctrl is tricky, for columns we must insertItem() then setItem() for the next columns
	wxListItem column1;
	column1.SetColumn(0);
	column1.SetId(newRowNumber);
	column1.SetText(column1Value);
	list->InsertItem(column1);

	wxListItem column2;
	column2.SetId(newRowNumber);
	column2.SetColumn(1);
	column2.SetText(column2Value);
	list->SetItem(column2);
}

void t4p::ListCtrlEdit(wxListCtrl* list, const wxString& column1Value, const wxString& column2Value, int rowIndex) {
	wxListItem column1;
	column1.SetColumn(0);
	column1.SetId(rowIndex);
	column1.SetText(column1Value);
	list->SetItem(column1);

	wxListItem column2;
	column2.SetId(rowIndex);
	column2.SetColumn(1);
	column2.SetText(column2Value);
	list->SetItem(column2);
}

void t4p::ListCtrlGet(wxListCtrl* list, wxString& column1Value, wxString& column2Value, int rowIndex) {

	// need to set the mask flag; otherwise in MSW the text will not be set
	// this assumes the given list is set to LC_REPORT mode
	wxListItem column1,
		column2;
	column1.SetColumn(0);
	column1.SetId(rowIndex);
	column1.m_mask = wxLIST_MASK_TEXT;
	column2.SetColumn(1);
	column2.SetId(rowIndex);
	column2.m_mask = wxLIST_MASK_TEXT;

	if (list->GetItem(column1) && list->GetItem(column2)) {
		column1Value = column1.GetText();
		column2Value = column2.GetText();
	}
}

int t4p::ListCtrlSelected(wxListCtrl* ctrl) {
	int selected = ctrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selected >= 0 && selected < ctrl->GetItemCount()) {
		return selected;
	}
	return wxNOT_FOUND;
}
