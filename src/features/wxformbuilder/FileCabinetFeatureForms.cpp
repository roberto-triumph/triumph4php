///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "FileCabinetFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

FileCabinetPanelGeneratedClass::FileCabinetPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* BodySizer;
	BodySizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	BodySizer->AddGrowableCol( 0 );
	BodySizer->AddGrowableRow( 1 );
	BodySizer->SetFlexibleDirection( wxBOTH );
	BodySizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	AddFileButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	AddFileButton->SetToolTip( wxT("Add a file to the cabinet") );
	
	AddFileButton->SetToolTip( wxT("Add a file to the cabinet") );
	
	TopSizer->Add( AddFileButton, 0, wxALL, 5 );
	
	AddDirectoryButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	AddDirectoryButton->SetToolTip( wxT("Add a directory to the cabinet") );
	
	AddDirectoryButton->SetToolTip( wxT("Add a directory to the cabinet") );
	
	TopSizer->Add( AddDirectoryButton, 0, wxALL, 5 );
	
	BodySizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	List = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_ICON|wxLC_SINGLE_SEL );
	BodySizer->Add( List, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
	
	// Connect Events
	AddFileButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileCabinetPanelGeneratedClass::OnAddFileClick ), NULL, this );
	AddDirectoryButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileCabinetPanelGeneratedClass::OnAddDirectoryClick ), NULL, this );
	List->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( FileCabinetPanelGeneratedClass::OnListItemActivated ), NULL, this );
}

FileCabinetPanelGeneratedClass::~FileCabinetPanelGeneratedClass()
{
	// Disconnect Events
	AddFileButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileCabinetPanelGeneratedClass::OnAddFileClick ), NULL, this );
	AddDirectoryButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileCabinetPanelGeneratedClass::OnAddDirectoryClick ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( FileCabinetPanelGeneratedClass::OnListItemActivated ), NULL, this );
	
}
