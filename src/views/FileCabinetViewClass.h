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
#ifndef T4P_FILECABINETVIEWCLASS_H
#define T4P_FILECABINETVIEWCLASS_H

#include <views/FeatureViewClass.h>
#include <features/FileCabinetFeatureClass.h>
#include <views/wxformbuilder/FileCabinetFeatureForms.h>
#include <widgets/FileTypeImageList.h>
#include <globals/FileCabinetItemClass.h>
#include <globals/Sqlite.h>

namespace t4p {

class FileCabinetViewClass : public t4p::FeatureViewClass {

public:

	FileCabinetViewClass(t4p::FileCabinetFeatureClass& feature);

	void AddEditMenuItems(wxMenu* editMenu);
	void AddViewMenuItems(wxMenu* viewMenu);

private:

	void OnViewFileCabinet(wxCommandEvent& event);
	void OnEditAddCurrentFileToCabinet(wxCommandEvent& event);

	t4p::FileCabinetFeatureClass& Feature;

	DECLARE_EVENT_TABLE()
};


/**
 * The FileCabinetPanelClass displays the saved files/directories
 * and allows the user to add a file/dir to the cabinet.
 */
class FileCabinetPanelClass : public FileCabinetPanelGeneratedClass {

public:

	FileCabinetPanelClass(wxWindow* parent, int id, t4p::FileCabinetFeatureClass& feature, wxWindow* mainWindow);

	/**
	 * add a file to the file cabinet list
	 */
	void AddItemToList(const t4p::FileCabinetItemClass& item);

private:

	// event handlers
	void OnAddFileClick(wxCommandEvent& event);
	void OnAddDirectoryClick(wxCommandEvent& event);
	void OnListItemActivated(wxListEvent& event);
	void OnListItemDelete(wxCommandEvent& event);
	void OnListItemRightClick(wxListEvent& event);
	void OnHelpButton(wxCommandEvent& event);
	void OnListItemOpen(wxCommandEvent& event);

	/**
	 * Deletes the item at index.  The item is deleted from
	 * the list and the file cabinet table.
	 *
	 * @param int index list item index
	 */
	void DeleteItemAt(int index);

	/**
	 * Opens the item at index.
	 *
	 * @param int index list item index
	 */
	void OpenItemAt(int index);

	/**
	 * fill the list control with the items that are
	 * already stored in the file cabinet
	 */
	void FillList();

	/**
	 * to list the existing files/dirs in the
	 * file cabinet
	 */
	t4p::SqliteFinderClass SqliteFinder;
	t4p::AllFileCabinetResultClass FileCabinet;

	/**
	 * to get the sqlite connection where the file cabinet
	 * is stored
	 */
	t4p::FileCabinetFeatureClass& Feature;

	/**
	 * images for the files/directories
	 *  The list control will own this pointer
	 */
	wxImageList* ImageList;

	/**
	 * The parent window. Used to make sure dialogs are centered
	 */
	wxWindow* MainWindow;

	enum ListImages {
		LIST_FOLDER = t4p::IMGLIST_NONE + 1
	};

	DECLARE_EVENT_TABLE()
};

}

#endif // T4P_FILECABINETVIEWCLASS_H
