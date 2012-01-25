///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "EnvironmentGeneratedDialogClass.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( WebBrowserEditPanelGeneratedClass, wxPanel )
	EVT_SIZE( WebBrowserEditPanelGeneratedClass::_wxFB_OnResize )
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
	
	RemoveSelectedBrowser = new wxButton( this, ID_REMOVE_BROWSER, _("Remove Selected Web Browser"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( RemoveSelectedBrowser, 0, wxALL, 5 );
	
	EditSelectedWebBrowser = new wxButton( this, ID_EDITSELECTEDWEBBROWSER, _("Edit Selected Web Browser"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( EditSelectedWebBrowser, 0, wxALL, 5 );
	
	AddBrowserButton = new wxButton( this, ID_ADD_BROWSER, _("Add Web Browser"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( AddBrowserButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	ButtonGridSizer->Add( ButtonSizer, 1, wxEXPAND, 5 );
	
	FlexSizer->Add( ButtonGridSizer, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 0 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	BrowserList = new wxListCtrl( this, ID_BROWSERLIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	GridSizer->Add( BrowserList, 1, wxALL|wxEXPAND, 5 );
	
	FlexSizer->Add( GridSizer, 3, wxEXPAND, 5 );
	
	BodyBoxSizer->Add( FlexSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BodyBoxSizer );
	this->Layout();
}

WebBrowserEditPanelGeneratedClass::~WebBrowserEditPanelGeneratedClass()
{
}

BEGIN_EVENT_TABLE( ApacheEnvironmentPanelGeneratedClass, wxPanel )
	EVT_SIZE( ApacheEnvironmentPanelGeneratedClass::_wxFB_OnResize )
	EVT_BUTTON( ID_SCAN, ApacheEnvironmentPanelGeneratedClass::_wxFB_OnScanButton )
END_EVENT_TABLE()

ApacheEnvironmentPanelGeneratedClass::ApacheEnvironmentPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* MainSizer;
	MainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 4, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 2 );
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
	
	wxFlexGridSizer* HostsSizer;
	HostsSizer = new wxFlexGridSizer( 1, 1, 0, 0 );
	HostsSizer->AddGrowableCol( 0 );
	HostsSizer->AddGrowableRow( 1 );
	HostsSizer->SetFlexibleDirection( wxBOTH );
	HostsSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	VirtualHostsLabel = new wxStaticText( this, wxID_ANY, _("Defined Virtual Hosts"), wxDefaultPosition, wxDefaultSize, 0 );
	VirtualHostsLabel->Wrap( -1 );
	HostsSizer->Add( VirtualHostsLabel, 0, wxALL|wxEXPAND, 5 );
	
	VirtualHostResults = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	HostsSizer->Add( VirtualHostResults, 1, wxALL|wxEXPAND, 5 );
	
	FlexGridSizer->Add( HostsSizer, 1, wxEXPAND, 5 );
	
	Gauge = new wxGauge( this, wxID_ANY, 1, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	FlexGridSizer->Add( Gauge, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* ButtonSizer;
	ButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	ScanButton = new wxButton( this, ID_SCAN, _("Scan For Configuration"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( ScanButton, 0, wxALL, 5 );
	
	FlexGridSizer->Add( ButtonSizer, 1, wxEXPAND, 5 );
	
	MainSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( MainSizer );
	this->Layout();
}

ApacheEnvironmentPanelGeneratedClass::~ApacheEnvironmentPanelGeneratedClass()
{
}

BEGIN_EVENT_TABLE( PhpEnvironmentPanelGeneratedClass, wxPanel )
	EVT_SIZE( PhpEnvironmentPanelGeneratedClass::_wxFB_OnResize )
	EVT_FILEPICKER_CHANGED( ID_PHP_FILE, PhpEnvironmentPanelGeneratedClass::_wxFB_OnPhpFileChanged )
END_EVENT_TABLE()

PhpEnvironmentPanelGeneratedClass::PhpEnvironmentPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* MainSizer;
	MainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	GridSizer->AddGrowableRow( 0 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HelpText = new wxStaticText( this, ID_HELPTEXT, _("This dialog shows the PHP configuration that is currently being used by MVC Editor.  This page lists the executable that  MVC Editor will use when running scripts or detecting the PHP framework that a project is using.\n\nChoose the location of the PHP binary.  If you are running on a Windows PC, choose php-win.exe."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( 450 );
	GridSizer->Add( HelpText, 1, wxALL|wxEXPAND, 5 );
	
	MainSizer->Add( GridSizer, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* PhpSizer;
	PhpSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxHORIZONTAL );
	
	PhpLabel = new wxStaticText( this, wxID_ANY, _("PHP Executable:"), wxDefaultPosition, wxDefaultSize, 0 );
	PhpLabel->Wrap( -1 );
	PhpSizer->Add( PhpLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	PhpExecutable = new wxTextCtrl( this, ID_PHP_EXECUTABLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	PhpSizer->Add( PhpExecutable, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	PhpExecutableFile = new wxFilePickerCtrl( this, ID_PHP_FILE, wxEmptyString, _("Location of PHP Executable"), wxT("*"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN );
	PhpSizer->Add( PhpExecutableFile, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	MainSizer->Add( PhpSizer, 0, wxEXPAND, 5 );
	
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
