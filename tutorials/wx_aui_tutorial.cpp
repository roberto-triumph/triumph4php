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
#include <wx/wx.h>
#include <wx/aui/aui.h>


/**
 * This is a simple test of the wx AUI system. It will create 2 panels
 */
class MyApp : public wxApp {
public:
	virtual bool OnInit();
};

class MyFrame: public wxFrame {
public:
	MyFrame();
	
private:
	wxAuiManager AuiManager;
	
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
	MyFrame* frame = new MyFrame();
	SetTopWindow(frame);
	frame->Show(true);	
	return true;
}

MyFrame::MyFrame() :
	wxFrame(NULL, wxID_ANY, wxT("test"), wxDefaultPosition, 
			wxSize(640, 480))
	, AuiManager() {

	wxAuiNotebook* book = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
		wxAUI_NB_TOP | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB);
		
	wxPanel* panel1 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(640, 380));
	wxPanel* panel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(640, 100));
		
	new wxTextCtrl(panel1, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(640, 380), wxTE_MULTILINE);
	new wxTextCtrl(panel2, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(640, 24), 0);
	book->AddPage(panel1, wxT("Code"));
	
	AuiManager.AddPane(book, wxAuiPaneInfo().CentrePane(
		).PaneBorder(false).Gripper(false).Floatable(false));
		
	AuiManager.AddPane(panel2, wxAuiPaneInfo().Bottom().Caption(
		_("Tools")).Floatable(false));
		
	AuiManager.SetManagedWindow(this);
	AuiManager.Update();
}
