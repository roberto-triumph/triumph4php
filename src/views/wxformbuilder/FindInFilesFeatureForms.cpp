///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "FindInFilesFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

FindInFilesDialogGeneratedClass::FindInFilesDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );

	BoxSizer->SetMinSize( wxSize( 500,-1 ) );
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 5, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
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

	RegExFindHelpButton = new wxButton( this, wxID_ANY, wxT(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	FindWithButtonSizer->Add( RegExFindHelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	FindText = new wxComboBox( this, ID_FINDTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxTE_PROCESS_ENTER );
	FindWithButtonSizer->Add( FindText, 1, wxALL|wxEXPAND, 5 );

	TextboxFlexGridSizer->Add( FindWithButtonSizer, 1, wxEXPAND, 5 );

	ReplaceWithLabel = new wxStaticText( this, wxID_ANY, wxT("Replace With:"), wxDefaultPosition, wxDefaultSize, 0 );
	ReplaceWithLabel->Wrap( -1 );
	TextboxFlexGridSizer->Add( ReplaceWithLabel, 0, wxALL, 5 );

	wxBoxSizer* ReplaceWithButtonSizer;
	ReplaceWithButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	RegexReplaceWithHelpButton = new wxButton( this, ID_REGEXREPLACEHELPBUTTON, wxT(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	ReplaceWithButtonSizer->Add( RegexReplaceWithHelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	ReplaceWithText = new wxComboBox( this, ID_REPLACEWITHTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER );
	ReplaceWithButtonSizer->Add( ReplaceWithText, 1, wxALL|wxEXPAND, 5 );

	TextboxFlexGridSizer->Add( ReplaceWithButtonSizer, 1, wxEXPAND, 5 );

	TopSizer->Add( TextboxFlexGridSizer, 1, wxEXPAND, 5 );

	FlexGridSizer->Add( TopSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* MidSizer;
	MidSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* DirectorySizer;
	DirectorySizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Directory To Search:") ), wxHORIZONTAL );

	Directory = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER );
	DirectorySizer->Add( Directory, 1, wxALL|wxEXPAND, 5 );

	DirectoryDirPicker = new wxDirPickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a directory to search in"), wxDefaultPosition, wxDefaultSize, wxDIRP_DIR_MUST_EXIST );
	DirectorySizer->Add( DirectoryDirPicker, 0, wxALL|wxEXPAND, 5 );

	MidSizer->Add( DirectorySizer, 1, wxALL|wxEXPAND, 5 );

	FlexGridSizer->Add( MidSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* MiddleSizer;
	MiddleSizer = new wxBoxSizer( wxHORIZONTAL );

	wxStaticBoxSizer* StaticBoxSizer;
	StaticBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("File Extensions To Search:") ), wxVERTICAL );

	FilesFilter = new wxComboBox( this, ID_FILESFILTER, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER );
	StaticBoxSizer->Add( FilesFilter, 1, wxALL|wxEXPAND, 5 );

	MiddleSizer->Add( StaticBoxSizer, 1, wxEXPAND|wxALL, 5 );

	FlexGridSizer->Add( MiddleSizer, 0, wxEXPAND, 5 );

	wxBoxSizer* BottomSizer;
	BottomSizer = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* OptionsSizer;
	OptionsSizer = new wxBoxSizer( wxHORIZONTAL );

	wxString FinderModeChoices[] = { wxT("Exact"), wxT("Case Insensitive"), wxT("Regular Expression") };
	int FinderModeNChoices = sizeof( FinderModeChoices ) / sizeof( wxString );
	FinderMode = new wxRadioBox( this, wxID_ANY, wxT("Find Mode"), wxDefaultPosition, wxDefaultSize, FinderModeNChoices, FinderModeChoices, 3, wxRA_SPECIFY_ROWS );
	FinderMode->SetSelection( 0 );
	OptionsSizer->Add( FinderMode, 1, wxEXPAND|wxRIGHT, 5 );

	wxStaticBoxSizer* CheckboxSizer;
	CheckboxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Options") ), wxVERTICAL );

	DoHiddenFiles = new wxCheckBox( this, wxID_ANY, wxT("Search Hidden Files"), wxDefaultPosition, wxDefaultSize, 0 );
	CheckboxSizer->Add( DoHiddenFiles, 0, wxALL, 5 );

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
	BoxSizer->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	RegExFindHelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnRegExFindHelpButton ), NULL, this );
	FindText->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( FindInFilesDialogGeneratedClass::OnKeyDown ), NULL, this );
	FindText->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( FindInFilesDialogGeneratedClass::OnKillFocusFindText ), NULL, this );
	FindText->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
	RegexReplaceWithHelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnRegExReplaceHelpButton ), NULL, this );
	ReplaceWithText->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( FindInFilesDialogGeneratedClass::OnKeyDown ), NULL, this );
	ReplaceWithText->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( FindInFilesDialogGeneratedClass::OnKillFocusReplaceText ), NULL, this );
	ReplaceWithText->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
	Directory->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( FindInFilesDialogGeneratedClass::OnKeyDown ), NULL, this );
	Directory->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
	DirectoryDirPicker->Connect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( FindInFilesDialogGeneratedClass::OnDirChanged ), NULL, this );
	DirectoryDirPicker->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( FindInFilesDialogGeneratedClass::OnKeyDown ), NULL, this );
	FilesFilter->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( FindInFilesDialogGeneratedClass::OnKeyDown ), NULL, this );
	FilesFilter->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
	ButtonsSizerCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
}

FindInFilesDialogGeneratedClass::~FindInFilesDialogGeneratedClass()
{
	// Disconnect Events
	RegExFindHelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnRegExFindHelpButton ), NULL, this );
	FindText->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( FindInFilesDialogGeneratedClass::OnKeyDown ), NULL, this );
	FindText->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( FindInFilesDialogGeneratedClass::OnKillFocusFindText ), NULL, this );
	FindText->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
	RegexReplaceWithHelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnRegExReplaceHelpButton ), NULL, this );
	ReplaceWithText->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( FindInFilesDialogGeneratedClass::OnKeyDown ), NULL, this );
	ReplaceWithText->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( FindInFilesDialogGeneratedClass::OnKillFocusReplaceText ), NULL, this );
	ReplaceWithText->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
	Directory->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( FindInFilesDialogGeneratedClass::OnKeyDown ), NULL, this );
	Directory->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
	DirectoryDirPicker->Disconnect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( FindInFilesDialogGeneratedClass::OnDirChanged ), NULL, this );
	DirectoryDirPicker->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( FindInFilesDialogGeneratedClass::OnKeyDown ), NULL, this );
	FilesFilter->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( FindInFilesDialogGeneratedClass::OnKeyDown ), NULL, this );
	FilesFilter->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );
	ButtonsSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogGeneratedClass::OnOkButton ), NULL, this );

}

FindInFilesHelpDialogGeneratedClass::FindInFilesHelpDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxFlexGridSizer* Sizer;
	Sizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	Sizer->SetFlexibleDirection( wxBOTH );
	Sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	HelpText = new wxStaticText( this, wxID_ANY, wxT("The find in files feature lets you search for a string in any directory in your file system."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( 400 );
	Sizer->Add( HelpText, 0, wxALL, 5 );

	HelpLink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("More about find in files in Triumph 4 PHP"), wxT("http://docs.triumph4php.com/find-in-files/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	Sizer->Add( HelpLink, 0, wxALL, 5 );

	ButtonSizer = new wxStdDialogButtonSizer();
	ButtonSizerOK = new wxButton( this, wxID_OK );
	ButtonSizer->AddButton( ButtonSizerOK );
	ButtonSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonSizer->AddButton( ButtonSizerCancel );
	ButtonSizer->Realize();
	Sizer->Add( ButtonSizer, 1, wxEXPAND, 5 );

	this->SetSizer( Sizer );
	this->Layout();
	Sizer->Fit( this );

	this->Centre( wxBOTH );
}

FindInFilesHelpDialogGeneratedClass::~FindInFilesHelpDialogGeneratedClass()
{
}

FindInFilesResultsPanelGeneratedClass::FindInFilesResultsPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 1 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* TopGridSizer;
	TopGridSizer = new wxFlexGridSizer( 1, 3, 0, 0 );
	TopGridSizer->AddGrowableCol( 1 );
	TopGridSizer->AddGrowableRow( 0 );
	TopGridSizer->SetFlexibleDirection( wxBOTH );
	TopGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxBoxSizer* ButtonSizer;
	ButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	ReplaceButton = new wxBitmapButton( this, ID_REPLACEBUTTON, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	ReplaceButton->SetToolTip( wxT("Replace selected hit") );

	ReplaceButton->SetToolTip( wxT("Replace selected hit") );

	ButtonSizer->Add( ReplaceButton, 0, wxALL, 5 );

	ReplaceAllInFileButton = new wxBitmapButton( this, ID_REPLACEALLINFILEBUTTON, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	ReplaceAllInFileButton->SetToolTip( wxT("Replace all hits in the current opened file") );

	ReplaceAllInFileButton->SetToolTip( wxT("Replace all hits in the current opened file") );

	ButtonSizer->Add( ReplaceAllInFileButton, 0, wxALL, 5 );

	ReplaceInAllFilesButton = new wxBitmapButton( this, ID_REPLACEINALLFILESBUTTON, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	ReplaceInAllFilesButton->SetToolTip( wxT("Replace all hits found in all files") );

	ReplaceInAllFilesButton->SetToolTip( wxT("Replace all hits found in all files") );

	ButtonSizer->Add( ReplaceInAllFilesButton, 0, wxALL, 5 );

	PreviousHitButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	PreviousHitButton->SetToolTip( wxT("Show the previous hit") );

	PreviousHitButton->SetToolTip( wxT("Show the previous hit") );

	ButtonSizer->Add( PreviousHitButton, 0, wxALL, 5 );

	NextHitButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	NextHitButton->SetToolTip( wxT("Show the next hit") );

	NextHitButton->SetToolTip( wxT("Show the next hit") );

	ButtonSizer->Add( NextHitButton, 0, wxALL, 5 );

	CopySelectedButton = new wxBitmapButton( this, ID_COPYSELETEDBUTTON, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	CopySelectedButton->SetToolTip( wxT("Copy selected hit to the clipboard") );

	CopySelectedButton->SetToolTip( wxT("Copy selected hit to the clipboard") );

	ButtonSizer->Add( CopySelectedButton, 0, wxALL, 5 );

	CopyAllButton = new wxBitmapButton( this, ID_COPYALLBUTTON, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	CopyAllButton->SetToolTip( wxT("Copy all hits to the clipboard") );

	CopyAllButton->SetToolTip( wxT("Copy all hits to the clipboard") );

	ButtonSizer->Add( CopyAllButton, 0, wxALL, 5 );

	StopButton = new wxBitmapButton( this, ID_STOPBUTTON, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	StopButton->SetToolTip( wxT("Stops the running search") );

	StopButton->SetToolTip( wxT("Stops the running search") );

	ButtonSizer->Add( StopButton, 0, wxALL, 5 );

	TopGridSizer->Add( ButtonSizer, 1, wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* ReplaceSizer;
	ReplaceSizer = new wxBoxSizer( wxHORIZONTAL );

	ReplaceLabel = new wxStaticText( this, wxID_ANY, wxT("Replace With:"), wxDefaultPosition, wxDefaultSize, 0 );
	ReplaceLabel->Wrap( -1 );
	ReplaceSizer->Add( ReplaceLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	RegexReplaceWithHelpButton = new wxButton( this, ID_REPLACEREGEXHELPBUTTON, wxT(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	RegexReplaceWithHelpButton->SetToolTip( wxT("Regular Expression Help") );

	ReplaceSizer->Add( RegexReplaceWithHelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	ReplaceWithText = new wxComboBox( this, ID_REPLACETEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	ReplaceSizer->Add( ReplaceWithText, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	TopGridSizer->Add( ReplaceSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* LabelsSizer;
	LabelsSizer = new wxBoxSizer( wxHORIZONTAL );

	FindLabel = new wxStaticText( this, wxID_ANY, wxT("Searched For:"), wxDefaultPosition, wxDefaultSize, 0 );
	FindLabel->Wrap( -1 );
	LabelsSizer->Add( FindLabel, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	ResultText = new wxStaticText( this, wxID_ANY, wxT("Status: <OK>"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	ResultText->Wrap( -1 );
	LabelsSizer->Add( ResultText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	TopGridSizer->Add( LabelsSizer, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	FlexGridSizer->Add( TopGridSizer, 1, wxEXPAND, 5 );

	ResultsList =  new wxDataViewListCtrl(this, ID_RESULTS_LIST);
	FlexGridSizer->Add( ResultsList, 1, wxALL|wxEXPAND, 5 );

	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND|wxALL, 5 );

	this->SetSizer( BoxSizer );
	this->Layout();

	// Connect Events
	ReplaceButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnReplaceButton ), NULL, this );
	ReplaceAllInFileButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnReplaceAllInFileButton ), NULL, this );
	ReplaceInAllFilesButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnReplaceInAllFilesButton ), NULL, this );
	PreviousHitButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnPreviousHitButton ), NULL, this );
	NextHitButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnNextHitButton ), NULL, this );
	CopySelectedButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnCopySelectedButton ), NULL, this );
	CopyAllButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnCopyAllButton ), NULL, this );
	StopButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnStopButton ), NULL, this );
	RegexReplaceWithHelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnRegExReplaceHelpButton ), NULL, this );
	ReplaceWithText->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( FindInFilesResultsPanelGeneratedClass::OnKillFocusReplaceText ), NULL, this );
}

FindInFilesResultsPanelGeneratedClass::~FindInFilesResultsPanelGeneratedClass()
{
	// Disconnect Events
	ReplaceButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnReplaceButton ), NULL, this );
	ReplaceAllInFileButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnReplaceAllInFileButton ), NULL, this );
	ReplaceInAllFilesButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnReplaceInAllFilesButton ), NULL, this );
	PreviousHitButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnPreviousHitButton ), NULL, this );
	NextHitButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnNextHitButton ), NULL, this );
	CopySelectedButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnCopySelectedButton ), NULL, this );
	CopyAllButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnCopyAllButton ), NULL, this );
	StopButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnStopButton ), NULL, this );
	RegexReplaceWithHelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesResultsPanelGeneratedClass::OnRegExReplaceHelpButton ), NULL, this );
	ReplaceWithText->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( FindInFilesResultsPanelGeneratedClass::OnKillFocusReplaceText ), NULL, this );

}
