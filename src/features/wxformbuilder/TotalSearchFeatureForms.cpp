///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "TotalSearchFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

TotalSearchDialogGeneratedClass::TotalSearchDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 6, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 4 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	SearchLabel = new wxStaticText( this, ID_SEARCHLABEL, _("Search String"), wxDefaultPosition, wxDefaultSize, 0 );
	SearchLabel->Wrap( -1 );
	FlexGridSizer->Add( SearchLabel, 0, wxALL|wxEXPAND, 5 );
	
	SearchText = new wxTextCtrl( this, ID_SEARCHTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	FlexGridSizer->Add( SearchText, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Project"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer4->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString ProjectChoiceChoices;
	ProjectChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, ProjectChoiceChoices, 0 );
	ProjectChoice->SetSelection( 0 );
	bSizer4->Add( ProjectChoice, 1, wxALL|wxEXPAND, 5 );
	
	FlexGridSizer->Add( bSizer4, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	MatchesLabel = new wxStaticText( this, wxID_ANY, _("Matches: None"), wxDefaultPosition, wxDefaultSize, 0 );
	MatchesLabel->Wrap( -1 );
	bSizer5->Add( MatchesLabel, 1, wxALL, 5 );
	
	CacheStatusLabel = new wxStaticText( this, wxID_ANY, _("Cache Status: OK       "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	CacheStatusLabel->Wrap( -1 );
	bSizer5->Add( CacheStatusLabel, 1, wxALL|wxEXPAND, 5 );
	
	FlexGridSizer->Add( bSizer5, 1, wxEXPAND, 5 );
	
	wxBoxSizer* ChecklistSizer;
	ChecklistSizer = new wxBoxSizer( wxHORIZONTAL );
	
	MatchesList = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	ChecklistSizer->Add( MatchesList, 1, wxALL|wxEXPAND, 5 );
	
	FlexGridSizer->Add( ChecklistSizer, 1, wxEXPAND, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonsSizer->AddButton( ButtonsSizerCancel );
	ButtonsSizerHelp = new wxButton( this, wxID_HELP );
	ButtonsSizer->AddButton( ButtonsSizerHelp );
	ButtonsSizer->Realize();
	FlexGridSizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	SearchText->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( TotalSearchDialogGeneratedClass::OnSearchKeyDown ), NULL, this );
	SearchText->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnSearchEnter ), NULL, this );
	ProjectChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnProjectChoice ), NULL, this );
	MatchesList->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( TotalSearchDialogGeneratedClass::OnMatchesListKeyDown ), NULL, this );
	MatchesList->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( TotalSearchDialogGeneratedClass::OnMatchesListDoubleClick ), NULL, this );
	ButtonsSizerCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnHelpButton ), NULL, this );
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnOkButton ), NULL, this );
}

TotalSearchDialogGeneratedClass::~TotalSearchDialogGeneratedClass()
{
	// Disconnect Events
	SearchText->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( TotalSearchDialogGeneratedClass::OnSearchKeyDown ), NULL, this );
	SearchText->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnSearchEnter ), NULL, this );
	ProjectChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnProjectChoice ), NULL, this );
	MatchesList->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( TotalSearchDialogGeneratedClass::OnMatchesListKeyDown ), NULL, this );
	MatchesList->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( TotalSearchDialogGeneratedClass::OnMatchesListDoubleClick ), NULL, this );
	ButtonsSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnHelpButton ), NULL, this );
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnOkButton ), NULL, this );
	
}
