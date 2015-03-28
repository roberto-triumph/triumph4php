///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "LintFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

LintResultsGeneratedPanelClass::LintResultsGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexSizer;
	FlexSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexSizer->AddGrowableCol( 0 );
	FlexSizer->AddGrowableRow( 1 );
	FlexSizer->SetFlexibleDirection( wxBOTH );
	FlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	RunButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	RunButton->SetToolTip( wxT("Run Lint Check Against Projects") );
	
	RunButton->SetToolTip( wxT("Run Lint Check Against Projects") );
	
	TopSizer->Add( RunButton, 0, wxALL, 5 );
	
	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	TopSizer->Add( HelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	SuppressionButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	SuppressionButton->SetToolTip( wxT("Show Lint Suppressions") );
	
	SuppressionButton->SetToolTip( wxT("Show Lint Suppressions") );
	
	TopSizer->Add( SuppressionButton, 0, wxALL, 5 );
	
	Label = new wxStaticText( this, wxID_ANY, wxT("PHP Lint Results"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	TopSizer->Add( Label, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	FlexSizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	ErrorsList = new wxDataViewListCtrl(this, ID_ERRORS_LIST);
	FlexSizer->Add( ErrorsList, 1, wxALL|wxEXPAND, 5 );
	
	BoxSizer->Add( FlexSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	RunButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintResultsGeneratedPanelClass::OnRunButton ), NULL, this );
	HelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintResultsGeneratedPanelClass::OnHelpButton ), NULL, this );
	SuppressionButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintResultsGeneratedPanelClass::OnSuppressionButton ), NULL, this );
}

LintResultsGeneratedPanelClass::~LintResultsGeneratedPanelClass()
{
	// Disconnect Events
	RunButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintResultsGeneratedPanelClass::OnRunButton ), NULL, this );
	HelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintResultsGeneratedPanelClass::OnHelpButton ), NULL, this );
	SuppressionButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintResultsGeneratedPanelClass::OnSuppressionButton ), NULL, this );
	
}

LintSuppressionsGeneratedPanelClass::LintSuppressionsGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexSizer;
	FlexSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexSizer->AddGrowableCol( 0 );
	FlexSizer->AddGrowableRow( 1 );
	FlexSizer->SetFlexibleDirection( wxBOTH );
	FlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	TopSizer->Add( HelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	AddButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	TopSizer->Add( AddButton, 0, wxALL, 5 );
	
	EditButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	TopSizer->Add( EditButton, 0, wxALL, 5 );
	
	DeleteButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	TopSizer->Add( DeleteButton, 0, wxALL, 5 );
	
	DeleteAllButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	TopSizer->Add( DeleteAllButton, 0, wxALL, 5 );
	
	Label = new wxStaticText( this, wxID_ANY, wxT("PHP Lint Suppressions"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	TopSizer->Add( Label, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	FlexSizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	SuppressionsList = new wxDataViewListCtrl(this, ID_SUPPRESSIONS_LIST);
	FlexSizer->Add( SuppressionsList, 1, wxALL|wxEXPAND, 5 );
	
	BoxSizer->Add( FlexSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	HelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintSuppressionsGeneratedPanelClass::OnHelpButton ), NULL, this );
	AddButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintSuppressionsGeneratedPanelClass::OnAddButton ), NULL, this );
	EditButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintSuppressionsGeneratedPanelClass::OnEditButton ), NULL, this );
	DeleteButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintSuppressionsGeneratedPanelClass::OnDeleteButton ), NULL, this );
	DeleteAllButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintSuppressionsGeneratedPanelClass::OnDeleteAllButton ), NULL, this );
	SuppressionsList->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( LintSuppressionsGeneratedPanelClass::OnKeyDown ), NULL, this );
}

LintSuppressionsGeneratedPanelClass::~LintSuppressionsGeneratedPanelClass()
{
	// Disconnect Events
	HelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintSuppressionsGeneratedPanelClass::OnHelpButton ), NULL, this );
	AddButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintSuppressionsGeneratedPanelClass::OnAddButton ), NULL, this );
	EditButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintSuppressionsGeneratedPanelClass::OnEditButton ), NULL, this );
	DeleteButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintSuppressionsGeneratedPanelClass::OnDeleteButton ), NULL, this );
	DeleteAllButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintSuppressionsGeneratedPanelClass::OnDeleteAllButton ), NULL, this );
	SuppressionsList->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( LintSuppressionsGeneratedPanelClass::OnKeyDown ), NULL, this );
	
}

LintSuppressionRuleGeneratedDialogClass::LintSuppressionRuleGeneratedDialogClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* BodySizer;
	BodySizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	BodySizer->AddGrowableCol( 0 );
	BodySizer->AddGrowableRow( 0 );
	BodySizer->AddGrowableRow( 1 );
	BodySizer->AddGrowableRow( 2 );
	BodySizer->SetFlexibleDirection( wxBOTH );
	BodySizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxVERTICAL );
	
	HelpLabel = new wxStaticText( this, wxID_ANY, wxT("A lint suppression rule describes an item that the lint check will disregard (ignore).  You may want to ignore certain checks for various reasons:\n\n- Ignore code from the vendor directory, you don't care if there is a lint error in one of the composer packages because you won't fix it.\n- Ignore a function that you use that is defined in a PHP extension that is not documented in PHP.net; Triumph will not know that it exists.\n- Ignore a class is created at run time via a mocking mechanism (eval); Triumph will not know that it exists."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpLabel->Wrap( 400 );
	TopSizer->Add( HelpLabel, 1, wxALL|wxEXPAND, 5 );
	
	BodySizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* MidSizer;
	MidSizer = new wxFlexGridSizer( 5, 2, 0, 0 );
	MidSizer->AddGrowableCol( 1 );
	MidSizer->SetFlexibleDirection( wxBOTH );
	MidSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	TypeLabel = new wxStaticText( this, wxID_ANY, wxT("Rule Type"), wxDefaultPosition, wxDefaultSize, 0 );
	TypeLabel->Wrap( -1 );
	MidSizer->Add( TypeLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString TypesChoices[] = { wxT("Skip Unknown Class"), wxT("Skip Unknown Method"), wxT("Skip Unknown Property"), wxT("Skip Unknown Function"), wxT("Skip Uninitalized Variable"), wxT("Skip Function Argument Count Mismatch"), wxT("Skip All") };
	int TypesNChoices = sizeof( TypesChoices ) / sizeof( wxString );
	Types = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, TypesNChoices, TypesChoices, 0 );
	Types->SetSelection( 0 );
	MidSizer->Add( Types, 1, wxALL|wxEXPAND, 5 );
	
	TargetLabel = new wxStaticText( this, wxID_ANY, wxT("Suppressed Item"), wxDefaultPosition, wxDefaultSize, 0 );
	TargetLabel->Wrap( -1 );
	MidSizer->Add( TargetLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	Target = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	MidSizer->Add( Target, 0, wxALL|wxEXPAND, 5 );
	
	LocationTypeLabel = new wxStaticText( this, wxID_ANY, wxT("Location Type"), wxDefaultPosition, wxDefaultSize, 0 );
	LocationTypeLabel->Wrap( -1 );
	MidSizer->Add( LocationTypeLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* RadioSizer;
	RadioSizer = new wxBoxSizer( wxHORIZONTAL );
	
	DirectoryRadio = new wxRadioButton( this, wxID_ANY, wxT("Directory"), wxDefaultPosition, wxDefaultSize, 0 );
	RadioSizer->Add( DirectoryRadio, 0, wxALL, 5 );
	
	FileRadio = new wxRadioButton( this, wxID_ANY, wxT("Single File"), wxDefaultPosition, wxDefaultSize, 0 );
	RadioSizer->Add( FileRadio, 0, wxALL, 5 );
	
	MidSizer->Add( RadioSizer, 1, wxEXPAND, 5 );
	
	DirectoryLabel = new wxStaticText( this, wxID_ANY, wxT("Directory"), wxDefaultPosition, wxDefaultSize, 0 );
	DirectoryLabel->Wrap( -1 );
	MidSizer->Add( DirectoryLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	Directory = new wxDirPickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE );
	MidSizer->Add( Directory, 0, wxALL|wxEXPAND, 5 );
	
	FileLabel = new wxStaticText( this, wxID_ANY, wxT("File"), wxDefaultPosition, wxDefaultSize, 0 );
	FileLabel->Wrap( -1 );
	MidSizer->Add( FileLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	File = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_USE_TEXTCTRL );
	MidSizer->Add( File, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	BodySizer->Add( MidSizer, 1, wxEXPAND, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonsSizer->AddButton( ButtonsSizerCancel );
	ButtonsSizer->Realize();
	BodySizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
	BodySizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	Types->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( LintSuppressionRuleGeneratedDialogClass::OnTypeChoice ), NULL, this );
	DirectoryRadio->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( LintSuppressionRuleGeneratedDialogClass::OnDirectoryRadio ), NULL, this );
	FileRadio->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( LintSuppressionRuleGeneratedDialogClass::OnFileRadio ), NULL, this );
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintSuppressionRuleGeneratedDialogClass::OnOkButton ), NULL, this );
}

LintSuppressionRuleGeneratedDialogClass::~LintSuppressionRuleGeneratedDialogClass()
{
	// Disconnect Events
	Types->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( LintSuppressionRuleGeneratedDialogClass::OnTypeChoice ), NULL, this );
	DirectoryRadio->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( LintSuppressionRuleGeneratedDialogClass::OnDirectoryRadio ), NULL, this );
	FileRadio->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( LintSuppressionRuleGeneratedDialogClass::OnFileRadio ), NULL, this );
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintSuppressionRuleGeneratedDialogClass::OnOkButton ), NULL, this );
	
}

LintPreferencesGeneratedPanelClass::LintPreferencesGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Lint Preferences") ), wxVERTICAL );
	
	wxFlexGridSizer* FlexGidSizer;
	FlexGidSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexGidSizer->AddGrowableCol( 0 );
	FlexGidSizer->AddGrowableRow( 1 );
	FlexGidSizer->SetFlexibleDirection( wxBOTH );
	FlexGidSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	Help = new wxStaticText( this, wxID_ANY, wxT("The lint preferences control the types of PHP errors that Triumph can detect.\n\nPerform lint check on file save \nWhen enabled, a lint check will be performed when the user saves a PHP file.\n\nCheck Uninitialized variables \nChecks for PHP variables that are being read before they have been initialized. Note \nthat this check is only done on variables inside a function or method.\n\nCheck Uninitialized global variables \nLike above, but it checks global variables. The reason it is disabled by default is because \nglobal variables are usually assigned via a template mechanism; checking for initialization \non global variables would lead to many false positives.\n\nCheck for unknown classes, methods and functions \nEach referenced class name, method name or function. In order for this check to\nwork properly there must be at least one defined project, and it must have\nbeen indexed.\n\nCheck function argument count mismatch\nWhen enabled, each function / method call will be checked to make sure\nthat the call has the correct number of required arguments. In order\nfor this check to work properly there must be at least one defined \nproject, and it must have been indexed."), wxDefaultPosition, wxDefaultSize, 0 );
	Help->Wrap( -1 );
	FlexGidSizer->Add( Help, 0, wxALL, 5 );
	
	wxBoxSizer* ChecksSizer;
	ChecksSizer = new wxBoxSizer( wxVERTICAL );
	
	CheckOnSave = new wxCheckBox( this, wxID_ANY, wxT("Perform lint check on file save"), wxDefaultPosition, wxDefaultSize, 0 );
	ChecksSizer->Add( CheckOnSave, 0, wxALL, 5 );
	
	CheckUnitializedVariables = new wxCheckBox( this, wxID_ANY, wxT("Check uninitialized variables"), wxDefaultPosition, wxDefaultSize, 0 );
	ChecksSizer->Add( CheckUnitializedVariables, 0, wxALL, 5 );
	
	CheckUnitializedGlobalVariables = new wxCheckBox( this, wxID_ANY, wxT("Check Uninitialized global variables"), wxDefaultPosition, wxDefaultSize, 0 );
	ChecksSizer->Add( CheckUnitializedGlobalVariables, 0, wxALL, 5 );
	
	CheckUnknownIdentifiers = new wxCheckBox( this, wxID_ANY, wxT("Check unknown classes, methods, and functions"), wxDefaultPosition, wxDefaultSize, 0 );
	ChecksSizer->Add( CheckUnknownIdentifiers, 0, wxALL, 5 );
	
	CheckFunctionArgumentCount = new wxCheckBox( this, wxID_ANY, wxT("Check function argument count mismatch"), wxDefaultPosition, wxDefaultSize, 0 );
	ChecksSizer->Add( CheckFunctionArgumentCount, 0, wxALL, 5 );
	
	FlexGidSizer->Add( ChecksSizer, 1, wxEXPAND, 5 );
	
	sbSizer2->Add( FlexGidSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( sbSizer2 );
	this->Layout();
}

LintPreferencesGeneratedPanelClass::~LintPreferencesGeneratedPanelClass()
{
}

LintErrorGeneratedPanelClass::LintErrorGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOTEXT ) );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 0 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	ErrorLabel = new wxStaticText( this, wxID_ANY, wxT("Error on line  57: unexpected '}', expected '{'"), wxDefaultPosition, wxDefaultSize, 0 );
	ErrorLabel->Wrap( -1 );
	GridSizer->Add( ErrorLabel, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* BottomSizer;
	BottomSizer = new wxBoxSizer( wxHORIZONTAL );
	
	HelpLabel = new wxStaticText( this, wxID_ANY, wxT("Press <space> to jump to error, ESC to dismiss"), wxDefaultPosition, wxDefaultSize, 0 );
	HelpLabel->Wrap( -1 );
	BottomSizer->Add( HelpLabel, 1, wxALL|wxEXPAND, 5 );
	
	GoToLink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("Go to error"), wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	BottomSizer->Add( GoToLink, 0, wxALL, 5 );
	
	DismissLink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("Dismiss"), wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	BottomSizer->Add( DismissLink, 0, wxALL, 5 );
	
	GridSizer->Add( BottomSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( GridSizer );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( LintErrorGeneratedPanelClass::OnKeyDown ) );
	GoToLink->Connect( wxEVT_COMMAND_HYPERLINK, wxHyperlinkEventHandler( LintErrorGeneratedPanelClass::OnGoToLink ), NULL, this );
	GoToLink->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( LintErrorGeneratedPanelClass::OnKeyDown ), NULL, this );
	DismissLink->Connect( wxEVT_COMMAND_HYPERLINK, wxHyperlinkEventHandler( LintErrorGeneratedPanelClass::OnDismissLink ), NULL, this );
	DismissLink->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( LintErrorGeneratedPanelClass::OnKeyDown ), NULL, this );
}

LintErrorGeneratedPanelClass::~LintErrorGeneratedPanelClass()
{
	// Disconnect Events
	this->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( LintErrorGeneratedPanelClass::OnKeyDown ) );
	GoToLink->Disconnect( wxEVT_COMMAND_HYPERLINK, wxHyperlinkEventHandler( LintErrorGeneratedPanelClass::OnGoToLink ), NULL, this );
	GoToLink->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( LintErrorGeneratedPanelClass::OnKeyDown ), NULL, this );
	DismissLink->Disconnect( wxEVT_COMMAND_HYPERLINK, wxHyperlinkEventHandler( LintErrorGeneratedPanelClass::OnDismissLink ), NULL, this );
	DismissLink->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( LintErrorGeneratedPanelClass::OnKeyDown ), NULL, this );
	
}

LintHelpDialogGeneratedDialogClass::LintHelpDialogGeneratedDialogClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HelpText = new wxStaticText( this, wxID_ANY, wxT("The Lint feature of Triumph 4 PHP detects PHP syntax errors, uninitialized variables, and unknown identifers. "), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( 400 );
	GridSizer->Add( HelpText, 0, wxALL, 5 );
	
	HelpLink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("More about lint checks in Triumph 4 PHP"), wxT("http://docs.triumph4php.com/php-linter/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	GridSizer->Add( HelpLink, 0, wxALL, 5 );
	
	ButtonSizer = new wxStdDialogButtonSizer();
	ButtonSizerOK = new wxButton( this, wxID_OK );
	ButtonSizer->AddButton( ButtonSizerOK );
	ButtonSizer->Realize();
	GridSizer->Add( ButtonSizer, 1, wxEXPAND, 5 );
	
	BoxSizer->Add( GridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	BoxSizer->Fit( this );
	
	this->Centre( wxBOTH );
}

LintHelpDialogGeneratedDialogClass::~LintHelpDialogGeneratedDialogClass()
{
}

LintSuppressionsHelpGeneratedDialogClass::LintSuppressionsHelpGeneratedDialogClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HelpLabel = new wxStaticText( this, wxID_ANY, wxT("Lint Suppressions\n\n The lint suppression class describes items that a lint check\n should disregard (ignore).  The user may want to ignore certain\n checks for various reasons:\n \n - some files are never modified by the user; the user\n   uses them as libraries which means that the user doesn't\n   care if the lint fails\n - some classes are defined in a PHP extension that is not\n   documented in PHP.net (for example, a PHP extension\n   for couchbase). The user wants to supress these classes\n   because they are defined, but not in the code so Triumph\n   will tag them as unknown classes.\n- clases may be dynamically created via a mocking mechanism\n   (eval), Triumph does not find these classes and\n   will tag them as unknown classes."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpLabel->Wrap( -1 );
	GridSizer->Add( HelpLabel, 0, wxALL, 5 );
	
	HelpLink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("More about lint suppressions in Triumph 4 PHP"), wxT("http://docs.triumph4php.com/php-linter/#suppressions"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	GridSizer->Add( HelpLink, 0, wxALL, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonsSizer->AddButton( ButtonsSizerCancel );
	ButtonsSizer->Realize();
	GridSizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( GridSizer );
	this->Layout();
	GridSizer->Fit( this );
	
	this->Centre( wxBOTH );
}

LintSuppressionsHelpGeneratedDialogClass::~LintSuppressionsHelpGeneratedDialogClass()
{
}
