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
#include <features/TestFeatureClass.h>
#include <search/FindInFilesClass.h>
#include <globals/String.h>
#include <wx/artprov.h>

int ID_GO_BUTTON = wxNewId();

void mvceditor::TestFeatureClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {
}

void mvceditor::TestFeatureClass::AddEditMenuItems(wxMenu* editMenu) {
}

void mvceditor::TestFeatureClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	toolBar->AddTool(ID_GO_BUTTON, wxT("GO"), wxArtProvider::GetBitmap(
	                     wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(16, 16)), wxT("GO"), wxITEM_NORMAL);
}

void mvceditor::TestFeatureClass::AddWindows() {
}

void mvceditor::TestFeatureClass::Go(wxCommandEvent& event) {
	//wxMessageBox(wxT("Clicked the GO! button"));
	wxString file = wxT("/home/roberto/workspace/sample_php_project/high_ascii.php");
	
	mvceditor::FindInFilesClass findInFiles;
	
	UnicodeString str;
	UFILE* f = u_fopen(file.ToAscii(), "r", NULL, NULL);
	if (f) {
		int read = u_file_read(str.getBuffer(1024), 1024, f);
		str.releaseBuffer(read);
		u_fclose(f);
	}
	GetNotebook()->LoadPage(file);
	mvceditor::CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl) {
		codeControl->SetText(mvceditor::IcuToWx(str));
	}
}

mvceditor::TestFeatureClass::TestFeatureClass(mvceditor::AppClass& app)
	: FeatureClass(app) {
}

BEGIN_EVENT_TABLE(mvceditor::TestFeatureClass, wxEvtHandler)
	EVT_MENU(ID_GO_BUTTON, TestFeatureClass::Go)
END_EVENT_TABLE()
