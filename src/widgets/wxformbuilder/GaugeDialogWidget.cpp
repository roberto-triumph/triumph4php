///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "GaugeDialogWidget.h"

///////////////////////////////////////////////////////////////////////////

GaugeDialogGeneratedClass::GaugeDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* BodySizer;
	BodySizer = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* FlexSizer;
	FlexSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	FlexSizer->AddGrowableCol( 0 );
	FlexSizer->AddGrowableRow( 0 );
	FlexSizer->SetFlexibleDirection( wxBOTH );
	FlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	Label = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	Label->Wrap( 430 );
	FlexSizer->Add( Label, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	Gauge = new wxGauge( this, ID_GAUGE, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	FlexSizer->Add( Gauge, 1, wxALL|wxEXPAND, 5 );

	HideButton = new wxButton( this, ID_HIDEBUTTON, _("Hide"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexSizer->Add( HideButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	BodySizer->Add( FlexSizer, 1, wxEXPAND, 5 );

	this->SetSizer( BodySizer );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	HideButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GaugeDialogGeneratedClass::OnHideButton ), NULL, this );
}

GaugeDialogGeneratedClass::~GaugeDialogGeneratedClass()
{
	// Disconnect Events
	HideButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GaugeDialogGeneratedClass::OnHideButton ), NULL, this );

}
