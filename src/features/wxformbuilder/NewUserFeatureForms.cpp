///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "NewUserFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

NewUserSettingsPanelGeneratedClass::NewUserSettingsPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* BodySizer;
	BodySizer = new wxFlexGridSizer( 4, 1, 0, 0 );
	BodySizer->AddGrowableCol( 0 );
	BodySizer->AddGrowableRow( 3 );
	BodySizer->SetFlexibleDirection( wxBOTH );
	BodySizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	MoreLabel = new wxStaticText( this, wxID_ANY, wxT("Thanks for downloading Triumph 4 PHP! Take a few seconds to configure Triumph to your liking. \n\nNote: All of these settings (and many more) can be changed at any time via the Edit ... Preferences menu."), wxDefaultPosition, wxDefaultSize, 0 );
	MoreLabel->Wrap( -1 );
	BodySizer->Add( MoreLabel, 1, wxALL|wxEXPAND, 15 );
	
	wxStaticBoxSizer* VersionUpdateSizer;
	VersionUpdateSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Version Updates") ), wxVERTICAL );
	
	VersionUpdateHelp = new wxStaticText( this, wxID_ANY, wxT("Triumph can check for new versions of the editor.  Version checking requires an active internet connection. If enabled, version update checks will be done once a week."), wxDefaultPosition, wxDefaultSize, 0 );
	VersionUpdateHelp->Wrap( 640 );
	VersionUpdateSizer->Add( VersionUpdateHelp, 0, wxALL|wxEXPAND, 5 );
	
	CheckForUpdates = new wxCheckBox( this, wxID_ANY, wxT("Check for new versions automatically "), wxDefaultPosition, wxDefaultSize, 0 );
	CheckForUpdates->SetValue(true); 
	VersionUpdateSizer->Add( CheckForUpdates, 0, wxALL|wxEXPAND, 10 );
	
	BodySizer->Add( VersionUpdateSizer, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* SettingsSizer;
	SettingsSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Settings Directory") ), wxVERTICAL );
	
	wxBoxSizer* SettingsLabelSizer;
	SettingsLabelSizer = new wxBoxSizer( wxVERTICAL );
	
	SettingsHelpLabel = new wxStaticText( this, wxID_ANY, wxT("This is the directory where Triumph will store its settings (projects list, tag cache, and preferences)."), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	SettingsHelpLabel->Wrap( -1 );
	SettingsLabelSizer->Add( SettingsHelpLabel, 2, wxALL|wxEXPAND, 5 );
	
	UserDataDirectory = new wxRadioButton( this, wxID_ANY, wxT("User Data Directory"), wxDefaultPosition, wxDefaultSize, 0 );
	UserDataDirectory->SetValue( true ); 
	SettingsLabelSizer->Add( UserDataDirectory, 0, wxALL|wxEXPAND, 5 );
	
	ApplicationDirectory = new wxRadioButton( this, wxID_ANY, wxT("Application Directory (Good for Portable installations)"), wxDefaultPosition, wxDefaultSize, 0 );
	SettingsLabelSizer->Add( ApplicationDirectory, 0, wxALL|wxEXPAND, 5 );
	
	CustomDirectory = new wxRadioButton( this, wxID_ANY, wxT("Custom Directory"), wxDefaultPosition, wxDefaultSize, 0 );
	SettingsLabelSizer->Add( CustomDirectory, 0, wxALL|wxEXPAND, 5 );
	
	SettingsSizer->Add( SettingsLabelSizer, 1, wxEXPAND, 5 );
	
	SettingsDirectory = new wxDirPickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_USE_TEXTCTRL );
	SettingsSizer->Add( SettingsDirectory, 0, wxALL|wxEXPAND, 5 );
	
	BodySizer->Add( SettingsSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
	BodySizer->Fit( this );
}

NewUserSettingsPanelGeneratedClass::~NewUserSettingsPanelGeneratedClass()
{
}

NewUserAssociationsPanelGeneratedClass::NewUserAssociationsPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* BodySizer;
	BodySizer = new wxFlexGridSizer( 1, 1, 0, 0 );
	BodySizer->AddGrowableCol( 0 );
	BodySizer->AddGrowableRow( 0 );
	BodySizer->SetFlexibleDirection( wxBOTH );
	BodySizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* StaticBoxSizer;
	StaticBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Editor Associations") ), wxVERTICAL );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 1 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* LabelSizer;
	LabelSizer = new wxBoxSizer( wxVERTICAL );
	
	HelpLabel = new wxStaticText( this, wxID_ANY, wxT("Triumph uses file extensions to determine how to display the source code contained within them. Triumph's syntax highlighting and source code parsing features use these settings to determine which files contain PHP, SQL, and CSS source code. You can change the file extensions if desired.\n\nEach input can contain 1 or more wilcard strings. \nThe allowed wildcard characters are:\n\n* : Will match on zero or more characters\n? : Will match on zero or one character\n; : Separator among multiple wildcards\n\nFor example, the value \"*.php;*.phtml\" will match files with the extension .php or the extension .phtml\n"), wxDefaultPosition, wxDefaultSize, 0 );
	HelpLabel->Wrap( 500 );
	LabelSizer->Add( HelpLabel, 1, wxALL|wxEXPAND, 5 );
	
	GridSizer->Add( LabelSizer, 0, 0, 5 );
	
	wxFlexGridSizer* AssociationsFormSizer;
	AssociationsFormSizer = new wxFlexGridSizer( 5, 4, 0, 0 );
	AssociationsFormSizer->AddGrowableCol( 1 );
	AssociationsFormSizer->AddGrowableCol( 3 );
	AssociationsFormSizer->SetFlexibleDirection( wxBOTH );
	AssociationsFormSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	PhpLabel = new wxStaticText( this, wxID_ANY, wxT("PHP && HTML Files"), wxDefaultPosition, wxDefaultSize, 0 );
	PhpLabel->Wrap( -1 );
	AssociationsFormSizer->Add( PhpLabel, 0, wxALL, 5 );
	
	PhpFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( PhpFileExtensions, 0, wxALL|wxEXPAND, 5 );
	
	JsLabel = new wxStaticText( this, wxID_ANY, wxT("JS Files"), wxDefaultPosition, wxDefaultSize, 0 );
	JsLabel->Wrap( -1 );
	AssociationsFormSizer->Add( JsLabel, 0, wxALL, 5 );
	
	JsFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( JsFileExtensions, 0, wxALL|wxEXPAND, 5 );
	
	CssLabel = new wxStaticText( this, wxID_ANY, wxT("CSS Files"), wxDefaultPosition, wxDefaultSize, 0 );
	CssLabel->Wrap( -1 );
	AssociationsFormSizer->Add( CssLabel, 0, wxALL, 5 );
	
	CssFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( CssFileExtensions, 0, wxALL|wxEXPAND, 5 );
	
	SqlLabel = new wxStaticText( this, wxID_ANY, wxT("SQL Files"), wxDefaultPosition, wxDefaultSize, 0 );
	SqlLabel->Wrap( -1 );
	AssociationsFormSizer->Add( SqlLabel, 0, wxALL, 5 );
	
	SqlFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( SqlFileExtensions, 0, wxALL|wxEXPAND, 5 );
	
	ConfigLabel = new wxStaticText( this, wxID_ANY, wxT("Config Files"), wxDefaultPosition, wxDefaultSize, 0 );
	ConfigLabel->Wrap( -1 );
	AssociationsFormSizer->Add( ConfigLabel, 0, wxALL, 5 );
	
	ConfigFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( ConfigFileExtensions, 0, wxALL|wxEXPAND, 5 );
	
	YamlLabel = new wxStaticText( this, wxID_ANY, wxT("YAML Files"), wxDefaultPosition, wxDefaultSize, 0 );
	YamlLabel->Wrap( -1 );
	AssociationsFormSizer->Add( YamlLabel, 0, wxALL, 5 );
	
	YamlFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( YamlFileExtensions, 0, wxALL|wxEXPAND, 5 );
	
	XmlLabel = new wxStaticText( this, wxID_ANY, wxT("XML Files"), wxDefaultPosition, wxDefaultSize, 0 );
	XmlLabel->Wrap( -1 );
	AssociationsFormSizer->Add( XmlLabel, 0, wxALL, 5 );
	
	XmlFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( XmlFileExtensions, 0, wxALL|wxEXPAND, 5 );
	
	MarkdownLabel = new wxStaticText( this, wxID_ANY, wxT("Markdown Files"), wxDefaultPosition, wxDefaultSize, 0 );
	MarkdownLabel->Wrap( -1 );
	AssociationsFormSizer->Add( MarkdownLabel, 0, wxALL, 5 );
	
	MarkdownFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( MarkdownFileExtensions, 0, wxALL|wxEXPAND, 5 );
	
	BashLabel = new wxStaticText( this, wxID_ANY, wxT("Bash Files"), wxDefaultPosition, wxDefaultSize, 0 );
	BashLabel->Wrap( -1 );
	AssociationsFormSizer->Add( BashLabel, 0, wxALL, 5 );
	
	BashFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( BashFileExtensions, 0, wxALL|wxEXPAND, 5 );
	
	MiscLabel = new wxStaticText( this, wxID_ANY, wxT("Misc. Files"), wxDefaultPosition, wxDefaultSize, 0 );
	MiscLabel->Wrap( -1 );
	AssociationsFormSizer->Add( MiscLabel, 0, wxALL, 5 );
	
	MiscFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( MiscFileExtensions, 0, wxALL|wxEXPAND, 5 );
	
	GridSizer->Add( AssociationsFormSizer, 1, wxEXPAND, 5 );
	
	StaticBoxSizer->Add( GridSizer, 1, wxEXPAND, 5 );
	
	BodySizer->Add( StaticBoxSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
}

NewUserAssociationsPanelGeneratedClass::~NewUserAssociationsPanelGeneratedClass()
{
}

NewUserPhpSettingsPanelGeneratedClass::NewUserPhpSettingsPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* BodySizer;
	BodySizer = new wxFlexGridSizer( 1, 1, 0, 0 );
	BodySizer->AddGrowableCol( 0 );
	BodySizer->AddGrowableRow( 0 );
	BodySizer->SetFlexibleDirection( wxBOTH );
	BodySizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* PhpLocationSizer;
	PhpLocationSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("PHP Location") ), wxVERTICAL );
	
	PhpHelpLabel = new wxStaticText( this, wxID_ANY, wxT("The PHP executable is used by Triumph when running scripts. It is also used when running framework detection scripts. \n\nChoose the location of the PHP binary.  If you are running on a Windows PC, choose php-win.exe.\n\nThe version dropdown will determine which version of PHP Triumph will use for source code parsing. Choose \"Auto\" to have Triumph use the same version as the PHP executable; or one of the other versions to use have Triumph use a different version."), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	PhpHelpLabel->Wrap( 600 );
	PhpLocationSizer->Add( PhpHelpLabel, 0, wxALL|wxEXPAND, 5 );
	
	PhpDetectorsLink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("More about framework detection in Triumph 4 PHP"), wxT("http://docs.triumph4php.com/framework-detection"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	PhpDetectorsLink->SetToolTip( wxT("https://code.google.com/p/triumph4php/wiki/FrameworkDetection") );
	
	PhpLocationSizer->Add( PhpDetectorsLink, 0, wxALL, 5 );
	
	Installed = new wxCheckBox( this, wxID_ANY, wxT("PHP is installed"), wxDefaultPosition, wxDefaultSize, 0 );
	PhpLocationSizer->Add( Installed, 0, wxALL|wxEXPAND, 5 );
	
	PhpExecutable = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select the PHP executable"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	PhpLocationSizer->Add( PhpExecutable, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* VersionSizer;
	VersionSizer = new wxBoxSizer( wxHORIZONTAL );
	
	VersionLabel = new wxStaticText( this, wxID_ANY, wxT("PHP Version"), wxDefaultPosition, wxDefaultSize, 0 );
	VersionLabel->Wrap( -1 );
	VersionSizer->Add( VersionLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString VersionChoices[] = { wxT("Auto"), wxT("PHP 5.3"), wxT("PHP 5.4") };
	int VersionNChoices = sizeof( VersionChoices ) / sizeof( wxString );
	Version = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, VersionNChoices, VersionChoices, 0 );
	Version->SetSelection( 2 );
	VersionSizer->Add( Version, 0, wxALL, 5 );
	
	PhpLocationSizer->Add( VersionSizer, 0, wxEXPAND, 5 );
	
	BodySizer->Add( PhpLocationSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
	BodySizer->Fit( this );
}

NewUserPhpSettingsPanelGeneratedClass::~NewUserPhpSettingsPanelGeneratedClass()
{
}
