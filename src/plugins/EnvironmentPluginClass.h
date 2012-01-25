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
#ifndef __environmentpluginclass__
#define __environmentpluginclass__

#include <PluginClass.h>
#include <environment/EnvironmentClass.h>
#include <plugins/wxformbuilder/EnvironmentGeneratedDialogClass.h>

namespace mvceditor {
	
/**
 * Panel that shows the apache virtual host config. It will also
 * have functionality to scan a directory for apache config files
 */
class ApacheEnvironmentPanelClass : public ApacheEnvironmentPanelGeneratedClass {

protected:
	// Handlers for EnvironmentGeneratedDialogClass events.
	void OnScanButton(wxCommandEvent& event);
	void OnOkButton(wxCommandEvent& event);

public:
	/** Constructor */
	ApacheEnvironmentPanelClass(wxWindow* parent, EnvironmentClass& environment);

private:
	
	/** 
	 * The configuration class
	 * 
	 * @var EnvironmentClass
	 */
	EnvironmentClass& Environment;
	
	/**
	 * Used to recurse through file system directories.
	 * 
	 * @var DirectorySearch
	 */
	DirectorySearchClass DirectorySearch;
	
	/**
	 * populate the dialog according to the ApacheClass settings
	 */
	void Populate();
	
	/**
	 * When idle and user clicked the search button, do the search
	 */
	void OnIdle(wxIdleEvent& event);
	
	/**
	 * When this panel is resized automatically re-adjust the wrapping the label 
	 */
	void OnResize(wxSizeEvent& event);
		
	enum States {
		FREE = 0,
		SEARCHING,
		OPENING_FILE
	};
	
	/**
	 * The current state.  We will be searching through directories in the same main application thread instead of
	 * another thread. a whole new thread is not really warranted in this case.
	 */
	States State;
	
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
	void Apply();
	
protected:

	/**
	 * When this panel is resized automatically re-adjust the wrapping the label 
	 */
	void OnResize(wxSizeEvent& event);
	
	void OnRemoveSelectedWebBrowser(wxCommandEvent& event);
	
	void OnAddWebBrowser(wxCommandEvent& event);
	
	void OnEditSelectedWebBrowser(wxCommandEvent& event);
	
private:

	/** 
	 * The configuration class
	 * 
	 * @var EnvironmentClass
	 */
	EnvironmentClass& Environment;
	
	/**
	 * The web browsers being modified. this is the map that is the recipient
	 * of all of the user's operations; it will be copied only when the
	 * user clicks OK.
	 */
	std::map<wxString, wxFileName> EditedWebBrowsers;
};

/**
 * Panel that shows the PHP binary path locations
 */
class PhpEnvironmentPanelClass : public PhpEnvironmentPanelGeneratedClass {

public:
	
	PhpEnvironmentPanelClass(wxWindow* parent, EnvironmentClass& environment);
	
protected:

	/**
	 * Handle the file picker changed event.
	 */
	void OnPhpFileChanged(wxFileDirPickerEvent& event);
	
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

	WebBrowserCreateDialogClass(wxWindow* parent, std::map<wxString, wxFileName> existingBrowsers, 
		wxString& name, wxFileName& webBrowserFileName);
	
protected:

	void OnOkButton(wxCommandEvent& event);
	
	/** 
	 * to prevent multiple browsers with the same name
	 */
	std::map<wxString, wxFileName> ExistingBrowsers;
	
	/**
	 * to transfer the chosen file path
	 */
	wxFileName& WebBrowserFileName;
	
};

class EnvironmentDialogClass : public wxPropertySheetDialog {

public:

	EnvironmentDialogClass(wxWindow* parent, EnvironmentClass& environment);
	
	/**
	 * make sure the layout is refreshed; needed for the sizers to expand to their proper size
	 */
	void Prepare();
	
	/**
	 * when user OKs the dialog transfer all data to the Environment data structure
	 */
	void OnOkButton(wxCommandEvent& event);
	
private:

	WebBrowserEditPanelClass* WebBrowserPanel;
	
	DECLARE_EVENT_TABLE()
};

/**
 * This plugin will handle the application stack configuration options.
 */
class EnvironmentPluginClass : public PluginClass {

public:

	EnvironmentPluginClass();
	
	/**
	 * Add the environment sub-menu.
	 * 
	 * @return wxMenuBar* the menu bar to add to.
	 */
	void AddProjectMenuItems(wxMenu* projectMenu);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
	/**
	 * Handle the menu item
	 */
	void OnMenuEnvironment(wxCommandEvent& event);
	
private:
	
	DECLARE_EVENT_TABLE()
};

}
#endif // __environmentpluginclass__
