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
#ifndef OUTLINE_VIEW_FEATURECLASS_H__
#define OUTLINE_VIEW_FEATURECLASS_H__

#include <features/FeatureClass.h>
#include <pelet/TokenClass.h>
#include <features/wxformbuilder/OutlineViewFeatureForms.h>
#include <widgets/ThreadWithHeartbeatClass.h>
#include <wx/imaglist.h>
#include <vector>

namespace mvceditor {
	
extern const wxEventType EVENT_RESOURCE_FINDER_COMPLETE;
extern const wxEventType EVENT_GLOBAL_CLASSES_COMPLETE;
	
class ResourceFinderCompleteEventClass : public wxEvent {
	
	public:
	
	/**
	 * Will contain all of the parsed resources. 
	 */
	std::vector<mvceditor::TagClass> Resources;
	
	ResourceFinderCompleteEventClass(int eventId, const std::vector<mvceditor::TagClass>& resources);
	
	wxEvent* Clone() const;
	
};


typedef void (wxEvtHandler::*ResourceFinderCompleteEventClassFunction)(ResourceFinderCompleteEventClass&);

#define EVT_RESOURCE_FINDER_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_RESOURCE_FINDER_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( ResourceFinderCompleteEventClassFunction, & fn ), (wxObject *) NULL ),

/**
 * A small class that will parse source into resources. We want to do 
 * this in a background thread in case the user is viewing a big file.
 */
class ResourceFinderBackgroundThreadClass : public ThreadWithHeartbeatClass {

public:

	/**
	 * @param runningThreads will get notified with EVENT_WORK_COMPLETE and the EVENT_RESOURCE_FINDER_COMPLETE
	 * events when parsing is complete.
	 */
	ResourceFinderBackgroundThreadClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * start to parses the given file in  a backgound thread. this will NOT create a thread, rather it
	 * will signal the already-started thread to parse the given file.
	 *
	 * @filename the file to parse
	 * @param phpVersion the php version to use for parsing the file.
	 */
	void Start(const wxString& fileName, pelet::Versions phpVersion);

protected:

	void BackgroundWork();

private:

	/**
	 * Since this thread will be alive as long as the program is running, we guard against access
	 * for FileName, PhpVersion variables. We want to start one thread that will handle all code notebook
	 * tab changes instead of creating one thread each time the user changes to a new tab.
	 */
	wxMutex Mutex;

	/**
	 * the current file being parsed
	 */
	wxString FileName;

	/**
	 * the version of PHP to check against when parsing
	 */
	pelet::Versions PhpVersion;

};

/**
 * This is a feature that is designed to let the user see the classes / methods of 
 * the opened files and of related files.  The related files / classes / methods that are mentioned
 * in the opened files.
 * 
 */
class OutlineViewFeatureClass : public FeatureClass {
public:
	
	/**
	 * Creates a new OutlineViewFeature.
	 */
	OutlineViewFeatureClass(mvceditor::AppClass& app);

	/**
	 * This feature will have a view menu entry
	 */
	void AddViewMenuItems(wxMenu* viewMenu);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
	/**
	 * When the right click context menu is chosen on some selected text, open that tag into the outline control.
	 * @param wxCommandEvent& event
	 */
	void OnOutlineMenu(wxCommandEvent& event);
	
	/**
	 * Outlines the currently opened code control.  The currently opened file (the meory buffer) is parsed and an outline
	 * is generated from the parsed tokens.
	 */
	void BuildOutlineCurrentCodeControl();
	
	/**
	 * Builds an outline based on the given line of text.  Note that this does not parse a file; it searches the
	 * global TagCache
	 * 
	 * @param wxString className the name of the class to build the outline for.
	 */
	std::vector<mvceditor::TagClass> BuildOutline(const wxString& className);
	
	/**
	 * Opens the file where the given tag is located.
	 * 
	 * @param wxString teh fully qualified tag
	 */
	void JumpToResource(const wxString& tag);

	std::vector<mvceditor::TagClass> SearchForResources(const wxString& text);
	
private:		
		
	/**
	 * Updates the outlines based on the currently opened (and focused) file.
	*/
	void OnContentNotebookPageChanged(wxAuiNotebookEvent& event);
	
	/**
	 * when the parsing is complete update the panel.
	 */
	void OnResourceFinderComplete(mvceditor::ResourceFinderCompleteEventClass& event);
		
	/**
	 * when a file is saved and the outline tab is opened, make sure to refresh the outline
	 * tab contents with the latest content
	 */
	void OnFileSaved(mvceditor::FileSavedEventClass& event);

	/**
	 * Since this thread will be alive as long as the program is running, we guard against access
	 * for FileName, PhpVersion  variables. We want to start one thread that will handle all code notebook
	 * tab changes instead of creating one thread each time the user changes to a new tab.
	 * Since ResourceFinderBackgroundThread class inherits from wxThread, it will be deleted
	 * automaticall and is NOT owned by this object.
	 */
	mvceditor::ResourceFinderBackgroundThreadClass* ResourceFinderBackgroundThread;
	
	DECLARE_EVENT_TABLE()
};

class OutlineViewPanelClass : public OutlineViewGeneratedPanelClass {

	public:

	/**
	 * Create a new outline view panel.
	 * 
	 * @param wxWindow* parent the parent window
	 * @param int windowId the window ID
	 * @param OutlineViewFeatureClass* feature the object that will execute the business logic. This panel will NOT
	 *        own the pointer.  The caller must DELETE the feature when appropriate. This parameter MUST NOT BE NULL!
	 * @param NotebookClass* notebook we need to listen to the notebook page change events so that the outline is updated to show
	 *        an outline of the newly opened page
	 */
	OutlineViewPanelClass(wxWindow* parent, int windowId, OutlineViewFeatureClass* feature, NotebookClass* notebook);
	
	/**
	 * update the status label
	 */
	void SetStatus(const wxString& status);

	/**
	 * update the Choice options
	 */
	void SetClasses(const std::vector<wxString>& classes);

	/**
	 * refresh the code control from the feature source strings
	 */
	 void RefreshOutlines(const std::vector<TagClass>& resources);
	
protected:

	/**
	 * Shows the help
	 */
	void OnHelpButton(wxCommandEvent& event);
	
	/**
	 * make the user pick a tag, then outline the tag
	 */
	void OnAddButton(wxCommandEvent& event);

	/**
	 * sync the outline with the currently opened file
	 */	
	void OnSyncButton(wxCommandEvent& event);

private:

	enum {
		IMAGE_OUTLINE_ROOT = 0,
		IMAGE_OUTLINE_CLASS,
		IMAGE_OUTLINE_METHOD,
		IMAGE_OUTLINE_PROPERTY,
		IMAGE_OUTLINE_DEFINE,
		IMAGE_OUTLINE_CLASS_CONSTANT,
		IMAGE_OUTLINE_NAMESPACE,
		IMAGE_OUTLINE_FUNCTION
	};

	wxImageList ImageList;

	/**
	 * The feature class that will execute all logic. 
	 * @var OutlineViewFeatureClass*
	 */
	OutlineViewFeatureClass* Feature;
	
	/**
	 * The notebook to listen (for page changing) events  to
	 */
	NotebookClass* Notebook;
	
	/**
	 * double clicking on a  tag name in the tree will make the editor open up that tag
	 */
	void OnTreeItemActivated(wxTreeEvent& event);

	/**
	 * Show the given tags in the outline tree.
	 */
	void AddTagsToOutline(const std::vector<mvceditor::TagClass>& tags);

	/**
	 * Add a class and all of its members into the tree outline at the given node.
	 *
	 */
	void AddClassToOutline(const UnicodeString& className, wxTreeItemId& classRoot);

	/**
	 * adds a tag as a child node of the tree control.
	 */
	void TagToNode(const mvceditor::TagClass& tag, wxTreeItemId& tagRoot);

};

class FileSearchDialogClass : public FileSearchDialogGeneratedClass {

public:

	/**
	 * @param parent the parent window
	 * @param feature to get the project list and to search for files
	 * @param chosenTags the tags that the user chose will be filled in here
	 */
	FileSearchDialogClass(wxWindow* parent, mvceditor::OutlineViewFeatureClass& feature, 
		std::vector<mvceditor::TagClass>& chosenTags);

protected:

	void OnOkButton(wxCommandEvent& event);
	void OnSearchText(wxCommandEvent& event);
	void OnProjectChoice(wxCommandEvent& event);
	void OnSearchKeyDown(wxKeyEvent& event);
	void OnSearchEnter(wxCommandEvent& event);
	void OnMatchesListDoubleClick(wxCommandEvent& event);
	void OnMatchesListKeyDown(wxKeyEvent& event);


private:

	/**
	 * populate the project choice 
	 */
	void Init();

	/**
	 * perform the file search based on the selected project and 
	 * the entered text
	 */
	void Search();

	/**
	 * show the tags in the results list
	 */
	void ShowTags(const wxString& query, const std::vector<mvceditor::TagClass>& tags);

	/**
	 * to get the project list and perform tag search
	 */
	mvceditor::OutlineViewFeatureClass& Feature;

	/**
	 * the tags that the were the result of the preivous search
	 */
	std::vector<mvceditor::TagClass> MatchingTags;
	
	/**
	 * the tags that the user chose
	 */
	std::vector<mvceditor::TagClass>& ChosenTags;
};

}

#endif