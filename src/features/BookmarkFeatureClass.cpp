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
#include <features/BookmarkFeatureClass.h>
#include <algorithm>

t4p::BookmarkClass::BookmarkClass()
: FileName()
, LineNumber(0)
, Handle(-1)
{
}

t4p::BookmarkClass::BookmarkClass(const wxFileName& fileName, int lineNumber, int handle)
: FileName(fileName)
, LineNumber(lineNumber)
, Handle(handle)
{
}

t4p::BookmarkClass::BookmarkClass(const t4p::BookmarkClass& src) 
: FileName()
, LineNumber(0) {
	Copy(src);
}

bool t4p::BookmarkClass::operator==(const t4p::BookmarkClass& other) {
	return other.LineNumber == LineNumber 
		&& other.FileName == FileName;
}

t4p::BookmarkClass& t4p::BookmarkClass::operator=(const t4p::BookmarkClass& src) {
	Copy(src);
	return *this;
}

void t4p::BookmarkClass::Copy(const t4p::BookmarkClass& src) {
	FileName = src.FileName;
	LineNumber = src.LineNumber;
	Handle = src.Handle;
}

t4p::BookmarkFeatureClass::BookmarkFeatureClass(t4p::AppClass& app)
: FeatureClass(app)
, Bookmarks()
, CurrentBookmarkIndex(-1)
{
}

void t4p::BookmarkFeatureClass::AddEditMenuItems(wxMenu* editMenu) {
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

void t4p::BookmarkFeatureClass::AddKeyboardShortcuts(std::vector<t4p::DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[t4p::MENU_BOOKMARK + 0] = wxT("Edit-Toggle Bookmark");
	menuItemIds[t4p::MENU_BOOKMARK + 1] = wxT("Edit-Go to next Bookmark");
	menuItemIds[t4p::MENU_BOOKMARK + 2] = wxT("Edit-Go to previous Bookmark");
	AddDynamicCmd(menuItemIds, shortcuts);
}


void t4p::BookmarkFeatureClass::OnEditToggleBookmark(wxCommandEvent& event) {
	t4p::CodeControlClass* ctrl = GetCurrentCodeControl();
	
	// not sure how to handle buffers that have not been saved at all
	if (!ctrl || ctrl->IsNew()) {
		return;
	}
	
	// look to see if the line is already bookmarked
	t4p::BookmarkClass toFind;
	toFind.FileName.Assign(ctrl->GetFileName());
	toFind.LineNumber = ctrl->GetCurrentLine() + 1; // scintilla lines start at 0
	
	std::vector<t4p::BookmarkClass>::iterator it;
	it = std::find(Bookmarks.begin(), Bookmarks.end(), toFind);
	if (it == Bookmarks.end()) {
			
		// line has not been bookmarked, lets add it
		wxFileName fileName(ctrl->GetFileName());
		int lineNumber = 0;
		int handle = -1;
		bool added = ctrl->BookmarkMarkCurrent(lineNumber, handle);
		if (added) {
			t4p::BookmarkClass newBookmark(fileName, lineNumber, handle);
			Bookmarks.push_back(newBookmark);
		}
	}
	else {
		
		// need to remove the bookmark from the list AND from
		// the code control
		ctrl->BookmarkClearAt(it->LineNumber);
		Bookmarks.erase(it);
	}
}

void t4p::BookmarkFeatureClass::OnEditClearAllBookmarks(wxCommandEvent& event) {
	Bookmarks.clear();
	
	// remove any existing bookmarks from the code controls also
	t4p::NotebookClass* notebook = GetNotebook();
	for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
		t4p::CodeControlClass* ctrl = notebook->GetCodeControl(i);
		ctrl->BookmarkClearAll();
	}
}


void t4p::BookmarkFeatureClass::OnEditNextBookmark(wxCommandEvent& event) {
	int nextIndex = CurrentBookmarkIndex + 1;
	int lastIndex = Bookmarks.size() - 1;
	if (nextIndex > lastIndex) {
		nextIndex = 0;
	}
	if (!Bookmarks.empty()) {
		ShowBookmark(Bookmarks[nextIndex]);
		CurrentBookmarkIndex = nextIndex;
	}
}

void t4p::BookmarkFeatureClass::OnEditPreviousBookmark(wxCommandEvent& event) {
	int prevIndex = CurrentBookmarkIndex - 1;
	if (prevIndex < 0) {
		prevIndex = Bookmarks.size() - 1;
	}
	if (!Bookmarks.empty()) {
		ShowBookmark(Bookmarks[prevIndex]);
		CurrentBookmarkIndex = prevIndex;
	}
}

void t4p::BookmarkFeatureClass::ShowBookmark(const t4p::BookmarkClass& bookmark) {
	t4p::NotebookClass* notebook = GetNotebook();
	
	t4p::CodeControlClass* bookmarkCtrl = NULL;
	int selectionIndex = -1;
	for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
		t4p::CodeControlClass* ctrl = notebook->GetCodeControl(i);
		if (!ctrl->IsNew()) {
			wxFileName ctrlFileName(ctrl->GetFileName());
			if (ctrlFileName == bookmark.FileName) {
				selectionIndex = i;
				bookmarkCtrl = ctrl;
				break;
			}
		}
	}
	
	if (bookmarkCtrl) {
		
		// the bookmark may be in a page that is not active, need to
		// swith notebook tabs if needed
		if (selectionIndex != notebook->GetSelection()) {
			notebook->SetSelection(selectionIndex);
		}
		
		// scintilla line numbers are 1 based, ours are 1 based coz we want
		// to show them to the user
		bookmarkCtrl->GotoLine(bookmark.LineNumber - 1);
	}
	else {
		
		// need to open the file first
		GetNotebook()->LoadPage(bookmark.FileName.GetFullPath());
		t4p::CodeControlClass* newlyOpenedCtrl = GetCurrentCodeControl();
		
		// now we add all bookmarks for the newly opened file
		AddBookmarks(bookmark.FileName, newlyOpenedCtrl);
		
		// scintilla line numbers are 1 based, ours are 1 based coz we want
		// to show them to the user
		newlyOpenedCtrl->GotoLine(bookmark.LineNumber - 1);
	}
}

void t4p::BookmarkFeatureClass::AddBookmarks(const wxFileName& fileName, t4p::CodeControlClass* ctrl) {
	std::vector<t4p::BookmarkClass>::iterator it;
	it = Bookmarks.begin(); 
	while (it != Bookmarks.end()) {
		if (it->FileName == fileName) {
			int handle = -1;
			bool added = ctrl->BookmarkMarkAt(it->LineNumber, handle);
			if (!added) {
				
				// file does not contain this line number, this is a bad bookmark
				it = Bookmarks.erase(it);
			}
			else {
				it->Handle = handle;
				++it;
			}
		}
	}
}

void t4p::BookmarkFeatureClass::OnStyledTextModified(wxStyledTextEvent& event) {
	int mask = wxSTC_MOD_INSERTTEXT | wxSTC_MOD_DELETETEXT;
	if (event.GetModificationType() & mask) {
		
		// lets update the location of the bookmarks in this file
		t4p::CodeControlClass* ctrl = (t4p::CodeControlClass*)event.GetEventObject();
		if (!ctrl->IsNew()) {
			wxFileName ctrlFileName(ctrl->GetFileName());
			std::vector<t4p::BookmarkClass>::iterator it;
			for (it = Bookmarks.begin(); it != Bookmarks.end(); ++it) {
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

void t4p::BookmarkFeatureClass::OnAppFileReverted(t4p::CodeControlEventClass& event) {
	t4p::CodeControlClass* ctrl = event.GetCodeControl();
	if (ctrl) {
		wxFileName ctrlFileName(ctrl->GetFileName());
		
		// file has been reverted, all bookmarks in the file have been deleted
		// delete the bookmarks from out list
		std::vector<t4p::BookmarkClass>::iterator it;
		it = Bookmarks.begin(); 
		while (it != Bookmarks.end()) {
			if (it->FileName == ctrlFileName) {
				it = Bookmarks.erase(it);
			}
			else {
				++it;
			}
		}
	}
}


BEGIN_EVENT_TABLE(t4p::BookmarkFeatureClass, t4p::FeatureClass)
	EVT_MENU(t4p::MENU_BOOKMARK + 0, t4p::BookmarkFeatureClass::OnEditToggleBookmark)
	EVT_MENU(t4p::MENU_BOOKMARK + 1, t4p::BookmarkFeatureClass::OnEditNextBookmark)
	EVT_MENU(t4p::MENU_BOOKMARK + 2, t4p::BookmarkFeatureClass::OnEditPreviousBookmark)
	EVT_MENU(t4p::MENU_BOOKMARK + 3, t4p::BookmarkFeatureClass::OnEditClearAllBookmarks)
	EVT_STC_MODIFIED(wxID_ANY, t4p::BookmarkFeatureClass::OnStyledTextModified)
	EVT_APP_FILE_REVERTED(t4p::BookmarkFeatureClass::OnAppFileReverted)
END_EVENT_TABLE()

