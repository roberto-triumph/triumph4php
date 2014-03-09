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
#ifndef __T4P_EDITORFEATURECLASS_H__
#define __T4P_EDITORFEATURECLASS_H__

#include <features/FeatureClass.h>
#include <features/wxformbuilder/EditorBehaviorFeatureForms.h>
#include <globals/CodeControlOptionsClass.h>

namespace t4p {
	
/**
 * The EditorFeatureClass exposes scintilla features like 
 * virtual space, multiple selection, and zoom
 */
class EditorBehaviorFeatureClass : public t4p::FeatureClass {

public:

	EditorBehaviorFeatureClass(t4p::AppClass& app);
	
	/**
	 * Handler to save the editor feature preferences.
	 */
	void OnPreferencesSaved(wxCommandEvent& event);

	void AddToolBarItems(wxAuiToolBar* toolBar);
	
	void AddEditMenuItems(wxMenu* editMenu);
	
	void AddViewMenuItems(wxMenu* viewMenu);
	
	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
	void AddPreferenceWindow(wxBookCtrlBase* parent);

private:

	void SetFeaturesOnNotebook();
	void SetFeatures(const t4p::CodeControlOptionsClass& options, wxStyledTextCtrl* codeCtrl);
	
	void OnAppFileOpened(t4p::CodeControlEventClass& event);
	void OnAppFileNew(t4p::CodeControlEventClass& event);
	
	void OnToggleRectangularSelection(wxCommandEvent& event);
	void OnToggleWordWrap(wxCommandEvent& event);
	void OnToggleIndentationGuides(wxCommandEvent& event);
	void OnToggleWhitespace(wxCommandEvent& event);
	void OnZoomIn(wxCommandEvent& event);
	void OnZoomOut(wxCommandEvent& event);
	void OnZoomReset(wxCommandEvent& event);
	void OnEditConvertEols(wxCommandEvent& event);
	
	DECLARE_EVENT_TABLE()
};

/**
 * panel that shows all options regarding editor behavior
 */
class EditorBehaviorPanelClass : public EditorBehaviorPanelGeneratedClass {

public:
	
	EditorBehaviorPanelClass(wxWindow* parent, CodeControlOptionsClass& options);
	bool TransferDataToWindow();
	bool TransferDataFromWindow();

protected:

	// Handlers for EditorBehaviorPanelGeneratedClass events.
	void OnIndentUsingSpaces(wxCommandEvent& event);
	void OnCheckRightMargin(wxCommandEvent& event);

};

}

#endif // __T4P_EDITORFEATURECLASS_H__
