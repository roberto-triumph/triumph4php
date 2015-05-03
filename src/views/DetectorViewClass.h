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
#ifndef T4P_DETECTORVIEWCLASS_H__
#define T4P_DETECTORVIEWCLASS_H__

#include <views/FeatureViewClass.h>
#include <actions/ActionClass.h>
#include <views/wxformbuilder/DetectorFeatureForms.h>
#include <features/DetectorFeatureClass.h>
#include <wx/imaglist.h>

namespace t4p {

class DetectorViewClass : public t4p::FeatureViewClass {

public:

	DetectorViewClass(t4p::DetectorFeatureClass& feature);
	
	void AddViewMenuItems(wxMenu* viewMenu);

	void AddNewMenu(wxMenuBar* menuBar);

	private:

	// menu handlers
	void OnViewUrlDetectors(wxCommandEvent& event);
	void OnViewTemplateFileDetectors(wxCommandEvent& event);
	void OnViewTagDetectors(wxCommandEvent& event);
	void OnViewDatabaseDetectors(wxCommandEvent& event);
	void OnViewConfigDetectors(wxCommandEvent& event);
	
	void OnPreferencesSaved(wxCommandEvent& event);
	
	t4p::DetectorFeatureClass& Feature;
	
	DECLARE_EVENT_TABLE()
};

class DetectorTreeHandlerClass : public wxEvtHandler {

public:

	/**
	 * @param detectorTree the wxTreeCtrl that renders the files. pointer will NOT be
	 *        owned by this class
	 * @param testButton pointer will NOT be owned by this class. this can be NULL. if NULL, 
	 *        it will be up to the caller to handle the test button event.
	 * @param addButton pointer will NOT be owned by this class
	 * @param helpButton pointer will NOT be owned by this class
	 * @param detector to get the file locations. pointer will NOT be owned by this class
	 * @param globals to fill the projects choice list 
	 * @param eventSink to send test button click events to
	 * @param rootImage the bitmap to be shown in the root of the tree
	 */
	DetectorTreeHandlerClass(wxTreeCtrl* detectorTree, wxButton* testButton, wxButton* addButton,
		wxBitmapButton* helpButton, wxChoice* projectChoice,
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
	wxBitmapButton* HelpButton;
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
	
	void OnHelpButton(wxCommandEvent& event);
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

private:

	t4p::TemplateFileTagsDetectorClass Detector;

	t4p::DetectorTreeHandlerClass Handler;

	/**
	 * The class to use for testing the detector
	 */
	t4p::UrlTagClass TestUrl;
	
	/**
	 * to access the url resource list
	 */
	t4p::GlobalsClass& Globals;

	t4p::RunningThreadsClass& RunningThreads;
	
	t4p::EventSinkClass& EventSink;
	
	void OnChooseUrlButton(wxCommandEvent& event);
	
	void OnTestButton(wxCommandEvent& event);
	
	void OnCallStackComplete(wxCommandEvent& event);
	
	void OnHelpButton(wxCommandEvent& event);
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
	
	void OnHelpButton(wxCommandEvent& event);
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
	
	void OnHelpButton(wxCommandEvent& event);
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
	
	void OnHelpButton(wxCommandEvent& event);
};

}

#endif
