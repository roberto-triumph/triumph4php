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
#include <widgets/ComboBoxHistoryClass.h>


t4p::ComboBoxHistoryClass::ComboBoxHistoryClass(wxComboBox* combo) 
	: wxEvtHandler()
	, Items()
	, Combo(combo) {
	if (NULL != Combo) {
		Combo->Connect(Combo->GetId(), wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(ComboBoxHistoryClass::OnComboSelected), NULL, this);
		Combo->Connect(Combo->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ComboBoxHistoryClass::OnComboSelected), NULL, this);
	}
}

t4p::ComboBoxHistoryClass::~ComboBoxHistoryClass() {
	if (NULL != Combo) {
		Combo->Disconnect(Combo->GetId(), wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(ComboBoxHistoryClass::OnComboSelected), NULL, this);
		Combo->Disconnect(Combo->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ComboBoxHistoryClass::OnComboSelected), NULL, this);
	}
}

void t4p::ComboBoxHistoryClass::Attach(wxComboBox* combo) {
	Combo = combo;
	Combo->Connect(Combo->GetId(), wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(ComboBoxHistoryClass::OnComboSelected), NULL, this);
	Combo->Connect(Combo->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ComboBoxHistoryClass::OnComboSelected), NULL, this);
	
	// prepopulate fromm previous instance
	Combo->Append(Items);
}

void t4p::ComboBoxHistoryClass::Detach() {
	Combo->Disconnect(Combo->GetId(), wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(ComboBoxHistoryClass::OnComboSelected), NULL, this);
	Combo->Disconnect(Combo->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ComboBoxHistoryClass::OnComboSelected), NULL, this);
	
	// save for next time
	Items = Combo->GetStrings();
	Combo = NULL;
}

void t4p::ComboBoxHistoryClass::Save() {
	int MAX_ITEMS = 18;
	wxString newValue = Combo->GetValue();
	int index = Combo->FindString(newValue);
	if (wxNOT_FOUND == index && !newValue.IsEmpty()) {
		Combo->Insert(newValue, 0);
	}
	int count = Combo->GetCount();
	if (count > MAX_ITEMS) {
		Combo->Delete(count - 1);
	}
}

void t4p::ComboBoxHistoryClass::OnComboSelected(wxCommandEvent& event) {
	if (NULL != Combo) {
		Save();
	}
	event.Skip();
}