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
#include <pelet/ParserClass.h>
#include <features/wxformbuilder/OutlineViewPluginPanelClass.h>
#include <widgets/ThreadWithHeartbeatClass.h>
#include <vector>

namespace mvceditor {
	
extern const wxEventType EVENT_RESOURCE_FINDER_COMPLETE;
extern const wxEventType EVENT_GLOBAL_CLASSES_COMPLETE;
	
class ResourceFinderCompleteEventClass : public wxEvent {
	
	public:
	
	/**
	 * Will contain all of the parsed resources. 
	 */
	std::vector<mvceditor::ResourceClass> Resources;
	
	ResourceFinderCompleteEventClass(int eventId, const std::vector<mvceditor::ResourceClass>& resources);
	
	wxEvent* Clone() const;
	
};

class GlobalClassesCompleteEventClass : public wxEvent {

public:

	GlobalClassesCompleteEventClass(int eventId, const std::vector<wxString>& allClasses);
	
	wxEvent* Clone() const;

	/**
	 * @return All of the classes that have been parsed by the
	 * global cache
	 */
	std::vector<wxString> GetAllClasses() const;

private:

	/**
	 * All of the classes that have been parsed by the
	 * global cache. Hiding this vector because we want to make sure
	 * it is deep copied every time since we this event will be handled
	 * by multiple threads.
	 */
	std::vector<wxString> AllClasses;
};


typedef void (wxEvtHandler::*ResourceFinderCompleteEventClassFunction)(ResourceFinderCompleteEventClass&);

#define EVT_RESOURCE_FINDER_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_RESOURCE_FINDER_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( ResourceFinderCompleteEventClassFunction, & fn ), (wxObject *) NULL ),


typedef void (wxEvtHandler::*GlobalClassesCompleteEventClassFunction)(GlobalClassesCompleteEventClass&);

#define EVT_GLOBAL_CLASSES_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_GLOBAL_CLASSES_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( GlobalClassesCompleteEventClassFunction, & fn ), (wxObject *) NULL ),


/**
 * A small class that will parse source into resources. We want to do 
 * this in a background thread in case the user is viewing a big file.
 */
class ResourceFinderBackgroundThreadClass : ThreadWithHeartbeatClass {

public:

	/**
	 * @param runningThreads will get notified with EVENT_WORK_COMPLETE and the EVENT_RESOURCE_FINDER_COMPLETE
	 * events when parsing is complete.
	 */
	ResourceFinderBackgroundThreadClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * start the background thread that parses the given file.
	 * @filename the file to parse
	 * @param environment to get the php version
	 * @param if thread was started, tghe thread Id will be set. the thread Id
	 *        can be used to stop the thread
	 * @see wxThread::GetId
	 */
	bool Start(const wxString& fileName, const EnvironmentClass& environment, wxThreadIdType& threadId);

protected:

	void BackgroundWork();

private:

	/**
	 * to parse the resources out of a file.
	 */
	ResourceFinderClass ResourceFinder;

	/**
	 * the current file being parsed
	 */
	wxString FileName;

};

/**
 * class to gather all of the classes from the global cache.
 * Need to do this in the background because the SQLite file
 * does not have an index on the resource_type column
 */
class GlobalClassesThreadClass : public mvceditor::ThreadWithHeartbeatClass {
	
public:

	GlobalClassesThreadClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);
	
	/**
	 * @param projects the currently projects, which contain the full paths to resource DB files to read
	 * @return TRUE if there is at least one enabled project
	 */
	bool Init(const std::vector<mvceditor::ProjectClass>& projects);
	
protected:

	void BackgroundWork();
	
private:

	/**
	 * location of the resource database files
	 */
	std::vector<wxFileName> ResourceDbFileNames;
	
	/**
	 * the resulting class list
	 */
	std::vector<wxString> AllClasses;
};

/**
 * This is a plugin that is designed to let the user see the classes / methods of 
 * the opened files and of related files.  The related files / classes / methods that are mentioned
 * in the opened files.
 * 
 */
class OutlineViewPluginClass : public PluginClass {
public:
	
	/**
	 * Creates a new OutlineViewPlugin.
	 */
	OutlineViewPluginClass(mvceditor::AppClass& app);

	/**
	 * This plugin will have a view menu entry
	 */
	void AddViewMenuItems(wxMenu* viewMenu);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
	/**
	 * When the right click context menu is chosen on some selected text, open that resource into the outline control.
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
	 * global ResourceCache
	 * 
	 * @param wxString className the name of the class to build the outline for.
	 */
	std::vector<mvceditor::ResourceClass> BuildOutline(const wxString& className);
	
	/**
	 * Opens the file where the given resource is located.
	 * 
	 * @param wxString teh fully qualified resource
	 */
	void JumpToResource(const wxString& resource);
	
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
	 * When all the classes from the global cache have been gathered
	 * update the outline panel
	 */
	void OnGlobalClassesComplete(mvceditor::GlobalClassesCompleteEventClass& event);
	
	DECLARE_EVENT_TABLE()
};

class OutlineViewPluginPanelClass : public OutlineViewPluginGeneratedPanelClass {

	public:

	/**
	 * Create a new outline view panel.
	 * 
	 * @param wxWindow* parent the parent window
	 * @param int windowId the window ID
	 * @param OutlineViewPluginClass* plugin the object that will execute the business logic. This panel will NOT
	 *        own the pointer.  The caller must DELETE the plugin when appropriate. This parameter MUST NOT BE NULL!
	 * @param NotebookClass* notebook we need to listen to the notebook page change events so that the outline is updated to show
	 *        an outline of the newly opened page
	 */
	OutlineViewPluginPanelClass(wxWindow* parent, int windowId, OutlineViewPluginClass* plugin, NotebookClass* notebook);
	
	/**
	 * update the status label
	 */
	void SetStatus(const wxString& status);

	/**
	 * update the Choice options
	 */
	void SetClasses(const std::vector<wxString>& classes);

	/**
	 * refresh the code control from the plugin source strings
	 */
	 void RefreshOutlines(const std::vector<ResourceClass>& resources);
	
protected:

	/**
	 * Shows the help
	 */
	void OnHelpButton(wxCommandEvent& event);
	
	/**
	 * take the selected choice, perform a resource lookup, and get the outline
	 */
	void OnChoice(wxCommandEvent& event);

	/**
	 * sync the outline with the currently opened file
	 */
	
	void OnSyncButton(wxCommandEvent& event);

private:

	/**
	 * The plugin class that will execute all logic. 
	 * @var OutlineViewPluginClass*
	 */
	OutlineViewPluginClass* Plugin;
	
	/**
	 * The notebook to listen (for page changing) events  to
	 */
	NotebookClass* Notebook;
	
	/**
	 * double clicking on a  resource name in the tree will make the editor open up that resource
	 */
	void OnTreeItemActivated(wxTreeEvent& event);
};

}

#endif