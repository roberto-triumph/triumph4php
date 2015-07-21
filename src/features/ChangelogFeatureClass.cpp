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
#include "features/ChangelogFeatureClass.h"
#include <wx/ffile.h>
#include "globals/Assets.h"
#include "Triumph.h"

t4p::ChangelogFeatureClass::ChangelogFeatureClass(t4p::AppClass& app)
    : FeatureClass(app)
    , LastVersion() {
}

void t4p::ChangelogFeatureClass::LoadPreferences(wxConfigBase* config) {
    config->Read(wxT("Changelog/LastVersion"), &LastVersion);
}

void t4p::ChangelogFeatureClass::OnAppReady(wxCommandEvent& event) {
    // version info is stored in a file
    // for releases, the distribution script will properly fill in the
    // version number using git describe
    wxString version;
    wxFileName versionFileName = t4p::VersionFileAsset();
    wxFFile file(versionFileName.GetFullPath());
    if (file.IsOpened()) {
        file.ReadAll(&version);
    }
    version.Trim(false).Trim(true);
    LastVersion.Trim(false).Trim(true);

    if (version.CmpNoCase(LastVersion) != 0) {
        // version changed, show the changelog
        ShowChangeLog();


        // store the version opened so that next time that the app
        // is opened we don't show the changelog
        LastVersion = version;
        wxConfigBase* config = wxConfigBase::Get(false);
        config->Write(wxT("Changelog/LastVersion"), LastVersion);
        config->Flush();
    }
}

void t4p::ChangelogFeatureClass::OnSavePreferences(wxCommandEvent& event) {
    wxConfigBase* config = wxConfigBase::Get(false);
    config->Write(wxT("Changelog/LastVersion"), LastVersion);
}

void t4p::ChangelogFeatureClass::ShowChangeLog() {
    wxFileName changelogFile = t4p::ChangeLogFileAsset();

    wxCommandEvent fileCmd(t4p::EVENT_CMD_FILE_OPEN);
    fileCmd.SetString(changelogFile.GetFullPath());
    App.EventSink.Publish(fileCmd);
}


BEGIN_EVENT_TABLE(t4p::ChangelogFeatureClass, t4p::FeatureClass)
    EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::ChangelogFeatureClass::OnAppReady)
END_EVENT_TABLE()
