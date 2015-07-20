/**
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#include <views/BookmarkViewClass.h>
#include <features/BookmarkFeatureClass.h>
#include <code_control/CodeControlClass.h>
#include <map>
#include <vector>

t4p::BookmarkViewClass::BookmarkViewClass(t4p::BookmarkFeatureClass& feature)
: FeatureViewClass()
, Feature(feature) {
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

void t4p::BookmarkViewClass::OnEditToggleBookmark(wxCommandEvent& event) {
	t4p::CodeControlClass* ctrl = GetCurrentCodeControl();

	// not sure how to handle buffers that have not been saved at all
	if (!ctrl || ctrl->IsNew()) {
		return;
	}

	// look to see if the line is already bookmarked
	t4p::BookmarkClass toFind;
	toFind.FileName.Assign(ctrl->GetFileName());
	toFind.LineNumber = ctrl->GetCurrentLine() + 1;  // scintilla lines start at 0

	std::vector<t4p::BookmarkClass>::iterator it;
	it = std::find(Feature.Bookmarks.begin(), Feature.Bookmarks.end(), toFind);
	if (it == Feature.Bookmarks.end()) {
		// line has not been bookmarked, lets add it
		wxFileName fileName(ctrl->GetFileName());
		int lineNumber = 0;
		int handle = -1;
		bool added = ctrl->BookmarkMarkCurrent(lineNumber, handle);
		if (added) {
			t4p::BookmarkClass newBookmark(fileName, lineNumber, handle);
			Feature.Bookmarks.push_back(newBookmark);
		}
	} else {
		// need to remove the bookmark from the list AND from
		// the code control
		ctrl->BookmarkClearAt(it->LineNumber);
		Feature.Bookmarks.erase(it);
	}
}

void t4p::BookmarkViewClass::OnEditClearAllBookmarks(wxCommandEvent& event) {
	Feature.Bookmarks.clear();

	// remove any existing bookmarks from the code controls also
	std::vector<t4p::CodeControlClass*> ctrls = AllCodeControls();
	for (size_t i = 0; i < ctrls.size(); ++i) {
		ctrls[i]->BookmarkClearAll();
	}
}


void t4p::BookmarkViewClass::OnEditNextBookmark(wxCommandEvent& event) {
	int nextIndex = Feature.CurrentBookmarkIndex + 1;
	int lastIndex = Feature.Bookmarks.size() - 1;
	if (nextIndex > lastIndex) {
		nextIndex = 0;
	}
	if (!Feature.Bookmarks.empty()) {
		ShowBookmark(Feature.Bookmarks[nextIndex]);
		Feature.CurrentBookmarkIndex = nextIndex;
	}
}

void t4p::BookmarkViewClass::OnEditPreviousBookmark(wxCommandEvent& event) {
	int prevIndex = Feature.CurrentBookmarkIndex - 1;
	if (prevIndex < 0) {
		prevIndex = Feature.Bookmarks.size() - 1;
	}
	if (!Feature.Bookmarks.empty()) {
		ShowBookmark(Feature.Bookmarks[prevIndex]);
		Feature.CurrentBookmarkIndex = prevIndex;
	}
}

void t4p::BookmarkViewClass::ShowBookmark(const t4p::BookmarkClass& bookmark) {
	t4p::CodeControlClass* bookmarkCtrl = FindCodeControlAndSelect(bookmark.FileName.GetFullPath());
	if (bookmarkCtrl) {
		bookmarkCtrl->GotoLineAndEnsureVisible(bookmark.LineNumber);
	} else {
		// need to open the file first
		LoadCodeControl(bookmark.FileName.GetFullPath());
		t4p::CodeControlClass* newlyOpenedCtrl = GetCurrentCodeControl();

		// now we add all bookmarks for the newly opened file
		AddBookmarks(bookmark.FileName, newlyOpenedCtrl);
		newlyOpenedCtrl->GotoLineAndEnsureVisible(bookmark.LineNumber);
	}
}

void t4p::BookmarkViewClass::AddBookmarks(const wxFileName& fileName, t4p::CodeControlClass* ctrl) {
	std::vector<t4p::BookmarkClass>::iterator it;
	it = Feature.Bookmarks.begin();
	while (it != Feature.Bookmarks.end()) {
		if (it->FileName == fileName) {
			int handle = -1;
			bool added = ctrl->BookmarkMarkAt(it->LineNumber, handle);
			if (!added) {
				// file does not contain this line number, this is a bad bookmark
				it = Feature.Bookmarks.erase(it);
			} else {
				it->Handle = handle;
				++it;
			}
		} else {
			++it;
		}
	}
}

void t4p::BookmarkViewClass::OnStyledTextModified(wxStyledTextEvent& event) {
	int mask = wxSTC_MOD_INSERTTEXT | wxSTC_MOD_DELETETEXT;
	if (event.GetModificationType() & mask) {
		// lets update the location of the bookmarks in this file
		t4p::CodeControlClass* ctrl = (t4p::CodeControlClass*)event.GetEventObject();
		if (!ctrl->IsNew()) {
			wxFileName ctrlFileName(ctrl->GetFileName());
			std::vector<t4p::BookmarkClass>::iterator it;
			for (it = Feature.Bookmarks.begin(); it != Feature.Bookmarks.end(); ++it) {
				if (it->FileName == ctrlFileName) {
					// only update when line >= 1.  if line == 0 then it means
					// that the entire text has been deleted. this is most likely
					// scenario when the user reloads the file.
					it->LineNumber = ctrl->BookmarkGetLine(it->Handle);
				}
			}
		}
	}
}

void t4p::BookmarkViewClass::OnAppFileReverted(t4p::CodeControlEventClass& event) {
	t4p::CodeControlClass* ctrl = event.GetCodeControl();
	if (ctrl) {
		wxFileName ctrlFileName(ctrl->GetFileName());

		// file has been reverted, all bookmarks in the file have been deleted
		// delete the bookmarks from out list
		std::vector<t4p::BookmarkClass>::iterator it;
		it = Feature.Bookmarks.begin();
		while (it != Feature.Bookmarks.end()) {
			if (it->FileName == ctrlFileName) {
				it = Feature.Bookmarks.erase(it);
			} else {
				++it;
			}
		}
	}
}

BEGIN_EVENT_TABLE(t4p::BookmarkViewClass, t4p::FeatureViewClass)
	EVT_MENU(t4p::MENU_BOOKMARK + 0, t4p::BookmarkViewClass::OnEditToggleBookmark)
	EVT_MENU(t4p::MENU_BOOKMARK + 1, t4p::BookmarkViewClass::OnEditNextBookmark)
	EVT_MENU(t4p::MENU_BOOKMARK + 2, t4p::BookmarkViewClass::OnEditPreviousBookmark)
	EVT_MENU(t4p::MENU_BOOKMARK + 3, t4p::BookmarkViewClass::OnEditClearAllBookmarks)
	EVT_STC_MODIFIED(wxID_ANY, t4p::BookmarkViewClass::OnStyledTextModified)
	EVT_APP_FILE_REVERTED(t4p::BookmarkViewClass::OnAppFileReverted)
END_EVENT_TABLE()
