///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "EditorMessagesPluginGeneratedClass.h"

///////////////////////////////////////////////////////////////////////////

EditorMessagesGeneratedPanelClass::EditorMessagesGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	GridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 2 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	Label = new wxStaticText( this, wxID_ANY, _("Editor Messages"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	GridSizer->Add( Label, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxHORIZONTAL );
	
	ClearButton = new wxButton( this, wxID_ANY, _("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	BoxSizer->Add( ClearButton, 0, wxALL, 5 );
	
	GridSizer->Add( BoxSizer, 1, wxEXPAND, 5 );
	
	Grid = new wxGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	Grid->CreateGrid( 5, 5 );
	Grid->EnableEditing( true );
	Grid->EnableGridLines( true );
	Grid->EnableDragGridSize( false );
	Grid->SetMargins( 0, 0 );
	
	// Columns
	Grid->AutoSizeColumns();
	Grid->EnableDragColMove( true );
	Grid->EnableDragColSize( true );
	Grid->SetColLabelSize( 30 );
	Grid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	Grid->AutoSizeRows();
	Grid->EnableDragRowSize( true );
	Grid->SetRowLabelSize( 80 );
	Grid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	Grid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	GridSizer->Add( Grid, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( GridSizer );
	this->Layout();
	
	// Connect Events
	ClearButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorMessagesGeneratedPanelClass::OnClearButton ), NULL, this );
}

EditorMessagesGeneratedPanelClass::~EditorMessagesGeneratedPanelClass()
{
	// Disconnect Events
	ClearButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorMessagesGeneratedPanelClass::OnClearButton ), NULL, this );
	
}
