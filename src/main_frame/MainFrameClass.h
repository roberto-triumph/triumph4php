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
#ifndef MVCEDITORMAINFRAMECLASS_H_
#define MVCEDITORMAINFRAMECLASS_H_

#include <globals/ProjectClass.h>
#include <main_frame/wxformbuilder/MainFrameForms.h>
#include <widgets/NotebookClass.h>
#include <features/FeatureClass.h>
#include <main_frame/PreferencesClass.h>
#include <globals/Events.h>
#include <wx/aui/aui.h>

namespace mvceditor {

// forward declaration to prevent recursive dependencies
class AppClass;

// defined at the bottom of this file
class MainFrameClass;

/**
 * This class is used to listen for app events.  It is a separate class
 * because the MainFrame should handle them; the app propagates
 * the menu, tool, and AUI events to the event sink; if the frame
 * were a listener to the sink then events would be
 * trigger indefinitely (since the frame would get an event, publish it
 * to the sink, and get notified of the new event).
 */
class AppEventListenerForFrameClass : public wxEvtHandler {

public:

	/**
	 * @param frame this class will not own the pointer
	*/
	AppEventListenerForFrameClass(MainFrameClass* frame);

private:

	void OnCmdFileOpen(wxCommandEvent& event);

	void OnPreferencesSaved(wxCommandEvent& event);

	void OnPreferencesExternallyUpdated(wxCommandEvent& event);

	void OnAppReady(wxCommandEvent& event);
	
	void OnCodeNotebookPageChanged(wxAuiNotebookEvent& event);
	
	void OnCodeNotebookPageClosed(wxAuiNotebookEvent& event);

	/**
	 * Need the frame to manipulate it
	 */
	MainFrameClass* MainFrame;

	DECLARE_EVENT_TABLE()
};

/**
 * The main frame; contains all menus, toolbars, and notebooks.  For now there is only one
 * instance of a main frame per app.
 */
class MainFrameClass : public MainFrameGeneratedClass {

public:
	
	MainFrameClass(const std::vector<FeatureClass*>& features, AppClass&  app,
		PreferencesClass& preferences);
	
	~MainFrameClass();
	
	/**
	 * Loads the given files into the application, one page for each file.
	 * File names must be fully qualified.
	 */
	void FileOpen(const std::vector<wxString>& filenames);

	/**
	 * get all of the feature's extra windows and menus and attach them to the main frame.
	 * This class will not own this pointer
	 */
	void LoadFeature(FeatureClass* feature);

	/**
	 * this should be called whenever a new window is added.
	 */
	void AuiManagerUpdate();

	/**
	 * For a re-draw of all dialogs and windows based on new updated preferences
	 */
	void PreferencesSaved();

	/**
	 * For a re-draw of all dialogs and windows based on new updated preferences
	 */
	void PreferencesExternallyUpdated();
	
	/**
	 * Updates the title bar with the name of the file being edited.
	 */
	void UpdateTitleBar();

protected:

	// Handlers for MainFrameGeneratedClass events.
	void OnClose(wxCloseEvent& event);
	
	/**
	 * Top Menu Bar handlers
	 */
	void OnFileSave(wxCommandEvent& event);
	void OnFilePhpNew(wxCommandEvent& event);
	void OnFileSqlNew(wxCommandEvent& event);
	void OnFileCssNew(wxCommandEvent& event);
	void OnFileTextNew(wxCommandEvent& event);
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
	 * Context menu handler
	 */
	void OnContextMenu(wxContextMenuEvent& event);	
	
private:
	
	/**
	 * Status bar accessor.  This status bar allows features to easily add gauges to the status bar.
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
	void OnContentNotebookPageChanged(wxAuiNotebookEvent& event);

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
	 * Add the shortcuts for this frame's menu bar into the preference's shortcut list
	 */
	void DefaultKeyboardShortcuts();

	/**
	 * propagate menu events to features, so that they can handle menu events themselves
	 * their own menus
	 */
	void OnAnyMenuCommandEvent(wxCommandEvent& event);

	/**
	 * propagate aui notebook events to features, so that they can handle menu events themselves
	 * their own menus
	 */
	void OnAnyAuiNotebookEvent(wxAuiNotebookEvent& event);

	/**
	 * propagate aui toolbar events to features, so that they can handle menu events themselves
	 * their own menus
	 */
	void OnAnyAuiToolbarEvent(wxAuiToolBarEvent& event);

	/**
	 * GUI framework object, used to programatically position the different windows
	 * 
	 * @var wxAuiManager
	 */
	wxAuiManager AuiManager;

	/**
	 * Additional functionality
	 */
	const std::vector<FeatureClass*>& Features;

	/**
	 * Used to listen for app events.
	 */
	AppEventListenerForFrameClass Listener;

	/**
	 * The application global
	 */
	AppClass& App;

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
	 * To hold the various feature
	 * panels in place.
	 * @var wxAuiNotebook*
	 */
	wxAuiNotebook* ToolsNotebook;
	
	/**
	 * To hold the various outline
	 * panels in place.
	 * @var wxAuiNotebook*
	 */
	wxAuiNotebook* OutlineNotebook;
	
	DECLARE_EVENT_TABLE()
};

}
#endif // MVCEDITORMAINFRAMECLASS_H_