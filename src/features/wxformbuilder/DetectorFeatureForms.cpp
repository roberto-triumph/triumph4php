///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "DetectorFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

DetectorPanelGeneratedClass::DetectorPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
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
	
	Label = new wxStaticText( this, wxID_ANY, _("Detectors"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	LabelSizer->Add( Label, 1, wxALL|wxEXPAND, 5 );
	
	FlexGridSizer->Add( LabelSizer, 1, wxEXPAND, 5 );
	
	DetectorTree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS|wxTR_SINGLE );
	FlexGridSizer->Add( DetectorTree, 1, wxALL|wxALIGN_BOTTOM|wxEXPAND, 5 );
	
	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	TestButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DetectorPanelGeneratedClass::OnTestButton ), NULL, this );
	AddButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DetectorPanelGeneratedClass::OnAddButton ), NULL, this );
	HelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DetectorPanelGeneratedClass::OnHelpButton ), NULL, this );
	DetectorTree->Connect( wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler( DetectorPanelGeneratedClass::OnTreeItemDelete ), NULL, this );
	DetectorTree->Connect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( DetectorPanelGeneratedClass::OnTreeItemEndLabelEdit ), NULL, this );
	DetectorTree->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( DetectorPanelGeneratedClass::OnTreeItemActivated ), NULL, this );
	DetectorTree->Connect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( DetectorPanelGeneratedClass::OnTreeItemRightClick ), NULL, this );
}

DetectorPanelGeneratedClass::~DetectorPanelGeneratedClass()
{
	// Disconnect Events
	TestButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DetectorPanelGeneratedClass::OnTestButton ), NULL, this );
	AddButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DetectorPanelGeneratedClass::OnAddButton ), NULL, this );
	HelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DetectorPanelGeneratedClass::OnHelpButton ), NULL, this );
	DetectorTree->Disconnect( wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler( DetectorPanelGeneratedClass::OnTreeItemDelete ), NULL, this );
	DetectorTree->Disconnect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( DetectorPanelGeneratedClass::OnTreeItemEndLabelEdit ), NULL, this );
	DetectorTree->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( DetectorPanelGeneratedClass::OnTreeItemActivated ), NULL, this );
	DetectorTree->Disconnect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( DetectorPanelGeneratedClass::OnTreeItemRightClick ), NULL, this );
	
}
