///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
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

	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	HelpButton->SetHelpText( wxT("Help") );

	HelpButton->SetHelpText( wxT("Help") );

	TopSizer->Add( HelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	BodySizer->Add( TopSizer, 1, wxEXPAND, 5 );

	List = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_ICON|wxLC_SINGLE_SEL );
	BodySizer->Add( List, 1, wxALL|wxEXPAND, 5 );

	this->SetSizer( BodySizer );
	this->Layout();

	// Connect Events
	AddFileButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileCabinetPanelGeneratedClass::OnAddFileClick ), NULL, this );
	AddDirectoryButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileCabinetPanelGeneratedClass::OnAddDirectoryClick ), NULL, this );
	HelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileCabinetPanelGeneratedClass::OnHelpButton ), NULL, this );
	List->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( FileCabinetPanelGeneratedClass::OnListItemActivated ), NULL, this );
	List->Connect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( FileCabinetPanelGeneratedClass::OnListItemRightClick ), NULL, this );
}

FileCabinetPanelGeneratedClass::~FileCabinetPanelGeneratedClass()
{
	// Disconnect Events
	AddFileButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileCabinetPanelGeneratedClass::OnAddFileClick ), NULL, this );
	AddDirectoryButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileCabinetPanelGeneratedClass::OnAddDirectoryClick ), NULL, this );
	HelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileCabinetPanelGeneratedClass::OnHelpButton ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( FileCabinetPanelGeneratedClass::OnListItemActivated ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( FileCabinetPanelGeneratedClass::OnListItemRightClick ), NULL, this );

}

FileCabinetFeatureHelpDialogGeneratedClass::FileCabinetFeatureHelpDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxFlexGridSizer* BodySizer;
	BodySizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	BodySizer->SetFlexibleDirection( wxBOTH );
	BodySizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	Help = new wxStaticText( this, wxID_ANY, wxT("The file cabinet stores locations of files and directories \nthat are frequently accessed.  An interesting fact about \nthe file cabinet is  that you may add any file or directory  \nto the file cabinet; the file or directory need not be \nlocated inside a defined project."), wxDefaultPosition, wxDefaultSize, 0 );
	Help->Wrap( -1 );
	BodySizer->Add( Help, 0, wxALL, 5 );

	MoreLink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("More About the File Cabinet in Triumph 4 PHP"), wxT("http://docs.triumph4php.com/file-cabinet"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	BodySizer->Add( MoreLink, 0, wxALL, 5 );

	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizer->Realize();
	BodySizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );

	this->SetSizer( BodySizer );
	this->Layout();
	BodySizer->Fit( this );

	this->Centre( wxBOTH );
}

FileCabinetFeatureHelpDialogGeneratedClass::~FileCabinetFeatureHelpDialogGeneratedClass()
{
}
