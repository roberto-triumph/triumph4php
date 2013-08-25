///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ProjectFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

ProjectPreferencesGeneratedPanelClass::ProjectPreferencesGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableRow( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* StaticBoxSizerExplorer;
	StaticBoxSizerExplorer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("label") ), wxVERTICAL );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 0 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* LabelSizer;
	LabelSizer = new wxBoxSizer( wxVERTICAL );
	
	HelpText = new wxStaticText( this, wxID_ANY, _("MVC Editor will use this program whenever the \"Explore\" button is clicked."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( -1 );
	LabelSizer->Add( HelpText, 0, wxALL, 5 );
	
	GridSizer->Add( LabelSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* FileSizer;
	FileSizer = new wxBoxSizer( wxHORIZONTAL );
	
	Label = new wxStaticText( this, wxID_ANY, _("Explorer Executable"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	FileSizer->Add( Label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	ExplorerExecutable = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	FileSizer->Add( ExplorerExecutable, 1, wxALL|wxEXPAND, 5 );
	
	FilePicker = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN );
	FileSizer->Add( FilePicker, 0, wxALL, 5 );
	
	GridSizer->Add( FileSizer, 1, wxEXPAND, 5 );
	
	StaticBoxSizerExplorer->Add( GridSizer, 1, wxEXPAND, 5 );
	
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
	AssociationsFormSizer = new wxFlexGridSizer( 4, 2, 0, 0 );
	AssociationsFormSizer->AddGrowableCol( 1 );
	AssociationsFormSizer->SetFlexibleDirection( wxBOTH );
	AssociationsFormSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	PhpLabel = new wxStaticText( this, wxID_ANY, _("PHP && HTML Files"), wxDefaultPosition, wxDefaultSize, 0 );
	PhpLabel->Wrap( -1 );
	AssociationsFormSizer->Add( PhpLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	PhpFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( PhpFileExtensions, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	CssLabel = new wxStaticText( this, wxID_ANY, _("CSS Files"), wxDefaultPosition, wxDefaultSize, 0 );
	CssLabel->Wrap( -1 );
	AssociationsFormSizer->Add( CssLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	CssFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( CssFileExtensions, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	SqlLabel = new wxStaticText( this, wxID_ANY, _("SQL Files"), wxDefaultPosition, wxDefaultSize, 0 );
	SqlLabel->Wrap( -1 );
	AssociationsFormSizer->Add( SqlLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	SqlFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( SqlFileExtensions, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	MiscLabel = new wxStaticText( this, wxID_ANY, _("Misc. Files"), wxDefaultPosition, wxDefaultSize, 0 );
	MiscLabel->Wrap( -1 );
	AssociationsFormSizer->Add( MiscLabel, 0, wxALL, 5 );
	
	MiscFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( MiscFileExtensions, 1, wxALL|wxEXPAND, 5 );
	
	AssociationsGridSizer->Add( AssociationsFormSizer, 3, wxEXPAND, 5 );
	
	StaticBoxSizerFileTypes->Add( AssociationsGridSizer, 1, wxEXPAND, 5 );
	
	fgSizer2->Add( StaticBoxSizerFileTypes, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( fgSizer2 );
	this->Layout();
	
	// Connect Events
	FilePicker->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( ProjectPreferencesGeneratedPanelClass::OnFileChanged ), NULL, this );
}

ProjectPreferencesGeneratedPanelClass::~ProjectPreferencesGeneratedPanelClass()
{
	// Disconnect Events
	FilePicker->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( ProjectPreferencesGeneratedPanelClass::OnFileChanged ), NULL, this );
	
}

ProjectDefinitionDialogGeneratedClass::ProjectDefinitionDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 7, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 4 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HelpLabel = new wxStaticText( this, wxID_ANY, _("A project is a list of directories where source code is located together with a human-friendly label. A project can contain multiple directories from different locations. Additionally, MVC Editor can be told to exclude certain files from the project."), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	HelpLabel->Wrap( 500 );
	FlexGridSizer->Add( HelpLabel, 1, wxALL|wxEXPAND, 5 );
	
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
	FlexGridSizer = new wxFlexGridSizer( 8, 1, 0, 0 );
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
	
	HelpLabel = new wxStaticText( this, wxID_ANY, _("This is the list of projects that MVC Editor looks at.\n\nA project is merely a list of directories where source code is located together with a human-firendly label. A project can contain multiple directories from different locations. Additionally, MVC Editor can be told to exclude certain files from the project.\n\nA project can be enabled or disabled.  This is represented by the checkbox to the left of each. When a project is disabled, MVC Editor will not look at its resource cache."), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	HelpLabel->Wrap( 400 );
	FlexGridSizer->Add( HelpLabel, 0, wxALL, 5 );
	
	wxArrayString ProjectsListChoices;
	ProjectsList = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, ProjectsListChoices, wxLB_EXTENDED );
	FlexGridSizer->Add( ProjectsList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* CrudButtonsSizer;
	CrudButtonsSizer = new wxBoxSizer( wxHORIZONTAL );
	
	SelectAll = new wxButton( this, wxID_ANY, _("Select All"), wxDefaultPosition, wxDefaultSize, 0 );
	CrudButtonsSizer->Add( SelectAll, 0, wxALL, 5 );
	
	AddButton = new wxButton( this, wxID_ANY, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	CrudButtonsSizer->Add( AddButton, 0, wxALL, 5 );
	
	AddFromDirectoryButton = new wxButton( this, wxID_ANY, _("Add Multiple"), wxDefaultPosition, wxDefaultSize, 0 );
	CrudButtonsSizer->Add( AddFromDirectoryButton, 0, wxALL, 5 );
	
	EditButton = new wxButton( this, wxID_ANY, _("Edit Selected"), wxDefaultPosition, wxDefaultSize, 0 );
	CrudButtonsSizer->Add( EditButton, 0, wxALL, 5 );
	
	RemoveButton = new wxButton( this, wxID_ANY, _("Remove Selected"), wxDefaultPosition, wxDefaultSize, 0 );
	CrudButtonsSizer->Add( RemoveButton, 0, wxALL, 5 );
	
	FlexGridSizer->Add( CrudButtonsSizer, 1, wxEXPAND, 5 );
	
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
	BoxSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	ProjectsList->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnProjectsListDoubleClick ), NULL, this );
	ProjectsList->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnProjectsListCheckbox ), NULL, this );
	SelectAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnSelectAllButton ), NULL, this );
	AddButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnAddButton ), NULL, this );
	AddFromDirectoryButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnAddFromDirectoryButton ), NULL, this );
	EditButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnEditButton ), NULL, this );
	RemoveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnRemoveButton ), NULL, this );
	ButtonsSizerCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnHelpButton ), NULL, this );
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnOkButton ), NULL, this );
}

ProjectListDialogGeneratedClass::~ProjectListDialogGeneratedClass()
{
	// Disconnect Events
	ProjectsList->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnProjectsListDoubleClick ), NULL, this );
	ProjectsList->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnProjectsListCheckbox ), NULL, this );
	SelectAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnSelectAllButton ), NULL, this );
	AddButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnAddButton ), NULL, this );
	AddFromDirectoryButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnAddFromDirectoryButton ), NULL, this );
	EditButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnEditButton ), NULL, this );
	RemoveButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnRemoveButton ), NULL, this );
	ButtonsSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnHelpButton ), NULL, this );
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectListDialogGeneratedClass::OnOkButton ), NULL, this );
	
}

MultipleSelectDialogGeneratedClass::MultipleSelectDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer7->AddGrowableCol( 0 );
	fgSizer7->AddGrowableRow( 1 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	Label = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	fgSizer7->Add( Label, 1, wxALL|wxEXPAND, 5 );
	
	wxArrayString ChecklistChoices;
	Checklist = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, ChecklistChoices, wxLB_NEEDED_SB|wxLB_SINGLE );
	fgSizer7->Add( Checklist, 1, wxALL|wxEXPAND, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonsSizer->AddButton( ButtonsSizerCancel );
	ButtonsSizer->Realize();
	fgSizer7->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	bSizer7->Add( fgSizer7, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer7 );
	this->Layout();
	bSizer7->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	ButtonsSizerCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultipleSelectDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultipleSelectDialogGeneratedClass::OnOkButton ), NULL, this );
}

MultipleSelectDialogGeneratedClass::~MultipleSelectDialogGeneratedClass()
{
	// Disconnect Events
	ButtonsSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultipleSelectDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultipleSelectDialogGeneratedClass::OnOkButton ), NULL, this );
	
}
