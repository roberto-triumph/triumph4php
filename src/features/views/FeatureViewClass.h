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
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef T4P_FEATUREVIEWCLASS_H__
#define T4P_FEATUREVIEWCLASS_H__

#include <widgets/StatusBarWithGaugeClass.h>
#include <widgets/NotebookClass.h>
#include <main_frame/PreferencesClass.h>
#include <wx/event.h>

namespace t4p {

/**
 * A feature view encapsulates the GUI of the application; it 
 * has access to the main frame, the tools, outline, and code notebooks,
 * the status bar gauge, and menus.
 * 
 * 
 * Window locations: Left (skinny window), Bottom (wide window), Main (Center). Features can create 
 * as many windows as they see fit (in any location).
 * 
 * Actions: Feature can define actions, menu items for those actions, 
 *          Also, context menu items for those actions, feature will be given the selected text
 * 
 * Toolbars: Features will be given the ability to define toolbar buttons or other controls, either 
 * left-justified or right-justified.
 * 
 * Feature views will be given:
 * 1) StatusBarWithGaugeClass to show progress to the user
 * 2) The tools window notebook.  features can add their windows
 * 3) The code notebook.  Features can query which is the currently opened file, any 
 *    selected text, etc...
 * 4) Note that unless specified, all pointers that a feature view is given (Notebook, status 
 *    bar, etc.. ) will be taken care of by the application and the feature SHOULD NOT delete 
 *    them.  However, any pointers created by the feature view will need to be deleted
 *    by the feature itself.  wxWindow pointers usually do not need to be deleted 
 *    because the wxWidgets framework manages them.
 *
 * Lifecycle:
 * There is at most 1 instance of any one feature view class, however there
 * may be zero instances in Mac OS X where an application may be running without
 * a main frame.
 * 
 * This class is meant to be inherited by all classes that desire to add
 * dialogs, panels, or menu items to the app. They should contain very little
 * application logic; that should be delegated to other classes.
 */
class FeatureViewClass : public wxEvtHandler {

public:
	
	FeatureViewClass();
	
	/**
	 * Set the windows. All of these pointers will NOT be
	 * owned by this class. The caller will still retain ownership.
	 * 
	 * @param StatusBarWithGaugeClass& statusBarWithGauge the status bar.
	 * @param NotebookClass& notebook the opened source code files
	 * @param wxAuiNotebook& toolsNotebook the parent window for all feature windows
	 * @param wxAuiNotebook& outlineNotebook the parent window for all outline type windows (left side)
	 * @param wxAuiManager auiManager the AUI manager used to update the frame
	 * @param wxMenuBar* menuBar the application menu bar
	 */
	void InitWindow(StatusBarWithGaugeClass* statusBarWithGauge, 
		NotebookClass* notebook, wxAuiNotebook* toolsNotebook, 
		wxAuiNotebook* outlineNotebook, wxAuiManager* auiManager, 
		wxMenuBar* menuBar);
	
	/**
	 * Add menu items to the view menu for this feature. Remeber to use the MenuIds enum when building
	 * menu items.
	 * 
	 * @param wxMenu* menu the view menu to add items to.
	 */
	virtual void AddViewMenuItems(wxMenu* viewMenu);

	/**
	 * Add menu items to the search menu for this feature. Remeber to use the MenuIds enum when building
	 * menu items.
	 * 
	 * @param wxMenu* menu the view menu to add items to.
	 */
	virtual void AddSearchMenuItems(wxMenu* searchMenu);

	/**
	 * Add menu items to the file menu for this feature. Remeber to use the MenuIds enum when building
	 * menu items.
	 * 
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	virtual void AddFileMenuItems(wxMenu* fileMenu);

	/**
	 * Add menu items to the edit menu for this feature. Remeber to use the MenuIds enum when building
	 * menu items.
	 * 
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	virtual void AddEditMenuItems(wxMenu* editMenu);

	/**
	 * Add menu items to the help menu for this feature. Remeber to use the MenuIds enum when building
	 * menu items.
	 * 
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	virtual void AddHelpMenuItems(wxMenu* helpMenu);
	
	/**
	 * Feature may create its own menu. The feature should override this method if it desires to create an entirely new menu.
	 * Remeber to use the MenuIds enum when building
	 * menu items.
	 * @param wxMenuBar* the menu bar to insert the new menu to
	 */
	virtual void AddNewMenu(wxMenuBar* menuBar);

	/**
	 * Adds items to the toolbar.  These items will be left aligned.
	 */
	virtual void AddToolBarItems(wxAuiToolBar* toolBar);
	
	/**
	 * Adds an arbritary window to the application. Use the AuiManager property  (AuiManager.AddPane) to add
	 * items. No need to call AuiManager.Update(), the application will do it.
	 */
	virtual void AddWindows();
	
	/**
	 * Add a tab to the preferences window. This method is invoked only when the user chooses Edit ... Preferences
	 * 
	 * @param wxBookCtrlBase* the parent that will contain all preference dialogs.  Once the feature's window is added, the 
	 * parent will take care of deletion. Note that to add a dialog you will need to call wxBookCtrlBase::AddPage
	 */
	virtual void AddPreferenceWindow(wxBookCtrlBase* parent);
	
	/**
	 * This method will be called every time the user right-clicks on an active code control window. Features may define
	 * special menu items  to be shown to the user when the user right-clicks on a code control. The active code control 
	 * can then be accessed via the NotebookClass.
	 * 
	 * @param wxMenu* menu the context menu to add items to.
	 */
	virtual void AddCodeControlClassContextMenuItems(wxMenu* menu);
	
	/**
	 * Subclasses can override this method to create their own shortcuts that will get serialized /deserialized
	 * properly; also by using this method the shortcuts will get registered properly; plus it will allow the user to
	 * edit the shortcuts via the preferences dialog.
	 * @param shortcuts the list of shortcuts to add to
	 */
	virtual void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);


protected:

	/**
	 * This is a helper method that will add each of the given menu items as a 
	 * shortcut.  The map will contain the menu Item IDs; each of these IDs will
	 * be used to lookup the Menu Item in the Menu Bar, and a DynamicCmd will be
	 * created based on the menu item. The map value (wxString) will be used as 
	 * the DynamicCmd's identifier.
	 * For example, if the map contains
	 *
	 *   menuItems[wxID_OPEN] = "Open-File"
	 *   menuItems[wxID_CLOSE] = "Close-File" 
	 *
	 * Then this method will create 2 DynamicCmds, assuming that the menu bar has
	 * menu items with the IDs wxID_OPEN and wxID_CLOSE.  "Open-File" will be the 
	 * identifier for the first command and "Close-File" will be the identifier for
	 * the second command.
	 * If a menu item is not found, and assertion is triggered.
	 */
	void AddDynamicCmd(std::map<int, wxString> menuItemIds,std::vector<DynamicCmdClass>& shortcuts);
	
	
	/**
	 * The AUI Manager is needed in cases where the different windows are repositioned programatically and the entire AUI
	 * needs to be re-drawn.
	 * 
	 * @var wxAuiManager*
	 */
	wxAuiManager* AuiManager;

private:
	
	/**
	 * The widget that feature uses to display status to the user.
	 * 
	 * @var StatusBarWithGaugeClass*
	 */
	StatusBarWithGaugeClass* StatusBarWithGauge;

	/**
	 * The container for the source code windows.
	 * 
	 * @var NotebookClass*
	 */
	NotebookClass* Notebook;
	
	/**
	 * Parent container that will hold all features' tools windows.
	 * 
	 * @var wxAuiNotebook*
	 */
	wxAuiNotebook* ToolsNotebook;

	/**
	 * Parent container that will hold all features' outline windows.
	 * 
	 * @var wxAuiNotebook*
	 */
	wxAuiNotebook* OutlineNotebook;

	/**
	 * The Application-wide menu bar.
	 */
	wxMenuBar* MenuBar;
	
};

}

#endif