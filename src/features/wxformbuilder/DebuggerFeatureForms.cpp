///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "DebuggerFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

DebuggerPanelGeneratedClass::DebuggerPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BodySizer;
	BodySizer = new wxBoxSizer( wxVERTICAL );
	
	Notebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	BodySizer->Add( Notebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
}

DebuggerPanelGeneratedClass::~DebuggerPanelGeneratedClass()
{
}

DebuggerStackPanelGeneratedClass::DebuggerStackPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* BodySizer;
	BodySizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	BodySizer->AddGrowableCol( 0 );
	BodySizer->AddGrowableRow( 1 );
	BodySizer->SetFlexibleDirection( wxBOTH );
	BodySizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	Label = new wxStaticText( this, wxID_ANY, wxT("Stack"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	TopSizer->Add( Label, 1, wxALL, 5 );
	
	StatusLabel = new wxStaticText( this, wxID_ANY, wxT("Status: Debugging session not active"), wxDefaultPosition, wxDefaultSize, 0 );
	StatusLabel->Wrap( -1 );
	TopSizer->Add( StatusLabel, 0, wxALL, 5 );
	
	BodySizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	StackList = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT );
	BodySizer->Add( StackList, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
}

DebuggerStackPanelGeneratedClass::~DebuggerStackPanelGeneratedClass()
{
}

DebuggerLogPanelGeneratedClass::DebuggerLogPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 1 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	ClearButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	TopSizer->Add( ClearButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	LogLabel = new wxStaticText( this, wxID_ANY, wxT("XDebug Log"), wxDefaultPosition, wxDefaultSize, 0 );
	LogLabel->Wrap( -1 );
	TopSizer->Add( LogLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	GridSizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	Text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	GridSizer->Add( Text, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( GridSizer );
	this->Layout();
	
	// Connect Events
	ClearButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerLogPanelGeneratedClass::OnClearButton ), NULL, this );
}

DebuggerLogPanelGeneratedClass::~DebuggerLogPanelGeneratedClass()
{
	// Disconnect Events
	ClearButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerLogPanelGeneratedClass::OnClearButton ), NULL, this );
	
}

DebuggerVariablePanelGeneratedClass::DebuggerVariablePanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 1 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	VariablesLabel = new wxStaticText( this, wxID_ANY, wxT("Variables"), wxDefaultPosition, wxDefaultSize, 0 );
	VariablesLabel->Wrap( -1 );
	TopSizer->Add( VariablesLabel, 1, wxALL|wxEXPAND, 5 );
	
	StatusLabel = new wxStaticText( this, wxID_ANY, wxT("Status: Debugger session active"), wxDefaultPosition, wxDefaultSize, 0 );
	StatusLabel->Wrap( -1 );
	TopSizer->Add( StatusLabel, 0, wxALL, 5 );
	
	GridSizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	VariablesList = new wxDataViewCtrl( this, wxID_ANY );
	GridSizer->Add( VariablesList, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( GridSizer );
	this->Layout();
}

DebuggerVariablePanelGeneratedClass::~DebuggerVariablePanelGeneratedClass()
{
}
