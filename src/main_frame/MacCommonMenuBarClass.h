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
#ifndef T4P_MACCOMMONMENUBARCLASS_H
#define T4P_MACCOMMONMENUBARCLASS_H

#include <wx/event.h>
#include <wx/menu.h>
#include <wx/app.h>
#include <memory>

namespace t4p {

/**
 * The man common menu bar is the menu bar that is
 * shown to the user has closed the main frame.  It
 * contains very simple menu that creates a new main
 * frame.
 *
 * This class allows the application to stay running
 * even after the main frame is closed; this behavior
 * is common to Mac Apps.  This class will contain the
 * menu handlers for the menu that is shown when
 * there is no main frame running.
 *
 * This class should only be used on Mac OS X, where
 * the main frame can be closed but the app is still
 * running.
 *
 * DEV note: not handling menu handlers here since
 * the menu events never reach this class, since
 * this class is not connected to the app's
 * event handler system
 */
class MacCommonMenuBarClass {

public:

	enum {
		ID_COMMON_MENU_NEW = wxID_HIGHEST + 1,
		ID_COMMON_MENU_OPEN
	};

	MacCommonMenuBarClass(wxApp& app);

	private:

	wxApp& App;

	/**
	 * menubar that is shown when the main frame is not around.
	 */
	std::auto_ptr<wxMenuBar> CommonMenuBar;

	DECLARE_EVENT_TABLE()
};

}

#endif

