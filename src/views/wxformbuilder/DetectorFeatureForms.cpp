///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
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
	TopSizer->Add( ProjectLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString ProjectChoiceChoices;
	ProjectChoice = new wxChoice( this, ID_PROJECT_CHOICE, wxDefaultPosition, wxDefaultSize, ProjectChoiceChoices, 0 );
	ProjectChoice->SetSelection( 0 );
	TopSizer->Add( ProjectChoice, 1, wxALL|wxEXPAND, 5 );

	TestButton = new wxButton( this, ID_TEST_DETECTOR_BUTTON, _("Test"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	TopSizer->Add( TestButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	AddButton = new wxButton( this, ID_ADD_BUTTON, _("Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	TopSizer->Add( AddButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	TopSizer->Add( HelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

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
	TopSizer->Add( ProjectLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString ProjectChoiceChoices;
	ProjectChoice = new wxChoice( this, ID_PROJECT_CHOICE, wxDefaultPosition, wxDefaultSize, ProjectChoiceChoices, 0 );
	ProjectChoice->SetSelection( 0 );
	TopSizer->Add( ProjectChoice, 1, wxALL|wxEXPAND, 5 );

	TestButton = new wxButton( this, ID_TEST_DETECTOR_BUTTON, _("Test"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	TopSizer->Add( TestButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	AddButton = new wxButton( this, ID_ADD_BUTTON, _("Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	TopSizer->Add( AddButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	TopSizer->Add( HelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	FlexGridSizer->Add( TopSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* UrlSizer;
	UrlSizer = new wxBoxSizer( wxHORIZONTAL );

	UrlLabel = new wxStaticText( this, wxID_ANY, _("Choose URL"), wxDefaultPosition, wxDefaultSize, 0 );
	UrlLabel->Wrap( -1 );
	UrlSizer->Add( UrlLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	UrlToTest = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	UrlSizer->Add( UrlToTest, 1, wxALL|wxEXPAND, 5 );

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
	TestButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateFilesDetectorPanelGeneratedClass::OnTestButton ), NULL, this );
	AddButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateFilesDetectorPanelGeneratedClass::OnAddButton ), NULL, this );
	ChooseUrlButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateFilesDetectorPanelGeneratedClass::OnChooseUrlButton ), NULL, this );
}

TemplateFilesDetectorPanelGeneratedClass::~TemplateFilesDetectorPanelGeneratedClass()
{
	// Disconnect Events
	TestButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateFilesDetectorPanelGeneratedClass::OnTestButton ), NULL, this );
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
	TopSizer->Add( ProjectLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString ProjectChoiceChoices;
	ProjectChoice = new wxChoice( this, ID_PROJECT_CHOICE, wxDefaultPosition, wxDefaultSize, ProjectChoiceChoices, 0 );
	ProjectChoice->SetSelection( 0 );
	TopSizer->Add( ProjectChoice, 1, wxALL|wxEXPAND, 5 );

	TestButton = new wxButton( this, ID_TEST_DETECTOR_BUTTON, _("Test"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	TopSizer->Add( TestButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	AddButton = new wxButton( this, ID_ADD_BUTTON, _("Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	TopSizer->Add( AddButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxPoint( -1,-1 ), wxSize( -1,-1 ), wxBU_AUTODRAW );
	TopSizer->Add( HelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

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

DatabaseDetectorPanelGeneratedClass::DatabaseDetectorPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
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
	TopSizer->Add( ProjectLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString ProjectChoiceChoices;
	ProjectChoice = new wxChoice( this, ID_PROJECT_CHOICE, wxDefaultPosition, wxDefaultSize, ProjectChoiceChoices, 0 );
	ProjectChoice->SetSelection( 0 );
	TopSizer->Add( ProjectChoice, 1, wxALL|wxEXPAND, 5 );

	TestButton = new wxButton( this, ID_TEST_DETECTOR_BUTTON, _("Test"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	TopSizer->Add( TestButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	AddButton = new wxButton( this, ID_ADD_BUTTON, _("Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	TopSizer->Add( AddButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	TopSizer->Add( HelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	FlexGridSizer->Add( TopSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* LabelSizer;
	LabelSizer = new wxBoxSizer( wxHORIZONTAL );

	Label = new wxStaticText( this, wxID_ANY, _("Database Detectors"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	LabelSizer->Add( Label, 1, wxALL|wxEXPAND, 5 );

	FlexGridSizer->Add( LabelSizer, 1, wxEXPAND, 5 );

	DetectorTree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS|wxTR_SINGLE );
	FlexGridSizer->Add( DetectorTree, 1, wxALL|wxALIGN_BOTTOM|wxEXPAND, 5 );

	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );

	this->SetSizer( BoxSizer );
	this->Layout();

	// Connect Events
	HelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DatabaseDetectorPanelGeneratedClass::OnHelpButton ), NULL, this );
}

DatabaseDetectorPanelGeneratedClass::~DatabaseDetectorPanelGeneratedClass()
{
	// Disconnect Events
	HelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DatabaseDetectorPanelGeneratedClass::OnHelpButton ), NULL, this );

}

ConfigDetectorPanelGeneratedClass::ConfigDetectorPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
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
	TopSizer->Add( ProjectLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString ProjectChoiceChoices;
	ProjectChoice = new wxChoice( this, ID_PROJECT_CHOICE, wxDefaultPosition, wxDefaultSize, ProjectChoiceChoices, 0 );
	ProjectChoice->SetSelection( 0 );
	TopSizer->Add( ProjectChoice, 1, wxALL|wxEXPAND, 5 );

	TestButton = new wxButton( this, ID_TEST_DETECTOR_BUTTON, _("Test"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	TopSizer->Add( TestButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	AddButton = new wxButton( this, ID_ADD_BUTTON, _("Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	TopSizer->Add( AddButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	TopSizer->Add( HelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	FlexGridSizer->Add( TopSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* LabelSizer;
	LabelSizer = new wxBoxSizer( wxHORIZONTAL );

	Label = new wxStaticText( this, wxID_ANY, _("Config Detectors"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	LabelSizer->Add( Label, 1, wxALL|wxEXPAND, 5 );

	FlexGridSizer->Add( LabelSizer, 1, wxEXPAND, 5 );

	DetectorTree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS|wxTR_SINGLE );
	FlexGridSizer->Add( DetectorTree, 1, wxALL|wxALIGN_BOTTOM|wxEXPAND, 5 );

	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );

	this->SetSizer( BoxSizer );
	this->Layout();

	// Connect Events
	HelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConfigDetectorPanelGeneratedClass::OnHelpButton ), NULL, this );
}

ConfigDetectorPanelGeneratedClass::~ConfigDetectorPanelGeneratedClass()
{
	// Disconnect Events
	HelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConfigDetectorPanelGeneratedClass::OnHelpButton ), NULL, this );

}

UrlDetectorHelpDialogGeneratedClass::UrlDetectorHelpDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* Sizer;
	Sizer = new wxBoxSizer( wxVERTICAL );

	HelpText = new wxStaticText( this, wxID_ANY, _("URL Detectors are PHP scripts that Triumph uses to find out\nall of the valid URL routes for your projects. \n\nTriumph can detect routes for CodeIgniter projects."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( 450 );
	Sizer->Add( HelpText, 0, wxALL, 5 );

	HelpLink = new wxHyperlinkCtrl( this, wxID_ANY, _("More about URL Detection in Triumph 4 PHP"), wxT("http://docs.triumph4php.com/url-detectors/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	Sizer->Add( HelpLink, 0, wxALL, 5 );

	ButtonSizer = new wxStdDialogButtonSizer();
	ButtonSizerOK = new wxButton( this, wxID_OK );
	ButtonSizer->AddButton( ButtonSizerOK );
	ButtonSizer->Realize();
	Sizer->Add( ButtonSizer, 1, wxEXPAND, 5 );

	this->SetSizer( Sizer );
	this->Layout();
	Sizer->Fit( this );

	this->Centre( wxBOTH );
}

UrlDetectorHelpDialogGeneratedClass::~UrlDetectorHelpDialogGeneratedClass()
{
}

TemplateFilesDetectorHelpGeneratedClass::TemplateFilesDetectorHelpGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* Sizer;
	Sizer = new wxBoxSizer( wxVERTICAL );

	HelpText = new wxStaticText( this, wxID_ANY, _("Template files detectors are PHP scripts that Triumph uses to find out all of the 'view' files for your projects.\n\nTriumph can detect view files for CodeIgniter projects."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( 450 );
	Sizer->Add( HelpText, 0, wxALL, 5 );

	HelpLink = new wxHyperlinkCtrl( this, wxID_ANY, _("More about Template File Detection in Triumph 4 PHP"), wxT("http://docs.triumph4php.com/template-file-detectors/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	Sizer->Add( HelpLink, 0, wxALL, 5 );

	ButtonSizer = new wxStdDialogButtonSizer();
	ButtonSizerOK = new wxButton( this, wxID_OK );
	ButtonSizer->AddButton( ButtonSizerOK );
	ButtonSizer->Realize();
	Sizer->Add( ButtonSizer, 1, wxEXPAND, 5 );

	this->SetSizer( Sizer );
	this->Layout();
	Sizer->Fit( this );

	this->Centre( wxBOTH );
}

TemplateFilesDetectorHelpGeneratedClass::~TemplateFilesDetectorHelpGeneratedClass()
{
}

ConfigDetectorsHelpDialogGeneratedClass::ConfigDetectorsHelpDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* Sizer;
	Sizer = new wxBoxSizer( wxVERTICAL );

	HelpText = new wxStaticText( this, wxID_ANY, _("Config Detectors are PHP scripts that Triumph uses to find out any config files that PHP frameworks use. \n\nTriumph can detect config files for CodeIgniter projects."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( 450 );
	Sizer->Add( HelpText, 0, wxALL, 5 );

	HelpLink = new wxHyperlinkCtrl( this, wxID_ANY, _("More about Config Detection in Triumph 4 PHP"), wxT("http://docs.triumph4php.com/config-detectors/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	Sizer->Add( HelpLink, 0, wxALL, 5 );

	ButtonSizer = new wxStdDialogButtonSizer();
	ButtonSizerOK = new wxButton( this, wxID_OK );
	ButtonSizer->AddButton( ButtonSizerOK );
	ButtonSizer->Realize();
	Sizer->Add( ButtonSizer, 1, wxEXPAND, 5 );

	this->SetSizer( Sizer );
	this->Layout();
	Sizer->Fit( this );

	this->Centre( wxBOTH );
}

ConfigDetectorsHelpDialogGeneratedClass::~ConfigDetectorsHelpDialogGeneratedClass()
{
}

DatabaseDetectorHelpDialogGenereatedClass::DatabaseDetectorHelpDialogGenereatedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* Sizer;
	Sizer = new wxBoxSizer( wxVERTICAL );

	HelpText = new wxStaticText( this, wxID_ANY, _("Database Detectors are PHP scripts that Triumph uses to find out any database connections that PHP frameworks dynamically create.\n\nTriumph can detect database connections for CodeIgniter projects."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( 450 );
	Sizer->Add( HelpText, 0, wxALL, 5 );

	HelpLink = new wxHyperlinkCtrl( this, wxID_ANY, _("More about Database Detection in Triumph 4 PHP"), wxT("http://docs.triumph4php.com/database-detectors/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	Sizer->Add( HelpLink, 0, wxALL, 5 );

	ButtonSizer = new wxStdDialogButtonSizer();
	ButtonSizerOK = new wxButton( this, wxID_OK );
	ButtonSizer->AddButton( ButtonSizerOK );
	ButtonSizer->Realize();
	Sizer->Add( ButtonSizer, 1, wxEXPAND, 5 );

	this->SetSizer( Sizer );
	this->Layout();
	Sizer->Fit( this );

	this->Centre( wxBOTH );
}

DatabaseDetectorHelpDialogGenereatedClass::~DatabaseDetectorHelpDialogGenereatedClass()
{
}
