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
#include "features/ProjectFeatureClass.h"
#include <wx/dir.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/platinfo.h>
#include <algorithm>
#include <vector>
#include "globals/Assets.h"
#include "globals/Errors.h"
#include "globals/Number.h"
#include "Triumph.h"

t4p::ProjectFeatureClass::ProjectFeatureClass(t4p::AppClass& app)
    : FeatureClass(app) {
}

t4p::ProjectFeatureClass::~ProjectFeatureClass() {
}

void t4p::ProjectFeatureClass::LoadPreferences(wxConfigBase* config) {
    // config will leave the defaults alone if keys are not found in the config
    config->Read(wxT("/Project/PhpFileExtensions"), &App.Globals.FileTypes.PhpFileExtensionsString);
    config->Read(wxT("/Project/CssFileExtensions"), &App.Globals.FileTypes.CssFileExtensionsString);
    config->Read(wxT("/Project/SqlFileExtensions"), &App.Globals.FileTypes.SqlFileExtensionsString);
    config->Read(wxT("/Project/JsFileExtensions"), &App.Globals.FileTypes.JsFileExtensionsString);
    config->Read(wxT("/Project/ConfigFileExtensions"), &App.Globals.FileTypes.ConfigFileExtensionsString);
    config->Read(wxT("/Project/CrontabFileExtensions"), &App.Globals.FileTypes.CrontabFileExtensionsString);
    config->Read(wxT("/Project/YamlFileExtensions"), &App.Globals.FileTypes.YamlFileExtensionsString);
    config->Read(wxT("/Project/XmlFileExtensions"), &App.Globals.FileTypes.XmlFileExtensionsString);
    config->Read(wxT("/Project/RubyFileExtensions"), &App.Globals.FileTypes.RubyFileExtensionsString);
    config->Read(wxT("/Project/LuaFileExtensions"), &App.Globals.FileTypes.LuaFileExtensionsString);
    config->Read(wxT("/Project/MarkdownFileExtensions"), &App.Globals.FileTypes.MarkdownFileExtensionsString);
    config->Read(wxT("/Project/BashFileExtensions"), &App.Globals.FileTypes.BashFileExtensionsString);
    config->Read(wxT("/Project/DiffFileExtensions"), &App.Globals.FileTypes.DiffFileExtensionsString);
    config->Read(wxT("/Project/MiscFileExtensions"), &App.Globals.FileTypes.MiscFileExtensionsString);

    App.Globals.Projects.clear();
    wxString key;
    long index;
    int projectIndex = 0;
    bool next = config->GetFirstGroup(key, index);
    while (next) {
        if (key.Find(wxT("Project_")) == 0) {
            t4p::ProjectClass newProject;
            int sourcesCount = 0;

            wxString keyLabel = wxString::Format(wxT("/Project_%d/Label"), projectIndex);
            wxString keyEnabled = wxString::Format(wxT("/Project_%d/IsEnabled"), projectIndex);
            wxString keySourceCount = wxString::Format(wxT("/Project_%d/SourceCount"), projectIndex);
            config->Read(keyLabel, &newProject.Label);
            config->Read(keyEnabled, &newProject.IsEnabled);
            config->Read(keySourceCount, &sourcesCount);
            for (int j = 0; j < sourcesCount; ++j) {
                wxString keyRootPath = wxString::Format(wxT("/Project_%d/Source_%d_RootDirectory"), projectIndex, j);
                wxString keyInclude = wxString::Format(wxT("/Project_%d/Source_%d_IncludeWildcards"), projectIndex, j);
                wxString keyExclude = wxString::Format(wxT("/Project_%d/Source_%d_ExcludeWildcards"), projectIndex, j);

                t4p::SourceClass src;
                wxString rootDir = config->Read(keyRootPath);
                wxString includeWildcards = config->Read(keyInclude);
                wxString excludeWildcards = config->Read(keyExclude);

                src.RootDirectory.AssignDir(rootDir);
                src.SetIncludeWildcards(includeWildcards);
                src.SetExcludeWildcards(excludeWildcards);
                if (src.RootDirectory.IsOk()) {
                    newProject.AddSource(src);
                }
            }
            if (newProject.HasSources()) {
                App.Globals.Projects.push_back(newProject);
                projectIndex++;
            }
        }
        next = config->GetNextGroup(key, index);
    }
}

void t4p::ProjectFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
    wxConfigBase* config = wxConfig::Get();
    config->Write(wxT("/Project/PhpFileExtensions"), App.Globals.FileTypes.PhpFileExtensionsString);
    config->Write(wxT("/Project/CssFileExtensions"), App.Globals.FileTypes.CssFileExtensionsString);
    config->Write(wxT("/Project/SqlFileExtensions"), App.Globals.FileTypes.SqlFileExtensionsString);
    config->Write(wxT("/Project/JsFileExtensions"), App.Globals.FileTypes.JsFileExtensionsString);
    config->Write(wxT("/Project/ConfigFileExtensions"), App.Globals.FileTypes.ConfigFileExtensionsString);
    config->Write(wxT("/Project/CrontabFileExtensions"), App.Globals.FileTypes.CrontabFileExtensionsString);
    config->Write(wxT("/Project/YamlFileExtensions"), App.Globals.FileTypes.YamlFileExtensionsString);
    config->Write(wxT("/Project/XmlFileExtensions"), App.Globals.FileTypes.XmlFileExtensionsString);
    config->Write(wxT("/Project/RubyFileExtensions"), App.Globals.FileTypes.RubyFileExtensionsString);
    config->Write(wxT("/Project/LuaFileExtensions"), App.Globals.FileTypes.LuaFileExtensionsString);
    config->Write(wxT("/Project/MarkdownFileExtensions"), App.Globals.FileTypes.MarkdownFileExtensionsString);
    config->Write(wxT("/ProjectBashFileExtensions"), App.Globals.FileTypes.BashFileExtensionsString);
    config->Write(wxT("/Project/DiffFileExtensions"), App.Globals.FileTypes.DiffFileExtensionsString);
    config->Write(wxT("/Project/MiscFileExtensions"), App.Globals.FileTypes.MiscFileExtensionsString);

    // remove all project from the config
    wxString key;
    long index = 0;
    bool next = config->GetFirstGroup(key, index);
    std::vector<wxString> keysToDelete;
    while (next) {
        if (key.Find(wxT("Project_")) == 0) {
            keysToDelete.push_back(key);
        }
        next = config->GetNextGroup(key, index);
    }
    for (size_t i = 0; i < keysToDelete.size(); ++i) {
        config->DeleteGroup(keysToDelete[i]);
    }

    for (size_t i = 0; i < App.Globals.Projects.size(); ++i) {
        t4p::ProjectClass project = App.Globals.Projects[i];
        wxString keyLabel = wxString::Format(wxT("/Project_%ld/Label"), i);
        wxString keyEnabled = wxString::Format(wxT("/Project_%ld/IsEnabled"), i);
        wxString keySourceCount = wxString::Format(wxT("/Project_%ld/SourceCount"), i);
        config->Write(keyLabel, project.Label);
        config->Write(keyEnabled, project.IsEnabled);
        config->Write(keySourceCount, static_cast<int>(project.Sources.size()));
        for (size_t j = 0; j < project.Sources.size(); ++j) {
            t4p::SourceClass source = project.Sources[j];
            wxString keyRootPath = wxString::Format(wxT("/Project_%ld/Source_%ld_RootDirectory"), i, j);
            wxString keyInclude = wxString::Format(wxT("/Project_%ld/Source_%ld_IncludeWildcards"), i, j);
            wxString keyExclude = wxString::Format(wxT("/Project_%ld/Source_%ld_ExcludeWildcards"), i, j);
            config->Write(keyRootPath, source.RootDirectory.GetFullPath());
            config->Write(keyInclude, source.IncludeWildcardsString());
            config->Write(keyExclude, source.ExcludeWildcardsString());
        }
    }
}

void t4p::ProjectFeatureClass::OnPreferencesExternallyUpdated(wxCommandEvent& event) {
    // start the sequence that will update all global data structures
    // at this point, we dont know which projects need to be reparsed
    // since another instance of triumph4php added them, it is assumed that
    // the other instance has parsed them and built the cache.
    // this instance will just load the cache into memory
    std::vector<t4p::ProjectClass> touchedProjects, removedProjects;
    App.Sequences.ProjectDefinitionsUpdated(touchedProjects, removedProjects);
}

void t4p::ProjectFeatureClass::CreateProject(const wxString& dir, bool doTag) {
    wxFileName rootPath;
    rootPath.AssignDir(dir);
    wxString projectName = rootPath.GetDirs().Last();

    t4p::ProjectClass newProject;
    t4p::SourceClass newSource;
    newSource.RootDirectory = rootPath;
    newSource.SetIncludeWildcards(wxT("*.*"));
    newProject.AddSource(newSource);
    newProject.Label = projectName;
    newProject.IsEnabled = true;

    App.Globals.Projects.push_back(newProject);

    wxCommandEvent evt(t4p::EVENT_APP_PREFERENCES_SAVED);
    App.EventSink.Publish(evt);
    wxConfigBase* config = wxConfig::Get();
    config->Flush();

    // signal that this app has modified the config file, that way the external
    // modification check fails and the user will not be prompted to reload the config
    App.UpdateConfigModifiedTime();

    if (doTag) {
        // user wants to re-tag newly enabled projects
        std::vector<t4p::ProjectClass> empty;
        std::vector<t4p::ProjectClass> touchedProjects;
        touchedProjects.push_back(newProject);
        App.Sequences.ProjectDefinitionsUpdated(touchedProjects, empty);
    }

    // notity the rest of the app that a project has been created.
    wxCommandEvent newProjectEvt(t4p::EVENT_APP_PROJECT_CREATED);
    newProjectEvt.SetString(dir);
    App.EventSink.Publish(newProjectEvt);
}

BEGIN_EVENT_TABLE(t4p::ProjectFeatureClass, FeatureClass)
    EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::ProjectFeatureClass::OnPreferencesSaved)
    EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED, t4p::ProjectFeatureClass::OnPreferencesExternallyUpdated)
END_EVENT_TABLE()
