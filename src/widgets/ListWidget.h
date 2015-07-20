/**
 * @copyright  2014 Roberto Perpuly
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

#ifndef SRC_WIDGETS_LISTWIDGET_H_
#define SRC_WIDGETS_LISTWIDGET_H_

#include <wx/listctrl.h>
#include <wx/string.h>

namespace t4p {
/**
 * A helper function to add a row into a list control. list control must have 2 columns
 *
 * @param list the list to add to
 * @param column1Value the value for column 1
 * @param column2Value the value for column 2
 */
void ListCtrlAdd(wxListCtrl* list, const wxString& column1Value, const wxString& column2Value);

/**
 * A helper function to edit a list control row's contents
 *
 * @param list the list to add to
 * @param column1Value the new value for column 1
 * @param column2Value the new value for column 2
 * @param rowIndex 0-based into of row to change
 */
void ListCtrlEdit(wxListCtrl* list, const wxString& column1Value, const wxString& column2Value, int rowIndex);

/**
 * extract the column values from the given list control
 * @param list the list to extract from
 * @param column1Value will be filled with the contents of (rowIndex, 0)
 * @param column2Value will be filled with the contents of (rowIndex, 1)
 * @param rowIndex the row to get
 */
void ListCtrlGet(wxListCtrl* list, wxString& column1Value, wxString& column2Value, int rowIndex);

/**
 * @return int the row number that is selected, or wxNOT_FOUND
 *         if no row is selected.
 */
int ListCtrlSelected(wxListCtrl* ctrl);
}  // namespace t4p

#endif  // SRC_WIDGETS_LISTWIDGET_H_
