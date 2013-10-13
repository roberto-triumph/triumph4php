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
#ifndef __ENVIRONMENTFEATURECLASS_H__
#define __ENVIRONMENTFEATURECLASS_H__

#include <features/FeatureClass.h>
#include <features/BackgroundFileReaderClass.h>
#include <globals/EnvironmentClass.h>
#include <features/wxformbuilder/EnvironmentFeatureForms.h>

namespace mvceditor {

/**
 * Event that will get generated by the ApacheFileReaderClass once an 
 * apache config file has been found.
 */
extern const wxEventType EVENT_APACHE_FILE_READ_COMPLETE;

/**
 * Event that will get generated by the ApacheFileReaderClass once an 
 * apache config file has been found.
 * This event contains the location of the Apache config file.
 */
class ApacheFileReadCompleteEventClass : public wxEvent {

public:

	mvceditor::ApacheClass Apache;

	ApacheFileReadCompleteEventClass(int eventId, const mvceditor::ApacheClass& apache);

	wxEvent* Clone() const;
	
};

typedef void (wxEvtHandler::*ApacheFileReadCompleteEventClassFunction)(ApacheFileReadCompleteEventClass&);

#define EVT_APACHE_FILE_READ_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_APACHE_FILE_READ_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( ApacheFileReadCompleteEventClassFunction, & fn ), (wxObject *) NULL ),

	
/**
 * A small class that will search for the Apache config files
 * in a background thread. The results of the config file parsing
 * will be wxPost'ed
 */
class ApacheFileReaderClass : public BackgroundFileReaderClass {

public:
	
	/**
	 * @param handler the event handler for the EVENT_APACHE_FILE_READ_COMPLETE and EVENT_WORK_* events
	 */
	ApacheFileReaderClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);
	
	/**
	 * setup the background thread (does not start the thread)
	 * @param startDirectory the directory to start looking in
	 */
	bool Init(const wxString& startDirectory); 

	wxString GetLabel() const;
	
protected:

	bool BackgroundFileMatch(const wxString& file);
	
	bool BackgroundFileRead(DirectorySearchClass& search);

private:

	ApacheClass ApacheResults;

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
	ApacheEnvironmentPanelClass(wxWindow* parent, mvceditor::RunningThreadsClass& runningThreads, EnvironmentClass& environment);
	
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
	mvceditor::RunningThreadsClass& RunningThreads;
		
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
	void OnApacheFileReadComplete(mvceditor::ApacheFileReadCompleteEventClass& event);
	
	/**
	 * When this panel is resized automatically re-adjust the wrapping the label 
	 */
	void OnResize(wxSizeEvent& event);
	
	/**
	 * tick the gauge here
	 */
	void OnActionProgress(mvceditor::ActionProgressEventClass& event);

	/**
	 * stop the gauge here
	 */
	void OnActionComplete(mvceditor::ActionEventClass& event);
	
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
	void OnNoPhpCheck(wxCommandEvent& event);
	
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

/**
 * This feature will handle the application stack configuration options.
 */
class EnvironmentFeatureClass : public FeatureClass {

public:

	EnvironmentFeatureClass(mvceditor::AppClass& app);

	/**
	 * Add the environment dialogs to the preferences notebook
	 */
	void AddPreferenceWindow(wxBookCtrlBase* parent);
	
	/**
	 * Handle the menu item
	 */
	void OnMenuEnvironment(wxCommandEvent& event);
	
private:

	void OnPreferencesSaved(wxCommandEvent& event);
	
	DECLARE_EVENT_TABLE()
};

}
#endif // __ENVIRONMENTFEATURECLASS_H__
