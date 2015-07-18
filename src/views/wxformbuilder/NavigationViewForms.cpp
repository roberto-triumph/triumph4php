///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "NavigationViewForms.h"

///////////////////////////////////////////////////////////////////////////

NavigationViewDialogGeneratedClass::NavigationViewDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	BodySizer = new wxGridSizer( 1, 2, 0, 0 );

	FilesSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Files") ), wxVERTICAL );

	FilesList = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_LIST|wxLC_SINGLE_SEL );
	FilesSizer->Add( FilesList, 1, wxALL|wxEXPAND, 5 );

	BodySizer->Add( FilesSizer, 1, wxEXPAND, 5 );

	PanelsSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Panels") ), wxVERTICAL );

	PanelsList = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_LIST|wxLC_SINGLE_SEL );
	PanelsSizer->Add( PanelsList, 1, wxALL|wxEXPAND, 5 );

	BodySizer->Add( PanelsSizer, 1, wxEXPAND, 5 );

	this->SetSizer( BodySizer );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	FilesList->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( NavigationViewDialogGeneratedClass::OnFilesListKeyDown ), NULL, this );
	FilesList->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( NavigationViewDialogGeneratedClass::OnFileItemActivated ), NULL, this );
	PanelsList->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( NavigationViewDialogGeneratedClass::OnPanelsListKeyDown ), NULL, this );
	PanelsList->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( NavigationViewDialogGeneratedClass::OnPanelItemActivated ), NULL, this );
}

NavigationViewDialogGeneratedClass::~NavigationViewDialogGeneratedClass()
{
	// Disconnect Events
	FilesList->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( NavigationViewDialogGeneratedClass::OnFilesListKeyDown ), NULL, this );
	FilesList->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( NavigationViewDialogGeneratedClass::OnFileItemActivated ), NULL, this );
	PanelsList->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( NavigationViewDialogGeneratedClass::OnPanelsListKeyDown ), NULL, this );
	PanelsList->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( NavigationViewDialogGeneratedClass::OnPanelItemActivated ), NULL, this );

}
