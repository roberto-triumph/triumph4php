///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "EnvironmentFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( WebBrowserEditPanelGeneratedClass, wxPanel )
	EVT_BUTTON( ID_MOVEDOWNBUTTON, WebBrowserEditPanelGeneratedClass::_wxFB_OnMoveDown )
	EVT_BUTTON( ID_MOVEUPBUTTON, WebBrowserEditPanelGeneratedClass::_wxFB_OnMoveUp )
	EVT_BUTTON( ID_REMOVE_BROWSER, WebBrowserEditPanelGeneratedClass::_wxFB_OnRemoveSelectedWebBrowser )
	EVT_BUTTON( ID_EDITSELECTEDWEBBROWSER, WebBrowserEditPanelGeneratedClass::_wxFB_OnEditSelectedWebBrowser )
	EVT_BUTTON( ID_ADD_BROWSER, WebBrowserEditPanelGeneratedClass::_wxFB_OnAddWebBrowser )
END_EVENT_TABLE()

WebBrowserEditPanelGeneratedClass::WebBrowserEditPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BodyBoxSizer;
	BodyBoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexSizer;
	FlexSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	FlexSizer->AddGrowableCol( 0 );
	FlexSizer->AddGrowableRow( 2 );
	FlexSizer->SetFlexibleDirection( wxBOTH );
	FlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* HelpSizer;
	HelpSizer = new wxFlexGridSizer( 1, 1, 0, 0 );
	HelpSizer->AddGrowableCol( 0 );
	HelpSizer->AddGrowableRow( 0 );
	HelpSizer->SetFlexibleDirection( wxBOTH );
	HelpSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HelpText = new wxStaticText( this, ID_HELPTEXT, _("This is the list of web browsers that MVC Editor is aware of. MVC Editor will be able to open web pages with any of these browsers. The name is a human friendly name that will be shown, the browser location is the full path to the web browser executable. It is possible to add multiple versions of the same browser  (two version of Firefox for example). You can add a new entry by clicking on the \"Add\" button."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( 500 );
	HelpSizer->Add( HelpText, 1, wxALL|wxEXPAND, 5 );
	
	FlexSizer->Add( HelpSizer, 2, wxEXPAND, 5 );
	
	wxFlexGridSizer* ButtonGridSizer;
	ButtonGridSizer = new wxFlexGridSizer( 1, 2, 0, 0 );
	ButtonGridSizer->AddGrowableCol( 0 );
	ButtonGridSizer->AddGrowableRow( 0 );
	ButtonGridSizer->SetFlexibleDirection( wxBOTH );
	ButtonGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	
	ButtonGridSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* ButtonSizer;
	ButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	MoveDownButton = new wxButton( this, ID_MOVEDOWNBUTTON, _("Move Down"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( MoveDownButton, 0, wxALL, 5 );
	
	MoveUpButton = new wxButton( this, ID_MOVEUPBUTTON, _("Move Up"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( MoveUpButton, 0, wxALL, 5 );
	
	RemoveSelectedBrowser = new wxButton( this, ID_REMOVE_BROWSER, _("Remove Selected"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( RemoveSelectedBrowser, 0, wxALL, 5 );
	
	EditSelectedWebBrowser = new wxButton( this, ID_EDITSELECTEDWEBBROWSER, _("Edit Selected"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( EditSelectedWebBrowser, 0, wxALL, 5 );
	
	AddBrowserButton = new wxButton( this, ID_ADD_BROWSER, _("Add New"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( AddBrowserButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	ButtonGridSizer->Add( ButtonSizer, 1, wxEXPAND, 5 );
	
	FlexSizer->Add( ButtonGridSizer, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 1, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 0 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	BrowserList = new wxListCtrl( this, ID_BROWSERLIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	BrowserList->SetMinSize( wxSize( -1,250 ) );
	
	GridSizer->Add( BrowserList, 1, wxALL|wxEXPAND, 5 );
	
	FlexSizer->Add( GridSizer, 3, wxEXPAND, 5 );
	
	BodyBoxSizer->Add( FlexSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BodyBoxSizer );
	this->Layout();
	BodyBoxSizer->Fit( this );
}

WebBrowserEditPanelGeneratedClass::~WebBrowserEditPanelGeneratedClass()
{
}

BEGIN_EVENT_TABLE( ApacheEnvironmentPanelGeneratedClass, wxPanel )
	EVT_UPDATE_UI( wxID_ANY, ApacheEnvironmentPanelGeneratedClass::_wxFB_OnUpdateUi )
	EVT_DIRPICKER_CHANGED( wxID_ANY, ApacheEnvironmentPanelGeneratedClass::_wxFB_OnDirChanged )
	EVT_BUTTON( ID_SCAN, ApacheEnvironmentPanelGeneratedClass::_wxFB_OnScanButton )
	EVT_BUTTON( ID_REMOVEBUTTON, ApacheEnvironmentPanelGeneratedClass::_wxFB_OnRemoveButton )
	EVT_BUTTON( ID_EDITBUTTON, ApacheEnvironmentPanelGeneratedClass::_wxFB_OnEditButton )
	EVT_BUTTON( ID_ADDBUTTON, ApacheEnvironmentPanelGeneratedClass::_wxFB_OnAddButton )
END_EVENT_TABLE()

ApacheEnvironmentPanelGeneratedClass::ApacheEnvironmentPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* MainSizer;
	MainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 5, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 4 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HelpText = new wxStaticText( this, wxID_ANY, _("This dialog shows the Apache configuration that is currently being used by MVC Editor.  This page lists all defined virtual hosts MVC Editor will use when showing PHP pages in web browsers."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( 500 );
	FlexGridSizer->Add( HelpText, 1, wxALL|wxALIGN_RIGHT|wxEXPAND, 5 );
	
	wxStaticBoxSizer* ConfigDirectorySizer;
	ConfigDirectorySizer = new wxStaticBoxSizer( new wxStaticBox( this, ID_CONFIG_DIRECTORY, _("Configuration Directory") ), wxVERTICAL );
	
	ApacheConfigurationDirectory = new wxDirPickerCtrl( this, wxID_ANY, wxEmptyString, _("Select the Apache Configuration Directory"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_USE_TEXTCTRL );
	ConfigDirectorySizer->Add( ApacheConfigurationDirectory, 1, wxALL|wxEXPAND, 5 );
	
	FlexGridSizer->Add( ConfigDirectorySizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* ScanButtonSizer;
	ScanButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	ScanButton = new wxButton( this, ID_SCAN, _("Scan For Configuration"), wxDefaultPosition, wxDefaultSize, 0 );
	ScanButtonSizer->Add( ScanButton, 0, wxALL, 5 );
	
	Gauge = new wxGauge( this, wxID_ANY, 1, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	ScanButtonSizer->Add( Gauge, 1, wxALL|wxEXPAND, 5 );
	
	FlexGridSizer->Add( ScanButtonSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* ManualButtonSizer;
	ManualButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	Manual = new wxCheckBox( this, ID_MANUAL, _("Manual Configuration"), wxDefaultPosition, wxDefaultSize, 0 );
	ManualButtonSizer->Add( Manual, 0, wxALL, 5 );
	
	RemoveButton = new wxButton( this, ID_REMOVEBUTTON, _("Remove Selected"), wxDefaultPosition, wxDefaultSize, 0 );
	ManualButtonSizer->Add( RemoveButton, 0, wxALL, 5 );
	
	EditButton = new wxButton( this, ID_EDITBUTTON, _("Edit Selected"), wxDefaultPosition, wxDefaultSize, 0 );
	ManualButtonSizer->Add( EditButton, 0, wxALL, 5 );
	
	AddButton = new wxButton( this, ID_ADDBUTTON, _("Add Virtual Host"), wxDefaultPosition, wxDefaultSize, 0 );
	ManualButtonSizer->Add( AddButton, 0, wxALL, 5 );
	
	FlexGridSizer->Add( ManualButtonSizer, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxFlexGridSizer* HostsSizer;
	HostsSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	HostsSizer->AddGrowableCol( 0 );
	HostsSizer->AddGrowableRow( 1 );
	HostsSizer->SetFlexibleDirection( wxBOTH );
	HostsSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	VirtualHostsLabel = new wxStaticText( this, wxID_ANY, _("Defined Virtual Hosts"), wxDefaultPosition, wxDefaultSize, 0 );
	VirtualHostsLabel->Wrap( -1 );
	HostsSizer->Add( VirtualHostsLabel, 0, wxALL|wxEXPAND, 5 );
	
	VirtualHostList = new wxListCtrl( this, ID_VIRTUALHOSTLIST, wxDefaultPosition, wxSize( -1,-1 ), wxLC_NO_SORT_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	VirtualHostList->SetMinSize( wxSize( -1,250 ) );
	
	HostsSizer->Add( VirtualHostList, 1, wxALL|wxEXPAND, 5 );
	
	FlexGridSizer->Add( HostsSizer, 1, wxEXPAND, 5 );
	
	MainSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( MainSizer );
	this->Layout();
}

ApacheEnvironmentPanelGeneratedClass::~ApacheEnvironmentPanelGeneratedClass()
{
}

BEGIN_EVENT_TABLE( PhpEnvironmentPanelGeneratedClass, wxPanel )
	EVT_CHECKBOX( ID_NOPHP, PhpEnvironmentPanelGeneratedClass::_wxFB_OnNoPhpCheck )
	EVT_FILEPICKER_CHANGED( ID_PHP_FILE, PhpEnvironmentPanelGeneratedClass::_wxFB_OnPhpFileChanged )
END_EVENT_TABLE()

PhpEnvironmentPanelGeneratedClass::PhpEnvironmentPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* MainSizer;
	MainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 2 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HelpText = new wxStaticText( this, ID_HELPTEXT, _("This dialog shows the PHP configuration that is currently being used by MVC Editor.  This page lists the executable that  MVC Editor will use when running scripts or detecting the PHP framework that a project is using.\n\nChoose the location of the PHP binary.  If you are running on a Windows PC, choose php-win.exe.\n\nThe version dropdown will determine which version of PHP MVC Editor will use for source code parsing. Choose \"Auto\" to have MVC Editor use the same version as the PHP executable; or one of the other versions to use have MVC Editor use a different version."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( 450 );
	GridSizer->Add( HelpText, 1, wxALL|wxEXPAND, 5 );
	
	NoPhp = new wxCheckBox( this, ID_NOPHP, _("I don't have PHP installed"), wxDefaultPosition, wxDefaultSize, 0 );
	GridSizer->Add( NoPhp, 0, wxALL, 5 );
	
	wxFlexGridSizer* PhpFormSizer;
	PhpFormSizer = new wxFlexGridSizer( 2, 2, 0, 0 );
	PhpFormSizer->AddGrowableCol( 1 );
	PhpFormSizer->SetFlexibleDirection( wxBOTH );
	PhpFormSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	PhpLabel = new wxStaticText( this, wxID_ANY, _("PHP Executable:"), wxDefaultPosition, wxDefaultSize, 0 );
	PhpLabel->Wrap( -1 );
	PhpFormSizer->Add( PhpLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* PhpSizer;
	PhpSizer = new wxBoxSizer( wxHORIZONTAL );
	
	PhpExecutable = new wxTextCtrl( this, ID_PHP_EXECUTABLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	PhpSizer->Add( PhpExecutable, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	PhpExecutableFile = new wxFilePickerCtrl( this, ID_PHP_FILE, wxEmptyString, _("Location of PHP Executable"), wxT("*"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN );
	PhpSizer->Add( PhpExecutableFile, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	PhpFormSizer->Add( PhpSizer, 1, wxEXPAND, 5 );
	
	VersionLabel = new wxStaticText( this, ID_VERSIONLABEL, _("Version:"), wxDefaultPosition, wxDefaultSize, 0 );
	VersionLabel->Wrap( -1 );
	PhpFormSizer->Add( VersionLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	Version = new wxComboBox( this, ID_VERSION, _("Auto"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxCB_READONLY );
	Version->Append( _("Auto") );
	Version->Append( _("PHP 5.3") );
	Version->Append( _("PHP 5.4") );
	bSizer10->Add( Version, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	PhpFormSizer->Add( bSizer10, 1, 0, 5 );
	
	GridSizer->Add( PhpFormSizer, 1, wxEXPAND, 5 );
	
	MainSizer->Add( GridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( MainSizer );
	this->Layout();
}

PhpEnvironmentPanelGeneratedClass::~PhpEnvironmentPanelGeneratedClass()
{
}

BEGIN_EVENT_TABLE( WebBrowserCreateDialogGeneratedClass, wxDialog )
	EVT_FILEPICKER_CHANGED( ID_WEBBROWSERPATH, WebBrowserCreateDialogGeneratedClass::_wxFB_OnFileChanged )
	EVT_BUTTON( wxID_OK, WebBrowserCreateDialogGeneratedClass::_wxFB_OnOkButton )
END_EVENT_TABLE()

WebBrowserCreateDialogGeneratedClass::WebBrowserCreateDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* MainSizer;
	MainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexSizer;
	FlexSizer = new wxFlexGridSizer( 2, 2, 0, 0 );
	FlexSizer->AddGrowableCol( 1 );
	FlexSizer->SetFlexibleDirection( wxBOTH );
	FlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	NameLabel = new wxStaticText( this, ID_NAMELABEL, _("Name"), wxDefaultPosition, wxDefaultSize, 0 );
	NameLabel->Wrap( -1 );
	FlexSizer->Add( NameLabel, 0, wxALL, 5 );
	
	WebBrowserLabel = new wxTextCtrl( this, ID_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	FlexSizer->Add( WebBrowserLabel, 1, wxALL|wxEXPAND, 5 );
	
	PathLabel = new wxStaticText( this, ID_PATHLABEL, _("Web Browser Location"), wxDefaultPosition, wxDefaultSize, 0 );
	PathLabel->Wrap( -1 );
	FlexSizer->Add( PathLabel, 0, wxALL, 5 );
	
	WebBrowserPath = new wxFilePickerCtrl( this, ID_WEBBROWSERPATH, wxEmptyString, _("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	FlexSizer->Add( WebBrowserPath, 1, wxALL|wxEXPAND, 5 );
	
	MainSizer->Add( FlexSizer, 1, wxEXPAND, 5 );
	
	Buttons = new wxStdDialogButtonSizer();
	ButtonsOK = new wxButton( this, wxID_OK );
	Buttons->AddButton( ButtonsOK );
	ButtonsCancel = new wxButton( this, wxID_CANCEL );
	Buttons->AddButton( ButtonsCancel );
	Buttons->Realize();
	MainSizer->Add( Buttons, 1, wxEXPAND, 5 );
	
	this->SetSizer( MainSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
}

WebBrowserCreateDialogGeneratedClass::~WebBrowserCreateDialogGeneratedClass()
{
}

BEGIN_EVENT_TABLE( VirtualHostCreateDialogGeneratedClass, wxDialog )
	EVT_BUTTON( wxID_OK, VirtualHostCreateDialogGeneratedClass::_wxFB_OnOkButton )
END_EVENT_TABLE()

VirtualHostCreateDialogGeneratedClass::VirtualHostCreateDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BodySizer;
	BodySizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 3, 2, 0, 0 );
	GridSizer->AddGrowableCol( 1 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	DirectoryLabel = new wxStaticText( this, ID_DIRECTORYLABEL, _("Root directory"), wxDefaultPosition, wxDefaultSize, 0 );
	DirectoryLabel->Wrap( -1 );
	GridSizer->Add( DirectoryLabel, 0, wxALL, 5 );
	
	RootDirectory = new wxDirPickerCtrl( this, ID_ROOTDIRECTORY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_USE_TEXTCTRL );
	GridSizer->Add( RootDirectory, 1, wxALL|wxEXPAND, 5 );
	
	HostnameLabel = new wxStaticText( this, ID_HOSTNAMELABEL, _("Host name (my.localhost:8080)"), wxDefaultPosition, wxDefaultSize, 0 );
	HostnameLabel->Wrap( -1 );
	GridSizer->Add( HostnameLabel, 1, wxALL|wxEXPAND, 5 );
	
	Hostname = new wxTextCtrl( this, ID_HOSTNAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	GridSizer->Add( Hostname, 1, wxALL|wxEXPAND, 5 );
	
	
	GridSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonsSizer->AddButton( ButtonsSizerCancel );
	ButtonsSizer->Realize();
	GridSizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	BodySizer->Add( GridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
	
	this->Centre( wxBOTH );
}

VirtualHostCreateDialogGeneratedClass::~VirtualHostCreateDialogGeneratedClass()
{
}
