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
#include <actions/ActionClass.h>
#include <features/wxformbuilder/OutlineViewFeatureForms.h>
#include <wx/imaglist.h>
#include <vector>

namespace mvceditor {

/**
 * grouping of a tag and its member tags.
 */
class OutlineSearchCompleteClass {

public:

	wxString Label;

	std::map<wxString, std::vector<mvceditor::TagClass> > Tags;

	OutlineSearchCompleteClass();

	OutlineSearchCompleteClass(const mvceditor::OutlineSearchCompleteClass& src);

	void Copy(const mvceditor::OutlineSearchCompleteClass& src);

	bool IsLabelFileName() const;
};

/**
 * event that is generated when a tag query is completed.  this event
 * contains the results of the search.
 */
class OutlineSearchCompleteEventClass : public wxEvent {
        
        public:
        
        /**
         * Will contain all of the resulting tags.
         */
        std::vector<mvceditor::OutlineSearchCompleteClass> Tags;
        
        OutlineSearchCompleteEventClass(int eventId, const std::vector<mvceditor::OutlineSearchCompleteClass>& tags);
        
        wxEvent* Clone() const;
        
};

extern const wxEventType EVENT_OUTLINE_SEARCH_COMPLETE;

typedef void (wxEvtHandler::*OutlineSearchCompleteEventClassFunction)(OutlineSearchCompleteEventClass&);

#define EVENT_OUTLINE_SEARCH_COMPLETE(id, fn) \
        DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_OUTLINE_SEARCH_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( OutlineSearchCompleteEventClassFunction, & fn ), (wxObject *) NULL ),

/**
 * class that will execute a query against the tag cache in the background.
 * the results will be posted in an event.
 */
class OutlineTagCacheSearchActionClass : public mvceditor::ActionClass {
	
public:

	OutlineTagCacheSearchActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * set the search parameters.  this should be called before the action is
	 * added to the run queue
	 *
	 * @param searches the search strings, can be either file names, full paths, or class names
	 * @param globals to get the locations of the tag dbs
	 */
	void SetSearch(const std::vector<UnicodeString>& searches, mvceditor::GlobalsClass& globals);

	wxString GetLabel() const;

protected:

	void BackgroundWork();

private:

	mvceditor::TagCacheClass TagCache;

	std::vector<UnicodeString> SearchStrings;
	
	/**
	 * the directories to look in
	 */
	std::vector<wxFileName> EnabledSourceDirs;
};

/**
 * This is a feature that is designed to let the user see the classes / methods of 
 * the opened files and of related files.  The related files / classes / methods that are mentioned
 * in the opened files.
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
	 * Opens the file where the given tag is located.
	 * 
	 * @param int the tag ID to jump to
	 */
	void JumpToResource(int tagId);

	/**
	 * start a tag search; will queue an action that will search the tag cache
	 * in a background thread and Post the results when complete. If multiple
	 * search strings are given, a search for each string will be done.
	 *
	 * @param searchStrings tags to look for
	 */
	void StartTagSearch(const std::vector<UnicodeString>& searchStrings);
	
private:		
		
	/**
	 * Updates the outlines based on the currently opened (and focused) file.
	*/
	void OnContentNotebookPageChanged(wxAuiNotebookEvent& event);
	
	/**
	 * when a file is closed, remove it from the outline tree
	 */
	void OnContentNotebookPageClosed(wxAuiNotebookEvent& event);

	/**
	 * This method will get called by the EVENT_WORKING_CACHE_COMPLETE event is generated;
	 * ie when parsing of the code in the active code control buffer has been completed.
	 */
	void OnWorkingCacheComplete(mvceditor::WorkingCacheCompleteEventClass& event);

	/**
	 * once tag searching finishes, update the tree control
	 */
	void OnTagSearchComplete(mvceditor::OutlineSearchCompleteEventClass& event);
	
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
	 * refresh the code control from the feature source strings
	 * adds the given tags to the outline tree, under a node that has the name of the file
	 * as its name
	 */
	void AddTagsToOutline(const std::vector<mvceditor::OutlineSearchCompleteClass>& tags);

	 /**
	  * @param fullPath file to remove from the outline tre
	  */
	 void RemoveFileFromOutline(const wxString& fullPath);
	
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

	/**
	 * show a context menu
	 */
	void OnTreeItemRightClick(wxTreeEvent& event);

private:

	// image IDs used by the Tree ImageList
	enum {
		IMAGE_OUTLINE_ROOT = 0,
		IMAGE_OUTLINE_FILE,
		IMAGE_OUTLINE_CLASS,
		IMAGE_OUTLINE_METHOD_PUBLIC,
		IMAGE_OUTLINE_METHOD_PROTECTED,
		IMAGE_OUTLINE_METHOD_PRIVATE,
		IMAGE_OUTLINE_METHOD_INHERITED,
		IMAGE_OUTLINE_PROPERTY_PUBLIC,
		IMAGE_OUTLINE_PROPERTY_PROTECTED,
		IMAGE_OUTLINE_PROPERTY_PRIVATE,
		IMAGE_OUTLINE_PROPERTY_INHERITED,
		IMAGE_OUTLINE_DEFINE,
		IMAGE_OUTLINE_CLASS_CONSTANT,
		IMAGE_OUTLINE_NAMESPACE,
		IMAGE_OUTLINE_FUNCTION,
		IMAGE_OUTLINE_ARGUMENT
	};

	/**
	 * these are the things that have been outlined.
	 */
	std::vector<mvceditor::OutlineSearchCompleteClass> OutlinedTags;

	/**
	 * this pointer will be managed by the tree control, since the tree control
	 * may use the pointer in the destructor.
	 */
	wxImageList* ImageList;

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
	 * if TRUE outline will show class methods
	 */
	bool ShowMethods;

	/**
	 * if TRUE outline will show class properties
	 */
	bool ShowProperties;

	/**
	 * if TRUE outline will show class constants
	 */
	bool ShowConstants;

	/**
	 * if TRUE then a class' inherited methods and properties are shown
	 */
	bool ShowInherited;

	/**
	 * if TRUE then only public methods, properties or constants are show
	 */
	bool ShowPublicOnly;

	/**
	 * if TRUE then function arguments are show
	 */
	bool ShowFunctionArgs;

	/**
	 * if TRUE then tags are sorted by name
	 */
	bool SortByName;

	/**
	 * if TRUE then tags are sorted by type (methods, properties, functions ...) then name
	 */
	bool SortByType;

	/**
	 * takes in class or file tags, perform searches for all tags in each of
	 * the files / classes given.
	 *
	 * @param tags that the user chose
	 */
	void SearchTagsToOutline(const std::vector<mvceditor::TagClass>& tags);
	
	/**
	 * double clicking on a  tag name in the tree will make the editor open up that tag
	 */
	void OnTreeItemActivated(wxTreeEvent& event);

	/**
	 * adds a tag as a child node of the tree control.
	 * @param tag the tag to add
	 * @param tagRoot the tree node to append to
	 * @param classNameNode the name of the class the tag belongs to (can be empty)
	 */
	void TagToNode(const mvceditor::TagClass& tag, wxTreeItemId& tagRoot, UnicodeString classNameNode);

	/**
	 * @return the tree node for the given full path. search is done 
	 *         by comparing tree item data and NOT the tree item name
	 */
	wxTreeItemId FindFileNode(const wxString& fullPath);

	/**
	 * handle the right click menu item for deletion
	 */
	void OnTreeMenuDelete(wxCommandEvent& event);

	/**
	 * handle the right click menu item for collapsing
	 */
	void OnTreeMenuCollapse(wxCommandEvent& event);

	/**
	 * handle the right click menu item for collapsing all items
	 */
	void OnTreeMenuCollapseAll(wxCommandEvent& event);

	/**
	 * handle the right click menu item for expanding all items
	 */
	void OnTreeMenuExpandAll(wxCommandEvent& event);

	/**
	 * handle the properties toggle; toggle showing class members on and off
	 */
	void OnPropertiesClick(wxCommandEvent& event);

	/**
	 * handle the properties toggle; toggle showing class methods on and off
	 */
	void OnMethodsClick(wxCommandEvent& event);

	/**
	 * handle the properties toggle; toggle showing class constants on and off
	 */
	void OnConstantsClick(wxCommandEvent& event);

	/**
	 * handle the show inherited toggle; toggle showing class inherited members on and off
	 */
	void OnInheritedClick(wxCommandEvent& event);

	/**
	 * handle the "public only" toggle; toggle showing private tags on and off
	 */
	void OnPublicOnlyClick(wxCommandEvent& event);

	/**
	 * handle the arguments toggle; toggle showing function arguments on and off
	 */
	void OnFunctionArgsClick(wxCommandEvent& event);

	/**
	 * handle the "filter" button click; show the menu to toogle different tags on or off
	 */
	void OnFilterLeftDown(wxMouseEvent& event);

	/**
	 * handle the "sort" button click; show the menu to toogle different sort options on or off
	 */
	void OnSortLeftDown(wxMouseEvent& event);

	/**
	 * handle the 'sort by type' toggle
	 */
	void OnSortByTypeClick(wxCommandEvent& event);

	/**
	 * handle the 'sort by name' toggle
	 */
	void OnSortByNameClick(wxCommandEvent& event);

	/**
	 * redaws the outline nodes based on the state of the various Show* flags.
	 */
	void RedrawOutline();

	DECLARE_EVENT_TABLE()

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