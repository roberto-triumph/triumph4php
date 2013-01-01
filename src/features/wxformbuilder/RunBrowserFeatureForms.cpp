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
	FlexGridSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 2 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	ProjectLabel = new wxStaticText( this, wxID_ANY, _("Project To Test"), wxDefaultPosition, wxDefaultSize, 0 );
	ProjectLabel->Wrap( -1 );
	TopSizer->Add( ProjectLabel, 0, wxALL|wxEXPAND, 5 );
	
	wxArrayString ProjectChoiceChoices;
	ProjectChoice = new wxChoice( this, ID_PROJECT_CHOICE, wxDefaultPosition, wxDefaultSize, ProjectChoiceChoices, 0 );
	ProjectChoice->SetSelection( 0 );
	TopSizer->Add( ProjectChoice, 1, wxALL|wxEXPAND, 5 );
	
	TestButton = new wxButton( this, ID_TEST_DETECTOR_BUTTON, _("Test"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	TopSizer->Add( TestButton, 0, wxALL, 5 );
	
	AddButton = new wxButton( this, ID_ADD_BUTTON, _("Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	TopSizer->Add( AddButton, 0, wxALL, 5 );
	
	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	TopSizer->Add( HelpButton, 0, wxALL, 5 );
	
	FlexGridSizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* LabelSizer;
	LabelSizer = new wxBoxSizer( wxHORIZONTAL );
	
	Label = new wxStaticText( this, wxID_ANY, _("URL Detectors"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	LabelSizer->Add( Label, 1, wxALL|wxEXPAND, 5 );
	
	FlexGridSizer->Add( LabelSizer, 1, wxEXPAND, 5 );
	
	UrlDetectorTree = new wxTreeCtrl( this, ID_DETECTOR_TREE, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS|wxTR_SINGLE );
	FlexGridSizer->Add( UrlDetectorTree, 1, wxALL|wxALIGN_BOTTOM|wxEXPAND, 5 );
	
	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	TestButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UrlDetectorPanelGeneratedClass::OnTestButton ), NULL, this );
	AddButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UrlDetectorPanelGeneratedClass::OnAddButton ), NULL, this );
	HelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UrlDetectorPanelGeneratedClass::OnHelpButton ), NULL, this );
	UrlDetectorTree->Connect( wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler( UrlDetectorPanelGeneratedClass::OnTreeItemDelete ), NULL, this );
	UrlDetectorTree->Connect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( UrlDetectorPanelGeneratedClass::OnTreeItemEndLabelEdit ), NULL, this );
	UrlDetectorTree->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( UrlDetectorPanelGeneratedClass::OnTreeItemActivated ), NULL, this );
	UrlDetectorTree->Connect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( UrlDetectorPanelGeneratedClass::OnTreeItemRightClick ), NULL, this );
}

UrlDetectorPanelGeneratedClass::~UrlDetectorPanelGeneratedClass()
{
	// Disconnect Events
	TestButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UrlDetectorPanelGeneratedClass::OnTestButton ), NULL, this );
	AddButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UrlDetectorPanelGeneratedClass::OnAddButton ), NULL, this );
	HelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UrlDetectorPanelGeneratedClass::OnHelpButton ), NULL, this );
	UrlDetectorTree->Disconnect( wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler( UrlDetectorPanelGeneratedClass::OnTreeItemDelete ), NULL, this );
	UrlDetectorTree->Disconnect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( UrlDetectorPanelGeneratedClass::OnTreeItemEndLabelEdit ), NULL, this );
	UrlDetectorTree->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( UrlDetectorPanelGeneratedClass::OnTreeItemActivated ), NULL, this );
	UrlDetectorTree->Disconnect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( UrlDetectorPanelGeneratedClass::OnTreeItemRightClick ), NULL, this );
	
}
