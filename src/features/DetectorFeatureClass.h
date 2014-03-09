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
#ifndef __T4P_DETECTORFEATURECLASS_H__
#define __T4P_DETECTORFEATURECLASS_H__

#include <features/FeatureClass.h>
#include <actions/ActionClass.h>
#include <features/wxformbuilder/DetectorFeatureForms.h>
#include <wx/imaglist.h>

namespace t4p {

class DetectorClass {

public:

	DetectorClass();

	virtual ~DetectorClass();

	virtual bool CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project) = 0;

	virtual wxString TestCommandLine(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project,
		const wxString& detectorScriptFullPath) = 0;

	virtual wxFileName LocalRootDir()  = 0;

	virtual wxFileName GlobalRootDir()  = 0;

	virtual wxFileName SkeletonFile() = 0;

	virtual wxString Label() = 0;

	virtual void HelpMessage()  = 0;

};

class UrlTagDetectorClass : public t4p::DetectorClass {
public:

	UrlTagDetectorClass();

	bool CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project);

	wxString TestCommandLine(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project,
		const wxString& detectorScriptFullPath);

	wxFileName LocalRootDir();

	wxFileName GlobalRootDir();

	wxFileName SkeletonFile();

	wxString Label();

	void HelpMessage();
};

class TemplateFileTagsDetectorClass : public t4p::DetectorClass {
public:

	TemplateFileTagsDetectorClass();

	bool CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project);

	wxString TestCommandLine(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project,
		const wxString& detectorScriptFullPath);

	wxFileName LocalRootDir();

	wxFileName GlobalRootDir();

	wxFileName SkeletonFile();

	wxString Label();

	void HelpMessage();
};

class TagDetectorClass : public t4p::DetectorClass {
public:

	TagDetectorClass();

	bool CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project);

	wxString TestCommandLine(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project,
		const wxString& detectorScriptFullPath);

	wxFileName LocalRootDir();

	wxFileName GlobalRootDir();

	wxFileName SkeletonFile();

	wxString Label();

	void HelpMessage();
};

class DatabaseTagDetectorClass : public t4p::DetectorClass {
public:

	DatabaseTagDetectorClass();

	bool CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project);

	wxString TestCommandLine(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project,
		const wxString& detectorScriptFullPath);

	wxFileName LocalRootDir();

	wxFileName GlobalRootDir();

	wxFileName SkeletonFile();

	wxString Label();

	void HelpMessage();
};

class ConfigTagDetectorClass : public t4p::DetectorClass {
public:

	ConfigTagDetectorClass();

	bool CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project);

	wxString TestCommandLine(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project,
		const wxString& detectorScriptFullPath);

	wxFileName LocalRootDir();

	wxFileName GlobalRootDir();

	wxFileName SkeletonFile();

	wxString Label();

	void HelpMessage();
};

class DetectorTreeHandlerClass : public wxEvtHandler {

public:

	/**
	 * @param detectorTree the wxTreeCtrl that renders the files. pointer will NOT be
	 *        owned by this class
	 * @param testButton pointer will NOT be owned by this class
	 * @param addButton pointer will NOT be owned by this class
	 * @param helpButton pointer will NOT be owned by this class
	 * @param detector to get the file locations. pointer will NOT be owned by this class
	 * @param globals to fill the projects choice list 
	 * @param eventSink to send test button click events to
	 * @param rootImage the bitmap to be shown in the root of the tree
	 */
	DetectorTreeHandlerClass(wxTreeCtrl* detectorTree, wxButton* testButton, wxButton* addButton,
		wxButton* helpButton, wxChoice* projectChoice,
		t4p::DetectorClass* detector,
		t4p::GlobalsClass& globals, t4p::EventSinkClass& eventSink,
		const wxBitmap& rootImage);

	~DetectorTreeHandlerClass();

	void Init();

	void UpdateProjects();

	// context menu handlers for the URL detector tree
	void OnMenuOpenDetector(wxCommandEvent& event);
	void OnMenuRenameDetector(wxCommandEvent& event);
	void OnMenuDeleteDetector(wxCommandEvent& event);

private:
	void FillSubTree(const wxString& detectorRootDir, wxTreeItemId treeItemDir);

	// tree control event handlers
	void OnTreeItemActivated(wxTreeEvent& event);
	void OnTreeItemRightClick(wxTreeEvent& event);
	void OnTreeItemDelete(wxTreeEvent& event);
	void OnTreeItemBeginLabelEdit(wxTreeEvent& event);
	void OnTreeItemEndLabelEdit(wxTreeEvent& event);

	// button click handlers
	void OnHelpButton(wxCommandEvent& event);
	void OnTestButton(wxCommandEvent& event);
	void OnAddButton(wxCommandEvent& event);

	// indices into ImageList
	enum {
		IMAGE_ROOT_DETECTOR = 0,
		IMAGE_FOLDER,
		IMAGE_FOLDER_OPEN,
		IMAGE_SCRIPT
	};
	
	/**
	 * this pointer will be managed by the tree control, since the tree control
	 * may use the pointer in the destructor.
	 */
	wxImageList* ImageList;
	wxTreeCtrl* DetectorTree;
	wxButton* TestButton;
	wxButton* AddButton;
	wxButton* HelpButton;
	wxChoice* ProjectChoice;

	t4p::DetectorClass* Detector;

	/**
	 * to access the project list
	 */
	t4p::GlobalsClass& Globals;

	/**
	 * to send app commands to open and run a file
	 */
	t4p::EventSinkClass& EventSink;

}; 

class UrlTagDetectorPanelClass : public UrlDetectorPanelGeneratedClass {

public:

	/**
	 * @param parent window parent
	 * @param id window ID
	 * @param globals to access the projects list
	 * @param eventSink to send the app file and run commands
	 */
	UrlTagDetectorPanelClass(wxWindow* parent, int id, t4p::GlobalsClass& globals,
		t4p::EventSinkClass& eventSink);

	~UrlTagDetectorPanelClass();

	/**
	 * This should be called when the detector tree needs to be updated.
	 */
	void Init();

	/**
	 * updates the project choice with the given projects. This should be called whenever
	 * the global project list has been changed.
	 * The project list will be read from GlobalsClass
	 */
	void UpdateProjects();

private:

	t4p::UrlTagDetectorClass Detector;

	t4p::DetectorTreeHandlerClass Handler;
};

class TemplateFileTagsDetectorPanelClass : public TemplateFilesDetectorPanelGeneratedClass {

public:

	/**
	 * @param parent window parent
	 * @param id window ID
	 * @param globals to access the projects list
	 * @param eventSink to send the app file and run commands
	 * @param runningThreads  used to run the background processes.
	 */
	TemplateFileTagsDetectorPanelClass(wxWindow* parent, int id, t4p::GlobalsClass& globals,
		t4p::EventSinkClass& eventSink, t4p::RunningThreadsClass& runningThreads);

	~TemplateFileTagsDetectorPanelClass();

	/**
	 * This should be called when the detector tree needs to be updated.
	 */
	void Init();

	/**
	 * updates the project choice with the given projects. This should be called whenever
	 * the global project list has been changed.
	 * The project list will be read from GlobalsClass
	 */
	void UpdateProjects();

protected:

	void OnChooseUrlButton(wxCommandEvent& event);

private:

	t4p::TemplateFileTagsDetectorClass Detector;

	t4p::DetectorTreeHandlerClass Handler;

	/**
	 * The URL to use for testing the detector
	 */
	t4p::UrlTagClass TestUrl;

	/**
	 * to access the url resource list
	 */
	t4p::GlobalsClass& Globals;

	t4p::RunningThreadsClass& RunningThreads;
};

class TagDetectorPanelClass : public TagDetectorPanelGeneratedClass {

public:

	/**
	 * @param parent window parent
	 * @param id window ID
	 * @param globals to access the projects list
	 * @param eventSink to send the app file and run commands
	 */
	TagDetectorPanelClass(wxWindow* parent, int id, t4p::GlobalsClass& globals,
		t4p::EventSinkClass& eventSink);

	~TagDetectorPanelClass();

	/**
	 * This should be called when the detector tree needs to be updated.
	 */
	void Init();

	/**
	 * updates the project choice with the given projects. This should be called whenever
	 * the global project list has been changed.
	 * The project list will be read from GlobalsClass
	 */
	void UpdateProjects();

protected:

	void OnChooseUrlButton(wxCommandEvent& event);

private:

	t4p::TagDetectorClass Detector;

	t4p::DetectorTreeHandlerClass Handler;
};

class DatabaseTagDetectorPanelClass : public DatabaseDetectorPanelGeneratedClass {

public:

	/**
	 * @param parent window parent
	 * @param id window ID
	 * @param globals to access the projects list
	 * @param eventSink to send the app file and run commands
	 */
	DatabaseTagDetectorPanelClass(wxWindow* parent, int id, t4p::GlobalsClass& globals,
		t4p::EventSinkClass& eventSink);

	~DatabaseTagDetectorPanelClass();

	/**
	 * This should be called when the detector tree needs to be updated.
	 */
	void Init();

	/**
	 * updates the project choice with the given projects. This should be called whenever
	 * the global project list has been changed.
	 * The project list will be read from GlobalsClass
	 */
	void UpdateProjects();

private:

	t4p::DatabaseTagDetectorClass Detector;

	t4p::DetectorTreeHandlerClass Handler;
};

class ConfigTagDetectorPanelClass : public ConfigDetectorPanelGeneratedClass {

public:

	/**
	 * @param parent window parent
	 * @param id window ID
	 * @param globals to access the projects list
	 * @param eventSink to send the app file and run commands
	 */
	ConfigTagDetectorPanelClass(wxWindow* parent, int id, t4p::GlobalsClass& globals,
		t4p::EventSinkClass& eventSink);

	~ConfigTagDetectorPanelClass();

	/**
	 * This should be called when the detector tree needs to be updated.
	 */
	void Init();

	/**
	 * updates the project choice with the given projects. This should be called whenever
	 * the global project list has been changed.
	 * The project list will be read from GlobalsClass
	 */
	void UpdateProjects();

private:

	t4p::ConfigTagDetectorClass Detector;

	t4p::DetectorTreeHandlerClass Handler;
};

class DetectorFeatureClass : public t4p::FeatureClass {

public:

	DetectorFeatureClass(t4p::AppClass& app);

	void AddViewMenuItems(wxMenu* viewMenu);

	void AddNewMenu(wxMenuBar* menuBar);

private:

	void OnViewUrlDetectors(wxCommandEvent& event);
	void OnViewTemplateFileDetectors(wxCommandEvent& event);
	void OnViewTagDetectors(wxCommandEvent& event);
	void OnViewDatabaseDetectors(wxCommandEvent& event);
	void OnViewConfigDetectors(wxCommandEvent& event);

	void OnRunUrlDetectors(wxCommandEvent& event);
	void OnRunTemplateFileDetectors(wxCommandEvent& event);
	void OnRunTagDetectors(wxCommandEvent& event);
	void OnRunDatabaseDetectors(wxCommandEvent& event);
	void OnRunConfigDetectors(wxCommandEvent& event);
	
	void OnPreferencesSaved(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()

};


}

#endif
