///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "LintPluginGeneratedClass.h"

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

LintPluginPreferencesGeneratedPanelClass::LintPluginPreferencesGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BozSizer;
	BozSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGidSizer;
	FlexGidSizer = new wxFlexGridSizer( 1, 1, 0, 0 );
	FlexGidSizer->AddGrowableCol( 0 );
	FlexGidSizer->AddGrowableRow( 1 );
	FlexGidSizer->SetFlexibleDirection( wxBOTH );
	FlexGidSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* TopSizer;
	TopSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Preferences") ), wxVERTICAL );
	
	CheckOnSave = new wxCheckBox( this, wxID_ANY, wxT("Perform lint check on file save"), wxDefaultPosition, wxDefaultSize, 0 );
	TopSizer->Add( CheckOnSave, 0, wxALL, 5 );
	
	FlexGidSizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* BottomSizer;
	BottomSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Settings") ), wxVERTICAL );
	
	SettingsLabel = new wxStaticText( this, wxID_ANY, wxT("Enter a list of files that you want the PHP Lint checker to ignore.\nOne or more wildcards; any files that match at least one of the wildcards will be skipped by the PHP Lint checker.\nThis setting is useful to ignore cache files, or for example Symfony skeleton files.\nOne line per wildcard; a wildcard can have either a '*' or a '?'.\n\n* = any number of characters\n? = 0 or 1 character\n\nA wildcard can have directory separators; but they must match the operation system's path separator."), wxDefaultPosition, wxDefaultSize, 0 );
	SettingsLabel->Wrap( -1 );
	BottomSizer->Add( SettingsLabel, 0, wxALL|wxEXPAND, 5 );
	
	IgnoreFiles = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	BottomSizer->Add( IgnoreFiles, 1, wxALL|wxEXPAND, 5 );
	
	FlexGidSizer->Add( BottomSizer, 1, wxEXPAND, 5 );
	
	BozSizer->Add( FlexGidSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BozSizer );
	this->Layout();
}

LintPluginPreferencesGeneratedPanelClass::~LintPluginPreferencesGeneratedPanelClass()
{
}
