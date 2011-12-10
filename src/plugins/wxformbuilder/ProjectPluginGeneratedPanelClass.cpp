///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ProjectPluginGeneratedPanelClass.h"

///////////////////////////////////////////////////////////////////////////

ProjectPluginGeneratedPanelClass::ProjectPluginGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableRow( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* StaticBoxSizerExplorer;
	StaticBoxSizerExplorer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("label") ), wxVERTICAL );
	
	wxGridBagSizer* GridBagSizer;
	GridBagSizer = new wxGridBagSizer( 0, 0 );
	GridBagSizer->AddGrowableCol( 1 );
	GridBagSizer->SetFlexibleDirection( wxBOTH );
	GridBagSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HelpText = new wxStaticText( this, wxID_ANY, _("MVC Editor will use this program whenever the \"Explore\" button is clicked."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( -1 );
	GridBagSizer->Add( HelpText, wxGBPosition( 0, 0 ), wxGBSpan( 1, 2 ), wxALL, 5 );
	
	Label = new wxStaticText( this, wxID_ANY, _("Explorer Executable"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	GridBagSizer->Add( Label, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	ExplorerExecutable = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	GridBagSizer->Add( ExplorerExecutable, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
	
	FilePicker = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN );
	GridBagSizer->Add( FilePicker, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALL, 5 );
	
	StaticBoxSizerExplorer->Add( GridBagSizer, 1, wxEXPAND, 5 );
	
	fgSizer2->Add( StaticBoxSizerExplorer, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* StaticBoxSizerFileTypes;
	StaticBoxSizerFileTypes = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Editor Associations") ), wxVERTICAL );
	
	wxFlexGridSizer* AssociationsGridSizer;
	AssociationsGridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	AssociationsGridSizer->AddGrowableCol( 0 );
	AssociationsGridSizer->AddGrowableRow( 0 );
	AssociationsGridSizer->AddGrowableRow( 1 );
	AssociationsGridSizer->SetFlexibleDirection( wxBOTH );
	AssociationsGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* AssociationsBoxSizer;
	AssociationsBoxSizer = new wxBoxSizer( wxVERTICAL );
	
	WildcardHelpText = new wxStaticText( this, wxID_ANY, _("You can associate to open settings control the type of editor that is  invoked when \neach file is open. The settings are any set of wildcard values. You can use \nwildcards to match files.  The allowed wildcards are:\n\n* : Will match on zero or more characters\n? : Will match on zero or one character\n; : Separator among multiple wildcards\n\nFor example, the value \"*.php;*.phtml\" will match files withe the extension .php or \nthe extension .phtml"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	WildcardHelpText->Wrap( -1 );
	AssociationsBoxSizer->Add( WildcardHelpText, 1, wxALL|wxEXPAND, 5 );
	
	AssociationsGridSizer->Add( AssociationsBoxSizer, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* AssociationsFormSizer;
	AssociationsFormSizer = new wxFlexGridSizer( 3, 2, 0, 0 );
	AssociationsFormSizer->AddGrowableCol( 1 );
	AssociationsFormSizer->SetFlexibleDirection( wxBOTH );
	AssociationsFormSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	PhpLabel = new wxStaticText( this, wxID_ANY, _("PHP && HTML Files"), wxDefaultPosition, wxDefaultSize, 0 );
	PhpLabel->Wrap( -1 );
	AssociationsFormSizer->Add( PhpLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	PhpFileFilters = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( PhpFileFilters, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	CssLabel = new wxStaticText( this, wxID_ANY, _("CSS Files"), wxDefaultPosition, wxDefaultSize, 0 );
	CssLabel->Wrap( -1 );
	AssociationsFormSizer->Add( CssLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	CssFileFilters = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( CssFileFilters, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	SqlLabel = new wxStaticText( this, wxID_ANY, _("SQL Files"), wxDefaultPosition, wxDefaultSize, 0 );
	SqlLabel->Wrap( -1 );
	AssociationsFormSizer->Add( SqlLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	SqlFileFilters = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( SqlFileFilters, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	AssociationsGridSizer->Add( AssociationsFormSizer, 3, wxEXPAND, 5 );
	
	StaticBoxSizerFileTypes->Add( AssociationsGridSizer, 1, wxEXPAND, 5 );
	
	fgSizer2->Add( StaticBoxSizerFileTypes, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( fgSizer2 );
	this->Layout();
	
	// Connect Events
	FilePicker->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( ProjectPluginGeneratedPanelClass::OnFileChanged ), NULL, this );
}

ProjectPluginGeneratedPanelClass::~ProjectPluginGeneratedPanelClass()
{
	// Disconnect Events
	FilePicker->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( ProjectPluginGeneratedPanelClass::OnFileChanged ), NULL, this );
	
}
