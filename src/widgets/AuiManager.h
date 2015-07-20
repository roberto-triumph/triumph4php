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
#ifndef T4P_AUIMANAGER_H
#define T4P_AUIMANAGER_H

#include <wx/aui/aui.h>
#include <vector>

namespace t4p {
// forward declaration, defined in another file
class NotebookClass;

/*
 * This is a bit of documentation about how Triumph handles
 * split notebooks and the frame layout:
 *
 * The number one thing is to understand how AUI positioning
 * works.  The wx_aui_tutorial program can give you an interactive
 * prototype that you can use after reading this.
 *
 * AUI has docks, rows, and positions (and layers, but Triumph does
 * not use them for now).
 *
 * A frame is divided up into 5 docks:
 * top, left, center, right, and bottom.
 *
 * Inside each dock, there can be multiple panels.  Each panel has
 * a row, a position, and a layer relative to all other panels in
 * the same dock. The positioning is dependant on the dock;
 * for example; in the right dock, the panel with row 0 is to the
 * right of the panel with row 1, and the panel with position 0
 * is above the panel with position 1. For the bottom dock, the
 * panel with row 0 is below the panel with row 1, and the panel
 * with position 0 is to the left of the panel with position 1.
 * You can now see that performing a "split horizontally" operation
 * means that we need to change either the row or the position
 * depending on which notebook we want to split. A slightly special
 * case is done when splitting the center panel, since AUI only
 * allows 1 window to be in the content pane, if we placed multiple
 * panels in the center pane AUI does not draw a sash and we can't
 * resize them. We split the center notebook by placing the new
 * notebook in either the bottom or right dock.
 *
 * Having the split operation be smooth (without flicker) took a great deal of
 * effort. It turns out that adding and removing notebooks as each split
 * is desired (on-demand) caused too much flicker for my liking,
 * even after I attempted to use wxWindowUpdateLocker and minimizing
 * the calls to wxAuiManager::Update. The solution was to just
 * create all notebooks at program start, then hide or show them
 * as needed. That is the purpose of these functions here: we
 * have functions to get only the visible notebooks, or to get all
 * of the notebooks.
 *
 */


/**
 * The maximum number of code notebooks that are available.
 */
extern const int AUI_MAX_CODE_NOTEBOOKS;

/**
 * Adds the notebook for use in the AUI manager. This prep
 * is specific to Triumph; Triumph will assign a specific
 * name to the noteboks' AUI pane info so that we can later
 * recall the notebooks with ease.
 *
 * @param auiManager the AUI manager of the main frame
 * @param notebook the notebook to be added to the AUI manager
 * @param notebookNumber notebook number 1 will be added to the center dock,
 *        all others will be added to the right dock.
 */
void AuiAddCodeNotebook(wxAuiManager& manager, t4p::NotebookClass* notebook, int notebookNumber);

/**
 * Fetch the code notebooks that the user can see.
 *
 * This function depends on the fact that all notebook class
 * objects a similar the same aui pane name ("Notebook N"); the
 * naming is taken care of by previous calls to AuiAddNotebook()
 *
 * @param auiManager the AUI manager of the main frame
 * @return vector all of the code notebooks that are in the
 *         main application frame that the user can see.
 */
std::vector<t4p::NotebookClass*> AuiVisibleCodeNotebooks(wxAuiManager& auiManager);

/**
 * Fetch the code notebooks that the user can see.
 *
 * This function depends on the fact that all notebook class
 * objects a similar the same aui pane name ("Notebook N"); the
 * naming is taken care of by previous calls to AuiAddNotebook()*
 *
 * @param auiManager the AUI manager of the main frame
 * @return vector all of the code notebooks that are in the
 *         main application frame, even ones that are not visible.
 */
std::vector<t4p::NotebookClass*> AuiAllCodeNotebooks(wxAuiManager& auiManager);

/**
 * "Reset" here means that we will modify the AUI panes to be
 * in the default state: The code notebook in the center pane will be visible,
 * and all other notebooks will no longer be visible.
 *
 * This function does NOT commit the AUI changes (does not call AuiManager::Update())
 * so that the caller can perform more AUI changes if needed.
 *
 * @param auiManager the AUI manager of the main frame
 */
void AuiResetCodeNotebooks(wxAuiManager& manager);

/**
 * Looks for and returns any one of the notebooks that is currently hidden.
 *
 * @param auiManager the AUI manager of the main frame
 * @return NotebookClass a hidden notebook, or NULL if there are no
 *         more hidden code notebooks
 */
t4p::NotebookClass* AuiNextHiddenCodeNotebook(wxAuiManager& auiManager);

/**
 * Calculate the highest visible row number for the given row. The
 * row determines where a window is positioned; this function is useful
 * when splitting notebooks where the dock is already occupied.
 *
 * @param auiManager the AUI manager of the main frame
 * @param dock the dock to query, top, bottom, left, or right
 * @return the highest row number
 */
int AuiRowCount(wxAuiManager& auiManager, int dock);
}

#endif

