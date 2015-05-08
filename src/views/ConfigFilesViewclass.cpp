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
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <views/ConfigFilesViewClass.h>#include <features/ConfigFilesFeatureClass.h>

t4p::ConfigFilesViewClass::ConfigFilesViewClass(t4p::ConfigFilesFeatureClass& feature): Feature(feature), ConfigMenu(NULL)
{
}void t4p::ConfigFilesViewClass::AddNewMenu(wxMenuBar* menuBar) {	ConfigMenu = new wxMenu();	menuBar->Append(ConfigMenu, _("Project Configs"));	// at this point the projects are not loaded yet must wait	// until projects are loaded so that we can load 	// the detected config files}void t4p::ConfigFilesViewClass::RebuildMenu() {	while (ConfigMenu->GetMenuItemCount() > 0) {		// use destroy because these are all submenus		ConfigMenu->Destroy(ConfigMenu->FindItemByPosition(0)->GetId());	}	std::vector<t4p::ConfigFilesFeaturePairClass> configPairs;	bool hasPairs = Feature.BuildConfigPairs(configPairs);	if (!hasPairs) {		return;	}	std::vector<t4p::ConfigFilesFeaturePairClass>::const_iterator configPair;	std::vector<t4p::ConfigTagClass>::const_iterator config;	size_t i = 0;	// make sure to not make more menu items than are allowed.	for (configPair = configPairs.begin(); 			configPair != configPairs.end() && i < t4p::ConfigFilesFeatureClass::MAX_CONFIG_MENU_ITEMS; 			++configPair) {		wxMenu* submenu = new wxMenu();		for (config = configPair->ConfigTags.begin(); config != configPair->ConfigTags.end(); ++config) {			submenu->Append(t4p::CONFIG_DETECTORS + i, 				config->MenuLabel() , 				config->ConfigFileName.GetFullPath(), 				wxITEM_NORMAL			);			i++;			if (i >= t4p::ConfigFilesFeatureClass::MAX_CONFIG_MENU_ITEMS) {				break;			}		}		wxString projectLabel(configPair->ProjectLabel);		projectLabel.Replace(wxT("&"), wxT("&&"));		ConfigMenu->AppendSubMenu(submenu, projectLabel);	}}void t4p::ConfigFilesViewClass::OnDetectorDbInitComplete(t4p::ActionEventClass& event) {	RebuildMenu();}void t4p::ConfigFilesViewClass::OnConfigFilesDetected(t4p::ActionEventClass& event) {	RebuildMenu();}

void t4p::ConfigFilesViewClass::OnConfigMenuItem(wxCommandEvent& event) {
	size_t index = event.GetId() - t4p::CONFIG_DETECTORS;
	Feature.OpenConfigItem(index);
}
BEGIN_EVENT_TABLE(t4p::ConfigFilesViewClass, t4p::FeatureViewClass) 
	EVT_MENU_RANGE(
		t4p::CONFIG_DETECTORS, 
		t4p::CONFIG_DETECTORS + t4p::ConfigFilesFeatureClass::MAX_CONFIG_MENU_ITEMS, 
		t4p::ConfigFilesViewClass::OnConfigMenuItem
	)	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_DETECTOR_DB_INIT,  t4p::ConfigFilesViewClass::OnDetectorDbInitComplete)	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_CONFIG_TAG_DETECTOR, t4p::ConfigFilesViewClass::OnConfigFilesDetected)END_EVENT_TABLE()