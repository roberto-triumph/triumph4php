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
	
	Label = new wxStaticText( this, wxID_ANY, wxT("PHP Lint Results"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	FlexSizer->Add( Label, 1, wxALL|wxEXPAND, 5 );
	
	ErrorsList = new wxListBox( this, ID_ERRORS_LIST, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL|wxLB_NEEDED_SB ); 
	FlexSizer->Add( ErrorsList, 1, wxALL|wxEXPAND, 5 );
	
	BoxSizer->Add( FlexSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	ErrorsList->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( LintResultsGeneratedPanelClass::OnListDoubleClick ), NULL, this );
}

LintResultsGeneratedPanelClass::~LintResultsGeneratedPanelClass()
{
	// Disconnect Events
	ErrorsList->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( LintResultsGeneratedPanelClass::OnListDoubleClick ), NULL, this );
	
}

LintPreferencesGeneratedPanelClass::LintPreferencesGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BozSizer;
	BozSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGidSizer;
	FlexGidSizer = new wxFlexGridSizer( 1, 1, 0, 0 );
	FlexGidSizer->AddGrowableCol( 0 );
	FlexGidSizer->AddGrowableRow( 0 );
	FlexGidSizer->SetFlexibleDirection( wxBOTH );
	FlexGidSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* TopSizer;
	TopSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Preferences") ), wxVERTICAL );
	
	CheckOnSave = new wxCheckBox( this, wxID_ANY, wxT("Perform lint check on file save"), wxDefaultPosition, wxDefaultSize, 0 );
	TopSizer->Add( CheckOnSave, 0, wxALL, 5 );
	
	FlexGidSizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	BozSizer->Add( FlexGidSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BozSizer );
	this->Layout();
}

LintPreferencesGeneratedPanelClass::~LintPreferencesGeneratedPanelClass()
{
}

LintErrorGeneratedPanelClass::LintErrorGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOTEXT ) );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer3->AddGrowableCol( 0 );
	fgSizer3->AddGrowableRow( 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	ErrorLabel = new wxStaticText( this, wxID_ANY, wxT("Error on line  57: unexpected '}', expected '{'"), wxDefaultPosition, wxDefaultSize, 0 );
	ErrorLabel->Wrap( -1 );
	fgSizer3->Add( ErrorLabel, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	HelpLabel = new wxStaticText( this, wxID_ANY, wxT("Press <space> to jump to error, ESC to dismiss"), wxDefaultPosition, wxDefaultSize, 0 );
	HelpLabel->Wrap( -1 );
	bSizer3->Add( HelpLabel, 1, wxALL|wxEXPAND, 5 );
	
	GoToLink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("Go to error"), wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer3->Add( GoToLink, 0, wxALL, 5 );
	
	DismissLink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("Dismiss"), wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer3->Add( DismissLink, 0, wxALL, 5 );
	
	fgSizer3->Add( bSizer3, 1, wxEXPAND, 5 );
	
	this->SetSizer( fgSizer3 );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( LintErrorGeneratedPanelClass::OnKeyDown ) );
	GoToLink->Connect( wxEVT_COMMAND_HYPERLINK, wxHyperlinkEventHandler( LintErrorGeneratedPanelClass::OnGoToLink ), NULL, this );
	DismissLink->Connect( wxEVT_COMMAND_HYPERLINK, wxHyperlinkEventHandler( LintErrorGeneratedPanelClass::OnDismissLink ), NULL, this );
}

LintErrorGeneratedPanelClass::~LintErrorGeneratedPanelClass()
{
	// Disconnect Events
	this->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( LintErrorGeneratedPanelClass::OnKeyDown ) );
	GoToLink->Disconnect( wxEVT_COMMAND_HYPERLINK, wxHyperlinkEventHandler( LintErrorGeneratedPanelClass::OnGoToLink ), NULL, this );
	DismissLink->Disconnect( wxEVT_COMMAND_HYPERLINK, wxHyperlinkEventHandler( LintErrorGeneratedPanelClass::OnDismissLink ), NULL, this );
	
}
