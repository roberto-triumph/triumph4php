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
#include <globals/Number.h>
#include <code_control/CodeControlClass.h>
#include <Triumph.h>

t4p::ConfigFilesFeaturePairClass::ConfigFilesFeaturePairClass()
: ProjectLabel()
, ConfigTags() {
	
}

t4p::ConfigFilesFeaturePairClass::ConfigFilesFeaturePairClass(const t4p::ConfigFilesFeaturePairClass& src)
: ProjectLabel()
, ConfigTags() {
	Copy(src);
}

t4p::ConfigFilesFeaturePairClass& t4p::ConfigFilesFeaturePairClass::operator=(const t4p::ConfigFilesFeaturePairClass& src) {
	Copy(src);
	return *this;
}

void t4p::ConfigFilesFeaturePairClass::Copy(const t4p::ConfigFilesFeaturePairClass& src) {
	ProjectLabel = src.ProjectLabel;
	ConfigTags = src.ConfigTags;
}

t4p::ConfigFilesFeatureClass::ConfigFilesFeatureClass(t4p::AppClass& app)
	: FeatureClass(app) 
	, ConfigTags() {

}

bool t4p::ConfigFilesFeatureClass::BuildConfigPairs(std::vector<t4p::ConfigFilesFeaturePairClass>& configPairs) {
	configPairs.clear();
	ConfigTags.clear();

	// get the menus; need to keep them in memory
	// because we use the index to know which menu was selected
	std::vector<t4p::ConfigTagClass> allConfigTags;
	t4p::ConfigTagFinderClass finder;
	if (App.Globals.DetectorCacheDbFileName.IsOk()) {
		finder.InitSession(&App.Globals.DetectorCacheSession);
		std::vector<wxFileName> sourceDirs = App.Globals.AllEnabledSourceDirectories();
		allConfigTags = finder.All(sourceDirs);
	}
	if (!allConfigTags.empty()) {

		// doing 2 for-loops because we need the index into ConfigTags to line up
		// with the way we create the menu items
		std::vector<t4p::ProjectClass>::const_iterator project;
		for (project = App.Globals.Projects.begin(); project != App.Globals.Projects.end(); ++project) {
			if (project->IsEnabled) {
				t4p::ConfigFilesFeaturePairClass pair;
				pair.ProjectLabel = project->Label;
				std::vector<t4p::ConfigTagClass>::const_iterator configTag;
				for (configTag = allConfigTags.begin(); configTag != allConfigTags.end(); ++configTag) {
					wxString fullPath = configTag->ConfigFileName.GetFullPath();
					if (project->IsASourceFile(fullPath, App.Globals.FileTypes)) {
						ConfigTags.push_back(*configTag);
						pair.ConfigTags.push_back(*configTag);
					}
				}
				if (!pair.ConfigTags.empty()) {
					configPairs.push_back(pair);
				}
			}
		}
	}
	return !configPairs.empty();
}

void t4p::ConfigFilesFeatureClass::OpenConfigItem(size_t index) {
	if (t4p::NumberLessThan(index, t4p::ConfigFilesFeatureClass::MAX_CONFIG_MENU_ITEMS) && index <ConfigTags.size()) {
		wxFileName fileName = ConfigTags[index].ConfigFileName;
		t4p::OpenFileCommandEventClass cmd(fileName.GetFullPath());
		App.EventSink.Publish(cmd);
	}
}

void t4p::ConfigFilesFeatureClass::OnFileSaved(t4p::CodeControlEventClass& event) {
	std::vector<t4p::ConfigTagClass>::const_iterator configTag;
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

BEGIN_EVENT_TABLE(t4p::ConfigFilesFeatureClass, t4p::FeatureClass) 
	EVT_APP_FILE_SAVED(t4p::ConfigFilesFeatureClass::OnFileSaved)
END_EVENT_TABLE()
