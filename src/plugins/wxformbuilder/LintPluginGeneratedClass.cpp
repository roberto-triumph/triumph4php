///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "LintPluginGeneratedClass.h"

///////////////////////////////////////////////////////////////////////////

LintResultsGeneratedPanelClass::LintResultsGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexSizer;
	FlexSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexSizer->AddGrowableCol( 0 );
	FlexSizer->AddGrowableRow( 1 );
	FlexSizer->SetFlexibleDirection( wxBOTH );
	FlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	Label = new wxStaticText( this, wxID_ANY, wxT("PHP Lint Results"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	FlexSizer->Add( Label, 1, wxALL|wxEXPAND, 5 );
	
	ErrorsList = new wxListBox( this, ID_ERRORS_LIST, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL|wxLB_NEEDED_SB ); 
	FlexSizer->Add( ErrorsList, 1, wxALL|wxEXPAND, 5 );
	
	BoxSizer->Add( FlexSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	ErrorsList->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( LintResultsGeneratedPanelClass::OnListDoubleClick ), NULL, this );
}

LintResultsGeneratedPanelClass::~LintResultsGeneratedPanelClass()
{
	// Disconnect Events
	ErrorsList->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( LintResultsGeneratedPanelClass::OnListDoubleClick ), NULL, this );
	
}
