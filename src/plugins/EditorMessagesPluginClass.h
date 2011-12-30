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
#ifndef __EDITORMESSAGESPLUGINCLASS_H__
#define __EDITORMESSAGESPLUGINCLASS_H__

#include <PluginClass.h>
#include <plugins/wxformbuilder/EditorMessagesPluginGeneratedClass.h>
#include <wx/log.h>

namespace mvceditor {

/**
 * This class will display a grid containing all of the 
 * messages.
 */
class EditorMessagesPanelClass : public EditorMessagesGeneratedPanelClass {

public:

	EditorMessagesPanelClass(wxWindow* parent, int id);

	/**
	 * Add a message to the display grid.
	 */
	void AddMessage(wxLogLevel level, const wxChar *msg, time_t timestamp);

protected:

	/**
	 * Removes all messages from the display grid.
	 */
	void OnClearButton(wxCommandEvent& event);

};

/**
 * This class is the handler for the editor message menu items.  
 */
class EditorMessagesPluginClass : public PluginClass, wxLog {

public:

	EditorMessagesPluginClass();

	void AddToolsMenuItems(wxMenu* toolsMenu);

	/**
	 * create the messages panel (or use the existing one) and
	 * add a message to it.
	 */
	void AddMessage(wxLogLevel level, const wxChar *msg, time_t timestamp);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);


private:

	/**
	 * When the user clicks on the editor messages menu
	 * show the editor messages window
	 */
	void OnMenu(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

/**
 * This class will take care of receiving the log messages
 * and proxying them to the plugin. 
 * It is done this way because wxWidgets will delete the Log target
 * (pointer given to wxLog::SetLogTarget()) so we need to
 * give it a pointer that is managed by wxWidgets
 */
class EditorMessagesLoggerClass : public wxLog {

public:

	EditorMessagesLoggerClass(EditorMessagesPluginClass& plugin);

	void DoLog(wxLogLevel level, const wxChar *msg, time_t timestamp);

private:

	EditorMessagesPluginClass& Plugin;
};

}
#endif