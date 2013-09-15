///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ExplorerFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

ModalExplorerGeneratedPanelClass::ModalExplorerGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
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
	
	FilterButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	TopSizer->Add( FilterButton, 0, wxALL, 5 );
	
	ParentButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	ParentButton->SetToolTip( wxT("Parent directory") );
	
	ParentButton->SetToolTip( wxT("Parent directory") );
	
	TopSizer->Add( ParentButton, 0, wxALL, 5 );
	
	RefreshButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	TopSizer->Add( RefreshButton, 0, wxALL, 5 );
	
	Directory = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER ); 
	TopSizer->Add( Directory, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	GridSizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* BodyGridSizer;
	BodyGridSizer = new wxFlexGridSizer( 1, 1, 0, 0 );
	BodyGridSizer->AddGrowableCol( 0 );
	BodyGridSizer->AddGrowableRow( 0 );
	BodyGridSizer->SetFlexibleDirection( wxBOTH );
	BodyGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	LeftPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* LeftPanelSizer;
	LeftPanelSizer = new wxBoxSizer( wxVERTICAL );
	
	List = new wxListCtrl( LeftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_EDIT_LABELS|wxLC_LIST );
	LeftPanelSizer->Add( List, 1, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	ListLabel = new wxStaticText( LeftPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	ListLabel->Wrap( -1 );
	LeftPanelSizer->Add( ListLabel, 0, wxALL|wxEXPAND, 5 );
	
	LeftPanel->SetSizer( LeftPanelSizer );
	LeftPanel->Layout();
	LeftPanelSizer->Fit( LeftPanel );
	BodyGridSizer->Add( LeftPanel, 1, wxEXPAND | wxALL, 5 );
	
	GridSizer->Add( BodyGridSizer, 1, wxEXPAND, 5 );
	
	BoxSizer->Add( GridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	FilterButton->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( ModalExplorerGeneratedPanelClass::OnFilterButtonLeftDown ), NULL, this );
	ParentButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ModalExplorerGeneratedPanelClass::OnParentButtonClick ), NULL, this );
	RefreshButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ModalExplorerGeneratedPanelClass::OnRefreshClick ), NULL, this );
	Directory->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ModalExplorerGeneratedPanelClass::OnDirectoryEnter ), NULL, this );
	List->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ModalExplorerGeneratedPanelClass::OnListKeyDown ), NULL, this );
	List->Connect( wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler( ModalExplorerGeneratedPanelClass::OnListEndLabelEdit ), NULL, this );
	List->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ModalExplorerGeneratedPanelClass::OnListItemActivated ), NULL, this );
	List->Connect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( ModalExplorerGeneratedPanelClass::OnListItemRightClick ), NULL, this );
	List->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ModalExplorerGeneratedPanelClass::OnListItemSelected ), NULL, this );
	List->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ModalExplorerGeneratedPanelClass::OnListRightDown ), NULL, this );
}

ModalExplorerGeneratedPanelClass::~ModalExplorerGeneratedPanelClass()
{
	// Disconnect Events
	FilterButton->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( ModalExplorerGeneratedPanelClass::OnFilterButtonLeftDown ), NULL, this );
	ParentButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ModalExplorerGeneratedPanelClass::OnParentButtonClick ), NULL, this );
	RefreshButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ModalExplorerGeneratedPanelClass::OnRefreshClick ), NULL, this );
	Directory->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ModalExplorerGeneratedPanelClass::OnDirectoryEnter ), NULL, this );
	List->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( ModalExplorerGeneratedPanelClass::OnListKeyDown ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler( ModalExplorerGeneratedPanelClass::OnListEndLabelEdit ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ModalExplorerGeneratedPanelClass::OnListItemActivated ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( ModalExplorerGeneratedPanelClass::OnListItemRightClick ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ModalExplorerGeneratedPanelClass::OnListItemSelected ), NULL, this );
	List->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ModalExplorerGeneratedPanelClass::OnListRightDown ), NULL, this );
	
}

ExplorerOptionsGeneratedPanelClass::ExplorerOptionsGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* PanelSizer;
	PanelSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* StaticBoxSizer;
	StaticBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Explorer Executables") ), wxVERTICAL );
	
	HelpLabel = new wxStaticText( this, wxID_ANY, wxT("MVC Editor will use these executables when the \"Open in File Manager\" or \"Open in Shell\" menus are used."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpLabel->Wrap( 400 );
	StaticBoxSizer->Add( HelpLabel, 1, wxALL|wxEXPAND, 15 );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 2, 2, 0, 0 );
	GridSizer->AddGrowableCol( 1 );
	GridSizer->AddGrowableRow( 0 );
	GridSizer->AddGrowableRow( 1 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	FileManagerLabel = new wxStaticText( this, wxID_ANY, wxT("File Manager"), wxDefaultPosition, wxDefaultSize, 0 );
	FileManagerLabel->Wrap( -1 );
	GridSizer->Add( FileManagerLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	FileManager = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL, wxDefaultValidator, wxT("File manager") );
	GridSizer->Add( FileManager, 0, wxALL|wxEXPAND, 5 );
	
	ShellLabel = new wxStaticText( this, wxID_ANY, wxT("Shell"), wxDefaultPosition, wxDefaultSize, 0 );
	ShellLabel->Wrap( -1 );
	GridSizer->Add( ShellLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	Shell = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL, wxDefaultValidator, wxT("Shell") );
	GridSizer->Add( Shell, 0, wxALL|wxEXPAND, 5 );
	
	StaticBoxSizer->Add( GridSizer, 1, wxEXPAND, 5 );
	
	PanelSizer->Add( StaticBoxSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( PanelSizer );
	this->Layout();
}

ExplorerOptionsGeneratedPanelClass::~ExplorerOptionsGeneratedPanelClass()
{
}
