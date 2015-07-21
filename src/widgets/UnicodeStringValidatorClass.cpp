/**
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#include "widgets/UnicodeStringValidatorClass.h"
#include <wx/combobox.h>
#include <wx/msgdlg.h>
#include <wx/textctrl.h>
#include "globals/String.h"

t4p::UnicodeStringValidatorClass::UnicodeStringValidatorClass(UnicodeString* data, bool doAllowEmpty)
	: wxValidator()
	, Data(data)
	, DoAllowEmpty(doAllowEmpty) {
}

wxObject* t4p::UnicodeStringValidatorClass::Clone() const {
	t4p::UnicodeStringValidatorClass* other = new UnicodeStringValidatorClass(Data, DoAllowEmpty);
	return other;
}

bool t4p::UnicodeStringValidatorClass::TransferFromWindow() {
	bool ret = false;
	wxTextCtrl* t = wxDynamicCast(GetWindow(), wxTextCtrl);
	wxComboBox* combo = wxDynamicCast(GetWindow(), wxComboBox);
	if (t) {
		wxString val = t->GetValue();
		*Data = t4p::WxToIcu(val);
		ret = true;
	} else if (combo) {
		wxString val = combo->GetValue();
		*Data = t4p::WxToIcu(val);
		ret = true;
	}
	return ret;
}

bool t4p::UnicodeStringValidatorClass::TransferToWindow() {
	bool ret = false;
	wxTextCtrl* t = wxDynamicCast(GetWindow(), wxTextCtrl);
	wxComboBox* combo = wxDynamicCast(GetWindow(), wxComboBox);
	if (t) {
		wxString val = t4p::IcuToWx(*Data);
		t->SetValue(val);
		ret = true;
	}
	if (combo) {
		wxString val = t4p::IcuToWx(*Data);
		combo->SetValue(val);
		ret = true;
	}
	return ret;
}

bool t4p::UnicodeStringValidatorClass::Validate(wxWindow* parent) {
	wxTextCtrl* t = wxDynamicCast(GetWindow(), wxTextCtrl);
	wxComboBox* combo = wxDynamicCast(GetWindow(), wxComboBox);
	wxString val;
	if (t) {
		val = t->GetValue();
	} else if (combo) {
		val = combo->GetValue();
	}

	if (val.IsEmpty() && !DoAllowEmpty) {
		wxString msg = GetWindow()->GetName();
		msg += _(" cannot be empty");
		wxMessageBox(msg, _("Error"), wxOK | wxCENTRE, parent);
		return false;
	}
	return true;
}
