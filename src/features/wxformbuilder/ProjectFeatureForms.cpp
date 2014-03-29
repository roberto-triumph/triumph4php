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
	wxFlexGridSizer* FlexSizer;
	FlexSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexSizer->AddGrowableCol( 0 );
	FlexSizer->AddGrowableRow( 0 );
	FlexSizer->SetFlexibleDirection( wxBOTH );
	FlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* StaticBoxSizerFileTypes;
	StaticBoxSizerFileTypes = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Editor Associations") ), wxVERTICAL );
	
	wxFlexGridSizer* AssociationsGridSizer;
	AssociationsGridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	AssociationsGridSizer->AddGrowableCol( 0 );
	AssociationsGridSizer->AddGrowableRow( 1 );
	AssociationsGridSizer->SetFlexibleDirection( wxBOTH );
	AssociationsGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* AssociationsBoxSizer;
	AssociationsBoxSizer = new wxBoxSizer( wxVERTICAL );
	
	WildcardHelpText = new wxStaticText( this, wxID_ANY, _("Triumph uses file extensions to determine how to display the source code contained within them. Triumph's syntax highlighting and source code parsing features use these settings to determine which files contain PHP, SQL, and CSS source code. You can change the file extensions if desired.\n\nEach input can contain 1 or more wilcard strings. \nThe allowed wildcard characters are:\n\n* : Will match on zero or more characters\n? : Will match on zero or one character\n; : Separator among multiple wildcards\n\nFor example, the value \"*.php;*.phtml\" will match files with the extension .php or the extension .phtml\n"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	WildcardHelpText->Wrap( 500 );
	AssociationsBoxSizer->Add( WildcardHelpText, 1, wxALL|wxEXPAND, 5 );
	
	AssociationsGridSizer->Add( AssociationsBoxSizer, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* AssociationsFormSizer;
	AssociationsFormSizer = new wxFlexGridSizer( 7, 4, 0, 0 );
	AssociationsFormSizer->AddGrowableCol( 1 );
	AssociationsFormSizer->AddGrowableCol( 3 );
	AssociationsFormSizer->SetFlexibleDirection( wxBOTH );
	AssociationsFormSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	PhpLabel = new wxStaticText( this, wxID_ANY, _("PHP && HTML Files"), wxDefaultPosition, wxDefaultSize, 0 );
	PhpLabel->Wrap( -1 );
	AssociationsFormSizer->Add( PhpLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	PhpFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( PhpFileExtensions, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	JsLabel = new wxStaticText( this, wxID_ANY, _("JS Files"), wxDefaultPosition, wxDefaultSize, 0 );
	JsLabel->Wrap( -1 );
	AssociationsFormSizer->Add( JsLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	JsFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( JsFileExtensions, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
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
	
	ConfigLabel = new wxStaticText( this, wxID_ANY, _("Config Files"), wxDefaultPosition, wxDefaultSize, 0 );
	ConfigLabel->Wrap( -1 );
	AssociationsFormSizer->Add( ConfigLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	ConfigFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( ConfigFileExtensions, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	CrontabLabel = new wxStaticText( this, wxID_ANY, _("Crontab Files"), wxDefaultPosition, wxDefaultSize, 0 );
	CrontabLabel->Wrap( -1 );
	AssociationsFormSizer->Add( CrontabLabel, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	CrontabFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( CrontabFileExtensions, 1, wxALL|wxEXPAND, 5 );
	
	YamlLabel = new wxStaticText( this, wxID_ANY, _("YAML Files"), wxDefaultPosition, wxDefaultSize, 0 );
	YamlLabel->Wrap( -1 );
	AssociationsFormSizer->Add( YamlLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	YamlFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( YamlFileExtensions, 1, wxALL|wxEXPAND, 5 );
	
	XmlLabel = new wxStaticText( this, wxID_ANY, _("XML Files"), wxDefaultPosition, wxDefaultSize, 0 );
	XmlLabel->Wrap( -1 );
	AssociationsFormSizer->Add( XmlLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	XmlFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( XmlFileExtensions, 1, wxALL|wxEXPAND, 5 );
	
	RubyLabel = new wxStaticText( this, wxID_ANY, _("Ruby Files"), wxDefaultPosition, wxDefaultSize, 0 );
	RubyLabel->Wrap( -1 );
	AssociationsFormSizer->Add( RubyLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	RubyFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( RubyFileExtensions, 1, wxALL|wxEXPAND, 5 );
	
	LuaLabel = new wxStaticText( this, wxID_ANY, _("Lua Files"), wxDefaultPosition, wxDefaultSize, 0 );
	LuaLabel->Wrap( -1 );
	AssociationsFormSizer->Add( LuaLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	LuaFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( LuaFileExtensions, 1, wxALL|wxEXPAND, 5 );
	
	MarkdownLabel = new wxStaticText( this, wxID_ANY, _("Markdown Files"), wxDefaultPosition, wxDefaultSize, 0 );
	MarkdownLabel->Wrap( -1 );
	AssociationsFormSizer->Add( MarkdownLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	MarkdownFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( MarkdownFileExtensions, 1, wxALL|wxEXPAND, 5 );
	
	DiffLabel = new wxStaticText( this, wxID_ANY, _("Diff Files"), wxDefaultPosition, wxDefaultSize, 0 );
	DiffLabel->Wrap( -1 );
	AssociationsFormSizer->Add( DiffLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	DiffFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( DiffFileExtensions, 1, wxALL|wxEXPAND, 5 );
	
	BashLabel = new wxStaticText( this, wxID_ANY, _("Bash Files"), wxDefaultPosition, wxDefaultSize, 0 );
	BashLabel->Wrap( -1 );
	AssociationsFormSizer->Add( BashLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	BashFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( BashFileExtensions, 1, wxALL|wxEXPAND, 5 );
	
	MiscLabel = new wxStaticText( this, wxID_ANY, _("Misc. Files"), wxDefaultPosition, wxDefaultSize, 0 );
	MiscLabel->Wrap( -1 );
	AssociationsFormSizer->Add( MiscLabel, 0, wxALL, 5 );
	
	MiscFileExtensions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	AssociationsFormSizer->Add( MiscFileExtensions, 1, wxALL|wxEXPAND, 5 );
	
	AssociationsGridSizer->Add( AssociationsFormSizer, 3, wxEXPAND, 5 );
	
	StaticBoxSizerFileTypes->Add( AssociationsGridSizer, 1, wxEXPAND, 5 );
	
	FlexSizer->Add( StaticBoxSizerFileTypes, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( FlexSizer );
	this->Layout();
}

ProjectPreferencesGeneratedPanelClass::~ProjectPreferencesGeneratedPanelClass()
{
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
	
	HelpLabel = new wxStaticText( this, wxID_ANY, _("A project is a list of directories where source code is located together with a human-friendly label. A project can contain multiple directories from different locations. Additionally, Triumph can be told to exclude certain files from the project."), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
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
	
	HelpLabel = new wxStaticText( this, wxID_ANY, _("Enter a list of files that you want Triumph to track and ignore. \n\nInclude and exclude wildcards can contain one or more wildcards;  any files that match do NOT match the exclude wildcards and DO match the include wildcards will be added to the project. Exclude wildcards take precedence over include wildcards.\n\nExlcude wildcards are useful to ignore cache files, or for example Symfony skeleton files.\n\nA wildcard can have either a '*' or a '?'.\n\n* = any number of characters\n? = 0 or 1 character\n\nMultiple wildcards are separated by the semicolon (';')\n\nA wildcard can have directory separators; but they must match the operation system's path separator."), wxDefaultPosition, wxDefaultSize, 0 );
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
	FlexGridSizer = new wxFlexGridSizer( 5, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 2 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HelpLabel = new wxStaticText( this, wxID_ANY, _("This is the list of projects that Triumph looks at.\n\nA project is merely a list of directories where source code is located together with a human-firendly label. A project can contain multiple directories from different locations. Additionally, Triumph can be told to exclude certain files from the project.\n\nA project can be enabled or disabled.  This is represented by the checkbox to the left of each. When a project is disabled, Triumph will not look at its tag cache."), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	HelpLabel->Wrap( 400 );
	FlexGridSizer->Add( HelpLabel, 0, wxALL, 5 );
	
	HelpLink = new wxHyperlinkCtrl( this, wxID_ANY, _("More About Projects In Triumph 4 PHP"), wxT("http://docs.triumph4php.com/projects/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	FlexGridSizer->Add( HelpLink, 0, wxALL, 5 );
	
	wxArrayString ProjectsListChoices;
	ProjectsList = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxSize( -1,200 ), ProjectsListChoices, wxLB_EXTENDED );
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
