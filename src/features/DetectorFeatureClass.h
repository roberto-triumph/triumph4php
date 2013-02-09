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
#ifndef __MVCEDITORDETECTORFEATURECLASS_H__
#define __MVCEDITORDETECTORFEATURECLASS_H__

#include <features/FeatureClass.h>
#include <features/wxformbuilder/DetectorFeatureForms.h>

namespace mvceditor {

class DetectorClass {

public:

	DetectorClass();

	virtual ~DetectorClass();

	virtual bool CanTest(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project) = 0;

	virtual wxString TestCommandLine(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project,
		const wxString& detectorScriptFullPath) = 0;

	virtual wxFileName LocalRootDir()  = 0;

	virtual wxFileName GlobalRootDir()  = 0;

	virtual wxFileName SkeletonFile() = 0;

	virtual wxString Label() = 0;

	virtual wxString HelpMessage()  = 0;

};

class UrlDetectorClass : public mvceditor::DetectorClass {
public:

	UrlDetectorClass();

	bool CanTest(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project);

	wxString TestCommandLine(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project,
		const wxString& detectorScriptFullPath);

	wxFileName LocalRootDir();

	wxFileName GlobalRootDir();

	wxFileName SkeletonFile();

	wxString Label();

	wxString HelpMessage();
};

class TemplateFilesDetectorClass : public mvceditor::DetectorClass {
public:

	TemplateFilesDetectorClass();

	bool CanTest(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project);

	wxString TestCommandLine(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project,
		const wxString& detectorScriptFullPath);

	wxFileName LocalRootDir();

	wxFileName GlobalRootDir();

	wxFileName SkeletonFile();

	wxString Label();

	wxString HelpMessage();
};

class TagDetectorClass : public mvceditor::DetectorClass {
public:

	TagDetectorClass();

	bool CanTest(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project);

	wxString TestCommandLine(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project,
		const wxString& detectorScriptFullPath);

	wxFileName LocalRootDir();

	wxFileName GlobalRootDir();

	wxFileName SkeletonFile();

	wxString Label();

	wxString HelpMessage();
};

class DatabaseDetectorClass : public mvceditor::DetectorClass {
public:

	DatabaseDetectorClass();

	bool CanTest(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project);

	wxString TestCommandLine(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project,
		const wxString& detectorScriptFullPath);

	wxFileName LocalRootDir();

	wxFileName GlobalRootDir();

	wxFileName SkeletonFile();

	wxString Label();

	wxString HelpMessage();
};

class ConfigDetectorClass : public mvceditor::DetectorClass {
public:

	ConfigDetectorClass();

	bool CanTest(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project);

	wxString TestCommandLine(const mvceditor::GlobalsClass& globals, const mvceditor::ProjectClass& project,
		const wxString& detectorScriptFullPath);

	wxFileName LocalRootDir();

	wxFileName GlobalRootDir();

	wxFileName SkeletonFile();

	wxString Label();

	wxString HelpMessage();
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
	 */
	DetectorTreeHandlerClass(wxTreeCtrl* detectorTree, wxButton* testButton, wxButton* addButton,
		wxButton* helpButton, wxChoice* projectChoice,
		mvceditor::DetectorClass* detector,
		mvceditor::GlobalsClass& globals, mvceditor::EventSinkClass& eventSink);

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

	
	wxTreeCtrl* DetectorTree;
	wxButton* TestButton;
	wxButton* AddButton;
	wxButton* HelpButton;
	wxChoice* ProjectChoice;

	mvceditor::DetectorClass* Detector;

	/**
	 * to access the project list
	 */
	mvceditor::GlobalsClass& Globals;

	/**
	 * to send app commands to open and run a file
	 */
	mvceditor::EventSinkClass& EventSink;

}; 

class UrlDetectorPanelClass : public UrlDetectorPanelGeneratedClass {

public:

	/**
	 * @param parent window parent
	 * @param id window ID
	 * @param globals to access the projects list
	 * @param eventSink to send the app file and run commands
	 */
	UrlDetectorPanelClass(wxWindow* parent, int id, mvceditor::GlobalsClass& globals,
		mvceditor::EventSinkClass& eventSink);

	~UrlDetectorPanelClass();

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

	mvceditor::UrlDetectorClass Detector;

	mvceditor::DetectorTreeHandlerClass Handler;
};

class TemplateFilesDetectorPanelClass : public TemplateFilesDetectorPanelGeneratedClass {

public:

	/**
	 * @param parent window parent
	 * @param id window ID
	 * @param globals to access the projects list
	 * @param eventSink to send the app file and run commands
	 * @param runningThreads  used to run the background processes.
	 */
	TemplateFilesDetectorPanelClass(wxWindow* parent, int id, mvceditor::GlobalsClass& globals,
		mvceditor::EventSinkClass& eventSink, mvceditor::RunningThreadsClass& runningThreads);

	~TemplateFilesDetectorPanelClass();

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

	mvceditor::TemplateFilesDetectorClass Detector;

	mvceditor::DetectorTreeHandlerClass Handler;

	/**
	 * The URL to use for testing the detector
	 */
	mvceditor::UrlResourceClass TestUrl;

	/**
	 * to access the url resource list
	 */
	mvceditor::GlobalsClass& Globals;

	mvceditor::RunningThreadsClass& RunningThreads;
};

class TagDetectorPanelClass : public TagDetectorPanelGeneratedClass {

public:

	/**
	 * @param parent window parent
	 * @param id window ID
	 * @param globals to access the projects list
	 * @param eventSink to send the app file and run commands
	 */
	TagDetectorPanelClass(wxWindow* parent, int id, mvceditor::GlobalsClass& globals,
		mvceditor::EventSinkClass& eventSink);

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

	mvceditor::TagDetectorClass Detector;

	mvceditor::DetectorTreeHandlerClass Handler;
};

class DatabaseDetectorPanelClass : public DatabaseDetectorPanelGeneratedClass {

public:

	/**
	 * @param parent window parent
	 * @param id window ID
	 * @param globals to access the projects list
	 * @param eventSink to send the app file and run commands
	 */
	DatabaseDetectorPanelClass(wxWindow* parent, int id, mvceditor::GlobalsClass& globals,
		mvceditor::EventSinkClass& eventSink);

	~DatabaseDetectorPanelClass();

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

	mvceditor::DatabaseDetectorClass Detector;

	mvceditor::DetectorTreeHandlerClass Handler;
};

class ConfigDetectorPanelClass : public ConfigDetectorPanelGeneratedClass {

public:

	/**
	 * @param parent window parent
	 * @param id window ID
	 * @param globals to access the projects list
	 * @param eventSink to send the app file and run commands
	 */
	ConfigDetectorPanelClass(wxWindow* parent, int id, mvceditor::GlobalsClass& globals,
		mvceditor::EventSinkClass& eventSink);

	~ConfigDetectorPanelClass();

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

	mvceditor::ConfigDetectorClass Detector;

	mvceditor::DetectorTreeHandlerClass Handler;
};

class DetectorFeatureClass : public mvceditor::FeatureClass {

public:

	DetectorFeatureClass(mvceditor::AppClass& app);

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
