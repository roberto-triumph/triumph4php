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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <features/ConfigFilesFeatureClass.h>
#include <actions/ConfigTagDetectorActionClass.h>
#include <MvcEditor.h>

// max amount of menu items to show
// the 100 is due to the menu ids allocated to each feature in
// the FeatureClass MenuIds enum
static size_t MAX_CONFIG_MENU_ITEMS = 100;

mvceditor::ConfigFilesFeatureClass::ConfigFilesFeatureClass(mvceditor::AppClass& app)
	: FeatureClass(app) 
	, ConfigPairs()
	, ConfigTags()
	, ConfigMenu(NULL) {

}

void mvceditor::ConfigFilesFeatureClass::AddNewMenu(wxMenuBar* menuBar) {
	ConfigMenu = new wxMenu(0);
	menuBar->Append(ConfigMenu, _("Project Configs"));

	// at this point the projects are not loaded yet must wait
	// until projects are loaded so that we can load 
	// the detected config files
}

void mvceditor::ConfigFilesFeatureClass::RebuildMenu() {
	ConfigPairs.clear();
	ConfigTags.clear();

	// get the menus; need to keep them in memory
	// because we use the index to know which menu was selected
	std::vector<mvceditor::ConfigTagClass> allConfigTags;
	mvceditor::ConfigTagFinderClass finder;
	if (App.Globals.DetectorCacheDbFileName.IsOk()) {
		finder.InitSession(&App.Globals.DetectorCacheSession);
		allConfigTags = finder.All();
	}
	if (!allConfigTags.empty()) {

		// doing 2 for-loops because we need the index into ConfigTags to line up
		// with the way we create the menu items
		std::vector<mvceditor::ProjectClass>::const_iterator project;
		for (project = App.Globals.Projects.begin(); project != App.Globals.Projects.end(); ++project) {
			if (project->IsEnabled) {
				ConfigPair pair;
				pair.ProjectLabel = project->Label;
				std::vector<mvceditor::ConfigTagClass>::const_iterator configTag;
				for (configTag = allConfigTags.begin(); configTag != allConfigTags.end(); ++configTag) {
					wxString fullPath = configTag->ConfigFileName.GetFullPath();
					if (project->IsASourceFile(fullPath)) {
						ConfigTags.push_back(*configTag);
						pair.ConfigTags.push_back(*configTag);
					}
				}
				if (!pair.ConfigTags.empty()) {
					ConfigPairs.push_back(pair);
				}
			}
		}
	}
	while (ConfigMenu->GetMenuItemCount() > 0) {

		// use destroy because these are all submenus
		ConfigMenu->Destroy(ConfigMenu->FindItemByPosition(0)->GetId());
	}
	std::vector<ConfigPair>::const_iterator configPair;
	std::vector<mvceditor::ConfigTagClass>::const_iterator config;
	size_t i = 0;

	// make sure to not make more menu items than are allowed.
	for (configPair = ConfigPairs.begin(); configPair != ConfigPairs.end() && i < MAX_CONFIG_MENU_ITEMS; ++configPair) {
		wxMenu* submenu = new wxMenu(0);
		for (config = configPair->ConfigTags.begin(); config != configPair->ConfigTags.end(); ++config) {
			submenu->Append(mvceditor::CONFIG_DETECTORS + i, config->MenuLabel() , config->ConfigFileName.GetFullPath(), wxITEM_NORMAL);
			i++;
			if (i >= MAX_CONFIG_MENU_ITEMS) {
				break;
			}
		}
		wxString projectLabel(configPair->ProjectLabel);
		projectLabel.Replace(wxT("&"), wxT("&&"));
		ConfigMenu->AppendSubMenu(submenu, projectLabel);
	}
}

void mvceditor::ConfigFilesFeatureClass::OnConfigFilesDetected(mvceditor::ActionEventClass& event) {
	RebuildMenu();
}

void mvceditor::ConfigFilesFeatureClass::OnConfigMenuItem(wxCommandEvent& event) {
	size_t index = event.GetId() - mvceditor::CONFIG_DETECTORS;
	if (index >= 0 && index < MAX_CONFIG_MENU_ITEMS && index < ConfigTags.size()) {
		wxFileName fileName = ConfigTags[index].ConfigFileName;
		wxCommandEvent cmd(mvceditor::EVENT_CMD_FILE_OPEN);
		cmd.SetString(fileName.GetFullPath());
		App.EventSink.Publish(cmd);
	}
}

void mvceditor::ConfigFilesFeatureClass::OnFileSaved(mvceditor::CodeControlEventClass& event) {
	std::vector<mvceditor::ConfigTagClass>::const_iterator configTag;
	bool isConfigFileSaved = false;
	wxString fileSaved = event.GetCodeControl()->GetFileName();
	for (configTag = ConfigTags.begin(); configTag != ConfigTags.end(); ++configTag) {
		isConfigFileSaved = configTag->ConfigFileName == fileSaved;
		if (isConfigFileSaved) {
			break;
		}
	}
	if (isConfigFileSaved) {
		App.Sequences.DatabaseDetection();
	}
}


BEGIN_EVENT_TABLE(mvceditor::ConfigFilesFeatureClass, mvceditor::FeatureClass) 
	EVT_ACTION_COMPLETE(mvceditor::ID_EVENT_ACTION_CONFIG_TAG_DETECTOR, mvceditor::ConfigFilesFeatureClass::OnConfigFilesDetected)
	EVT_MENU_RANGE(mvceditor::CONFIG_DETECTORS, mvceditor::CONFIG_DETECTORS + MAX_CONFIG_MENU_ITEMS, mvceditor::ConfigFilesFeatureClass::OnConfigMenuItem)
	EVT_APP_FILE_SAVED(mvceditor::ConfigFilesFeatureClass::OnFileSaved)
END_EVENT_TABLE()