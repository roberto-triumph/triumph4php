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
	Filter->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ChooseUrlDialogGeneratedClass::OnKeyDown ), NULL, this );
	Filter->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnText ), NULL, this );
	Filter->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnTextEnter ), NULL, this );
	UrlList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnListItemSelected ), NULL, this );
	ButtonSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnOkButton ), NULL, this );
}

ChooseUrlDialogGeneratedClass::~ChooseUrlDialogGeneratedClass()
{
	// Disconnect Events
	Filter->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( ChooseUrlDialogGeneratedClass::OnKeyDown ), NULL, this );
	Filter->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnText ), NULL, this );
	Filter->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnTextEnter ), NULL, this );
	UrlList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnListItemSelected ), NULL, this );
	ButtonSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChooseUrlDialogGeneratedClass::OnOkButton ), NULL, this );
	
}

UrlDetectorPanelGeneratedClass::UrlDetectorPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 1 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	Label = new wxStaticText( this, wxID_ANY, _("URL Detectors"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	FlexGridSizer->Add( Label, 0, wxALL, 5 );
	
	UrlDetectorTree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS );
	FlexGridSizer->Add( UrlDetectorTree, 1, wxALL|wxALIGN_BOTTOM|wxEXPAND, 5 );
	
	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	UrlDetectorTree->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( UrlDetectorPanelGeneratedClass::OnTreeItemActivated ), NULL, this );
}

UrlDetectorPanelGeneratedClass::~UrlDetectorPanelGeneratedClass()
{
	// Disconnect Events
	UrlDetectorTree->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( UrlDetectorPanelGeneratedClass::OnTreeItemActivated ), NULL, this );
	
}
