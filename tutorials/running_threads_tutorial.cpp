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
#include <wx/menu.h>
#include <wx/sizer.h>
#include <actions/ActionClass.h>

/**
 * This is an app that exercises the RunningThreads class and shows the 
 * proper usage.
 */
class MyApp : public wxApp {
public:
	virtual bool OnInit();

	mvceditor::RunningThreadsClass RunningThreads;
};

static int ActionCount = 0;

class MyFrame: public wxFrame {
public:
	MyFrame(mvceditor::RunningThreadsClass& runningThreads);
private:
	void AddMenu();
	void OnExit(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnStartNewThread(wxCommandEvent& event);
	void OnStopThread(wxCommandEvent& event);
	void OnStopAllThread(wxCommandEvent& event);
	void OnThreadRunning(wxCommandEvent& event);
	void OnThreadComplete(wxCommandEvent& event);

	mvceditor::RunningThreadsClass& RunningThreads;
	wxTextCtrl* Text;
	wxThreadIdType RunningActionId;

	DECLARE_EVENT_TABLE()
};

class MyAction : public mvceditor::ActionClass {

public:
	MyAction(mvceditor::RunningThreadsClass& runningThreads, int eventId, wxString label);
	wxString GetLabel() const;
protected:
	void BackgroundWork();
	wxString Label;
};

const wxEventType EVENT_RUNNING = wxNewEventType();
const int ID_THREAD = wxNewId();
enum {
	MENU_START_THREAD = 1,
	MENU_STOP_THREAD,
	MENU_STOP_ALL_THREAD,
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
	MyFrame* frame = new MyFrame(RunningThreads);
	SetTopWindow(frame);
	frame->Show(true);	
	return true;
}

MyFrame::MyFrame(mvceditor::RunningThreadsClass& runningThreads) :
	wxFrame(NULL, wxID_ANY, wxT("running threads tutorial"), wxDefaultPosition, 
			wxSize(640, 480)) 
	, RunningThreads(runningThreads) 
	, RunningActionId(0) {
	RunningThreads.AddEventHandler(this);

	SetSizeHints( wxDefaultSize, wxDefaultSize );
	Text = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(Text, 1, wxALL | wxEXPAND, 5);
	SetSizer(sizer);
	Layout();
	Centre(wxBOTH);
	AddMenu();
}

void MyFrame::AddMenu() {
	wxMenuBar* menuBar = new wxMenuBar;
	wxMenu* menu = new wxMenu;
	menu->Append(MENU_START_THREAD, _("Start an action"), _("Start an action"), wxITEM_NORMAL);
	menu->Append(MENU_STOP_THREAD, _("Stop the current action"), _("Stop the current action"), wxITEM_NORMAL);
	menu->Append(MENU_STOP_ALL_THREAD, _("Stop ALL actions"), _("Stop ALL actions"), wxITEM_NORMAL);
	menu->Append(wxID_EXIT, _("Exit"), _("Exit the app"), wxITEM_NORMAL);
	menuBar->Append(menu, _("File"));
	SetMenuBar(menuBar);
}

void MyFrame::OnStartNewThread(wxCommandEvent& event) {
	MyAction* action = new MyAction(RunningThreads, ID_THREAD, wxString::Format(wxT("Action-%d"), ActionCount));
	ActionCount++;
	RunningActionId = RunningThreads.Queue(action);
	Text->AppendText(wxString::Format(_("Action started...\n")));
}

void MyFrame::OnStopThread(wxCommandEvent& event) {
	RunningThreads.CancelAction(RunningActionId);
	RunningActionId = 0;
}

void MyFrame::OnStopAllThread(wxCommandEvent& event) {
	RunningThreads.StopAll();
	Text->AppendText(_("All threads stopped...\n"));
	RunningActionId = 0;
}

void MyFrame::OnExit(wxCommandEvent &event) {
	RunningThreads.StopAll();
	RunningThreads.RemoveEventHandler(this);
	Destroy();
}

void MyFrame::OnClose(wxCloseEvent& event) {
	RunningThreads.StopAll();
	RunningThreads.RemoveEventHandler(this);
	event.Skip();
}

MyAction::MyAction(mvceditor::RunningThreadsClass& runningThreads, int eventId, wxString label)
	: mvceditor::ActionClass(runningThreads, eventId) 
	, Label(label) {

}

void MyAction::BackgroundWork() {
	while (!IsCancelled()) {
		wxCommandEvent evt(EVENT_RUNNING);
		evt.SetId(ID_THREAD);
		evt.SetString(wxString::Format(_("%s is running...\n"), Label));
		PostEvent(evt);
		wxThread::Sleep(2000);
	}
}
wxString MyAction::GetLabel() const {
	return Label;
}

void MyFrame::OnThreadRunning(wxCommandEvent& event) {
	Text->AppendText(event.GetString());
}

void MyFrame::OnThreadComplete(wxCommandEvent& event) {
	Text->AppendText(event.GetString());
	Text->AppendText(wxT("Work complete\n"));
	RunningActionId = 0;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(MENU_START_THREAD, MyFrame::OnStartNewThread)
	EVT_MENU(MENU_STOP_THREAD, MyFrame::OnStopThread)
	EVT_MENU(MENU_STOP_ALL_THREAD, MyFrame::OnStopAllThread)
	EVT_MENU(wxID_EXIT, MyFrame::OnExit)
	EVT_CLOSE(MyFrame::OnClose)
	EVT_COMMAND(ID_THREAD, EVENT_RUNNING, MyFrame::OnThreadRunning)
	EVT_COMMAND(ID_THREAD, mvceditor::EVENT_WORK_COMPLETE, MyFrame::OnThreadComplete)
END_EVENT_TABLE()