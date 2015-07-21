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
#ifndef SRC_VIEWS_NOTEBOOKLAYOUTVIEWCLASS_H_
#define SRC_VIEWS_NOTEBOOKLAYOUTVIEWCLASS_H_

#include <wx/menu.h>
#include "features/FeatureClass.h"
#include "features/NotebookLayoutFeatureClass.h"
#include "views/FeatureViewClass.h"

namespace t4p {
/**
 * The NotebookLayoutViewClass view allows the user to create multiple
 * notebooks.  This allows for the user to view files side-by-side in
 * a vertical or horizontal manner. The user is able to quiickly
 * create 2 - 3 columns, or 2 - 3 rows of notebooks. The user can
 * also split a notebook at any time.
 *
 */
class NotebookLayoutViewClass : public t4p::FeatureViewClass {
 public:
    NotebookLayoutViewClass(t4p::NotebookLayoutFeatureClass& feature);

    void AddViewMenuItems(wxMenu* viewMenu);

    void AddToolBarItems(wxAuiToolBar* toolBar);

 private:
    /**
     * handler for the notebook menu event
     */
    void OnNotebookMenu(wxCommandEvent& event);

    /**
     * Create notebooks as 2, or 3 columns
     */
    void OnNotebookCreateColumns(wxCommandEvent& event);

    /**
     * Create notebooks as 2 or 3 rows
     */
    void OnNotebookCreateRows(wxCommandEvent& event);

    /**
     * Create notebooks as 2x2, or 3x2, or 2x3 grid
     */
    void OnNotebookCreateGrid(wxCommandEvent& event);

    /**
     * Create a single notebook
     */
    void OnNotebookReset(wxCommandEvent& event);

    /**
     * To get access to the globals; needed by the notebook.
     */
    // t4p::NotebookLayoutFeatureClass& Feature;

    DECLARE_EVENT_TABLE()
};
}  // namespace t4p

#endif  // SRC_VIEWS_NOTEBOOKLAYOUTVIEWCLASS_H_
