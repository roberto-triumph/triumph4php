///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "RunBrowserFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

ChooseUrlDialogGeneratedClass::ChooseUrlDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BodySizer;
	BodySizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 2 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* FilterSizer;
	FilterSizer = new wxStaticBoxSizer( new wxStaticBox( this, ID_FILTERSIZER, _("Filter URLs") ), wxVERTICAL );
	
	Filter = new wxTextCtrl( this, ID_FILTER, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	FilterSizer->Add( Filter, 1, wxALL|wxEXPAND, 5 );
	
	GridSizer->Add( FilterSizer, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* TopSizer;
	TopSizer = new wxFlexGridSizer( 1, 4, 0, 0 );
	TopSizer->AddGrowableCol( 0 );
	TopSizer->SetFlexibleDirection( wxBOTH );
	TopSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	OpenLabel = new wxStaticText( this, wxID_ANY, _("Choose URL to open in the browser"), wxDefaultPosition, wxDefaultSize, 0 );
	OpenLabel->Wrap( -1 );
	TopSizer->Add( OpenLabel, 1, wxALL|wxEXPAND, 5 );
	
	GridSizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	UrlList = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE ); 
	GridSizer->Add( UrlList, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* ExtraSizer;
	ExtraSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	ExtraSizer->AddGrowableCol( 0 );
	ExtraSizer->AddGrowableRow( 1 );
	ExtraSizer->SetFlexibleDirection( wxHORIZONTAL );
	ExtraSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	ExtraLabel = new wxStaticText( this, wxID_ANY, _("Extra"), wxDefaultPosition, wxDefaultSize, 0 );
	ExtraLabel->Wrap( -1 );
	ExtraSizer->Add( ExtraLabel, 1, wxALL|wxEXPAND, 5 );
	
	ExtraText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	ExtraSizer->Add( ExtraText, 0, wxALL|wxEXPAND, 5 );
	
	GridSizer->Add( ExtraSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* FinalSizer;
	FinalSizer = new wxBoxSizer( wxVERTICAL );
	
	FinalLabel = new wxStaticText( this, wxID_ANY, _("Final URL"), wxDefaultPosition, wxDefaultSize, 0 );
	FinalLabel->Wrap( -1 );
	FinalSizer->Add( FinalLabel, 0, wxALL, 5 );
	
	FinalUrlLabel = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	FinalUrlLabel->Wrap( -1 );
	FinalSizer->Add( FinalUrlLabel, 0, wxALL|wxEXPAND, 5 );
	
	GridSizer->Add( FinalSizer, 1, wxEXPAND, 5 );
	
	ButtonSizer = new wxStdDialogButtonSizer();
	ButtonSizerOK = new wxButton( this, wxID_OK );
	ButtonSizer->AddButton( ButtonSizerOK );
	ButtonSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonSizer->AddButton( ButtonSizerCancel );
	ButtonSizer->Realize();
	GridSizer->Add( ButtonSizer, 1, wxEXPAND, 5 );
	
	BodySizer->Add( GridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	Filter->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ChooseUrlDialogGeneratedClass::OnFilterKeyDown ), NULL, this );
	Filter->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnFilterText ), NULL, this );
	Filter->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnFilterTextEnter ), NULL, this );
	UrlList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnListItemSelected ), NULL, this );
	ExtraText->Connect( wxEVT_CHAR, wxKeyEventHandler( ChooseUrlDialogGeneratedClass::OnExtraChar ), NULL, this );
	ExtraText->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnExtraText ), NULL, this );
	ButtonSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnOkButton ), NULL, this );
}

ChooseUrlDialogGeneratedClass::~ChooseUrlDialogGeneratedClass()
{
	// Disconnect Events
	Filter->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( ChooseUrlDialogGeneratedClass::OnFilterKeyDown ), NULL, this );
	Filter->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnFilterText ), NULL, this );
	Filter->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnFilterTextEnter ), NULL, this );
	UrlList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnListItemSelected ), NULL, this );
	ExtraText->Disconnect( wxEVT_CHAR, wxKeyEventHandler( ChooseUrlDialogGeneratedClass::OnExtraChar ), NULL, this );
	ExtraText->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnExtraText ), NULL, this );
	ButtonSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnOkButton ), NULL, this );
	
}
