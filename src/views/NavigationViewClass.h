/**
 * @copyright  2015 Roberto Perpuly
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
#ifndef SRC_VIEWS_NAVIGATIONVIEWCLASS_H_
#define SRC_VIEWS_NAVIGATIONVIEWCLASS_H_

#include <wx/aui/auibook.h>
#include <vector>
#include "views/FeatureViewClass.h"
#include "views/wxformbuilder/NavigationViewForms.h"

namespace t4p {
// forward declaration, defined in another file
class NotebookClass;

/**
 * The navigation view shows the user a panel
 * that allows them to change the focus to another
 * code control, or even another outline or tools
 * window.
 * You might wonder why we don't trigger the navigation
 * dialog with the popular shortcut of CTRL+TAB. The reason
 * for this is that I cannot seem to be able to catch
 * the key event of CTRL+TAB; you can see this by running
 * the keyboard sample in wxWidgets on Mac OS X; when using a custom
 * window (not a text box), the window does not seem
 * to catch CTRL+TAB key events.
 */
class NavigationViewClass : public t4p::FeatureViewClass {
	public:
	NavigationViewClass();

	void AddEditMenuItems(wxMenu* menu);

	void AddKeyboardShortcuts(std::vector<t4p::DynamicCmdClass>& commands);

	private:
	/**
	 * menu handler
	 */
	void OnEditNavigatePane(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

/**
 * Represents a selection of the NavigationViewDialog Class.
 * It holds the notebook to change the page of and
 * the index of the page to be selected.
 */
class NavigationChoiceClass {
	public:
	/**
	 * The notebook that the user chose to be focused on
	 * This class does not own this pointer.
	 */
	wxAuiNotebook* Notebook;

	/**
	 * The page of the notebook that should be focused on
	 */
	int PageIndex;

	NavigationChoiceClass();
};

/**
 * Shows the user the list of opened files from all
 * visible code notebooks, plus the list of all other
 * notebooks (lint results, find in files results, outline).
 */
class NavigationViewDialogClass : public NavigationViewDialogGeneratedClass {
	public:
	NavigationViewDialogClass(wxWindow* parent,
		t4p::NavigationViewClass& view,
		wxAuiManager& auiManager,
		std::vector<t4p::NotebookClass*> notebooks,
		wxAuiNotebook* outlineNotebook,
		wxAuiNotebook* toolsNotebook,
		t4p::NavigationChoiceClass& choice);

	protected:
	void OnFilesListKeyDown(wxKeyEvent& event);
	void OnPanelsListKeyDown(wxKeyEvent& event);
	void OnFileItemActivated(wxListEvent& event);
	void OnPanelItemActivated(wxListEvent& event);
	private:
	std::vector<t4p::NotebookClass*> CodeNotebooks;
	wxAuiNotebook* OutlineNotebook;
	wxAuiNotebook* ToolsNotebook;

	t4p::NavigationChoiceClass& Choice;
};
}  // namespace t4p

#endif  // SRC_VIEWS_NAVIGATIONVIEWCLASS_H_
