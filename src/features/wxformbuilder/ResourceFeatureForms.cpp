///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ResourceFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

IndexingDialogGeneratedClass::IndexingDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BodySizer;
	BodySizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexSizer;
	FlexSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexSizer->AddGrowableCol( 0 );
	FlexSizer->AddGrowableRow( 0 );
	FlexSizer->SetFlexibleDirection( wxBOTH );
	FlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	Gauge = new wxGauge( this, ID_GAUGE, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	FlexSizer->Add( Gauge, 1, wxALL|wxEXPAND, 5 );
	
	HideButton = new wxButton( this, ID_HIDEBUTTON, _("Hide"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexSizer->Add( HideButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	BodySizer->Add( FlexSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	HideButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IndexingDialogGeneratedClass::OnHideButton ), NULL, this );
}

IndexingDialogGeneratedClass::~IndexingDialogGeneratedClass()
{
	// Disconnect Events
	HideButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IndexingDialogGeneratedClass::OnHideButton ), NULL, this );
	
}

ResourceSearchDialogGeneratedClass::ResourceSearchDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 5, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 3 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	SearchLabel = new wxStaticText( this, ID_SEARCHLABEL, _("Search String"), wxDefaultPosition, wxDefaultSize, 0 );
	SearchLabel->Wrap( -1 );
	FlexGridSizer->Add( SearchLabel, 0, wxALL|wxEXPAND, 5 );
	
	SearchText = new wxTextCtrl( this, ID_SEARCHTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	FlexGridSizer->Add( SearchText, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	MatchesLabel = new wxStaticText( this, wxID_ANY, _("Matches: None"), wxDefaultPosition, wxDefaultSize, 0 );
	MatchesLabel->Wrap( -1 );
	bSizer4->Add( MatchesLabel, 1, wxALL|wxEXPAND, 5 );
	
	CacheStatusLabel = new wxStaticText( this, wxID_ANY, _("Cache Status: OK       "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	CacheStatusLabel->Wrap( -1 );
	bSizer4->Add( CacheStatusLabel, 1, wxALL, 5 );
	
	FlexGridSizer->Add( bSizer4, 1, wxEXPAND, 5 );
	
	wxBoxSizer* ChecklistSizer;
	ChecklistSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxArrayString MatchesListChoices;
	MatchesList = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, MatchesListChoices, 0 );
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
	SearchText->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ResourceSearchDialogGeneratedClass::OnSearchKeyDown ), NULL, this );
	SearchText->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnSearchText ), NULL, this );
	SearchText->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnSearchEnter ), NULL, this );
	MatchesList->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnMatchesListDoubleClick ), NULL, this );
	MatchesList->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ResourceSearchDialogGeneratedClass::OnMatchesListKeyDown ), NULL, this );
	ButtonsSizerCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnHelpButton ), NULL, this );
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnOkButton ), NULL, this );
}

ResourceSearchDialogGeneratedClass::~ResourceSearchDialogGeneratedClass()
{
	// Disconnect Events
	SearchText->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( ResourceSearchDialogGeneratedClass::OnSearchKeyDown ), NULL, this );
	SearchText->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnSearchText ), NULL, this );
	SearchText->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnSearchEnter ), NULL, this );
	MatchesList->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnMatchesListDoubleClick ), NULL, this );
	MatchesList->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( ResourceSearchDialogGeneratedClass::OnMatchesListKeyDown ), NULL, this );
	ButtonsSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnHelpButton ), NULL, this );
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnOkButton ), NULL, this );
	
}
