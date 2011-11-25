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
#include <language/ParserClass.h>
#include <plugins/wxformbuilder/OutlineViewPluginPanelClass.h>
#include <vector>

namespace mvceditor {

/**
 * This is a plugin that is designed to let the user see the classes / methods of 
 * the opened files and of related files.  The related files / classes / methods that are mentioned
 * in the opened files.
 * 
 */
class OutlineViewPluginClass : public PluginClass, public ClassObserverClass, public ClassMemberObserverClass, public FunctionObserverClass {
public:

	/**
	 * Holds the text for the outline of the currently viewed file
	 * @var UnicodeString
	 */
	UnicodeString CurrentOutline;
	
	/**
	 * Holds the text for the outline of the resources the user asked for.
	 * @var UnicodeString
	 */
	UnicodeString Outline2;
	
	/**
	 * Holds the text for the PHP Doc of the resource the user asked for
	 * @var UnicodeString
	 */
	UnicodeString PhpDoc;
		
	/**
	 * Creates a new OutlineViewPlugin.
	 */
	OutlineViewPluginClass();

	/**
	 * This plugin will have a tools menu entry
	 */
	void AddToolsMenuItems(wxMenu* editMenu);
	
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
	 * ResourceFinder.
	 * 
	 * @param wxString line this should be a line from a previosuly built outline
	 */
	void BuildOutline(const wxString& line);
	
	/**
	 * Builds PHPDoc based on the given line of text.
	 * 
	 * @param wxString line this should be a line from a previosuly built outline
	 */
	void BuildPhpDoc(const wxString& line);
	
	/**
	 * Opens the file where the given resource is located.
	 * 
	 * @param wxString teh fully qualified resource
	 */
	void JumpToResource(const wxString& resource);
	
	/**
	 * Implementation of ClassObserver.  In this method we will append to the outline buffer.
	 */
	void ClassFound(const UnicodeString& className, const UnicodeString& signature, 
		const UnicodeString& comment);
	
	/**
	 * Implementation of ClassObserver.  In this method we will append to the outline buffer.
	 */
	void DefineDeclarationFound(const UnicodeString& variableName, const UnicodeString& variableValue, 
		const UnicodeString& comment);
		
	/**
	 * Implementation of ClassMemberObserver.  In this method we will append to the outline buffer.
	 */
	void MethodFound(const UnicodeString& className, const UnicodeString& methodName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment,
		TokenClass::TokenIds visibility, bool isStatic);
		
	/**
	 * Implementation of ClassMemberObserver.  In this method we will append to the outline buffer.
	 */
	void PropertyFound(const UnicodeString& className, const UnicodeString& propertyName, 
		const UnicodeString& propertyType, const UnicodeString& comment, 
		TokenClass::TokenIds visibility, bool isConst, bool isStatic);
		
	/**
	 * Implementation of FunctionObserver.  In this method we will append to the outline buffer.
	 */
	void FunctionFound(const UnicodeString& functionName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment);
	
private:		
	
	/**
	 * To parse the class/methods from the files
	 * @var ParserClass
	 */
	ParserClass Parser;
	
	/**
	 * Outline to keep the results of the current outline.  We will be continually appending to this vector.
	 * We need this vector because for the current outline we dont use ResourceFinder hence the results are not sorted
	 */
	std::vector<UnicodeString> CurrentOutlineLines;
	
	/**
	 * Returns a human-friendly outline based on the raw text in CurrentOutlineLines
	 * 
	 * @return UnicodeString
	 */
	UnicodeString HumanFriendlyOutline();
		
	/**
	 * Updates the outlines based on the currently opened (and focused) file.
	*/
	void OnContentNotebookPageChanged(wxAuiNotebookEvent& event);
	
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
	 * refresh the code control from the plugin source strings
	 */
	 void RefreshOutlines();
	
protected:

	/**
	 * Shows the help
	 */
	void OnHelpButton(wxCommandEvent& event);
	
	/**
	 * take the entered text, perform a resource lookup, and get the outline
	 */
	void OnLookupButton(wxCommandEvent& event);

	/**
	 * sync the outline with the currently opened file
	 */
	void OnSyncButton(wxCommandEvent& event);

private:

	/**
	 * The code control for the current file
	 * @var CodeControlClass
	 */
	CodeControlClass* Outline1;
	
	/**
	 * The code control for the PHPDoc comments
	 * @var CodeControlClass
	 */
	CodeControlClass* Outline3;

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
	 * when the panel is closed do some cleanup.
	 */
	void OnClose(wxCloseEvent& event);

	/**
	 * On right-click we will allow the user to jump to a resource
	 */
	void OnContextMenu(wxContextMenuEvent& event);

	/**
	 * take the user to a resource
	 */
	void OnOutlineJumpTo(wxCommandEvent& event);

	/**
	 * show the user the resource comment
	 */
	void OnOutlineJumpToComment(wxCommandEvent& event);

	/**
	 * Get the resource that was clicked on by the user.  This method will figure out which of the outline windows the user clicked
	 * on, get the selected line, and parse out the resource. The return of this method is suitable for passing to
	 * ResourceFinderClass::Prepare and it will make ResourceFinderClass::CollectFullyQualifiedMatch return TRUE.
	 * @param wxCommandEvent the menu event
	 */
	wxString ResourceFromOutline(wxCommandEvent& event);

	
	DECLARE_EVENT_TABLE()
	
};

}

#endif