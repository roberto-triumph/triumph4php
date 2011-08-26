///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "EnvironmentGeneratedDialogClass.h"

///////////////////////////////////////////////////////////////////////////

EnvironmentGeneratedDialogClass::EnvironmentGeneratedDialogClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* MainSizer;
	MainSizer = new wxBoxSizer( wxVERTICAL );
	
	HelpText = new wxStaticText( this, wxID_ANY, _("This dialog shows the LAMP configuration that is currently being used by MVC Editor.  This page lists all defined virtual hosts and PHP executables MVC Editor will use when running scripts or showing them in the browser."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( 500 );
	MainSizer->Add( HelpText, 0, wxALL|wxEXPAND|wxALIGN_RIGHT, 5 );
	
	wxGridBagSizer* GridBagSizer;
	GridBagSizer = new wxGridBagSizer( 0, 0 );
	GridBagSizer->AddGrowableCol( 0 );
	GridBagSizer->AddGrowableCol( 1 );
	GridBagSizer->AddGrowableRow( 0 );
	GridBagSizer->SetFlexibleDirection( wxBOTH );
	GridBagSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* ApacheSizer;
	ApacheSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Apache") ), wxVERTICAL );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	ConfigurationDirectoryLabel = new wxStaticText( this, wxID_ANY, _("Configuration Directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	ConfigurationDirectoryLabel->Wrap( -1 );
	bSizer6->Add( ConfigurationDirectoryLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	ApacheConfigurationDirectory = new wxDirPickerCtrl( this, wxID_ANY, wxEmptyString, _("Select the Apache Configuration Directory"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_USE_TEXTCTRL );
	bSizer6->Add( ApacheConfigurationDirectory, 1, wxALL, 5 );
	
	ApacheSizer->Add( bSizer6, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->AddGrowableRow( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	wxBoxSizer* HostsSizer;
	HostsSizer = new wxBoxSizer( wxVERTICAL );
	
	VirtualHostsLabel = new wxStaticText( this, wxID_ANY, _("Defined Virtual Hosts"), wxDefaultPosition, wxDefaultSize, 0 );
	VirtualHostsLabel->Wrap( -1 );
	HostsSizer->Add( VirtualHostsLabel, 0, wxALL|wxEXPAND, 5 );
	
	VirtualHostResults = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	HostsSizer->Add( VirtualHostResults, 1, wxALL|wxEXPAND, 5 );
	
	fgSizer1->Add( HostsSizer, 1, wxEXPAND, 5 );
	
	ApacheSizer->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	GridBagSizer->Add( ApacheSizer, wxGBPosition( 0, 0 ), wxGBSpan( 1, 2 ), wxEXPAND, 5 );
	
	wxStaticBoxSizer* PhpSizer;
	PhpSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("PHP") ), wxHORIZONTAL );
	
	PhpLabel = new wxStaticText( this, wxID_ANY, _("PHP Executable:"), wxDefaultPosition, wxDefaultSize, 0 );
	PhpLabel->Wrap( -1 );
	PhpSizer->Add( PhpLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	PhpExecutable = new wxTextCtrl( this, ID_PHP_EXECUTABLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	PhpSizer->Add( PhpExecutable, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	PhpExecutableFile = new wxFilePickerCtrl( this, ID_PHP_FILE, wxEmptyString, _("Location of PHP Executable"), wxT("*"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN );
	PhpSizer->Add( PhpExecutableFile, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	GridBagSizer->Add( PhpSizer, wxGBPosition( 1, 0 ), wxGBSpan( 1, 2 ), wxEXPAND, 5 );
	
	Gauge = new wxGauge( this, wxID_ANY, 1, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	GridBagSizer->Add( Gauge, wxGBPosition( 2, 0 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	ScanButton = new wxButton( this, ID_SCAN, _("Scan For Configuration"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( ScanButton, 0, wxALL, 5 );
	
	
	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	OkButton = new wxButton( this, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( OkButton, 0, wxALL, 5 );
	
	GridBagSizer->Add( bSizer2, wxGBPosition( 3, 0 ), wxGBSpan( 1, 2 ), wxEXPAND, 5 );
	
	MainSizer->Add( GridBagSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( MainSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	ScanButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvironmentGeneratedDialogClass::OnScanButton ), NULL, this );
	OkButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvironmentGeneratedDialogClass::OnOkButton ), NULL, this );
}

EnvironmentGeneratedDialogClass::~EnvironmentGeneratedDialogClass()
{
	// Disconnect Events
	ScanButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvironmentGeneratedDialogClass::OnScanButton ), NULL, this );
	OkButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvironmentGeneratedDialogClass::OnOkButton ), NULL, this );
}
