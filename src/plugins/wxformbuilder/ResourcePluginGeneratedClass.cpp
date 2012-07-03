///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ResourcePluginGeneratedClass.h"

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
	
	SearchText = new wxTextCtrl( this, ID_SEARCHTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( SearchText, 1, wxALL|wxEXPAND, 5 );
	
	ResultsLabel = new wxStaticText( this, ID_RESULTSLABEL, _("Status: OK"), wxDefaultPosition, wxDefaultSize, 0 );
	ResultsLabel->Wrap( -1 );
	FlexGridSizer->Add( ResultsLabel, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* ChecklistSizer;
	ChecklistSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxArrayString ResultsChoices;
	Results = new wxCheckListBox( this, ID_RESULTS, wxDefaultPosition, wxDefaultSize, ResultsChoices, 0 );
	ChecklistSizer->Add( Results, 1, wxALL|wxEXPAND, 5 );
	
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
	Results->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnResultsDoubleClick ), NULL, this );
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
	Results->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnResultsDoubleClick ), NULL, this );
	ButtonsSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnHelpButton ), NULL, this );
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ResourceSearchDialogGeneratedClass::OnOkButton ), NULL, this );
	
}
