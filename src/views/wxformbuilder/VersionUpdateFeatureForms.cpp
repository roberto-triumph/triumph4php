///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "VersionUpdateFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

VersionUpdateGeneratedDialogClass::VersionUpdateGeneratedDialogClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* DialogSizer;
	DialogSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	StatusLabel = new wxStaticText( this, wxID_ANY, wxT("Status:"), wxDefaultPosition, wxDefaultSize, 0 );
	StatusLabel->Wrap( -1 );
	bSizer4->Add( StatusLabel, 0, wxALL, 5 );
	
	Result = new wxStaticText( this, wxID_ANY, wxT("Checking for updates"), wxDefaultPosition, wxDefaultSize, 0 );
	Result->Wrap( -1 );
	bSizer4->Add( Result, 0, wxALL, 5 );
	
	DialogSizer->Add( bSizer4, 0, wxEXPAND, 5 );
	
	Gauge = new wxGauge( this, wxID_ANY, 1, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	Gauge->SetValue( 1 ); 
	DialogSizer->Add( Gauge, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	CurrentLabel = new wxStaticText( this, wxID_ANY, wxT("Current Version"), wxDefaultPosition, wxDefaultSize, 0 );
	CurrentLabel->Wrap( -1 );
	fgSizer1->Add( CurrentLabel, 0, wxALL, 5 );
	
	CurrentVersion = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	CurrentVersion->Wrap( -1 );
	fgSizer1->Add( CurrentVersion, 1, wxALL, 5 );
	
	NewLabel = new wxStaticText( this, wxID_ANY, wxT("New Version"), wxDefaultPosition, wxDefaultSize, 0 );
	NewLabel->Wrap( -1 );
	fgSizer1->Add( NewLabel, 0, wxALL, 5 );
	
	NewVersion = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	NewVersion->Wrap( -1 );
	fgSizer1->Add( NewVersion, 0, wxALL, 5 );
	
	DialogSizer->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	DownloadLink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("View Downloads"), wxT("http://triumph4php.com/downloads"), wxDefaultPosition, wxDefaultSize, wxHL_ALIGN_CENTRE );
	DialogSizer->Add( DownloadLink, 0, wxALL|wxEXPAND, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizer->Realize();
	DialogSizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( DialogSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VersionUpdateGeneratedDialogClass::OnOkButton ), NULL, this );
}

VersionUpdateGeneratedDialogClass::~VersionUpdateGeneratedDialogClass()
{
	// Disconnect Events
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VersionUpdateGeneratedDialogClass::OnOkButton ), NULL, this );
	
}

VersionUpdatePreferencesGeneratedPanelClass::VersionUpdatePreferencesGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* PanelSizer;
	PanelSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* UpdatesSizer;
	UpdatesSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Version Updates") ), wxVERTICAL );
	
	HelpText = new wxStaticText( this, wxID_ANY, wxT("Triumph can check for new versions of the itself.  Version checking requires an active internet connection. If enabled, version update checks will be done once a week.\n\nNote that this is only a version check, you will have to download the new version and install it manually."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( 550 );
	UpdatesSizer->Add( HelpText, 0, wxALL, 5 );
	
	CheckForUpdates = new wxCheckBox( this, wxID_ANY, wxT("Check for new versions automatically "), wxDefaultPosition, wxDefaultSize, 0 );
	UpdatesSizer->Add( CheckForUpdates, 0, wxALL, 10 );
	
	PanelSizer->Add( UpdatesSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( PanelSizer );
	this->Layout();
}

VersionUpdatePreferencesGeneratedPanelClass::~VersionUpdatePreferencesGeneratedPanelClass()
{
}
