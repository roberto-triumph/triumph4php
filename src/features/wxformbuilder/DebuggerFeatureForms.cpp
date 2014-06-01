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

DebuggerLogPanelGeneratedClass::DebuggerLogPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 1 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	LogLabel = new wxStaticText( this, wxID_ANY, wxT("Log"), wxDefaultPosition, wxDefaultSize, 0 );
	LogLabel->Wrap( -1 );
	GridSizer->Add( LogLabel, 0, wxALL, 5 );
	
	Text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	GridSizer->Add( Text, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( GridSizer );
	this->Layout();
}

DebuggerLogPanelGeneratedClass::~DebuggerLogPanelGeneratedClass()
{
}
