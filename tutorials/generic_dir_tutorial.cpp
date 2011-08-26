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
 * @author     $Author: robertop2004@gmail.com $
 * @date       $Date: 2011-06-10 17:19:31 -0700 (Fri, 10 Jun 2011) $
 * @version    $Rev: 555 $ 
 */
#include <wx/wx.h>
#include <wx/dirctrl.h>

/**
 * This is the bare-bones test of the wxWidgets directory control (wxGenericDirCtrl).
 */

// this is the directory that willl be displayed by the dir control.
const wxString DIRECTORY_TO_SHOW = wxGetHomeDir();

class MyApp : public wxApp {
public:
	virtual bool OnInit();
};

class MyFrame: public wxFrame {
public:
	MyFrame();
	
};

class MyDirCtrl : public wxGenericDirCtrl {
	
public:
	MyDirCtrl(wxWindow* parent);
	
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
			wxSize(640, 480)) {
	new MyDirCtrl(this);
}

MyDirCtrl::MyDirCtrl(wxWindow* parent)
	: wxGenericDirCtrl(parent, wxID_ANY, DIRECTORY_TO_SHOW)
{
	
}
