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
 #include <features/views/BookmarkViewClass.h>
 #include <features/BookmarkFeatureClass.h>
 
 t4p::BookmarkViewClass::BookmarkViewClass()
: FeatureViewClass() {
}

void t4p::BookmarkViewClass::AddEditMenuItems(wxMenu* editMenu) {
	editMenu->Append(t4p::MENU_BOOKMARK + 0,
		_("Toggle Bookmark\tCTRL+B"), 
		_("Toggle Bookmark on or off in the current cursor location"), 
		wxITEM_NORMAL
	);
	editMenu->Append(t4p::MENU_BOOKMARK + 1,
		_("Go to next Bookmark\tF2"), 
		_("Navigate the cursor to the next bookmark"), 
		wxITEM_NORMAL
	);
	editMenu->Append(t4p::MENU_BOOKMARK + 2,
		_("Go to previous Bookmark\tSHIFT+F2"), 
		_("Navigate the cursor to the previous bookmark"), 
		wxITEM_NORMAL
	);
	editMenu->Append(t4p::MENU_BOOKMARK + 3,
		_("Clear all Bookmarks"), 
		_("Clears all of the bookmarks for all files"), 
		wxITEM_NORMAL
	);
}

void t4p::BookmarkViewClass::AddKeyboardShortcuts(std::vector<t4p::DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[t4p::MENU_BOOKMARK + 0] = wxT("Edit-Toggle Bookmark");
	menuItemIds[t4p::MENU_BOOKMARK + 1] = wxT("Edit-Go to next Bookmark");
	menuItemIds[t4p::MENU_BOOKMARK + 2] = wxT("Edit-Go to previous Bookmark");
	AddDynamicCmd(menuItemIds, shortcuts);
}