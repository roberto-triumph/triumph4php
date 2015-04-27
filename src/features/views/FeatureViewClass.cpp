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
#include <features/views/FeatureViewClass.h>

t4p::FeatureViewClass::FeatureViewClass() 
: wxEvtHandler() {}void t4p::FeatureViewClass::InitWindow(		StatusBarWithGaugeClass* statusBarWithGauge, 		NotebookClass* notebook, wxAuiNotebook* toolsNotebook, 		wxAuiNotebook* outlineNotebook, wxAuiManager* auiManager, 		wxMenuBar* menuBar) {	AuiManager = auiManager;	StatusBarWithGauge = statusBarWithGauge;	Notebook = notebook;	ToolsNotebook = toolsNotebook;	OutlineNotebook = outlineNotebook;	MenuBar = menuBar;
}void t4p::FeatureViewClass::AddViewMenuItems(wxMenu* viewMenu) {}void t4p::FeatureViewClass::AddSearchMenuItems(wxMenu* searchMenu) {}void t4p::FeatureViewClass::AddFileMenuItems(wxMenu* fileMenu) {}void t4p::FeatureViewClass::AddEditMenuItems(wxMenu* editMenu) {}void t4p::FeatureViewClass::AddHelpMenuItems(wxMenu* editMenu) {}void t4p::FeatureViewClass::AddNewMenu(wxMenuBar* menuBar) {}void t4p::FeatureViewClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {}void t4p::FeatureViewClass::AddKeyboardShortcuts(std::vector<t4p::DynamicCmdClass>& shortcuts) {}void t4p::FeatureViewClass::AddWindows() {}void t4p::FeatureViewClass::AddPreferenceWindow(wxBookCtrlBase* parent) {}void t4p::FeatureViewClass::AddToolBarItems(wxAuiToolBar* toolBar) {}void t4p::FeatureViewClass::AddDynamicCmd(std::map<int, wxString> menuItemIds, std::vector<t4p::DynamicCmdClass>& shortcuts) {	for (std::map<int, wxString>::iterator it = menuItemIds.begin(); it != menuItemIds.end(); ++it) {		wxMenuItem* item = MenuBar->FindItem(it->first);		wxASSERT_MSG(item, wxT("Menu item not found:") + it->second);		t4p::DynamicCmdClass cmd(item, it->second);		shortcuts.push_back(cmd);	}}