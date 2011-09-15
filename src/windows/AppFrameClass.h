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
#ifndef MVCEDITORAPPFRAMECLASS_H_
#define MVCEDITORAPPFRAMECLASS_H_

#include <php_frameworks/ProjectClass.h>
#include <windows/wxformbuilder/AppFrameGeneratedClass.h>
#include <widgets/NotebookClass.h>
#include <PluginClass.h>
#include <PreferencesClass.h>
#include <wx/aui/aui.h>

namespace mvceditor {

/**
 * The main frame; contains all menus, toolbars, and notebooks.  For now there is only one
 * instance of a main frame per app.
 */
class AppFrameClass : public AppFrameGeneratedClass {

public:
	/** Constructor */
	AppFrameClass(const std::vector<PluginClass*>& plugins,
		wxEvtHandler& appHandler, EnvironmentClass& environment,
		PreferencesClass& preferences);
	
	~AppFrameClass();
	
	/**
	 * Loads the given files into the application, one page for each file.
	 * File names must be fully qualified.
	 */
	void FileOpen(const std::vector<wxString>& filenames);

	/**
	 * notify all sub-windows of a new project.
	 * This class will not own this pointer
	 */
	void OnProjectOpened(ProjectClass* project);

	/**
	 * when a project is closed, close any open windows
	 */
	void OnProjectClosed();

	/**
	 * get all of the plugin's extra windows and menus and attach them to the main frame.
	 * This class will not own this pointer
	 */
	void LoadPlugin(PluginClass* plugin);

	/**
	 * this should be called whenever a new window is added.
	 */
	void AuiManagerUpdate();

protected:

	// Handlers for AppFrameGeneratedClass events.
	void OnClose(wxCloseEvent& event);
	
	/**
	 * Top Menu Bar handlers
	 */
	void OnFileSave(wxCommandEvent& event);
	void OnFileNew(wxCommandEvent& event);
	void OnFileRun(wxCommandEvent& event);
	void OnFileOpen(wxCommandEvent& event);
	void OnFileSaveAs(wxCommandEvent& event);
	void OnFileRevert(wxCommandEvent& event);
	void OnFileClose(wxCommandEvent& event);
	void OnFileExit(wxCommandEvent& event);
	void OnEditCut(wxCommandEvent& event);
	void OnEditCopy(wxCommandEvent& event);
	void OnEditPaste(wxCommandEvent& event);
	void OnEditSelectAll(wxCommandEvent& event);
	void OnEditPreferences(wxCommandEvent& event);
	void OnEditContentAssist(wxCommandEvent& event);
	void OnEditCallTip(wxCommandEvent& event);
	void OnHelpAbout(wxCommandEvent& event);

	/**
	 * project menu handlers
	 */
	void OnProjectOpen(wxCommandEvent& event);
	
	/**
	 * Context menu handler
	 */
	void OnContextMenu(wxContextMenuEvent& event);	
	
private:

	/**
	 * Status bar accessor.  This status bar allows plugins to easily add gauges to the status bar.
	 * 
	 * @return StatusBarWithGaugeClass do NOT delete the pointer.  This class will take care of memory management.
	 */
	StatusBarWithGaugeClass* GetStatusBarWithGauge();
	
	/**
	 * When a page is modified, enable the save button
	 */
	void EnableSave(wxStyledTextEvent& event);
	
	/**
	 * When a page is saved, disable the save button
	 */
	void DisableSave(wxStyledTextEvent& event);
	
	/**
	 * Change the state of various menus / buttons
	 * when page is changed.
	 */
	void PhpSourceCodeCtrlPageChanged(wxAuiNotebookEvent& event);

	/**
	 * Save the currently active tab.
	 */
	void SaveCurrentFile(wxCommandEvent& event);

	/**
	 * Creates the toolbar buttons
	 */
	void CreateToolBarButtons();
	
	/**
	 * Toggle various widgets on or off based on the application state. 
	 */
	void OnUpdateUi(wxUpdateUIEvent& event);

 	/**
 	 * Handle the Undo popup menu event
	 */
	void OnUndo(wxCommandEvent& event);

	/**
	 * Handle the Redo popup menu event
	 */
	void OnRedo(wxCommandEvent& event);

	/**
	 * handle the cut line menu event
	 */
	void OnCutLine(wxCommandEvent& event);

	/**
	 * handle the duplicate line menu event
	 */
	void OnDuplicateLine(wxCommandEvent& event);

	/**
	 * handle the delete line menu event
	 */
	void OnDeleteLine(wxCommandEvent& event);

	/**
	 * handle the transpose line menu event
	 */
	void OnTransposeLine(wxCommandEvent& event);

	/**
	 * handle the copy line menu event
	 */
	void OnCopyLine(wxCommandEvent& event);

	/**
	 * handle the lowercase selected menu event
	 */
	void OnLowecase(wxCommandEvent& event);

	/**
	 * handle the uppercase selected menu event
	 */
	void OnUppercase(wxCommandEvent& event);
	
	/*
	 * Draw the current cursor line and pos
	 */
	void OnCodeControlUpdate(wxStyledTextEvent& event);

	/**
	 * When the tools notebook has no more pages; hide it
	 */
	void OnToolsNotebookPageClosed(wxAuiNotebookEvent& event);
	
	/**
	 * GUI framework object, used to programatically position the different windows
	 * 
	 * @var wxAuiManager
	 */
	wxAuiManager AuiManager;

	/**
	 * Additional functionality
	 */
	const std::vector<PluginClass*>& Plugins;

	/**
	 * will send events to the top-level app
	 */
	wxEvtHandler& AppHandler;
	
	/**
	 * The environment stack.
	 * 
	 * @var EnvironmentClass
	 */
	EnvironmentClass& Environment;

	/**
	 * The user preferences
	 * 
	 * @var PreferencesClass;
	 */
	PreferencesClass& Preferences;

	/**
	 * The toolbar instance
	 * @var wxAuiToolBar*
	 */
	wxAuiToolBar* ToolBar;
	
	/**
	 * To hold the various plugin
	 * panels in place.
	 * @var wxAuiNotebook*
	 */
	wxAuiNotebook* ToolsNotebook;
	
	DECLARE_EVENT_TABLE()
};

}
#endif // MVCEDITORAPPFRAMECLASS_H_