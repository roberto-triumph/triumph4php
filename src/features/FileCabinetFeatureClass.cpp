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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <features/FileCabinetFeatureClass.h>
#include <Triumph.h>
#include <globals/Assets.h>

static int ID_FILE_CABINET_PANEL = wxNewId();
static int ID_FILE_CABINET_DELETE = wxNewId();

t4p::FileCabinetFeatureClass::FileCabinetFeatureClass(t4p::AppClass& app)
: FeatureClass(app)
, Store()
{
}

void t4p::FileCabinetFeatureClass::AddEditMenuItems(wxMenu* editMenu) {
	editMenu->Append(t4p::MENU_FILE_CABINET + 1, _("Add Current File to Cabinet"),
		_("Add Current File to the File Cabinet"), wxITEM_NORMAL);
}

void t4p::FileCabinetFeatureClass::AddViewMenuItems(wxMenu* viewMenu) {
	viewMenu->Append(t4p::MENU_FILE_CABINET + 0, _("File Cabinet"),
		_("Show the File Cabinet"), wxITEM_NORMAL);
}

void t4p::FileCabinetFeatureClass::OnViewFileCabinet(wxCommandEvent& event) {
	wxWindow* window = FindToolsWindow(ID_FILE_CABINET_PANEL);
	t4p::FileCabinetPanelClass* panel = NULL;
	if (window) {
		panel = (t4p::FileCabinetPanelClass*)window;
		SetFocusToToolsWindow(panel);
	}
	else {
		panel = new t4p::FileCabinetPanelClass(GetToolsNotebook(), ID_FILE_CABINET_PANEL, *this);
		AddToolsWindow(panel, _("File Cabinet"), wxT("File cabinet"), t4p::BitmapImageAsset(wxT("file-cabinet")));
	}
}

t4p::FileCabinetItemClass t4p::FileCabinetFeatureClass::AddDirectoryToCabinet(const wxString& dir) {
	t4p::FileCabinetItemClass fileCabinetItem;
	fileCabinetItem.FileName.AssignDir(dir);
	Store.Store(App.Globals.ResourceCacheSession, fileCabinetItem);
	return fileCabinetItem;
}

t4p::FileCabinetItemClass t4p::FileCabinetFeatureClass::AddFileToCabinet(const wxString& file) {
	t4p::FileCabinetItemClass fileCabinetItem;
	fileCabinetItem.FileName.Assign(file);
	Store.Store(App.Globals.ResourceCacheSession, fileCabinetItem);
	return fileCabinetItem;
}

void t4p::FileCabinetFeatureClass::DeleteCabinetItem(int id) {
	Store.Delete(App.Globals.ResourceCacheSession, id);
}

void t4p::FileCabinetFeatureClass::OnEditAddCurrentFileToCabinet(wxCommandEvent& event) {
	t4p::CodeControlClass* ctrl = GetNotebook()->GetCurrentCodeControl();
	if (!ctrl || ctrl->IsNew()) {
		return;
	}
	wxWindow* window = FindToolsWindow(ID_FILE_CABINET_PANEL);
	t4p::FileCabinetPanelClass* panel = NULL;
	if (window) {
		panel = (t4p::FileCabinetPanelClass*)window;
		SetFocusToToolsWindow(panel);
	}
	else {
		panel = new t4p::FileCabinetPanelClass(GetToolsNotebook(), ID_FILE_CABINET_PANEL, *this);
		AddToolsWindow(panel, _("File Cabinet"), wxT("File cabinet"), t4p::BitmapImageAsset(wxT("file-cabinet")));
	}

	wxString filename = ctrl->GetFileName();
	t4p::FileCabinetItemClass fileItem = AddFileToCabinet(filename);
	panel->AddItemToList(fileItem);
}

t4p::FileCabinetPanelClass::FileCabinetPanelClass(wxWindow* parent, int id, t4p::FileCabinetFeatureClass& feature)
: FileCabinetPanelGeneratedClass(parent, id)
, SqliteFinder()
, FileCabinet()
, Feature(feature) {
	AddFileButton->SetBitmap(t4p::BitmapImageAsset(wxT("document-plus")));
	AddDirectoryButton->SetBitmap(t4p::BitmapImageAsset(wxT("directory-plus")));
	ImageList = new wxImageList(16, 16);
	
	t4p::FileTypeImageList(*ImageList);
	ImageList->Add(t4p::BitmapImageAsset(wxT("folder-horizontal")));
	
	List->AssignImageList(ImageList, wxIMAGE_LIST_NORMAL);
	SqliteFinder.InitSession(&Feature.App.Globals.ResourceCacheSession);
	FillList();
	
	wxAcceleratorEntry entries[1];
	entries[0].Set(wxACCEL_NORMAL, WXK_DELETE, ID_FILE_CABINET_DELETE);
	
	wxAcceleratorTable accel(1, entries);
	List->SetAcceleratorTable(accel);
}

void t4p::FileCabinetPanelClass::FillList() {
	List->DeleteAllItems();
	
	bool found = SqliteFinder.Exec(&FileCabinet);
	if (found) {
		do {
			FileCabinet.Next();
			AddItemToList(FileCabinet.Item);
		} while (FileCabinet.More());
	}
}

void t4p::FileCabinetPanelClass::AddItemToList(const t4p::FileCabinetItemClass& fileItem) {
	if (fileItem.FileName.GetFullName().empty() && 
		!fileItem.FileName.GetDirs().empty()) {
		wxListItem listItem;
		listItem.SetText(fileItem.FileName.GetDirs().back());
		listItem.SetImage(LIST_FOLDER);
		listItem.SetId(List->GetItemCount());
		listItem.SetData(fileItem.Id);
		List->InsertItem(listItem);
	}
	else {
		wxListItem listItem;
		listItem.SetText(fileItem.FileName.GetFullName());
		listItem.SetImage(t4p::FileTypeImageId(Feature.App.Globals.FileTypes, fileItem.FileName));
		listItem.SetId(List->GetItemCount());
		listItem.SetData(fileItem.Id);
		List->InsertItem(listItem);
	}
}


void t4p::FileCabinetPanelClass::OnAddDirectoryClick(wxCommandEvent& event) {
	wxDirDialog dialog(this, _("Choose a directory to add to the File Cabinet"), wxEmptyString,
		wxDD_DIR_MUST_EXIST);
	if (dialog.ShowModal() != wxID_CANCEL) {
		t4p::FileCabinetItemClass fileItem = Feature.AddDirectoryToCabinet(dialog.GetPath());
		AddItemToList(fileItem);
	}
}

void t4p::FileCabinetPanelClass::OnAddFileClick(wxCommandEvent& event) {
	wxFileDialog dialog(this, _("Choose a file to add to the File Cabinet"), wxEmptyString,
		wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (dialog.ShowModal() != wxID_CANCEL) {
		t4p::FileCabinetItemClass fileItem = Feature.AddFileToCabinet(dialog.GetPath());		
		AddItemToList(fileItem);
	}
}

void t4p::FileCabinetPanelClass::OnListItemActivated(wxListEvent& event) {
	
	// need to get the file/dir to open. to get the filename, we get
	// the file_cabinet_item_id from the list, then query the
	// db
	wxListItem listItem = event.GetItem();
	int fileCabinetItemId = (int) listItem.GetData();
	
	t4p::SingleFileCabinetResultClass result;
	result.Init(fileCabinetItemId);
	bool found = SqliteFinder.Exec(&result);
	if (found) {
		
		// we create the proper event to tell the app to
		// open the file/dir
		result.Next();
		if (result.Item.IsFile()) {
			t4p::OpenFileCommandEventClass openFileCmd(result.Item.FileName.GetFullPath());
			Feature.App.EventSink.Publish(openFileCmd);
		}
		else if (result.Item.IsDir()) {
			wxCommandEvent openDirEvt(t4p::EVENT_CMD_DIR_OPEN);
			openDirEvt.SetString(result.Item.FileName.GetPath());
			Feature.App.EventSink.Publish(openDirEvt);
		}
	}
}

void t4p::FileCabinetPanelClass::OnListItemDelete(wxCommandEvent& event) {
	long index = -1;
	index = List->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (index >= 0) {
		
		// delete from the database AND the list control
		long dbId = List->GetItemData(index);	
		Feature.DeleteCabinetItem(dbId);
		List->DeleteItem(index);
	}
}

BEGIN_EVENT_TABLE(t4p::FileCabinetFeatureClass, t4p::FeatureClass)
	EVT_MENU(t4p::MENU_FILE_CABINET + 0, t4p::FileCabinetFeatureClass::OnViewFileCabinet)
	EVT_MENU(t4p::MENU_FILE_CABINET + 1, t4p::FileCabinetFeatureClass::OnEditAddCurrentFileToCabinet)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::FileCabinetPanelClass, FileCabinetPanelGeneratedClass)
	EVT_MENU(ID_FILE_CABINET_DELETE, t4p::FileCabinetPanelClass::OnListItemDelete)
END_EVENT_TABLE()
