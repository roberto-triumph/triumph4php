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
#ifndef SRC_MAIN_FRAME_MAINFRAMECLASS_H_
#define SRC_MAIN_FRAME_MAINFRAMECLASS_H_

#include <wx/aui/aui.h>
#include <wx/stc/stc.h>
#include <wx/wizard.h>
#include <vector>
#include "actions/ActionClass.h"
#include "actions/SequenceClass.h"
#include "globals/Events.h"
#include "globals/ProjectClass.h"
#include "main_frame/PreferencesClass.h"
#include "main_frame/wxformbuilder/MainFrameForms.h"
#include "widgets/StatusBarWithGaugeClass.h"

namespace t4p {
// forward declaration to prevent recursive dependencies
class AppClass;

// defined at the bottom of this file
class MainFrameClass;

// forward declarations, defined in other files
class FeatureViewClass;

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
    void OnPreferencesSaved(wxCommandEvent& event);

    void OnPreferencesExternallyUpdated(wxCommandEvent& event);

    void OnAppReady(wxCommandEvent& event);

    void OnAppFilePageChanged(t4p::CodeControlEventClass& event);

    void OnAppFileNotebookChanged(t4p::CodeControlEventClass& event);

    void OnAppFileClosed(t4p::CodeControlEventClass& event);

    void OnAppFileCreated(wxCommandEvent& event);

    void OnAppRequestUserAttention(wxCommandEvent& event);

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
    /**
     * @brief
     * @param featureViews the views (to get menu bars, toolbars). This class
     *        will not own the pointers, although the main fram will tell the
     *        app to delete them when the user closes the main frame
     * @param app the application, used to get the preferences
     * @param guiApp to change the "exit on frame close" option
     * @param configModifiedTimer to stop checking for updates to the
     *        config file while the user is making changes to the app preferences
     *
     */
    MainFrameClass(const std::vector<FeatureViewClass*>& featureViews,
                   AppClass&  app, wxApp& guiApp, wxTimer& configModifiedTimer);

    ~MainFrameClass();

    /**
     * get all of the feature view's extra windows and menus and attach them to the main frame.
     */
    void LoadFeatureView(FeatureViewClass& view);

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

    /**
     * need to call this once all items have been added to the toolbar
     */
    void RealizeToolbar();

    /**
     * creates a new code control and attaches it to the
     * notebook
     */
    void CreateNewCodeCtrl();

    /**
     * Checks code notebooks to see if they have any more pages;if a notebook
     * does not have any pages it is scheduled for deletion
     */
    void UpdateNotebooks();

 protected:
    // Handlers for MainFrameGeneratedClass events.
    void OnClose(wxCloseEvent& event);

    /**
     * Top Menu Bar handlers
     */
    void OnFileExit(wxCommandEvent& event);
    void OnEditPreferences(wxCommandEvent& event);
    void OnViewToggleTools(wxCommandEvent& event);
    void OnViewToggleOutline(wxCommandEvent& event);
    void OnHelpAbout(wxCommandEvent& event);
    void OnHelpCredits(wxCommandEvent& event);
    void OnHelpLicense(wxCommandEvent& event);
    void OnHelpManual(wxCommandEvent& event);

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
    t4p::StatusBarWithGaugeClass* GetStatusBarWithGauge();

    /**
     * When a page is modified, enable the save button
     */
    void OnStcSavePointReached(wxStyledTextEvent& event);

    /**
     * When a page is saved, disable the save button
     */
    void OnStcSavedPointLeft(wxStyledTextEvent& event);

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
     * propagate wizard events to features, so that they can handle wizard events themselves
     */
    void OnAnyWizardEvent(wxWizardEvent& event);

    /**
     * when a sequence has started show the Gauge
     */
    void OnSequenceStart(wxCommandEvent& event);

    /**
     * when a sequence has completed stop the Gauge
     */
    void OnSequenceComplete(wxCommandEvent& event);

    /**
     * these are also events that get generated by actions, but in this case the event
     * has percentage complete numbers so we want to show a determinate gauge
     */
    void OnDeterminateProgress(t4p::SequenceProgressEventClass& event);

    /**
     * set the application-wide font.  this is non-trivial because we
     * want to change the tab fonts on all notebooks also
     */
    void SetApplicationFont();

    /**
     * GUI framework object, used to programatically position the different windows
     *
     * @var wxAuiManager
     */
    wxAuiManager AuiManager;

    /**
     * Additional functionality, this class will not own the pointers
     */
    const std::vector<FeatureViewClass*>& FeatureViews;

    /**
     * Used to listen for app events.
     */
    AppEventListenerForFrameClass Listener;

    /**
     * The application global
     */
    AppClass& App;

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

    /**
     * The running wxApp
     */
    wxApp& GuiApp;

    /**
     * to stop checking for config changes when the
     * user is actually editing the preferences
     */
    wxTimer& ConfigModifiedTimer;

    DECLARE_EVENT_TABLE()
};
}  // namespace t4p

#endif  // SRC_MAIN_FRAME_MAINFRAMECLASS_H_
