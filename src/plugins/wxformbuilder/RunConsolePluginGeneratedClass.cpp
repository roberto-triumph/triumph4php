///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "RunConsolePluginGeneratedClass.h"

///////////////////////////////////////////////////////////////////////////

RunConsolePanelGeneratedClass::RunConsolePanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* MainSizer;
	MainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 1 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	Command = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	TopSizer->Add( Command, 1, wxALL, 5 );
	
	RunButton = new wxButton( this, wxID_ANY, _("Run"), wxDefaultPosition, wxDefaultSize, 0 );
	TopSizer->Add( RunButton, 0, wxALL, 5 );
	
	ClearButton = new wxButton( this, wxID_ANY, _("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	TopSizer->Add( ClearButton, 0, wxALL, 5 );
	
	FlexGridSizer->Add( TopSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* BottomSizer;
	BottomSizer = new wxBoxSizer( wxVERTICAL );
	
	OutputWindow = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	BottomSizer->Add( OutputWindow, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	FlexGridSizer->Add( BottomSizer, 0, wxEXPAND, 5 );
	
	MainSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( MainSizer );
	this->Layout();
	
	// Connect Events
	Command->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( RunConsolePanelGeneratedClass::RunCommand ), NULL, this );
	RunButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunConsolePanelGeneratedClass::RunCommand ), NULL, this );
	ClearButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunConsolePanelGeneratedClass::OnClear ), NULL, this );
}

RunConsolePanelGeneratedClass::~RunConsolePanelGeneratedClass()
{
	// Disconnect Events
	Command->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( RunConsolePanelGeneratedClass::RunCommand ), NULL, this );
	RunButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunConsolePanelGeneratedClass::RunCommand ), NULL, this );
	ClearButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunConsolePanelGeneratedClass::OnClear ), NULL, this );
	
}

ChooseUrlDialogGeneratedClass::ChooseUrlDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BodySizer;
	BodySizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 1 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	OpenLabel = new wxStaticText( this, wxID_ANY, _("Choose URL to open in the browser"), wxDefaultPosition, wxDefaultSize, 0 );
	OpenLabel->Wrap( -1 );
	GridSizer->Add( OpenLabel, 0, wxALL, 5 );
	
	UrlList = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE ); 
	GridSizer->Add( UrlList, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* ExtraSizer;
	ExtraSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("URL extra") ), wxVERTICAL );
	
	Extra = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	ExtraSizer->Add( Extra, 1, wxALL|wxEXPAND, 5 );
	
	GridSizer->Add( ExtraSizer, 1, wxEXPAND, 5 );
	
	CompleteLabel = new wxStaticText( this, wxID_ANY, _("Complete URL:"), wxDefaultPosition, wxDefaultSize, 0 );
	CompleteLabel->Wrap( -1 );
	GridSizer->Add( CompleteLabel, 1, wxALL|wxEXPAND|wxALIGN_BOTTOM, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonsSizer->AddButton( ButtonsSizerCancel );
	ButtonsSizer->Realize();
	GridSizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	BodySizer->Add( GridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	UrlList->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ChooseUrlDialogGeneratedClass::OnUpdateUi ), NULL, this );
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnOkButton ), NULL, this );
}

ChooseUrlDialogGeneratedClass::~ChooseUrlDialogGeneratedClass()
{
	// Disconnect Events
	UrlList->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ChooseUrlDialogGeneratedClass::OnUpdateUi ), NULL, this );
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnOkButton ), NULL, this );
	
}
