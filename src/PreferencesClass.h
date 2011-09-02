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
#ifndef __preferencesclass__
#define __preferencesclass__

#include <wx/keybinder.h>
#include <widgets/CodeControlOptionsClass.h>

namespace mvceditor {

/**
 * This class is the composite set of all preferences//options that a user can change.
 */
class PreferencesClass {

public:

	/**
	 * Construct preferences. Creates on keyboard profile.
	 */
	PreferencesClass();
	
	/**
	 * Cleanup (delete) the keyboard profile pointers
	 */
	~PreferencesClass();
	
	/**
	 * Load the preferences from persistent storage.
	 * 
	 * @param wxFrame* we need a frame so we can attach keyboard listeners to it as well as get the list of menu items.
	 */
	void Load(wxFrame* frame);
	
	/**
	 * Save the preferences to persistent storage.
	 */
	void Save();
	
	/**
	 * Apply the keyboard shortcuts to the window.  This process involves undoing any previous keyboard shortcuts
	 * and attaching keyboard listeners to the given window. This m,ethod should get called whenver the keyboard
	 * short cuts have changed; if this method does not get called then the shortcuts wont take effect.
	 */
	void LoadKeyboardShortcuts(wxWindow* window);

	/**
	 * Set the config object where the settings are stored on wxConfigBase; after a call to InitConfig()
	 * any other source code can get the config object using wxConfigBase::Get
	 */
	static void InitConfig();
	
	/**
	 * The options for the source code editor
	 * 
	 * @var CodeControlOptionsClass
	 */
	CodeControlOptionsClass CodeControlOptions;
	
	/**
	 * The key shortcuts.  This class will own all pointers in the array and will delete them during
	 * destruction.
	 * 
	 * @var wxKeyProfileArray
	 */
	wxKeyProfileArray KeyProfiles;

};

}
#endif // __preferencesclass__
