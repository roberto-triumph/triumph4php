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
#include <features/views/TestViewClass.h>
#include <search/FindInFilesClass.h>
#include <globals/String.h>
#include <wx/artprov.h>

int ID_GO_BUTTON = wxNewId();

void t4p::TestViewClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {
}

void t4p::TestViewClass::AddEditMenuItems(wxMenu* editMenu) {
}

void t4p::TestViewClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	toolBar->AddTool(ID_GO_BUTTON, wxT("GO"), wxArtProvider::GetBitmap(
	                     wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(16, 16)), wxT("GO"), wxITEM_NORMAL);
}

void t4p::TestViewClass::AddWindows() {
}

void t4p::TestViewClass::Go(wxCommandEvent& event) {
	wxMessageBox(wxT("Clicked the GO! button"));
	Feature.Go();
}

t4p::TestViewClass::TestViewClass(t4p::TestFeatureClass& feature)
	: FeatureViewClass() 
	, Feature(feature) {
		
}

BEGIN_EVENT_TABLE(t4p::TestViewClass, t4p::FeatureViewClass)
	EVT_MENU(ID_GO_BUTTON, TestViewClass::Go)
END_EVENT_TABLE()
