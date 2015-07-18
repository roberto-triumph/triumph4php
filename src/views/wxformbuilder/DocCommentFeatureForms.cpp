///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "DocCommentFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

DocCommentPanelGeneratedClass::DocCommentPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOTEXT ) );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );

	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 1 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );

	PhpSiteDocs = new wxHyperlinkCtrl( this, ID_PHP_SITE_LINK, wxT("php.net docs"), wxT("php.net"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	TopSizer->Add( PhpSiteDocs, 0, wxALL, 5 );

	CloseLink = new wxHyperlinkCtrl( this, ID_CLOSE_LINK, wxT("Close"), wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	TopSizer->Add( CloseLink, 0, wxALL|wxALIGN_RIGHT, 5 );

	GridSizer->Add( TopSizer, 1, wxALIGN_RIGHT, 5 );

	Text = new wxRichTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxHSCROLL|wxVSCROLL|wxWANTS_CHARS );
	Text->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOTEXT ) );
	Text->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );

	GridSizer->Add( Text, 1, wxEXPAND | wxALL, 5 );

	BoxSizer->Add( GridSizer, 1, wxEXPAND, 5 );

	this->SetSizer( BoxSizer );
	this->Layout();

	// Connect Events
	this->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( DocCommentPanelGeneratedClass::OnKeyDown ) );
	PhpSiteDocs->Connect( wxEVT_COMMAND_HYPERLINK, wxHyperlinkEventHandler( DocCommentPanelGeneratedClass::OnPhpSiteDocs ), NULL, this );
	PhpSiteDocs->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( DocCommentPanelGeneratedClass::OnKeyDown ), NULL, this );
	CloseLink->Connect( wxEVT_COMMAND_HYPERLINK, wxHyperlinkEventHandler( DocCommentPanelGeneratedClass::OnClose ), NULL, this );
	CloseLink->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( DocCommentPanelGeneratedClass::OnKeyDown ), NULL, this );
	Text->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( DocCommentPanelGeneratedClass::OnKeyDown ), NULL, this );
}

DocCommentPanelGeneratedClass::~DocCommentPanelGeneratedClass()
{
	// Disconnect Events
	this->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( DocCommentPanelGeneratedClass::OnKeyDown ) );
	PhpSiteDocs->Disconnect( wxEVT_COMMAND_HYPERLINK, wxHyperlinkEventHandler( DocCommentPanelGeneratedClass::OnPhpSiteDocs ), NULL, this );
	PhpSiteDocs->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( DocCommentPanelGeneratedClass::OnKeyDown ), NULL, this );
	CloseLink->Disconnect( wxEVT_COMMAND_HYPERLINK, wxHyperlinkEventHandler( DocCommentPanelGeneratedClass::OnClose ), NULL, this );
	CloseLink->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( DocCommentPanelGeneratedClass::OnKeyDown ), NULL, this );
	Text->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( DocCommentPanelGeneratedClass::OnKeyDown ), NULL, this );

}
