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
 * @author     $Author: robertop2004@gmail.com $
 * @date       $Date: 2011-06-10 17:19:31 -0700 (Fri, 10 Jun 2011) $
 * @version    $Rev: 555 $ 
 */
#ifndef __resourcepluginclass__
#define __resourcepluginclass__

#include <PluginClass.h>
#include <plugins/wxformbuilder/ResourcePluginGeneratedClass.h>
#include <search/DirectorySearchClass.h>
#include <search/ResourceFinderClass.h>
#include <wx/string.h>
#include <wx/stc/stc.h>

namespace mvceditor {
	
class ResourcePluginClass : public PluginClass, DirectoryWalkerClass {

public:

	/**
	 * the text being searched for. Any text entered here is not assumed to be any specific type;
	 * a resource search will be done as per normal ResourceFinderClass rules.
	 * 
	 * @var wxString
	 * @see ResourceFinderClass
	 */
	wxString JumpToText;
	
	/**
	 * create a resource plugin.
	 */
	ResourcePluginClass();

	void AddProjectMenuItems(wxMenu* projectMenu);

	void AddToolBarItems(wxAuiToolBar* toolBar);
	
	void AddWindows();
	
	void OnProjectOpened();
	
	void AddCodeControlClassContextMenuItems(wxMenu* menu);
	
	/**
	 * Implement the walk method of DirectoryWalkerClass. We will 
	 * count the files here during the idle event instead of using the PRECISE
	 * flag on the DirectorySearchClass.  This is to prevent screen freeze.
	 */
	virtual bool Walk(const wxString& file);
	
	/**
	 * Searches for a file that  matches FileText.  if a single match is found, then the file is opened / brought to the front.
	 * If multiple files match, then user will be prompted to pick a file to open.
	 */
	void SearchForFiles();
	
	/**
	 * Searches for a file that  matches JumpToText.  if a single match is found, then the file is opened / brought to the front.
	 * If multiple files match, then user will be prompted to pick a file to open.
	 */
	void SearchForResources();
	
private:

	/**
	 * Will use the idle event to do the resource lookups.
	 */
	void OnIdleEvent(wxIdleEvent& event);

	/**
	 * Handle the results of the resource lookups.
	 */
	void ShowJumpToResults();
	
	/**
	 * Toggle various widgets on or off based on the application state. 
	 */
	void OnUpdateUi(wxUpdateUIEvent& event);
	
	/**
	 * Recurse through project files and gather resources.
	 */
	void OnProjectIndex(wxCommandEvent& event);
	
	/**
	 * Handle to Go To Source menu event
	 */
	void OnJump(wxCommandEvent& event);
	
	/**
	 * During the timer we will pulse the gauge.
	 */
	void OnTimer(wxTimerEvent& event);
	
	/**
	 * Opens the page and sets the cursor on the function/method/property/file that was searched for by the
	 * resource finder
	 * 
	 * @param ResourceFinderClass* will use this to set the cursor on the resource that was searched for
	 * @param int resourceMatchIndex the index into the resource matches
	 */
	void LoadPageFromResourceFinder(ResourceFinderClass* resourceFinder, int resourceMatchIndex);
	
	/**
	 * Get the resource finder.  Do NOT delete the returned pointer.
	 * 
	 * @return ResourceFinderClass* 
	 */
	ResourceFinderClass* GetResourceFinder() const;
	
	/**
	 * Returns true if files in the project have NOT already been cached by the resource finder. This does not
	 * necesaarily mean that the resource finder has parsed them; if so far all resource lookups have been for
	 * file names then the resource finder has not parsed a single file.  What it does mean is that the next call
	 * to ResourceFinderClass::LocateResourceInFileSystem will be really slow because the appropriate cache has
	 * not been built. (ie file lookups will be fast after the first file lookup, class lookups will be
	 * slow until the second file lookup)
	 *
	 * @return bool
	 */
	bool NeedToIndex() const;

	/**
	 * The various states control what this plugin does during the IDLE events. This class will perform
	 * the resource lookups in the main thread, so we need this flag to determine what action to take.
	 * 
	 * states go in this order:
	 * FREE -> GOTO_COUNT_FILES -> GOTO -> FREE
	 * FREE -> INDEX_COUNT_FILES -> INDEX -> FREE
	 * 
	 */
	enum States {
		FREE,
		GOTO_COUNT_FILES,
		GOTO,		
		INDEX_COUNT_FILES,
		INDEX
	};
	
	/**
	 * Used to iterate through directories
	 * 
	 * @var DirectorySearch
	 */
	DirectorySearchClass DirectorySearch;
	
	/**
	 * The "Index project" menu item
	 * @var wxMenuItem* 
	 */
	wxMenuItem* ProjectIndexMenu;
	
	/**
	 * To increment the gauge smoothly.
	 */
	wxTimer Timer;
	
	/**
	 * we won't use a separate thread to fo the resource finder.  we will use the idle event instead. This State flag will
	 * signal the idle event to wake up.
	 */
	States State;
	
	/**
	 * The number of files that will be searched. 
	 */
	int FileCount;
	
	/**
	 * Flag that will store when files have been parsed.
	 * @var bool
	 */
	bool HasCodeLookups;

	/**
	 * Flag that will store when file names have been walked over and cached.
	 * @var bool
	 */
	bool HasFileLookups;

	DECLARE_EVENT_TABLE()
};

class ResourcePluginPanelClass : public ResourcePluginGeneratedPanelClass {
public:

	ResourcePluginPanelClass(wxWindow* parent, ResourcePluginClass& resource, NotebookClass* notebook);
	
	/**
	 * Set the focus on the search control
	 */
	void FocusOnSearchControl();
	
protected:

	/**
	 * When a file is chosen make the notebook show the file
	 * @param wxCommandEvent& the event
	 */
	void OnFilesComboCombobox(wxCommandEvent& event);
	
	/**
	 * When the user presses enter iniate a file search for the entered text.
	 * @param wxCommandEvent& the event
	 */
	void OnFilesComboTextEnter(wxCommandEvent& event);
	
	/**
	 * When the user clicks the help button help text will be shown
	 * @param wxCommandEvent& the event
	 */
	void OnHelpButtonClick(wxCommandEvent& event);


private:

	/**
	 * The resource plugin reference.  The dialog will use this reference to actually perform the search.
	 * 
	 * @var ResourcePluginClass
	 */
	ResourcePluginClass& ResourcePlugin;
	
	/**
	 * The notebook that holds all of the opened files
	 * 
	 * @var NotebookClass*
	 */
	NotebookClass* Notebook;
	
	/**
	 * When user changes a page update the FilesCombo box
	 * 
	 * @param wxAuiNotebookEvent& event
	 */
	void OnPageChanged(wxAuiNotebookEvent& event);
	
	/**
	 * When user closes all page clear the FilesCombo box
	 * 
	 * @param wxAuiNotebookEvent& event
	 */
	void OnPageClosed(wxAuiNotebookEvent& event);
	
	/**
	 * Disconnect from the page changing events.
	 * 
	 * @param wxCloseEvent& event
	 */
	void OnClose(wxCloseEvent& event);
	
	DECLARE_EVENT_TABLE()
};

}
#endif // __resourcepluginclass__
