///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "DetectorFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

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
	
	DetectorTree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS|wxTR_SINGLE );
	FlexGridSizer->Add( DetectorTree, 1, wxALL|wxALIGN_BOTTOM|wxEXPAND, 5 );
	
	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	HelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UrlDetectorPanelGeneratedClass::OnHelpButton ), NULL, this );
}

UrlDetectorPanelGeneratedClass::~UrlDetectorPanelGeneratedClass()
{
	// Disconnect Events
	HelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UrlDetectorPanelGeneratedClass::OnHelpButton ), NULL, this );
	
}

TemplateFilesDetectorPanelGeneratedClass::TemplateFilesDetectorPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 4, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 3 );
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
	
	wxBoxSizer* UrlSizer;
	UrlSizer = new wxBoxSizer( wxHORIZONTAL );
	
	UrlToTestLabel = new wxStaticText( this, wxID_ANY, _("URL To Test"), wxDefaultPosition, wxDefaultSize, 0 );
	UrlToTestLabel->Wrap( -1 );
	UrlSizer->Add( UrlToTestLabel, 0, wxALL, 5 );
	
	UrlToTest = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	UrlSizer->Add( UrlToTest, 1, wxALL, 5 );
	
	ChooseUrlButton = new wxButton( this, wxID_ANY, _("Choose URL"), wxDefaultPosition, wxDefaultSize, 0 );
	UrlSizer->Add( ChooseUrlButton, 0, wxALL, 5 );
	
	FlexGridSizer->Add( UrlSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* LabelSizer;
	LabelSizer = new wxBoxSizer( wxHORIZONTAL );
	
	Label = new wxStaticText( this, wxID_ANY, _("Template Files Detectors"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	LabelSizer->Add( Label, 1, wxALL|wxEXPAND, 5 );
	
	FlexGridSizer->Add( LabelSizer, 1, wxEXPAND, 5 );
	
	DetectorTree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS|wxTR_SINGLE );
	FlexGridSizer->Add( DetectorTree, 1, wxALL|wxALIGN_BOTTOM|wxEXPAND, 5 );
	
	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	AddButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateFilesDetectorPanelGeneratedClass::OnAddButton ), NULL, this );
	ChooseUrlButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateFilesDetectorPanelGeneratedClass::OnChooseUrlButton ), NULL, this );
}

TemplateFilesDetectorPanelGeneratedClass::~TemplateFilesDetectorPanelGeneratedClass()
{
	// Disconnect Events
	AddButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateFilesDetectorPanelGeneratedClass::OnAddButton ), NULL, this );
	ChooseUrlButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateFilesDetectorPanelGeneratedClass::OnChooseUrlButton ), NULL, this );
	
}

TagDetectorPanelGeneratedClass::TagDetectorPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
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
	
	Label = new wxStaticText( this, wxID_ANY, _("Tag Detectors"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	LabelSizer->Add( Label, 1, wxALL|wxEXPAND, 5 );
	
	FlexGridSizer->Add( LabelSizer, 1, wxEXPAND, 5 );
	
	DetectorTree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS|wxTR_SINGLE );
	FlexGridSizer->Add( DetectorTree, 1, wxALL|wxALIGN_BOTTOM|wxEXPAND, 5 );
	
	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	HelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDetectorPanelGeneratedClass::OnHelpButton ), NULL, this );
}

TagDetectorPanelGeneratedClass::~TagDetectorPanelGeneratedClass()
{
	// Disconnect Events
	HelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDetectorPanelGeneratedClass::OnHelpButton ), NULL, this );
	
}
