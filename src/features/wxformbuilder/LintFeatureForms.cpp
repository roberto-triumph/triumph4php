///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
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
	
	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	TopSizer->Add( HelpButton, 0, wxALL, 5 );
	
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
	HelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintResultsGeneratedPanelClass::OnHelpButton ), NULL, this );
}

LintResultsGeneratedPanelClass::~LintResultsGeneratedPanelClass()
{
	// Disconnect Events
	HelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LintResultsGeneratedPanelClass::OnHelpButton ), NULL, this );
	
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
	
	Help = new wxStaticText( this, wxID_ANY, wxT("The lint preferences control the types of PHP errors that Triumph can detect.\n\nPerform lint check on file save \nWhen enabled, a lint check will be performed when the user saves a PHP file.\n\nCheck Uninitialized variables \nChecks for PHP variables that are being read before they have been initialized. Note \nthat this check is only done on variables inside a function or method.\n\nCheck Uninitialized global variables \nLike above, but it checks global variables. The reason it is disabled by default is because \nglobal variables are usually assigned via a template mechanism; checking for initialization \non global variables would lead to many false positives.\n\nCheck for unknown classes, methods and functions \nEach referenced class name, method name or function. In order for this check to\nwork properly there must be at least one defined project, and it must have\nbeen indexed.\n"), wxDefaultPosition, wxDefaultSize, 0 );
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
