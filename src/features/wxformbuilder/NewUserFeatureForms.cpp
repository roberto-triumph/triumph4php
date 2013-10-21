///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "NewUserFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

NewUserDialogGeneratedClass::NewUserDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* DialogSizer;
	DialogSizer = new wxBoxSizer( wxVERTICAL );
	
	MoreLabel = new wxStaticText( this, wxID_ANY, wxT("Thanks for downloading MVC Editor!\nTake a few seconds to configure MVC Editor to your liking.\n\nNote: All of these settings (and many more) can be changed at any time via the Edit ... Preferences menu."), wxDefaultPosition, wxDefaultSize, 0 );
	MoreLabel->Wrap( -1 );
	DialogSizer->Add( MoreLabel, 0, wxALL|wxEXPAND, 15 );
	
	wxStaticBoxSizer* VersionUpdateSizer;
	VersionUpdateSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Version Updates") ), wxVERTICAL );
	
	VersionUpdateHelp = new wxStaticText( this, wxID_ANY, wxT("MVC Editor can check for new versions of the editor.  Version checking requires an active internet connection. If enabled, version update checks will be done once a week."), wxDefaultPosition, wxDefaultSize, 0 );
	VersionUpdateHelp->Wrap( 550 );
	VersionUpdateSizer->Add( VersionUpdateHelp, 0, wxALL, 5 );
	
	CheckForUpdates = new wxCheckBox( this, wxID_ANY, wxT("Check for new versions automatically "), wxDefaultPosition, wxDefaultSize, 0 );
	VersionUpdateSizer->Add( CheckForUpdates, 0, wxALL|wxEXPAND, 10 );
	
	DialogSizer->Add( VersionUpdateSizer, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* SettingsSizer;
	SettingsSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Settings Directory") ), wxVERTICAL );
	
	wxBoxSizer* SettingsLabelSizer;
	SettingsLabelSizer = new wxBoxSizer( wxVERTICAL );
	
	SettingsHelpLabel = new wxStaticText( this, wxID_ANY, wxT("This is the directory where MVC Editor will store its settings (projects list, tag cache, and preferences)."), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	SettingsHelpLabel->Wrap( -1 );
	SettingsLabelSizer->Add( SettingsHelpLabel, 2, wxALL|wxEXPAND, 5 );
	
	UserDataDirectory = new wxRadioButton( this, wxID_ANY, wxT("User Data Directory"), wxDefaultPosition, wxDefaultSize, 0 );
	SettingsLabelSizer->Add( UserDataDirectory, 0, wxALL|wxEXPAND, 5 );
	
	ApplicationDirectory = new wxRadioButton( this, wxID_ANY, wxT("Application Directory (Good for Portable installations)"), wxDefaultPosition, wxDefaultSize, 0 );
	SettingsLabelSizer->Add( ApplicationDirectory, 0, wxALL|wxEXPAND, 5 );
	
	CustomDirectory = new wxRadioButton( this, wxID_ANY, wxT("Custom Directory"), wxDefaultPosition, wxDefaultSize, 0 );
	SettingsLabelSizer->Add( CustomDirectory, 0, wxALL|wxEXPAND, 5 );
	
	SettingsSizer->Add( SettingsLabelSizer, 1, wxEXPAND, 5 );
	
	SettingsDirectory = new wxDirPickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_USE_TEXTCTRL );
	SettingsSizer->Add( SettingsDirectory, 0, wxALL|wxEXPAND, 5 );
	
	DialogSizer->Add( SettingsSizer, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* PhpLocationSizer;
	PhpLocationSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("PHP Location") ), wxVERTICAL );
	
	PhpHelpLabel = new wxStaticText( this, wxID_ANY, wxT("The PHP executable is used by MVC Editor when running scripts. It is also used when running framework detection scripts. \n\nChoose the location of the PHP binary.  If you are running on a Windows PC, choose php-win.exe."), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	PhpHelpLabel->Wrap( 550 );
	PhpLocationSizer->Add( PhpHelpLabel, 1, wxALL, 5 );
	
	PhpDetectorsLink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("More about framework detection in MVC Editor"), wxT("https://code.google.com/p/mvc-editor/wiki/FrameworkDetection"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	PhpDetectorsLink->SetToolTip( wxT("https://code.google.com/p/mvc-editor/wiki/FrameworkDetection") );
	
	PhpLocationSizer->Add( PhpDetectorsLink, 0, wxALL, 5 );
	
	Installed = new wxCheckBox( this, wxID_ANY, wxT("PHP is installed"), wxDefaultPosition, wxDefaultSize, 0 );
	PhpLocationSizer->Add( Installed, 0, wxALL|wxEXPAND, 5 );
	
	PhpExecutable = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select the PHP executable"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	PhpLocationSizer->Add( PhpExecutable, 0, wxALL|wxEXPAND, 5 );
	
	DialogSizer->Add( PhpLocationSizer, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* FileSizer;
	FileSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("File Extensions") ), wxVERTICAL );
	
	FileExtensionsHelpLabel = new wxStaticText( this, wxID_ANY, wxT("MVC Editor uses file extensions to determine how to display the source code contained within them. MVC Editor's syntax highlighting and source code parsing features use these settings to determine which files contain PHP, SQL, and CSS source code. You can change the file extensions if desired.\n\nEach input can contain 1 or more wilcard strings. \nThe allowed wildcard characters are:\n\n* : Will match on zero or more characters\n? : Will match on zero or one character\n; : Separator among multiple wildcards\n\nFor example, the value \"*.php;*.phtml\" will match files with the extension .php or the extension .phtml"), wxDefaultPosition, wxDefaultSize, 0 );
	FileExtensionsHelpLabel->Wrap( 550 );
	FileSizer->Add( FileExtensionsHelpLabel, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* FlexSizer;
	FlexSizer = new wxFlexGridSizer( 4, 2, 0, 0 );
	FlexSizer->AddGrowableCol( 1 );
	FlexSizer->SetFlexibleDirection( wxHORIZONTAL );
	FlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	PhpLabel = new wxStaticText( this, wxID_ANY, wxT("PHP && HTML files"), wxDefaultPosition, wxDefaultSize, 0 );
	PhpLabel->Wrap( -1 );
	FlexSizer->Add( PhpLabel, 0, wxALL, 5 );
	
	PhpFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	FlexSizer->Add( PhpFileExtensions, 1, wxALL|wxEXPAND, 5 );
	
	SqlLabel = new wxStaticText( this, wxID_ANY, wxT("SQL Files"), wxDefaultPosition, wxDefaultSize, 0 );
	SqlLabel->Wrap( -1 );
	FlexSizer->Add( SqlLabel, 0, wxALL, 5 );
	
	SqlFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	FlexSizer->Add( SqlFileExtensions, 0, wxALL|wxEXPAND, 5 );
	
	CssLabel = new wxStaticText( this, wxID_ANY, wxT("CSS Files"), wxDefaultPosition, wxDefaultSize, 0 );
	CssLabel->Wrap( -1 );
	FlexSizer->Add( CssLabel, 0, wxALL, 5 );
	
	CssFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	FlexSizer->Add( CssFileExtensions, 0, wxALL|wxEXPAND, 5 );
	
	MiscLabel = new wxStaticText( this, wxID_ANY, wxT("Misc. Files"), wxDefaultPosition, wxDefaultSize, 0 );
	MiscLabel->Wrap( -1 );
	FlexSizer->Add( MiscLabel, 0, wxALL, 5 );
	
	MiscFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	FlexSizer->Add( MiscFileExtensions, 0, wxALL|wxEXPAND, 5 );
	
	FileSizer->Add( FlexSizer, 1, wxEXPAND, 5 );
	
	DialogSizer->Add( FileSizer, 0, wxEXPAND, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizer->Realize();
	DialogSizer->Add( ButtonsSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( DialogSizer );
	this->Layout();
	DialogSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewUserDialogGeneratedClass::OnOkButton ), NULL, this );
}

NewUserDialogGeneratedClass::~NewUserDialogGeneratedClass()
{
	// Disconnect Events
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewUserDialogGeneratedClass::OnOkButton ), NULL, this );
	
}
