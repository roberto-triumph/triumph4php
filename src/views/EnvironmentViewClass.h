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
#ifndef SRC_VIEWS_ENVIRONMENTVIEWCLASS_H_
#define SRC_VIEWS_ENVIRONMENTVIEWCLASS_H_

#include <map>
#include <vector>
#include "actions/ActionClass.h"
#include "globals/EnvironmentClass.h"
#include "views/FeatureViewClass.h"
#include "views/wxformbuilder/EnvironmentFeatureForms.h"

namespace t4p {
// forward declaration, declared in another file
class ApacheFileReadCompleteEventClass;
class EnvironmentFeatureClass;

class EnvironmentViewClass : public t4p::FeatureViewClass {
 public:
    EnvironmentViewClass(t4p::EnvironmentFeatureClass& feature);

    /**
     * Add the environment dialogs to the preferences notebook
     */
    void AddPreferenceWindow(wxBookCtrlBase* parent);

 private:
    t4p::EnvironmentFeatureClass& Feature;
};

/**
 * A dialog that edits a virtual host mapping.
 */
class VirtualHostCreateDialogClass : public VirtualHostCreateDialogGeneratedClass {
 public:
    VirtualHostCreateDialogClass(wxWindow* parent, std::map<wxString, wxString> existingVirtualHosts,
                                 wxString& hostname, wxFileName& rootDirectory);

 protected:
    /**
     * here we will do duplicate checks; the same directory may not be entered twice
     */
    void OnOkButton(wxCommandEvent& event);

 private:
    /**
     * This is used to check that the same directory is not entered twice
     */
    std::map<wxString, wxString> ExistingVirtualHosts;

    /**
     * Used because there is to validator for wxDirPickerCtrl
     */
    wxFileName& RootDirectoryFileName;
};

/**
 * Panel that shows the apache virtual host config. It will also
 * have functionality to scan a directory for apache config files
 */
class ApacheEnvironmentPanelClass : public ApacheEnvironmentPanelGeneratedClass {
 protected:
    void OnScanButton(wxCommandEvent& event);
    void OnAddButton(wxCommandEvent& event);
    void OnEditButton(wxCommandEvent& event);
    void OnRemoveButton(wxCommandEvent& event);

 public:
    /** Constructor */
    ApacheEnvironmentPanelClass(wxWindow* parent, t4p::RunningThreadsClass& runningThreads, EnvironmentClass& environment);

    ~ApacheEnvironmentPanelClass();

    /**
     * transfers the settings from the window to the Environment data structure
     */
    bool TransferDataFromWindow();

 private:
    /**
     * The configuration class
     *
     * @var EnvironmentClass
     */
    EnvironmentClass& Environment;

    /**
     * keeps track of background threads
     */
    t4p::RunningThreadsClass& RunningThreads;

    /**
     * A copy of the current virtual hosts; this is the data structure that the
     * user modifies (while the dialog is opened)
     */
    ApacheClass EditedApache;

    /**
     * the action identifier, used to stop any running actions
     */
    int RunningActionId;

    /**
     * populate the dialog according to the ApacheClass settings
     */
    void OnApacheFileReadComplete(t4p::ApacheFileReadCompleteEventClass& event);

    /**
     * When this panel is resized automatically re-adjust the wrapping the label
     */
    void OnResize(wxSizeEvent& event);

    /**
     * tick the gauge here
     */
    void OnActionProgress(t4p::ActionProgressEventClass& event);

    /**
     * stop the gauge here
     */
    void OnActionComplete(t4p::ActionEventClass& event);

    /**
     * Fills in the dialogs based on the Apache environment
     */
    void Populate();

    /**
     * disable the CRUD buttons according to the Manual flag
     */
    void OnUpdateUi(wxUpdateUIEvent& event);

    /**
     * when the user picks a directory start the scan
     */
    void OnDirChanged(wxFileDirPickerEvent& event);

    DECLARE_EVENT_TABLE()
};

/**
 * Panel that displays the configured web browser executable paths
 */
class WebBrowserEditPanelClass : public WebBrowserEditPanelGeneratedClass {
 public:
    WebBrowserEditPanelClass(wxWindow* parent, EnvironmentClass& environment);

    /**
     * applies the settings that were changed to the Environment reference [given
     * in the constructor].
     */
    bool TransferDataFromWindow();

 protected:
    /**
     * When this panel is resized automatically re-adjust the wrapping the label
     */
    void OnResize(wxSizeEvent& event);

    void OnRemoveSelectedWebBrowser(wxCommandEvent& event);

    void OnAddWebBrowser(wxCommandEvent& event);

    void OnEditSelectedWebBrowser(wxCommandEvent& event);

    void OnMoveUp(wxCommandEvent& event);

    void OnMoveDown(wxCommandEvent& event);

 private:
    /**
     * The configuration class
     *
     * @var EnvironmentClass
     */
    EnvironmentClass& Environment;

    /**
     * The web browsers being modified. this is the vector that is the recipient
     * of all of the user's operations; it will be copied only when the
     * user clicks OK.
     */
    std::vector<WebBrowserClass> EditedWebBrowsers;
};

/**
 * Panel that shows the PHP binary path locations
 */
class PhpEnvironmentPanelClass : public PhpEnvironmentPanelGeneratedClass {
 public:
    PhpEnvironmentPanelClass(wxWindow* parent, EnvironmentClass& environment);

    bool TransferDataFromWindow();

 protected:
    /**
     * Handle the file picker changed event.
     */
    void OnPhpFileChanged(wxFileDirPickerEvent& event);

    /*
     * disable the file picker when the checkbox is checked
     */
    void OnInstalledCheck(wxCommandEvent& event);

    /**
     * When this panel is resized automatically re-adjust the wrapping the label
     */
    void OnResize(wxSizeEvent& event);

 private:
    /**
     * The configuration class
     *
     * @var EnvironmentClass
     */
    EnvironmentClass& Environment;
};

/**
 * Dialog that allows the user to enter a web browser name and executable
 * location
 */
class WebBrowserCreateDialogClass : public WebBrowserCreateDialogGeneratedClass {
 public:
    WebBrowserCreateDialogClass(wxWindow* parent, std::vector<WebBrowserClass> existingBrowsers,
                                WebBrowserClass& newBrowser);

 protected:
    void OnOkButton(wxCommandEvent& event);

    /**
     * to prevent multiple browsers with the same name
     */
    std::vector<WebBrowserClass> ExistingBrowsers;

    /**
     * to transfer the chosen file path
     */
    WebBrowserClass& NewBrowser;

    /**
     * Safe the original name so that when editing we can tell that the name is not changing
     * and the "duplicate" name check won't be run (allow the user to edit a browser
     * path only).
     */
    wxString OriginalName;
};
}  // namespace t4p

#endif  // SRC_VIEWS_ENVIRONMENTVIEWCLASS_H_
