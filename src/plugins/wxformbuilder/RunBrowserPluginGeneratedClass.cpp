///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "RunBrowserPluginGeneratedClass.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( ChooseUrlDialogGeneratedClass, wxDialog )
	EVT_UPDATE_UI( wxID_ANY, ChooseUrlDialogGeneratedClass::_wxFB_OnUpdateUi )
	EVT_BUTTON( wxID_OK, ChooseUrlDialogGeneratedClass::_wxFB_OnOkButton )
END_EVENT_TABLE()

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
}

ChooseUrlDialogGeneratedClass::~ChooseUrlDialogGeneratedClass()
{
}
