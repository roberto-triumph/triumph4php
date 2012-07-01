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

ProjectDefinitionDialogGeneratedClass::ProjectDefinitionDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 6, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 3 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	LabelStatic = new wxStaticText( this, wxID_ANY, _("Project Label"), wxDefaultPosition, wxDefaultSize, 0 );
	LabelStatic->Wrap( -1 );
	FlexGridSizer->Add( LabelStatic, 0, wxALL|wxEXPAND, 5 );
	
	Label = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( Label, 0, wxALL|wxEXPAND, 5 );
	
	SourcesLabel = new wxStaticText( this, wxID_ANY, _("Project Sources"), wxDefaultPosition, wxDefaultSize, 0 );
	SourcesLabel->Wrap( -1 );
	FlexGridSizer->Add( SourcesLabel, 1, wxALL|wxEXPAND, 5 );
	
	SourcesList = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE ); 
	FlexGridSizer->Add( SourcesList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* CrudButtonsSizer;
	CrudButtonsSizer = new wxBoxSizer( wxHORIZONTAL );
	
	AddButton = new wxButton( this, wxID_ANY, _("Add Source"), wxDefaultPosition, wxDefaultSize, 0 );
	CrudButtonsSizer->Add( AddButton, 0, wxALL, 5 );
	
	RemoveButton = new wxButton( this, wxID_ANY, _("Remove Source"), wxDefaultPosition, wxDefaultSize, 0 );
	CrudButtonsSizer->Add( RemoveButton, 0, wxALL, 5 );
	
	EditButton = new wxButton( this, wxID_ANY, _("Edit Source"), wxDefaultPosition, wxDefaultSize, 0 );
	CrudButtonsSizer->Add( EditButton, 0, wxALL, 5 );
	
	FlexGridSizer->Add( CrudButtonsSizer, 1, wxEXPAND, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonsSizer->AddButton( ButtonsSizerCancel );
	ButtonsSizer->Realize();
	FlexGridSizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	SourcesList->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ProjectDefinitionDialogGeneratedClass::OnSourcesListDoubleClick ), NULL, this );
	AddButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectDefinitionDialogGeneratedClass::OnAddSource ), NULL, this );
	RemoveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectDefinitionDialogGeneratedClass::OnRemoveSource ), NULL, this );
	EditButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectDefinitionDialogGeneratedClass::OnEditSource ), NULL, this );
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectDefinitionDialogGeneratedClass::OnOkButton ), NULL, this );
}

ProjectDefinitionDialogGeneratedClass::~ProjectDefinitionDialogGeneratedClass()
{
	// Disconnect Events
	SourcesList->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ProjectDefinitionDialogGeneratedClass::OnSourcesListDoubleClick ), NULL, this );
	AddButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectDefinitionDialogGeneratedClass::OnAddSource ), NULL, this );
	RemoveButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectDefinitionDialogGeneratedClass::OnRemoveSource ), NULL, this );
	EditButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectDefinitionDialogGeneratedClass::OnEditSource ), NULL, this );
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectDefinitionDialogGeneratedClass::OnOkButton ), NULL, this );
	
}

ProjectSourceDialogGeneratedClass::ProjectSourceDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 6, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 0 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HelpLabel = new wxStaticText( this, wxID_ANY, _("Enter a list of files that you want MVC Editor to track and ignore. \n\nInclude and exclude wildcards can contain one or more wildcards;  any files that match do NOT match the exclude wildcards and DO match the include wildcards will be added to the project. Exclude wildcards take precedence over include wildcards.\n\nExlcude wildcards are useful to ignore cache files, or for example Symfony skeleton files.\n\nA wildcard can have either a '*' or a '?'.\n\n* = any number of characters\n? = 0 or 1 character\n\nMultiple wildcards are separated by the semicolon (';')\n\nA wildcard can have directory separators; but they must match the operation system's path separator."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpLabel->Wrap( 500 );
	FlexGridSizer->Add( HelpLabel, 0, wxALL, 5 );
	
	RootDirectoryLabel = new wxStaticText( this, wxID_ANY, _("Root Directory"), wxDefaultPosition, wxDefaultSize, 0 );
	RootDirectoryLabel->Wrap( -1 );
	FlexGridSizer->Add( RootDirectoryLabel, 0, wxALL|wxEXPAND, 5 );
	
	RootDirectory = new wxDirPickerCtrl( this, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DIR_MUST_EXIST|wxDIRP_USE_TEXTCTRL );
	FlexGridSizer->Add( RootDirectory, 0, wxALL|wxEXPAND, 5 );
	
	IncludeWildcardsLabel = new wxStaticText( this, wxID_ANY, _("Include Wildcards"), wxDefaultPosition, wxDefaultSize, 0 );
	IncludeWildcardsLabel->Wrap( -1 );
	FlexGridSizer->Add( IncludeWildcardsLabel, 0, wxALL|wxEXPAND, 5 );
	
	IncludeWildcards = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( IncludeWildcards, 0, wxALL|wxEXPAND, 5 );
	
	ExcludeWildcardsLabel = new wxStaticText( this, wxID_ANY, _("Exclude Wildcards"), wxDefaultPosition, wxDefaultSize, 0 );
	ExcludeWildcardsLabel->Wrap( -1 );
	FlexGridSizer->Add( ExcludeWildcardsLabel, 0, wxALL|wxEXPAND, 5 );
	
	ExcludeWildcards = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( ExcludeWildcards, 0, wxALL|wxEXPAND, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonsSizer->AddButton( ButtonsSizerCancel );
	ButtonsSizer->Realize();
	FlexGridSizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSourceDialogGeneratedClass::OnOkButton ), NULL, this );
}

ProjectSourceDialogGeneratedClass::~ProjectSourceDialogGeneratedClass()
{
	// Disconnect Events
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSourceDialogGeneratedClass::OnOkButton ), NULL, this );
	
}

ProjectListDialogGeneratedClass::ProjectListDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 4, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 1 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	ProjectsLabel = new wxStaticText( this, wxID_ANY, _("Defined Projects"), wxDefaultPosition, wxDefaultSize, 0 );
	ProjectsLabel->Wrap( -1 );
	FlexGridSizer->Add( ProjectsLabel, 0, wxALL|wxEXPAND, 5 );
	
	wxArrayString ProjectsListChoices;
	ProjectsList = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, ProjectsListChoices, 0 );
	FlexGridSizer->Add( ProjectsList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* CrudButtonsSizer;
	CrudButtonsSizer = new wxBoxSizer( wxHORIZONTAL );
	
	AddButton = new wxButton( this, wxID_ANY, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	CrudButtonsSizer->Add( AddButton, 0, wxALL, 5 );
	
	RemoveButton = new wxButton( this, wxID_ANY, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	CrudButtonsSizer->Add( RemoveButton, 0, wxALL, 5 );
	
	EditButton = new wxButton( this, wxID_ANY, _("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
	CrudButtonsSizer->Add( EditButton, 0, wxALL, 5 );
	
	FlexGridSizer->Add( CrudButtonsSizer, 1, wxEXPAND, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonsSizer->AddButton( ButtonsSizerCancel );
	ButtonsSizer->Realize();
	FlexGridSizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	ProjectsList->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnProjectsListDoubleClick ), NULL, this );
	ProjectsList->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnProjectsListCheckbox ), NULL, this );
	AddButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnAddButton ), NULL, this );
	RemoveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnRemoveButton ), NULL, this );
	EditButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnEditButton ), NULL, this );
	ButtonsSizerCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnOkButton ), NULL, this );
}

ProjectListDialogGeneratedClass::~ProjectListDialogGeneratedClass()
{
	// Disconnect Events
	ProjectsList->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnProjectsListDoubleClick ), NULL, this );
	ProjectsList->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnProjectsListCheckbox ), NULL, this );
	AddButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnAddButton ), NULL, this );
	RemoveButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnRemoveButton ), NULL, this );
	EditButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnEditButton ), NULL, this );
	ButtonsSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnOkButton ), NULL, this );
	
}
