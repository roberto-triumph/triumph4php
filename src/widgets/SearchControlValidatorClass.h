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
#ifndef SRC_WIDGETS_SEARCHCONTROLVALIDATORCLASS_H_
#define SRC_WIDGETS_SEARCHCONTROLVALIDATORCLASS_H_

#include <wx/srchctrl.h>
#include <wx/validate.h>

/**
 * Class that can be used to transfer wxString data from wxSearchCtrl
 * It seems that the wxGenericValidator or wxTextCtrl do not work for wxSearchCtrl
 */
class SearchControlValidatorClass : public wxValidator {
 public:
    SearchControlValidatorClass(wxString* data);

    virtual bool TransferToWindow();

    virtual bool TransferFromWindow();

    virtual bool Validate(wxWindow* parent);

    virtual wxObject* Clone() const;

 private:
    SearchControlValidatorClass();

    /**
     * The data
     */
    wxString* Data;
};

#endif  // SRC_WIDGETS_SEARCHCONTROLVALIDATORCLASS_H_
