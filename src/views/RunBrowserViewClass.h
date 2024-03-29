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
#ifndef SRC_VIEWS_RUNBROWSERVIEWCLASS_H_
#define SRC_VIEWS_RUNBROWSERVIEWCLASS_H_

#include <wx/string.h>
#include <memory>
#include <vector>
#include "code_control/CodeControlClass.h"
#include "features/RunBrowserFeatureClass.h"
#include "globals/Events.h"
#include "views/FeatureViewClass.h"
#include "widgets/GaugeDialogClass.h"

namespace t4p {
class RunBrowserViewClass : public FeatureViewClass {
 public:
    RunBrowserViewClass(t4p::RunBrowserFeatureClass& feature);

    void AddWindows();

    void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

 private:
    /**
     * when a file is saved and that file is from an enabled project
     * we set a dirty flag so that we can re-run url detection
     * when the user clicks on the Search For Urls button
     */
    void OnFileSaved(t4p::CodeControlEventClass& event);

    /**
     * run the chosen URL in (an external) web browser
     */
    void OnRunInWebBrowser(wxCommandEvent& event);

    void OnBrowserToolDropDown(wxAuiToolBarEvent& event);

    void OnBrowserToolMenuItem(wxCommandEvent& event);

    void OnUrlToolDropDown(wxAuiToolBarEvent& event);

    void OnUrlToolMenuItem(wxCommandEvent& event);

    void OnUrlSearchTool(wxCommandEvent& event);

    /**
     * to show progress to the user
     */
    void OnProcessInProgress(wxCommandEvent& event);

    /**
     * show the user the URL dialog and open the chosen url
     */
    void ShowUrlDialog();

    void SetCurrentBrowser();

    /**
     * if we started the URL detection process because of a user
     * click, then when url detection is complete we will show
     * the user the ChooseUrlDialog
     */
    void OnUrlDetectionComplete(t4p::ActionEventClass& event);

    void OnPreferencesSaved(wxCommandEvent& event);

    /**
     * contains the application logic
     */
    t4p::RunBrowserFeatureClass& Feature;

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

    /**
     * a bigger progress bar to show the user while url detection is running. this
     * dialog is shown so that the user has more feedback that just the bottom
     * progress bar in the status bar.
     */
    t4p::GaugeDialogClass* GaugeDialog;

    wxMenuItem* RunInBrowser;

    wxAuiToolBar* BrowserToolbar;

    /**
     * A 'dirty' flag so that we can re-run url detection
     * when the user clicks on the Search For Urls button when we know that
     * the URL cache needs to be updated.
     */
    bool IsUrlCacheStale;

    /**
     * a flag that will be set when we start the url detection process
     * from a user click. when the url detection ends we will show the
     * ChooseUrlDialog
     */
    bool IsWaitingForUrlDetection;

    DECLARE_EVENT_TABLE()
};
}  // namespace t4p

#endif  // SRC_VIEWS_RUNBROWSERVIEWCLASS_H_
