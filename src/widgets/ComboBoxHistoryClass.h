/**
 * @copyright  2009-2011 Roberto Perpuly
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
#ifndef SRC_WIDGETS_COMBOBOXHISTORYCLASS_H_
#define SRC_WIDGETS_COMBOBOXHISTORYCLASS_H_

#include <wx/combobox.h>
#include <wx/event.h>

namespace t4p {
/**
 * This class add functionality to a wxComboBox.  It will remember previous user inputs and recall them.
 * It will do so by listening to its events and acting accordingly.
 * This class also works with combo boxes withing modal dialogs accross different dialog instances (with the caveat that
 * this instance be alive longer than the dialog).  If using modal dialogs, use the Attach() and Detach() methods instead
 * of th
 */
class ComboBoxHistoryClass : public wxEvtHandler {
 public:
    /**
     * Construct a new history
     * @param The combo box we are to add history to.  We will not own this pointer; calling code needs to delete it.
     * Can be NULL.  IF NULL, nothing happens.
     */
    ComboBoxHistoryClass(wxComboBox* combo = NULL);

    /**
     * Cleanup.
     */
    ~ComboBoxHistoryClass();

    /**
     * This method is appropriate for combo boxes withing modal dialogs. Will prepopulate items from the
     * previous runs.  Note that you must call detach so that events are properly disconnected.
     * Also note that this object can only Attach to one combobox at a time.
     */
    void Attach(wxComboBox* combo);

    /**
     * This method can be called to save the current combo's value to the history.  While this object
     * is attached to the EVT_TEXT_ENTER event. there may be other cases when the history needs to be saved
     * (for example if the dialog has an OK button).
     * Precondition: combobox MUST have been given in the constructor or via the Attach() method.
     */
    void Save();

    /**
     * Cleanup for comboxes inside modal dialogs.
     */
    void Detach();

 private:
    /**
     * We will use this only for modal dialogs; in order to persist items accross many instances of the same dialog.
     * @param wxArrayString
     */
    wxArrayString Items;

    /**
     * The combo box we are managing.  We will not own this pointer; calling code needs to delete it
     */
    wxComboBox* Combo;

    /**
     * When an item is chosen, add it to the combo box list.
     *
     * @param wxCommandEvent* event
     */
    void OnComboSelected(wxCommandEvent& event);
};
}  // namespace t4p

#endif  // SRC_WIDGETS_COMBOBOXHISTORYCLASS_H_
