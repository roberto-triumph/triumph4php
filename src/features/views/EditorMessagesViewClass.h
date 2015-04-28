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
#ifndef T4P_EDITORMESSAGESVIEWCLASS_H__
#define T4P_EDITORMESSAGESVIEWCLASS_H__

#include <features/views/FeatureViewClass.h>
#include <features/wxformbuilder/EditorMessagesFeatureForms.h>

namespace t4p {
	
// forward declaration, defined in another file
class EditorLogEventClass;

class EditorMessagesViewClass : public t4p::FeatureViewClass {

public:

	EditorMessagesViewClass();
	
	void AddViewMenuItems(wxMenu* toolsMenu);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
private:

	/**
	 * When the user clicks on the editor messages menu
	 * show the editor messages window
	 */
	void OnMenu(wxCommandEvent& event);
	
	void OnAppLog(t4p::EditorLogEventClass& event);
		
	/**
	 * create the messages panel (or use the existing one) and
	 * add a message to it.
	 */
	void AddMessage(wxLogLevel level, const wxChar *msg, time_t timestamp);

	DECLARE_EVENT_TABLE();
};

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

}

#endif
