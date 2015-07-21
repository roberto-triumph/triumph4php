/**
 * @copyright  2013 Roberto Perpuly
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
#include "views/NewUserViewClass.h"
#include <wx/fileconf.h>
#include <wx/stdpaths.h>
#include <wx/valgen.h>
#include <wx/wizard.h>
#include "globals/Assets.h"
#include "Triumph.h"
#include "widgets/NonEmptyTextValidatorClass.h"

static int ID_NEW_USER_DIALOG = wxNewId();
static int ID_NEW_USER_PHP_SETTINGS = wxNewId();
static int ID_NEW_USER_TIMER = wxNewId();

t4p::NewUserViewClass::NewUserViewClass(t4p::NewUserFeatureClass& feature)
    : FeatureViewClass()
    , Timer(this, ID_NEW_USER_TIMER)
    , Feature(feature) {
}

void t4p::NewUserViewClass::OnAppReady(wxCommandEvent &event) {
    wxFileName settingsDir = t4p::SettingsDirAsset();
    if (!settingsDir.IsOk()) {
        Timer.Start(1000, wxTIMER_ONE_SHOT);
    }
}

void t4p::NewUserViewClass::OnTimer(wxTimerEvent& event) {
    wxFileName settingsDir;
    wxWizard wizard(GetMainWindow(), wxID_ANY, _("Welcome New User"));

    wxWizardPageSimple* page1 = new wxWizardPageSimple(&wizard);
    page1->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxWizardPageSimple* page2 = new wxWizardPageSimple(&wizard);
    page2->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxWizardPageSimple* page3 = new wxWizardPageSimple(&wizard);
    page3->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    wxWizardPageSimple::Chain(page1, page2);
    wxWizardPageSimple::Chain(page2, page3);
    new t4p::NewUserSettingsPanelClass(page1, Feature.App.Globals, Feature.App.Preferences, settingsDir);
    new t4p::NewUserPhpSettingsPanelClass(page2, Feature.App.Globals);
    new t4p::NewUserAssociationsPanelClass(page3, Feature.App.Globals);

    wizard.GetPageAreaSizer()->Add(page1);
    wizard.GetPageAreaSizer()->Add(page2);
    wizard.GetPageAreaSizer()->Add(page3);

    if (wizard.RunWizard(page1)) {
        // true means that we will
        // re trigger the app start sequence, that way the tag dbs get created
        // in the new location
        Feature.App.SavePreferences(settingsDir, true);
    } else {
        // use default settings
        // use the user data directory
        // create our subDir if it does not exist
        wxStandardPaths paths = wxStandardPaths::Get();
        wxFileName tempDir;
        tempDir.AssignDir(paths.GetUserConfigDir());
        tempDir.AppendDir(wxT(".triumph4php"));
        if (!tempDir.DirExists()) {
            bool created = wxMkdir(tempDir.GetPath(), 0777);
            if (!created) {
                wxMessageBox(wxT("Could not create directory: ") + tempDir.GetPath(), wxT("Error"));
            }
        }

        // revert settings to defaults
        Feature.App.Preferences.CheckForUpdates = true;
        Feature.App.Globals.Environment.Php.Installed = false;
        Feature.App.Globals.Environment.Php.Version = pelet::PHP_54;
        Feature.App.Globals.FileTypes.PhpFileExtensionsString = wxT("*.php");
        Feature.App.Globals.FileTypes.JsFileExtensionsString = wxT("*.js");
        Feature.App.Globals.FileTypes.CssFileExtensionsString = wxT("*.css");
        Feature.App.Globals.FileTypes.SqlFileExtensionsString = wxT("*.sql");
        Feature.App.Globals.FileTypes.ConfigFileExtensionsString = wxT("*.conf;*.ini;.htaccess");
        Feature.App.Globals.FileTypes.YamlFileExtensionsString = wxT("*.yml");
        Feature.App.Globals.FileTypes.XmlFileExtensionsString = wxT("*.xml");
        Feature.App.Globals.FileTypes.MarkdownFileExtensionsString = wxT("*.md");
        Feature.App.Globals.FileTypes.BashFileExtensionsString = wxT("*.sh");
        Feature.App.Globals.FileTypes.MiscFileExtensionsString = wxT("*.json;*.twig;*.txt");


        Feature.App.SavePreferences(settingsDir, true);
    }
}

void t4p::NewUserViewClass::OnWizardCancel(wxWizardEvent& event) {
    wxMessageBox(_("Default settings will be used"), _("Triumph 4 PHP"));
}

t4p::NewUserSettingsPanelClass::NewUserSettingsPanelClass(wxWindow *parent,
        t4p::GlobalsClass &globals,
        t4p::PreferencesClass& preferences,
        wxFileName& configFileDir)
    : NewUserSettingsPanelGeneratedClass(parent, ID_NEW_USER_DIALOG)
    , Preferences(preferences)
    , ConfigFileDir(configFileDir) {
    wxString label = UserDataDirectory->GetLabel();
    wxStandardPaths paths = wxStandardPaths::Get();
    wxFileName tempDir;
    tempDir.AssignDir(paths.GetUserConfigDir());
    tempDir.AppendDir(wxT(".triumph4php"));
    label += wxT(" (") + tempDir.GetPath() + wxT(")");
    UserDataDirectory->SetLabel(label);

    wxGenericValidator updateValidator(&Preferences.CheckForUpdates);
    CheckForUpdates->SetValidator(updateValidator);

    TransferDataToWindow();
}

void t4p::NewUserSettingsPanelClass::OnUpdateUi(wxUpdateUIEvent& event) {
    SettingsDirectory->Enable(CustomDirectory->GetValue());
}

bool t4p::NewUserSettingsPanelClass::TransferDataFromWindow() {
    bool good = NewUserSettingsPanelGeneratedClass::TransferDataFromWindow();
    if (!good) {
        return good;
    }
    if (CustomDirectory->GetValue()) {
        // if user chose to store settings in a custom directory it must exist
        wxString settingsDir = SettingsDirectory->GetPath();
        if (!wxFileName::DirExists(settingsDir)) {
            wxMessageBox(wxT("Custom settings directory must exist."), wxT("Error"));
            return false;
        }
        if (!wxFileName::IsDirWritable(settingsDir)) {
            wxMessageBox(wxT("Custom settings directory is not writable."), wxT("Error"));
            return false;
        }
    }
    wxStandardPaths paths = wxStandardPaths::Get();
    if (ApplicationDirectory->GetValue()) {
        // create our subDir if it does not exist
        wxFileName tempDir;
        tempDir.AssignDir(paths.GetExecutablePath());

        // since we distribute the binary in  a "bin/" dir
        // lets remove it so that the settings dir is in the top-level
        // of the installation directory
        tempDir.RemoveLastDir();
        tempDir.AppendDir(wxT(".triumph4php"));
        if (!tempDir.DirExists()) {
            bool created = wxMkdir(tempDir.GetPath(), 0777);
            if (!created) {
                wxMessageBox(wxT("Could not create directory: ") + tempDir.GetPath(), wxT("Error"));
                return false;
            }
        }
        if (!tempDir.IsDirWritable()) {
            wxMessageBox(wxT("Settings directory is not writable: ") + tempDir.GetPath(), wxT("Error"));
            return false;
        }
        ConfigFileDir = tempDir;
    } else if (CustomDirectory->GetValue()) {
        ConfigFileDir.AssignDir(SettingsDirectory->GetPath());
    } else {
        // use the user data directory
        // create our subDir if it does not exist
        wxFileName tempDir;
        tempDir.AssignDir(paths.GetUserConfigDir());
        tempDir.AppendDir(wxT(".triumph4php"));
        if (!tempDir.DirExists()) {
            bool created = wxMkdir(tempDir.GetPath(), 0777);
            if (!created) {
                wxMessageBox(wxT("Could not create directory: ") + tempDir.GetPath(), wxT("Error"));
                return false;
            }
        } else if (!tempDir.IsDirWritable()) {
            wxMessageBox(wxT("Settings directory is not writable: ") + tempDir.GetPath(), wxT("Error"));
            return false;
        }
        ConfigFileDir = tempDir;
    }
    return true;
}

t4p::NewUserAssociationsPanelClass::NewUserAssociationsPanelClass(wxWindow* parent, t4p::GlobalsClass& globals)
    : NewUserAssociationsPanelGeneratedClass(parent, ID_NEW_USER_PHP_SETTINGS)
    , Globals(globals) {
    NonEmptyTextValidatorClass phpFileExtensionsValidator(&Globals.FileTypes.PhpFileExtensionsString, PhpLabel);
    PhpFileExtensions->SetValidator(phpFileExtensionsValidator);

    NonEmptyTextValidatorClass cssFileExtensionsValidator(&Globals.FileTypes.CssFileExtensionsString, CssLabel);
    CssFileExtensions->SetValidator(cssFileExtensionsValidator);

    NonEmptyTextValidatorClass sqlFileExtensionsValidator(&Globals.FileTypes.SqlFileExtensionsString, SqlLabel);
    SqlFileExtensions->SetValidator(sqlFileExtensionsValidator);

    NonEmptyTextValidatorClass jsFileExtensionsValidator(&Globals.FileTypes.JsFileExtensionsString, JsLabel);
    JsFileExtensions->SetValidator(jsFileExtensionsValidator);

    NonEmptyTextValidatorClass configFileExtensionsValidator(&Globals.FileTypes.ConfigFileExtensionsString, ConfigLabel);
    ConfigFileExtensions->SetValidator(configFileExtensionsValidator);

    NonEmptyTextValidatorClass yamlFileExtensionsValidator(&Globals.FileTypes.YamlFileExtensionsString, YamlLabel);
    YamlFileExtensions->SetValidator(yamlFileExtensionsValidator);

    NonEmptyTextValidatorClass xmlFileExtensionsValidator(&Globals.FileTypes.XmlFileExtensionsString, XmlLabel);
    XmlFileExtensions->SetValidator(xmlFileExtensionsValidator);

    NonEmptyTextValidatorClass markdownFileExtensionsValidator(&Globals.FileTypes.MarkdownFileExtensionsString, MarkdownLabel);
    MarkdownFileExtensions->SetValidator(markdownFileExtensionsValidator);

    NonEmptyTextValidatorClass bashFileExtensionsValidator(&Globals.FileTypes.BashFileExtensionsString, BashLabel);
    BashFileExtensions->SetValidator(bashFileExtensionsValidator);

    NonEmptyTextValidatorClass miscFileExtensionsValidator(&Globals.FileTypes.MiscFileExtensionsString, MiscLabel);
    MiscFileExtensions->SetValidator(miscFileExtensionsValidator);

    TransferDataToWindow();
}

t4p::NewUserPhpSettingsPanelClass::NewUserPhpSettingsPanelClass(wxWindow* parent, t4p::GlobalsClass& globals)
    : NewUserPhpSettingsPanelGeneratedClass(parent, wxID_ANY)
    , Globals(globals) {
    wxGenericValidator phpInstalledValidator(&Globals.Environment.Php.Installed);
    Installed->SetValidator(phpInstalledValidator);
}

void t4p::NewUserPhpSettingsPanelClass::OnUpdateUi(wxUpdateUIEvent& event) {
    PhpExecutable->Enable(Installed->GetValue());
}

bool t4p::NewUserPhpSettingsPanelClass::TransferDataFromWindow() {
    bool good = NewUserPhpSettingsPanelGeneratedClass::TransferDataFromWindow();
    if (!good) {
        return good;
    }
    if (Installed->GetValue()) {
        // only if the user has php installed do we check to see if the
        // executable is good
        wxString phpPath = PhpExecutable->GetPath();
        if (!wxFileName::FileExists(phpPath)) {
            wxMessageBox(wxT("PHP Executable must exist."), wxT("Error"));
            return false;
        }
    } else if (Version->GetSelection() == 0) {
        // no PHP then dont try to detect version
        // but the user must have not have chosen auto
        wxMessageBox(wxT("Since PHP is not available you must provide the PHP version to use during parsing."), wxT("Error"));
        return false;
    } else if (Version->GetSelection() == 1) {
        Globals.Environment.Php.IsAuto = 0;
        Globals.Environment.Php.Version = pelet::PHP_53;
    } else {
        Globals.Environment.Php.IsAuto = 0;
        Globals.Environment.Php.Version = pelet::PHP_54;
    }
    Globals.Environment.Php.PhpExecutablePath = PhpExecutable->GetPath();
    return true;
}

BEGIN_EVENT_TABLE(t4p::NewUserSettingsPanelClass, NewUserSettingsPanelGeneratedClass)
    EVT_UPDATE_UI(ID_NEW_USER_DIALOG, t4p::NewUserSettingsPanelClass::OnUpdateUi)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::NewUserPhpSettingsPanelClass, NewUserPhpSettingsPanelGeneratedClass)
    EVT_UPDATE_UI(ID_NEW_USER_PHP_SETTINGS, t4p::NewUserPhpSettingsPanelClass::OnUpdateUi)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::NewUserViewClass, t4p::FeatureViewClass)
    EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::NewUserViewClass::OnAppReady)
    EVT_TIMER(ID_NEW_USER_TIMER, t4p::NewUserViewClass::OnTimer)
    EVT_WIZARD_CANCEL(wxID_ANY, t4p::NewUserViewClass::OnWizardCancel)
END_EVENT_TABLE()
