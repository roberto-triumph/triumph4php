///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "FindInFilesPluginDialogGeneratedClass.h"

///////////////////////////////////////////////////////////////////////////

FindInFilesDialogGeneratedClass::FindInFilesDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 0 );
	FlexGridSizer->SetFlexibleDirection( wxVERTICAL );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* TextboxFlexGridSizer;
	TextboxFlexGridSizer = new wxFlexGridSizer( 4, 1, 0, 0 );
	TextboxFlexGridSizer->AddGrowableCol( 0 );
	TextboxFlexGridSizer->SetFlexibleDirection( wxVERTICAL );
	TextboxFlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	FindLabel = new wxStaticText( this, wxID_ANY, wxT("Text To Find:"), wxDefaultPosition, wxDefaultSize, 0 );
	FindLabel->Wrap( -1 );
	TextboxFlexGridSizer->Add( FindLabel, 0, wxALL, 5 );
	
	wxBoxSizer* FindWithButtonSizer;
	FindWithButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	RegexFindHelpButton = new wxButton( this, wxID_ANY, wxT(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	RegexFindHelpButton->SetFont( wxFont( 8, 74, 90, 92, false, wxT("Tahoma") ) );
	
	FindWithButtonSizer->Add( RegexFindHelpButton, 0, wxALL, 5 );
	
	FindText = new wxComboBox( this, ID_FINDTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxTE_PROCESS_ENTER ); 
	FindWithButtonSizer->Add( FindText, 1, wxALL|wxEXPAND, 5 );
	
	TextboxFlexGridSizer->Add( FindWithButtonSizer, 1, wxEXPAND, 5 );
	
	ReplaceWithLabel = new wxStaticText( this, wxID_ANY, wxT("Replace With:"), wxDefaultPosition, wxDefaultSize, 0 );
	ReplaceWithLabel->Wrap( -1 );
	TextboxFlexGridSizer->Add( ReplaceWithLabel, 0, wxALL, 5 );
	
	wxBoxSizer* ReplaceWithButtonSizer;
	ReplaceWithButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	RegexReplaceWithHelpButton = new wxButton( this, wxID_ANY, wxT(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	RegexReplaceWithHelpButton->SetFont( wxFont( 8, 74, 90, 92, false, wxT("Tahoma") ) );
	
	ReplaceWithButtonSizer->Add( RegexReplaceWithHelpButton, 0, wxALL, 5 );
	
	ReplaceWithText = new wxComboBox( this, ID_REPLACEWITHTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER ); 
	ReplaceWithButtonSizer->Add( ReplaceWithText, 1, wxALL|wxEXPAND, 5 );
	
	TextboxFlexGridSizer->Add( ReplaceWithButtonSizer, 1, wxEXPAND, 5 );
	
	TopSizer->Add( TextboxFlexGridSizer, 1, wxEXPAND, 5 );
	
	FlexGridSizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* MidSizer;
	MidSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* DirectorySizer;
	DirectorySizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Directory To Search:") ), wxVERTICAL );
	
	Directory = new wxDirPickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a directory to search in"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_USE_TEXTCTRL );
	DirectorySizer->Add( Directory, 0, wxALL|wxEXPAND, 5 );
	
	MidSizer->Add( DirectorySizer, 1, wxALL|wxEXPAND, 5 );
	
	FlexGridSizer->Add( MidSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* MiddleSizer;
	MiddleSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* StaticBoxSizer;
	StaticBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("File Extensions To Search:") ), wxVERTICAL );
	
	FilesFilter = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER ); 
	StaticBoxSizer->Add( FilesFilter, 1, wxALL|wxEXPAND, 5 );
	
	MiddleSizer->Add( StaticBoxSizer, 1, wxEXPAND|wxALL, 5 );
	
	FlexGridSizer->Add( MiddleSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* BottomSizer;
	BottomSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* OptionsSizer;
	OptionsSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxString FinderModeChoices[] = { wxT("Exact"), wxT("Regular Expression") };
	int FinderModeNChoices = sizeof( FinderModeChoices ) / sizeof( wxString );
	FinderMode = new wxRadioBox( this, wxID_ANY, wxT("Find Mode"), wxDefaultPosition, wxDefaultSize, FinderModeNChoices, FinderModeChoices, 3, wxRA_SPECIFY_ROWS );
	FinderMode->SetSelection( 0 );
	OptionsSizer->Add( FinderMode, 1, wxEXPAND|wxRIGHT, 5 );
	
	wxStaticBoxSizer* CheckboxSizer;
	CheckboxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Options") ), wxHORIZONTAL );
	
	CaseSensitive = new wxCheckBox( this, wxID_ANY, wxT("Case Sensitive"), wxDefaultPosition, wxDefaultSize, 0 );
	CaseSensitive->SetValue(true); 
	CheckboxSizer->Add( CaseSensitive, 0, wxLEFT, 5 );
	
	OptionsSizer->Add( CheckboxSizer, 1, wxEXPAND, 5 );
	
	BottomSizer->Add( OptionsSizer, 2, wxEXPAND|wxALL, 5 );
	
	FlexGridSizer->Add( BottomSizer, 1, wxEXPAND, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonsSizer->AddButton( ButtonsSizerCancel );
	ButtonsSizer->Realize();
	FlexGridSizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	RegexFindHelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnRegExFindHelpButton ), NULL, this );
	FindText->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
	RegexReplaceWithHelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnRegExReplaceHelpButton ), NULL, this );
	ReplaceWithText->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
	FilesFilter->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
	ButtonsSizerCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
}

FindInFilesDialogGeneratedClass::~FindInFilesDialogGeneratedClass()
{
	// Disconnect Events
	RegexFindHelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnRegExFindHelpButton ), NULL, this );
	FindText->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
	RegexReplaceWithHelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnRegExReplaceHelpButton ), NULL, this );
	ReplaceWithText->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
	FilesFilter->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
	ButtonsSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
	
}

FindInFilesResultsPanelGeneratedClass::FindInFilesResultsPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 1, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 1 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* ButtonsSizer;
	ButtonsSizer = new wxBoxSizer( wxHORIZONTAL );
	
	ReplaceButton = new wxButton( this, ID_REPLACE_BUTTON, wxT("Replace"), wxDefaultPosition, wxDefaultSize, 0 );
	ReplaceButton->Enable( false );
	
	ButtonsSizer->Add( ReplaceButton, 0, wxALL, 5 );
	
	ReplaceAllInFileButton = new wxButton( this, ID_REPLACE_ALL_IN_FILE_BUTTON, wxT("Replace All In File"), wxDefaultPosition, wxDefaultSize, 0 );
	ReplaceAllInFileButton->Enable( false );
	
	ButtonsSizer->Add( ReplaceAllInFileButton, 0, wxALL, 5 );
	
	ReplaceInAllFilesButton = new wxButton( this, ID_REPLACE_ALL_BUTTON, wxT("Replace All"), wxDefaultPosition, wxDefaultSize, 0 );
	ReplaceInAllFilesButton->Enable( false );
	
	ButtonsSizer->Add( ReplaceInAllFilesButton, 0, wxALL, 5 );
	
	StopButton = new wxButton( this, ID_STOP_BUTTON, wxT("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
	StopButton->Enable( false );
	
	ButtonsSizer->Add( StopButton, 0, wxALL, 5 );
	
	CopySelectedButton = new wxButton( this, ID_COPY_SELECTED_BUTTON, wxT("Copy Selected"), wxDefaultPosition, wxDefaultSize, 0 );
	CopySelectedButton->Enable( false );
	
	ButtonsSizer->Add( CopySelectedButton, 0, wxALL, 5 );
	
	CopyAllButton = new wxButton( this, ID_COPY_ALL_BUTTON, wxT("Copy All"), wxDefaultPosition, wxDefaultSize, 0 );
	CopyAllButton->Enable( false );
	
	ButtonsSizer->Add( CopyAllButton, 0, wxALL, 5 );
	
	TopSizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	ResultText = new wxStaticText( this, wxID_ANY, wxT("Status: <OK>"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	ResultText->Wrap( -1 );
	TopSizer->Add( ResultText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	FlexGridSizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	ResultsList = new wxListBox( this, ID_FIND_IN_FILES_RESULTS, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_NEEDED_SB ); 
	FlexGridSizer->Add( ResultsList, 1, wxALL|wxEXPAND, 5 );
	
	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	ReplaceButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnReplaceButton ), NULL, this );
	ReplaceAllInFileButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnReplaceAllInFileButton ), NULL, this );
	ReplaceInAllFilesButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnReplaceInAllFilesButton ), NULL, this );
	StopButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnStopButton ), NULL, this );
	CopySelectedButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnCopySelectedButton ), NULL, this );
	CopyAllButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnCopyAllButton ), NULL, this );
	ResultsList->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnDoubleClick ), NULL, this );
}

FindInFilesResultsPanelGeneratedClass::~FindInFilesResultsPanelGeneratedClass()
{
	// Disconnect Events
	ReplaceButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnReplaceButton ), NULL, this );
	ReplaceAllInFileButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnReplaceAllInFileButton ), NULL, this );
	ReplaceInAllFilesButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnReplaceInAllFilesButton ), NULL, this );
	StopButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnStopButton ), NULL, this );
	CopySelectedButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnCopySelectedButton ), NULL, this );
	CopyAllButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnCopyAllButton ), NULL, this );
	ResultsList->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnDoubleClick ), NULL, this );
	
}
