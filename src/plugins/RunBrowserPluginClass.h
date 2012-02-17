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
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __RUNBROWSERPLUGINCLASS_H__
#define __RUNBROWSERPLUGINCLASS_H__

#include <PluginClass.h>
#include <plugins/wxformbuilder/RunBrowserPluginGeneratedClass.h>
#include <php_frameworks/FrameworkDetectorClass.h>
#include <environment/UrlResourceClass.h>
#include <wx/string.h>
#include <vector>

namespace mvceditor {
	
/** 
 * small class that will be used in the UrlChoice Dialog; to let the user
 * select a URL to run, along with any parameters the user wants
 */
class UrlChoiceClass {
public:

	std::vector<UrlResourceClass> UrlList;
	
	wxString Extra;
	
	int ChosenIndex;
	
	/**
	 * @param urlList list of URLs that the user will choose from.  The list should be the result of the URL
	 * detector
	 * @param fileName the file that corresponds to the given URLs (the file name helps to determine the virtual host)
	 * @param environment to get the virtual host
	 */
	UrlChoiceClass(const std::vector<UrlResourceClass>& urlList, const wxString& fileName, EnvironmentClass* environment);
	
	/**
	 * @return UrlResourceClass URL [that the user chose] plus the Extra
	 */
	UrlResourceClass ChosenUrl() const;
	
};

/**
 * small dialog that allows the user to choose a URL to run
 */
class ChooseUrlDialogClass : public ChooseUrlDialogGeneratedClass {
	
public:
	
	UrlChoiceClass& UrlChoice;

	ChooseUrlDialogClass(wxWindow* parent, UrlChoiceClass& urlChoice);
	
protected:

	void OnOkButton(wxCommandEvent& event);
	
	/**
	 * updates the URL label so that the user can see the exact URL to 
	 * be opened in the browser
	 */
	void OnUpdateUi(wxUpdateUIEvent& event);
};
	
class RunBrowserPluginClass : public PluginClass {

public:

	RunBrowserPluginClass();
	
	~RunBrowserPluginClass();
	
	void AddToolsMenuItems(wxMenu* toolsMenu);
	
	void AddToolBarItems(wxAuiToolBar* toolBar);
	
	void AddWindows();
	
	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
protected:

	void LoadPreferences(wxConfigBase* config);

private:

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
	 * When URL detection succeeds, the URLs are added
	 * to the dialog that is shown to the user.
	 */
	void OnUrlDetectionComplete(UrlDetectedEventClass& event);
	
	/**
	 * If URL detection fails, then most likely this is an environment issue
	 * (PHP binary not found)
	 */
	void OnUrlDetectionFailed(wxCommandEvent& event);
	
	/**
	 * A local instance of the detector so that we can resolve
	 * framework URLs according to each specific PHP framework routing rules
	 * Needs to be local because this class needs to be the event handler
	 * for the url detection (event handler is given in the PhpFrameworkDetector
	 * class constructor)
	 */
	PhpFrameworkDetectorClass* PhpFrameworks;
	
	wxMenuItem* RunInBrowser;
	
	wxAuiToolBar* BrowserToolbar;
	
	DECLARE_EVENT_TABLE()
};

}

#endif