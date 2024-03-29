///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
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

	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	TopSizer->Add( HelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	Directory = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER );
	TopSizer->Add( Directory, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	GridSizer->Add( TopSizer, 1, wxEXPAND, 5 );

	wxFlexGridSizer* BodyGridSizer;
	BodyGridSizer = new wxFlexGridSizer( 1, 1, 0, 0 );
	BodyGridSizer->AddGrowableCol( 0 );
	BodyGridSizer->AddGrowableRow( 0 );
	BodyGridSizer->SetFlexibleDirection( wxBOTH );
	BodyGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	Splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH );
	Splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( ModalExplorerGeneratedPanelClass::SplitterOnIdle ), NULL, this );

	SourcesPanel = new wxPanel( Splitter, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
	wxBoxSizer* SourcesSizer;
	SourcesSizer = new wxBoxSizer( wxVERTICAL );

	SourcesList = new wxListCtrl( SourcesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_SINGLE_SEL|wxLC_SMALL_ICON );
	SourcesSizer->Add( SourcesList, 1, wxALL|wxEXPAND, 5 );

	SourcesLabel = new wxStaticText( SourcesPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	SourcesLabel->Wrap( -1 );
	SourcesSizer->Add( SourcesLabel, 0, wxALL|wxEXPAND, 5 );

	SourcesPanel->SetSizer( SourcesSizer );
	SourcesPanel->Layout();
	SourcesSizer->Fit( SourcesPanel );
	FilesPanel = new wxPanel( Splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* FilesPanelSizer;
	FilesPanelSizer = new wxBoxSizer( wxVERTICAL );

	List = new wxListCtrl( FilesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_EDIT_LABELS|wxLC_LIST );
	FilesPanelSizer->Add( List, 1, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );

	ListLabel = new wxStaticText( FilesPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	ListLabel->Wrap( -1 );
	FilesPanelSizer->Add( ListLabel, 0, wxALL|wxEXPAND, 5 );

	FilesPanel->SetSizer( FilesPanelSizer );
	FilesPanel->Layout();
	FilesPanelSizer->Fit( FilesPanel );
	Splitter->SplitVertically( SourcesPanel, FilesPanel, 164 );
	BodyGridSizer->Add( Splitter, 1, wxEXPAND, 5 );

	GridSizer->Add( BodyGridSizer, 1, wxEXPAND, 5 );

	BoxSizer->Add( GridSizer, 1, wxEXPAND, 5 );

	this->SetSizer( BoxSizer );
	this->Layout();

	// Connect Events
	FilterButton->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( ModalExplorerGeneratedPanelClass::OnFilterButtonLeftDown ), NULL, this );
	ParentButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ModalExplorerGeneratedPanelClass::OnParentButtonClick ), NULL, this );
	RefreshButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ModalExplorerGeneratedPanelClass::OnRefreshClick ), NULL, this );
	HelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ModalExplorerGeneratedPanelClass::OnHelpButton ), NULL, this );
	Directory->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ModalExplorerGeneratedPanelClass::OnDirectoryEnter ), NULL, this );
	SourcesList->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ModalExplorerGeneratedPanelClass::OnSourceActivated ), NULL, this );
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
	HelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ModalExplorerGeneratedPanelClass::OnHelpButton ), NULL, this );
	Directory->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ModalExplorerGeneratedPanelClass::OnDirectoryEnter ), NULL, this );
	SourcesList->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ModalExplorerGeneratedPanelClass::OnSourceActivated ), NULL, this );
	List->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( ModalExplorerGeneratedPanelClass::OnListKeyDown ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler( ModalExplorerGeneratedPanelClass::OnListEndLabelEdit ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ModalExplorerGeneratedPanelClass::OnListItemActivated ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( ModalExplorerGeneratedPanelClass::OnListItemRightClick ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ModalExplorerGeneratedPanelClass::OnListItemSelected ), NULL, this );
	List->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ModalExplorerGeneratedPanelClass::OnListRightDown ), NULL, this );

}

ExplorerOutlineGeneratedPanelClass::ExplorerOutlineGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 2 );
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

	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	TopSizer->Add( HelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	GridSizer->Add( TopSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* DirSizer;
	DirSizer = new wxBoxSizer( wxVERTICAL );

	Directory = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER );
	DirSizer->Add( Directory, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	GridSizer->Add( DirSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* FilesPanelSizer;
	FilesPanelSizer = new wxBoxSizer( wxVERTICAL );

	List = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_EDIT_LABELS|wxLC_NO_HEADER|wxLC_REPORT );
	FilesPanelSizer->Add( List, 1, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );

	ListLabel = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	ListLabel->Wrap( -1 );
	FilesPanelSizer->Add( ListLabel, 0, wxALL|wxEXPAND, 5 );

	GridSizer->Add( FilesPanelSizer, 1, wxEXPAND, 5 );

	BoxSizer->Add( GridSizer, 1, wxEXPAND, 5 );

	this->SetSizer( BoxSizer );
	this->Layout();

	// Connect Events
	FilterButton->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( ExplorerOutlineGeneratedPanelClass::OnFilterButtonLeftDown ), NULL, this );
	ParentButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ExplorerOutlineGeneratedPanelClass::OnParentButtonClick ), NULL, this );
	RefreshButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ExplorerOutlineGeneratedPanelClass::OnRefreshClick ), NULL, this );
	HelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ExplorerOutlineGeneratedPanelClass::OnHelpButton ), NULL, this );
	Directory->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( ExplorerOutlineGeneratedPanelClass::OnDirectorySelected ), NULL, this );
	Directory->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ExplorerOutlineGeneratedPanelClass::OnDirectoryEnter ), NULL, this );
	List->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ExplorerOutlineGeneratedPanelClass::OnListKeyDown ), NULL, this );
	List->Connect( wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler( ExplorerOutlineGeneratedPanelClass::OnListEndLabelEdit ), NULL, this );
	List->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ExplorerOutlineGeneratedPanelClass::OnListItemActivated ), NULL, this );
	List->Connect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( ExplorerOutlineGeneratedPanelClass::OnListItemRightClick ), NULL, this );
	List->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ExplorerOutlineGeneratedPanelClass::OnListItemSelected ), NULL, this );
	List->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ExplorerOutlineGeneratedPanelClass::OnListRightDown ), NULL, this );
}

ExplorerOutlineGeneratedPanelClass::~ExplorerOutlineGeneratedPanelClass()
{
	// Disconnect Events
	FilterButton->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( ExplorerOutlineGeneratedPanelClass::OnFilterButtonLeftDown ), NULL, this );
	ParentButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ExplorerOutlineGeneratedPanelClass::OnParentButtonClick ), NULL, this );
	RefreshButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ExplorerOutlineGeneratedPanelClass::OnRefreshClick ), NULL, this );
	HelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ExplorerOutlineGeneratedPanelClass::OnHelpButton ), NULL, this );
	Directory->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( ExplorerOutlineGeneratedPanelClass::OnDirectorySelected ), NULL, this );
	Directory->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ExplorerOutlineGeneratedPanelClass::OnDirectoryEnter ), NULL, this );
	List->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( ExplorerOutlineGeneratedPanelClass::OnListKeyDown ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler( ExplorerOutlineGeneratedPanelClass::OnListEndLabelEdit ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ExplorerOutlineGeneratedPanelClass::OnListItemActivated ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( ExplorerOutlineGeneratedPanelClass::OnListItemRightClick ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ExplorerOutlineGeneratedPanelClass::OnListItemSelected ), NULL, this );
	List->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ExplorerOutlineGeneratedPanelClass::OnListRightDown ), NULL, this );

}

ExplorerHelpGeneratedDialogClass::ExplorerHelpGeneratedDialogClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxFlexGridSizer* Sizer;
	Sizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	Sizer->SetFlexibleDirection( wxBOTH );
	Sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	HelpText = new wxStaticText( this, wxID_ANY, wxT("The explorer panel can be used to browse any directory in your file system."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( -1 );
	Sizer->Add( HelpText, 0, wxALL, 5 );

	HelpLink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("More about the explorer panel in Triumph 4 PHP"), wxT("http://docs.triumph4php.com/explorer/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	Sizer->Add( HelpLink, 0, wxALL, 5 );

	ButtonSizer = new wxStdDialogButtonSizer();
	ButtonSizerOK = new wxButton( this, wxID_OK );
	ButtonSizer->AddButton( ButtonSizerOK );
	ButtonSizer->Realize();
	Sizer->Add( ButtonSizer, 1, wxEXPAND, 5 );

	this->SetSizer( Sizer );
	this->Layout();
	Sizer->Fit( this );

	this->Centre( wxBOTH );
}

ExplorerHelpGeneratedDialogClass::~ExplorerHelpGeneratedDialogClass()
{
}

ExplorerOptionsGeneratedPanelClass::ExplorerOptionsGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* PanelSizer;
	PanelSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* StaticBoxSizer;
	StaticBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Explorer Executables") ), wxVERTICAL );

	HelpLabel = new wxStaticText( this, wxID_ANY, wxT("Triumph will use these executables when the \"Open in File Manager\" or \"Open in Shell\" menus are used."), wxDefaultPosition, wxDefaultSize, 0 );
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

ExplorerNewFileGeneratedDialogClass::ExplorerNewFileGeneratedDialogClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	Label = new wxStaticText( this, wxID_ANY, wxT("Please enter a file name"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	GridSizer->Add( Label, 1, wxALL|wxEXPAND, 5 );

	FileNameText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	GridSizer->Add( FileNameText, 1, wxALL|wxEXPAND, 5 );

	BoxSizer->Add( GridSizer, 1, wxEXPAND, 5 );

	ButtonSizer = new wxStdDialogButtonSizer();
	ButtonSizerOK = new wxButton( this, wxID_OK );
	ButtonSizer->AddButton( ButtonSizerOK );
	ButtonSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonSizer->AddButton( ButtonSizerCancel );
	ButtonSizer->Realize();
	BoxSizer->Add( ButtonSizer, 1, wxEXPAND, 5 );

	this->SetSizer( BoxSizer );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	FileNameText->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ExplorerNewFileGeneratedDialogClass::OnTextEnter ), NULL, this );
	ButtonSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ExplorerNewFileGeneratedDialogClass::OnOkButton ), NULL, this );
}

ExplorerNewFileGeneratedDialogClass::~ExplorerNewFileGeneratedDialogClass()
{
	// Disconnect Events
	FileNameText->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ExplorerNewFileGeneratedDialogClass::OnTextEnter ), NULL, this );
	ButtonSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ExplorerNewFileGeneratedDialogClass::OnOkButton ), NULL, this );

}
