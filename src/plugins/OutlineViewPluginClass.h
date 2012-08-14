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
#ifndef OUTLINE_VIEW_PLUGIN_CLASS_H
#define OUTLINE_VIEW_PLUGIN_CLASS_H

#include <PluginClass.h>
#include <pelet/ParserClass.h>
#include <plugins/wxformbuilder/OutlineViewPluginPanelClass.h>
#include <widgets/ThreadWithHeartbeatClass.h>
#include <vector>

namespace mvceditor {
	
extern const wxEventType EVENT_RESOURCE_FINDER_COMPLETE;
	
class ResourceFinderCompleteEventClass : public wxEvent {
	
	public:
	
	/**
	 * Will contain all of the parsed resources. 
	 */
	std::vector<mvceditor::ResourceClass> Resources;
	
	ResourceFinderCompleteEventClass(const std::vector<mvceditor::ResourceClass>& resources);
	
	wxEvent* Clone() const;
	
};

typedef void (wxEvtHandler::*ResourceFinderCompleteEventClassFunction)(ResourceFinderCompleteEventClass&);

#define EVT_RESOURCE_FINDER_COMPLETE(fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_RESOURCE_FINDER_COMPLETE, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( ResourceFinderCompleteEventClassFunction, & fn ), (wxObject *) NULL ),

/**
 * A small class that will parse source into resources. We want to do 
 * this in a background thread in case the user is viewing a big file.
 */
class ResourceFinderBackgroundThreadClass : ThreadWithHeartbeatClass {

public:

	/**
	 * @param handler will get notified with EVENT_WORK_COMPLETE and the EVENT_RESOURCE_FINDER_COMPLETE
	 * events when parsing is complete.
	 */
	ResourceFinderBackgroundThreadClass(wxEvtHandler& handler, mvceditor::RunningThreadsClass& runningThreads);

	/**
	 * start the background thread that parses the given file.
	 * @filename the file to parse
	 * @param environment to get the php version
	 */
	bool Start(const wxString& fileName, const EnvironmentClass& environment);

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
 * This is a plugin that is designed to let the user see the classes / methods of 
 * the opened files and of related files.  The related files / classes / methods that are mentioned
 * in the opened files.
 * 
 */
class OutlineViewPluginClass : public PluginClass {
public:

	/**
	 * will also parse a piece of source code in a background thread and will wxPostEvent
	 * the results to us. This pointer self-destructs
	 */
	ResourceFinderBackgroundThreadClass* ResourceFinderBackground;
	
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
	 * This plugin will add menui items to the context menu
	 */
	void AddCodeControlClassContextMenuItems(wxMenu* menu);
	
	/**
	 * When the right click context menu is chosen on some selected text, open that resource into the outline control.
	 * @param wxCommandEvent& event
	 */
	void OnContextMenuOutline(wxCommandEvent& event);
	
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
	 * when thread is done null the reference
	 */
	void OnWorkComplete(wxCommandEvent& event);
	
	/**
	 * when the parsing is complete update the panel.
	 */
	void OnResourceFinderComplete(mvceditor::ResourceFinderCompleteEventClass& event);
	
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
	void SetClasses(const std::vector<ResourceClass>& classes);

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