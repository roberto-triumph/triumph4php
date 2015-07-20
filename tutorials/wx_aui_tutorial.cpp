/**
 * @copyright  2009-2011 Roberto Perpuly
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
#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/stc/stc.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/valgen.h>

/*
 * This sample will show a bit about how the AUI layout
 * mechanism works.  The tutorial creates many panes;
 * the user can adjust each property (row, position,
 * and layer) to see how the changes affects the placement
 * of the panes
 */

static const int ID_MENU_ADJUST_1 = wxNewId();
static const int ID_MENU_ADJUST_2 = wxNewId();
static const int ID_MENU_ADJUST_3 = wxNewId();
static const int ID_MENU_ADJUST_4 = wxNewId();
static const int ID_MENU_ADJUST_5 = wxNewId();
static const int ID_MENU_ADJUST_6 = wxNewId();
static const int ID_MENU_ADJUST_7 = wxNewId();
static const int ID_MENU_MOVE_7 = wxNewId();

class ChangeDialogGeneratedClass : public wxDialog {
	private:
	protected:
		wxStaticText* LayerLabel;
		wxSpinCtrl* Layer;
		wxStaticText* RowLabel;
		wxSpinCtrl* Row;
		wxStaticText* PositionLabel;
		wxSpinCtrl* Position;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;

	public:
	ChangeDialogGeneratedClass(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Change"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
	~ChangeDialogGeneratedClass();
};

ChangeDialogGeneratedClass::ChangeDialogGeneratedClass(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style) {
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxFlexGridSizer* BodySizer;
	BodySizer = new wxFlexGridSizer(2, 1, 0, 0);
	BodySizer->SetFlexibleDirection(wxBOTH);
	BodySizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxFlexGridSizer* FormSizer;
	FormSizer = new wxFlexGridSizer(3, 2, 0, 0);
	FormSizer->AddGrowableCol(1);
	FormSizer->AddGrowableRow(0);
	FormSizer->AddGrowableRow(1);
	FormSizer->AddGrowableRow(2);
	FormSizer->SetFlexibleDirection(wxHORIZONTAL);
	FormSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	LayerLabel = new wxStaticText(this, wxID_ANY, wxT("Layer"), wxDefaultPosition, wxDefaultSize, 0);
	LayerLabel->Wrap(-1);
	FormSizer->Add(LayerLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5);

	Layer = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0);
	FormSizer->Add(Layer, 1, wxALL|wxEXPAND, 5);

	RowLabel = new wxStaticText(this, wxID_ANY, wxT("Row"), wxDefaultPosition, wxDefaultSize, 0);
	RowLabel->Wrap(-1);
	FormSizer->Add(RowLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5);

	Row = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 2);
	FormSizer->Add(Row, 0, wxALL|wxEXPAND, 5);

	PositionLabel = new wxStaticText(this, wxID_ANY, wxT("Position"), wxDefaultPosition, wxDefaultSize, 0);
	PositionLabel->Wrap(-1);
	FormSizer->Add(PositionLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5);

	Position = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0);
	FormSizer->Add(Position, 0, wxALL|wxEXPAND, 5);

	BodySizer->Add(FormSizer, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton(this, wxID_OK);
	m_sdbSizer1->AddButton(m_sdbSizer1OK);
	m_sdbSizer1Cancel = new wxButton(this, wxID_CANCEL);
	m_sdbSizer1->AddButton(m_sdbSizer1Cancel);
	m_sdbSizer1->Realize();
	bSizer1->Add(m_sdbSizer1, 1, wxEXPAND, 5);

	BodySizer->Add(bSizer1, 1, wxEXPAND, 5);

	this->SetSizer(BodySizer);
	this->Layout();

	this->Centre(wxBOTH);
}

ChangeDialogGeneratedClass::~ChangeDialogGeneratedClass() {
}


class ChangeDialogClass : public ChangeDialogGeneratedClass {
	public:
	ChangeDialogClass(wxWindow* parent, wxString name, int& dock, int& row, int& position, int& layer)
	: ChangeDialogGeneratedClass(parent, wxID_ANY,  "Change " + name) {
		wxGenericValidator valRow(&row);
		Row->SetValidator(valRow);

		wxGenericValidator valPosition(&position);
		Position->SetValidator(valPosition);

		wxGenericValidator valLayer(&layer);
		Layer->SetValidator(valLayer);

		TransferDataToWindow();
	}
};


/**
 * This is a simple test of the wx AUI system. It will create 2 panels
 */
class MyApp : public wxApp {
	public:
	bool OnInit();
};

class MyFrame: public wxFrame {
	public:
	MyFrame();
	~MyFrame();

	private:
	void OnAdjust(wxCommandEvent& event);
	void OnMovePanel7(wxCommandEvent& event);
	wxAuiManager AuiManager;

	DECLARE_EVENT_TABLE()
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

	wxPanel* panel1 = new wxPanel(this, wxID_ANY, wxDefaultPosition);
	wxPanel* panel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(640, 100));
	wxPanel* panel3 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(640, 100));
	wxPanel* panel4 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(640, 100));
	wxPanel* panel5 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(640, 100));
	wxPanel* panel6 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(640, 100));
	wxPanel* panel7 = new wxPanel(this, wxID_ANY, wxDefaultPosition);
	wxStyledTextCtrl* txt = new wxStyledTextCtrl(panel7);
	wxBoxSizer* panel7Sizer =  new wxBoxSizer(wxVERTICAL);
	panel7Sizer->Add(txt, wxSizerFlags().Expand().Proportion(1));
	panel7->SetSizerAndFit(panel7Sizer);
	txt->SetText("# this is some code for panel 7\n\n");
	panel7->Layout();

	txt = new wxStyledTextCtrl(panel1);
	wxBoxSizer* contentSizer =  new wxBoxSizer(wxVERTICAL);
	contentSizer->Add(txt, wxSizerFlags().Expand().Proportion(1));
	panel1->SetSizerAndFit(contentSizer);
	panel1->Layout();
	txt->SetText("# this is some code for panel 1\n\n");

	new wxTextCtrl(panel2, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(640, 24), 0);
	book->AddPage(panel1, wxT("Code"));

	AuiManager.AddPane(book, wxAuiPaneInfo().CentrePane()
		.Caption("book").PaneBorder(false).Gripper(false).Floatable(false).Name("panel1"));

	AuiManager.AddPane(panel2, wxAuiPaneInfo().Bottom().Caption(
		_("panel2")).Floatable(false).Name("panel2"));
	AuiManager.AddPane(panel3, wxAuiPaneInfo().Left().Caption(
		_("panel3")).Floatable(false).Name("panel3"));
	AuiManager.AddPane(panel4, wxAuiPaneInfo().Right().Caption(
		_("panel4")).Floatable(false).Name("panel4"));
	AuiManager.AddPane(panel5, wxAuiPaneInfo().Bottom().Caption(
		_("panel5")).Floatable(false).Name("panel5"));
	AuiManager.AddPane(panel6, wxAuiPaneInfo().Left().Caption(
		_("panel6")).Floatable(false).Name("panel6"));
	AuiManager.AddPane(panel7, wxAuiPaneInfo().Right().Caption(
		_("panel7")).Floatable(false).Name("panel7"));
	AuiManager.SetManagedWindow(this);
	AuiManager.Update();

	wxMenuBar* bar = new wxMenuBar();
	wxMenu* menu = new wxMenu();
	menu->Append(ID_MENU_ADJUST_1, "adjust panel 1", "adjust panel 1", wxITEM_NORMAL);
	menu->Append(ID_MENU_ADJUST_2, "adjust panel 2", "adjust panel 2", wxITEM_NORMAL);
	menu->Append(ID_MENU_ADJUST_3, "adjust panel 3", "adjust panel 3", wxITEM_NORMAL);
	menu->Append(ID_MENU_ADJUST_4, "adjust panel 4", "adjust panel 4", wxITEM_NORMAL);
	menu->Append(ID_MENU_ADJUST_5, "adjust panel 5", "adjust panel 5", wxITEM_NORMAL);
	menu->Append(ID_MENU_ADJUST_6, "adjust panel 6", "adjust panel 6", wxITEM_NORMAL);
	menu->Append(ID_MENU_ADJUST_7, "adjust panel 7", "adjust panel 7", wxITEM_NORMAL);
	menu->Append(ID_MENU_MOVE_7, "move panel 7", "move panel 7 to the bottom", wxITEM_NORMAL);
	bar->Append(menu, "File");
	SetMenuBar(bar);
}

MyFrame::~MyFrame() {
	AuiManager.UnInit();
}

void MyFrame::OnAdjust(wxCommandEvent& event) {
	wxString name;
	if (event.GetId() == ID_MENU_ADJUST_1) {
		name = "panel1";
	}
	if (event.GetId() == ID_MENU_ADJUST_2) {
		name = "panel2";
	}
	if (event.GetId() == ID_MENU_ADJUST_3) {
		name = "panel3";
	}
	if (event.GetId() == ID_MENU_ADJUST_4) {
		name = "panel4";
	}
	if (event.GetId() == ID_MENU_ADJUST_5) {
		name = "panel5";
	}
	if (event.GetId() == ID_MENU_ADJUST_6) {
		name = "panel6";
	}
	if (event.GetId() == ID_MENU_ADJUST_7) {
		name = "panel7";
	}
	wxAuiPaneInfo& info = AuiManager.GetPane(name);
	if (!info.IsOk()) {
		wxMessageBox("Could not find window by menu ID");
		return;
	}
	ChangeDialogClass dialog(this, name, info.dock_direction, info.dock_row, info.dock_pos, info.dock_layer);
	if (dialog.ShowModal() == wxID_OK) {
		wxAuiPaneInfoArray& infos = AuiManager.GetAllPanes();
		for (size_t i = 0; i < infos.size(); i++) {
			wxString caption = wxString::Format("direction%d row=%d position=%d layer=%d",
				infos[i].dock_direction, infos[i].dock_row, infos[i].dock_pos, infos[i].dock_layer);
			infos[i].Caption(infos[i].name + " " + caption);
		}
		AuiManager.Update();
	}
}

void MyFrame::OnMovePanel7(wxCommandEvent& event) {
	wxAuiPaneInfo panelInfo = AuiManager.GetPane("panel7");
	if (!panelInfo.IsOk()) {
		wxMessageBox("Panel does not exist.");
		return;
	}
	wxWindow* panel7 = panelInfo.window;
	panelInfo.Bottom().Row(0);
	AuiManager.InsertPane(panel7, panelInfo, wxAUI_INSERT_ROW);
	AuiManager.Update();
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(ID_MENU_ADJUST_1, MyFrame::OnAdjust)
	EVT_MENU(ID_MENU_ADJUST_2, MyFrame::OnAdjust)
	EVT_MENU(ID_MENU_ADJUST_3, MyFrame::OnAdjust)
	EVT_MENU(ID_MENU_ADJUST_4, MyFrame::OnAdjust)
	EVT_MENU(ID_MENU_ADJUST_5, MyFrame::OnAdjust)
	EVT_MENU(ID_MENU_ADJUST_6, MyFrame::OnAdjust)
	EVT_MENU(ID_MENU_ADJUST_7, MyFrame::OnAdjust)
	EVT_MENU(ID_MENU_MOVE_7, MyFrame::OnMovePanel7)
END_EVENT_TABLE()
