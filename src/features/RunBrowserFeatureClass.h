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
#ifndef __RUNBROWSERFEATURECLASS_H__
#define __RUNBROWSERFEATURECLASS_H__

#include <features/FeatureClass.h>
#include <features/wxformbuilder/RunBrowserFeatureForms.h>
#include <globals/UrlResourceClass.h>
#include <wx/string.h>
#include <vector>
#include <memory>

namespace mvceditor {

/**
 * small dialog that allows the user to choose a URL to run. User can also
 * add, delete, and modify items in the global URL list (URLResourceFinderClass).
 */
class ChooseUrlDialogClass : public ChooseUrlDialogGeneratedClass {
	
public:

	ChooseUrlDialogClass(wxWindow* parent, UrlResourceFinderClass* urls, UrlResourceClass& chosenUrl);
	
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
	UrlResourceFinderClass* UrlResourceFinder;

	/**
	 * The URL that the user selected.
	 */
	UrlResourceClass& ChosenUrl;
};

class UrlDetectorPanelClass : public UrlDetectorPanelGeneratedClass {

public:

	UrlDetectorPanelClass(wxWindow* parent, int id, mvceditor::GlobalsClass& globals,
		mvceditor::EventSinkClass& eventSink);

	/**
	 * This should be called when the detector tree needs to be updated.
	 */
	void Init();

	/**
	 * updates the project choice with the given projects. This should be called whenever
	 * the global project list has been changed.
	 * The project list will be read from GlobalsClass
	 */
	void UpdateProjects();

protected:
	void OnTreeItemActivated(wxTreeEvent& event);
	void OnTreeItemRightClick(wxTreeEvent& event);
	void OnTreeItemDelete(wxTreeEvent& event);
	void OnHelpButton(wxCommandEvent& event);
	void OnTestButton(wxCommandEvent& event);
	void OnAddButton(wxCommandEvent& event);
	void OnTreeItemEndLabelEdit(wxTreeEvent& event);

private:

	void FillSubTree(const wxString& detectorRootDir, wxTreeItemId treeItemDir);

	// context menu handlers for the URL detector tree
	void OnMenuOpenDetector(wxCommandEvent& event);
	void OnMenuRenameDetector(wxCommandEvent& event);
	void OnMenuDeleteDetector(wxCommandEvent& event);

	/**
	 * to access the project list
	 */
	mvceditor::GlobalsClass& Globals;

	/**
	 * to send app commands to open and run a file
	 */
	mvceditor::EventSinkClass& EventSink;

	DECLARE_EVENT_TABLE()
};
	
class RunBrowserFeatureClass : public FeatureClass {

public:

	/**
	 * The most recent URLs that have been run. New URLs are added after the 
	 * user picks one from the "Search For URLs..." button, and they are deleted
	 * only after the list has reached a certain size limit
	 * These urls have a Project scope; when a project is opened any previous
	 * URLs are cleared.
	 */
	std::vector<UrlResourceClass> RecentUrls;

	RunBrowserFeatureClass(mvceditor::AppClass& app);
	
	void AddWindows();
	
	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

	void LoadPreferences(wxConfigBase* config);
		
private:

	void OnPreferencesSaved(wxCommandEvent& event);

	/**
	 * run the chosen URL in (an external) web browser 
	 */
	void OnRunInWebBrowser(wxCommandEvent& event);
	
	void OnBrowserToolDropDown(wxAuiToolBarEvent& event);
	
	void OnBrowserToolMenuItem(wxCommandEvent& event);
	
	void OnUrlToolDropDown(wxAuiToolBarEvent& event);
	
	void OnUrlToolMenuItem(wxCommandEvent& event);
	
	void OnUrlSearchTool(wxCommandEvent& event);

	void AddNewMenu(wxMenuBar* menuBar);

	void OnViewUrlDetectors(wxCommandEvent& event);

	void OnRunUrlDetectors(wxCommandEvent& event);

	/**
	 * when a project is opened clean the Recent list
	 */
	void OnUrlResourceActionComplete(wxCommandEvent& event);

	/**
	 * to show progress to the user
	 */
	void OnProcessInProgress(wxCommandEvent& event);

	/**
	 * show the user the URL dialog and open the chosen url
	 */
	void ShowUrlDialog();
		
	/**
	 * A popup menu to show the currently configured browsers. The same popup menu will be
	 * reused multiple times. This class will own it,  that's why we use auto_ptr here
	 */
	std::auto_ptr<wxMenu> BrowserMenu;

	/**
	 * A popup menu to show the currently detected URLs. The same popup menu will be
	 * reused multiple times. This class will own it,  that's why we use auto_ptr here
	 */
	std::auto_ptr<wxMenu> UrlMenu;

	wxMenuItem* RunInBrowser;
	
	wxAuiToolBar* BrowserToolbar;
	
	DECLARE_EVENT_TABLE()
};

}

#endif