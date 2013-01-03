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
#ifndef __MVCEDITOR_CHOOSEURLDIALOGCLASS_H__
#define __MVCEDITOR_CHOOSEURLDIALOGCLASS_H__

#include <features/wxformbuilder/RunBrowserFeatureForms.h>
#include <globals/UrlResourceClass.h>
#include <wx/wx.h>

namespace mvceditor {

/**
 * Small dialog that allows the user to choose a URL to run. Url list
 * is populated from the URL detector cache (UrlResourceFinder).
 */
class ChooseUrlDialogClass : public ChooseUrlDialogGeneratedClass {
	
public:

	ChooseUrlDialogClass(wxWindow* parent, UrlResourceFinderClass& urls, UrlResourceClass& chosenUrl);
	
protected:

	void OnOkButton(wxCommandEvent& event);

	/**
	 * events on the filter textbox, so that the list gets updates
	 * as the user types in text
	 */
	void OnFilterText(wxCommandEvent& event);
	void OnFilterTextEnter(wxCommandEvent& event);
	void OnFilterKeyDown(wxKeyEvent& event);
	
	/**
	 * updates the URL label so that the user can see the exact URL to 
	 * be opened in the browser
	 */
	void OnListItemSelected(wxCommandEvent& event);

	/**
	 * events on the extra textbox, so that the list gets updates
	 * as the user types in text
	 */
	void OnExtraText(wxCommandEvent& event);
	void OnExtraChar(wxKeyEvent& event);

private:

	/**
	 * The list of URLs, it will contain URLs that were detected and URLs that were input
	 * by the user.
	 * This pointer will not be owned by thie class
	 */
	UrlResourceFinderClass& UrlResourceFinder;

	/**
	 * The URL that the user selected.
	 */
	UrlResourceClass& ChosenUrl;
};

}

#endif