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
#ifndef __FINDERVALIDATORCLASS_H__
#define __FINDERVALIDATORCLASS_H__

#include <wx/validate.h>
#include <wx/radiobox.h>
#include <search/FinderClass.h>

namespace mvceditor {

/**
 * This class is used to validate that a finder has a correctly formed regular expression
 * if the finder is in regex mode. Currently this validator
 * can handle either wxTextCtrl of wxComboBox only.
 */
class FinderValidatorClass : public wxValidator {

public:
	
	/**
	 * @param The object to validate. This class will not own this pointer.
	 * @param the mode radio button. this needs to be 
	 *        checked so that we skip reg ex validation
	 *        when the expression is not a reg ex
	 */
	FinderValidatorClass(FinderClass* finder, wxRadioBox* modeRadio);

	virtual bool TransferToWindow();
	
	virtual bool TransferFromWindow();
	
	virtual bool Validate(wxWindow* parent);
	
	virtual wxObject* Clone() const;

private:

	FinderValidatorClass();
	
	/**
	 * The mode radio button; this needs to be 
	 * checked so that we skip reg ex validation
	 * when the expression is not a reg ex
	 */
	wxRadioBox* ModeRadio;

	/**
	 * The object to validate. This class will not own this pointer.
	 */
	FinderClass* Finder;
};
}

#endif