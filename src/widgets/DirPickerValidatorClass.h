/**
 * @copyright  2013 Roberto Perpuly
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
#ifndef SRC_WIDGETS_DIRPICKERVALIDATORCLASS_H_
#define SRC_WIDGETS_DIRPICKERVALIDATORCLASS_H_

#include <wx/filename.h>
#include <wx/validate.h>

namespace t4p {
/**
 * This class is used to transfer a value from a dir picker control. Currently this validator
 * can handle either wxDirPickerCtrl only.
 */
class DirPickerValidatorClass : public wxValidator {
	public:
	/**
	 * @param The string to validate / transfer. This class will not own this pointer.
	 */
	DirPickerValidatorClass(wxFileName* data);

	virtual bool TransferToWindow();

	virtual bool TransferFromWindow();

	virtual bool Validate(wxWindow* parent);

	virtual wxObject* Clone() const;

	private:
	/**
	 * The object to validate. This class will not own this pointer.
	 */
	wxFileName* Data;
};
}  // namespace t4p

#endif  // SRC_WIDGETS_DIRPICKERVALIDATORCLASS_H_
