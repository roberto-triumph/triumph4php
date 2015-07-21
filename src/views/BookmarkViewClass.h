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
#ifndef SRC_VIEWS_BOOKMARKVIEWCLASS_H_
#define SRC_VIEWS_BOOKMARKVIEWCLASS_H_

#include <wx/stc/stc.h>
#include <vector>
#include "features/BookmarkFeatureClass.h"
#include "globals/Events.h"
#include "views/FeatureViewClass.h"

namespace t4p {
/**
 * The bookmark feature keeps track of bookmarks at the user's request.
 * The bookmark feature allows the user to cycle through their
 * bookmarkes using keyboard shortcuts.
 *
 * While the user specifies a file/line number, in reality the
 * true power of a bookmark lies in the ability to point to a
 * section of code rather than a line number itself. For example,
 * the user bookmarks the start of a function. Even if the user
 * adds a new function before the bookmarked function, the bookmark
 * will still point to the bookmarked function.
 *
 * Bookmarks can be created on any file, not just files inside
 * of projects.
 *
 * For now, bookmarks are not persisted across restarts.  This
 * is because bookmarks are often used for short periods of time.
 */
class BookmarkViewClass : public t4p::FeatureViewClass {
 public:
    BookmarkViewClass(t4p::BookmarkFeatureClass& feature);

    /**
     * the user can toggle bookmarks via the edit menu
     */
    void AddEditMenuItems(wxMenu* editMenu);

    /**
     * adds keyboard shortcuts for the bookmark functionality
     */
    void AddKeyboardShortcuts(std::vector<t4p::DynamicCmdClass>& shortcuts);

 private:
    /**
     * this menu handler will toggle the bookmark in the current
     * file / line on or off.
     */
    void OnEditToggleBookmark(wxCommandEvent& event);

    /**
     * this menu handler will clear all bookmarks
     */
    void OnEditClearAllBookmarks(wxCommandEvent& event);

    /**
     * takes the user to the next bookmarked place
     */
    void OnEditNextBookmark(wxCommandEvent& event);

    /**
     * takes the user to the previous bookmarked place
     */
    void OnEditPreviousBookmark(wxCommandEvent& event);

    /**
     * takes the user to the given bookmakrk. if the file
     * is not open, it will be opened.
     */
    void ShowBookmark(const t4p::BookmarkClass& bookmark);

    /**
     * Adds all bookmarks for the file to the given code control
     * but it does not acutally move the cursor
     */
    void AddBookmarks(const wxFileName& fileName, t4p::CodeControlClass* ctrl);

    /**
     * we capture event from the styled text control so that we know
     * when a bookmark has moved up/down a line.  If that's the case,
     * then we update our internal bookmarks list
     */
    void OnStyledTextModified(wxStyledTextEvent& event);

    /**
     * when the user reverts a file, we must add in the bookmarks
     * again, since they get deleted on file reload
     */
    void OnAppFileReverted(t4p::CodeControlEventClass& event);

    t4p::BookmarkFeatureClass& Feature;

    DECLARE_EVENT_TABLE()
};
}  // namespace t4p

#endif  // SRC_VIEWS_BOOKMARKVIEWCLASS_H_
