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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <widgets/FilePickerValidatorClass.h>
#include <wx/filepicker.h>
#include <wx/msgdlg.h>

t4p::FilePickerValidatorClass::FilePickerValidatorClass(wxFileName* data)
: wxValidator() 
, Data(data) {

}

bool t4p::FilePickerValidatorClass::TransferToWindow() {
	bool ret = false;
	wxFilePickerCtrl* ctrl = wxDynamicCast(GetWindow(), wxFilePickerCtrl);
	if (ctrl) {
		ctrl->SetPath(Data->GetFullPath());
		ret = true;
	}
	return ret;
}

bool t4p::FilePickerValidatorClass::TransferFromWindow() {
	bool ret = false;
	wxFilePickerCtrl* ctrl = wxDynamicCast(GetWindow(), wxFilePickerCtrl);
	if (ctrl) {
		Data->Assign(ctrl->GetPath());
		ret = true;
	}
	return ret;
}

bool t4p::FilePickerValidatorClass::Validate(wxWindow* parent) {
	bool ret = false;
	wxFilePickerCtrl* ctrl = wxDynamicCast(GetWindow(), wxFilePickerCtrl);
	if (ctrl) {
		ret = !ctrl->GetPath().IsEmpty();
		if (!ret) {
			wxMessageBox(ctrl->GetName() + _(" must have a value"), _("Error"));
		}
	}
	return ret;
}

wxObject* t4p::FilePickerValidatorClass::Clone() const {
	t4p::FilePickerValidatorClass* other = new t4p::FilePickerValidatorClass(Data);
	return other;
}