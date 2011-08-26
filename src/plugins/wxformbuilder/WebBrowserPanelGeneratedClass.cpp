///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "WebBrowserPanelGeneratedClass.h"

///////////////////////////////////////////////////////////////////////////

WebBrowserPanelGeneratedClass::WebBrowserPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* MainSizer;
	MainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexSizer;
	FlexSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexSizer->AddGrowableCol( 0 );
	FlexSizer->AddGrowableRow( 1 );
	FlexSizer->SetFlexibleDirection( wxBOTH );
	FlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxHORIZONTAL );
	
	Url = new wxTextCtrl( this, ID_URL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	Url->SetMaxLength( 4000 ); 
	BoxSizer->Add( Url, 1, wxALL|wxEXPAND, 5 );
	
	GoButton = new wxButton( this, ID_GO, _("Go"), wxDefaultPosition, wxDefaultSize, 0 );
	BoxSizer->Add( GoButton, 0, wxALL|wxEXPAND, 5 );
	
	FlexSizer->Add( BoxSizer, 1, wxEXPAND, 5 );
	
	WebControl = new wxWebControl(this, ID_WEB_BROWSER);
	FlexSizer->Add( WebControl, 1, wxALL|wxEXPAND, 5 );
	
	MainSizer->Add( FlexSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( MainSizer );
	this->Layout();
	
	// Connect Events
	Url->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( WebBrowserPanelGeneratedClass::OnGoButton ), NULL, this );
	GoButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WebBrowserPanelGeneratedClass::OnGoButton ), NULL, this );
}

WebBrowserPanelGeneratedClass::~WebBrowserPanelGeneratedClass()
{
	// Disconnect Events
	Url->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( WebBrowserPanelGeneratedClass::OnGoButton ), NULL, this );
	GoButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WebBrowserPanelGeneratedClass::OnGoButton ), NULL, this );
}
