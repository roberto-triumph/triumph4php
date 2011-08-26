///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
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
