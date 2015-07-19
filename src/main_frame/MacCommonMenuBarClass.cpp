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
#include <main_frame/MacCommonMenuBarClass.h>

t4p::MacCommonMenuBarClass::MacCommonMenuBarClass(wxApp& app)
: App(app)
, CommonMenuBar() {
	// on mac, let the app stay running when the main frame is
	// closed.  this is the correct behavior (like most mac apps)
	#ifdef __WXMAC__
		App.SetExitOnFrameDelete(false);
		CommonMenuBar.reset(new wxMenuBar());
		wxMenu* menu = new wxMenu();
		menu->Append(ID_COMMON_MENU_NEW, _("New File"),  _("Create a new file"), wxITEM_NORMAL);
		menu->Append(ID_COMMON_MENU_OPEN, _("Open File"),  _("Open an existing file"), wxITEM_NORMAL);
		CommonMenuBar->Append(menu, _("File"));
		wxMenuBar::MacSetCommonMenuBar(CommonMenuBar.get());
	#endif
}

