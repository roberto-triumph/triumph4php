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
#include <widgets/NonEmptyTextValidatorClass.h>
#include <wx/msgdlg.h>

NonEmptyTextValidatorClass::NonEmptyTextValidatorClass(wxString* data, wxStaticText* label)
	: wxValidator()
	, Data(data)
	, Label(label) {
}

NonEmptyTextValidatorClass::NonEmptyTextValidatorClass()
	: wxValidator()
	, Data(NULL)
	, Label(NULL) {
}

wxObject* NonEmptyTextValidatorClass::Clone() const {
	NonEmptyTextValidatorClass* other = new NonEmptyTextValidatorClass();
	other->Data = Data;
	other->Label = Label;
	return other;
}

bool NonEmptyTextValidatorClass::TransferFromWindow() {
	bool ret = false;
	wxTextCtrl* window = wxDynamicCast(GetWindow(), wxTextCtrl);
	if (window) {
		wxString val = window->GetValue();
		*Data = val;
		ret = true;
	}
	return ret;
}

bool NonEmptyTextValidatorClass::TransferToWindow() {
	bool ret = false;
	wxTextCtrl* window = wxDynamicCast(GetWindow(), wxTextCtrl);
	if (window) {
		wxString val = *Data;
		window->SetValue(val);
		ret = true;
	}
	return ret;
}

bool NonEmptyTextValidatorClass::Validate(wxWindow* parent) {
	bool isEmpty = true;
	wxTextCtrl* window = wxDynamicCast(GetWindow(), wxTextCtrl);
	if (window) {
		wxString value = window->GetValue();
		isEmpty = value.Trim(true).Trim(false).IsEmpty();
	}
	if (isEmpty) {
		wxString msg = Label != NULL ? Label->GetLabel() : wxT("");
		msg += _(" must have a value.");
		wxMessageBox(msg, _("Error"), wxOK, parent);
	}
	return !isEmpty;
}
