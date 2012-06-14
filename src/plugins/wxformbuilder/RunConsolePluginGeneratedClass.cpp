///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "RunConsolePluginGeneratedClass.h"

///////////////////////////////////////////////////////////////////////////

RunConsolePanelGeneratedClass::RunConsolePanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* MainSizer;
	MainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 1 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	Command = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	TopSizer->Add( Command, 1, wxALL, 5 );
	
	RunButton = new wxButton( this, wxID_ANY, _("Run"), wxDefaultPosition, wxDefaultSize, 0 );
	TopSizer->Add( RunButton, 0, wxALL, 5 );
	
	ClearButton = new wxButton( this, wxID_ANY, _("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	TopSizer->Add( ClearButton, 0, wxALL, 5 );
	
	StoreButton = new wxButton( this, wxID_ANY, _("Store Command"), wxDefaultPosition, wxDefaultSize, 0 );
	TopSizer->Add( StoreButton, 0, wxALL, 5 );
	
	FlexGridSizer->Add( TopSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* BottomSizer;
	BottomSizer = new wxBoxSizer( wxVERTICAL );
	
	OutputWindow = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	BottomSizer->Add( OutputWindow, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	FlexGridSizer->Add( BottomSizer, 0, wxEXPAND, 5 );
	
	MainSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( MainSizer );
	this->Layout();
	
	// Connect Events
	Command->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( RunConsolePanelGeneratedClass::RunCommand ), NULL, this );
	RunButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunConsolePanelGeneratedClass::RunCommand ), NULL, this );
	ClearButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunConsolePanelGeneratedClass::OnClear ), NULL, this );
	StoreButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunConsolePanelGeneratedClass::OnStoreButton ), NULL, this );
}

RunConsolePanelGeneratedClass::~RunConsolePanelGeneratedClass()
{
	// Disconnect Events
	Command->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( RunConsolePanelGeneratedClass::RunCommand ), NULL, this );
	RunButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunConsolePanelGeneratedClass::RunCommand ), NULL, this );
	ClearButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunConsolePanelGeneratedClass::OnClear ), NULL, this );
	StoreButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunConsolePanelGeneratedClass::OnStoreButton ), NULL, this );
	
}

CliCommandEditDialogGeneratedClass::CliCommandEditDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* ContentSizer;
	ContentSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxFlexGridSizer* RightSizer;
	RightSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	RightSizer->AddGrowableCol( 0 );
	RightSizer->AddGrowableRow( 1 );
	RightSizer->AddGrowableRow( 2 );
	RightSizer->SetFlexibleDirection( wxBOTH );
	RightSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* FormSizer;
	FormSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FormFlexSizer;
	FormFlexSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FormFlexSizer->AddGrowableCol( 0 );
	FormFlexSizer->AddGrowableRow( 0 );
	FormFlexSizer->AddGrowableRow( 1 );
	FormFlexSizer->SetFlexibleDirection( wxBOTH );
	FormFlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* FormColumn1Sizer;
	FormColumn1Sizer = new wxBoxSizer( wxVERTICAL );
	
	ExecutableLabel = new wxStaticText( this, wxID_ANY, _("Executable"), wxDefaultPosition, wxDefaultSize, 0 );
	ExecutableLabel->Wrap( -1 );
	FormColumn1Sizer->Add( ExecutableLabel, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	Executable = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( Executable, 1, wxALL, 5 );
	
	ExecutableFilePicker = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, _("Select an executable"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN );
	bSizer12->Add( ExecutableFilePicker, 0, wxALL|wxEXPAND, 5 );
	
	FormColumn1Sizer->Add( bSizer12, 1, wxEXPAND, 5 );
	
	FormFlexSizer->Add( FormColumn1Sizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	ArgumentsLabel = new wxStaticText( this, wxID_ANY, _("Arguments"), wxDefaultPosition, wxDefaultSize, 0 );
	ArgumentsLabel->Wrap( -1 );
	bSizer11->Add( ArgumentsLabel, 0, wxALL, 5 );
	
	Arguments = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	bSizer11->Add( Arguments, 1, wxALL|wxEXPAND, 5 );
	
	FormFlexSizer->Add( bSizer11, 1, wxEXPAND, 5 );
	
	wxBoxSizer* FormColumn2Sizer;
	FormColumn2Sizer = new wxBoxSizer( wxVERTICAL );
	
	DescriptionLabel = new wxStaticText( this, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
	DescriptionLabel->Wrap( -1 );
	FormColumn2Sizer->Add( DescriptionLabel, 0, wxALL, 5 );
	
	Description = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	FormColumn2Sizer->Add( Description, 0, wxALL|wxEXPAND, 5 );
	
	FormFlexSizer->Add( FormColumn2Sizer, 1, wxEXPAND, 5 );
	
	FormSizer->Add( FormFlexSizer, 1, wxEXPAND, 5 );
	
	RightSizer->Add( FormSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* CheckSizer;
	CheckSizer = new wxBoxSizer( wxVERTICAL );
	
	WaitForArguments = new wxCheckBox( this, wxID_ANY, _("Wait For Arguments"), wxDefaultPosition, wxDefaultSize, 0 );
	CheckSizer->Add( WaitForArguments, 0, wxALL, 5 );
	
	ShowInToolbar = new wxCheckBox( this, wxID_ANY, _("Show In Toolbar"), wxDefaultPosition, wxDefaultSize, 0 );
	CheckSizer->Add( ShowInToolbar, 0, wxALL, 5 );
	
	RightSizer->Add( CheckSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* HelpSizer;
	HelpSizer = new wxBoxSizer( wxVERTICAL );
	
	HelpLabel = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	HelpLabel->Wrap( 400 );
	HelpSizer->Add( HelpLabel, 1, wxALL|wxEXPAND, 5 );
	
	RightSizer->Add( HelpSizer, 1, wxEXPAND, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonsSizer->AddButton( ButtonsSizerCancel );
	ButtonsSizerHelp = new wxButton( this, wxID_HELP );
	ButtonsSizer->AddButton( ButtonsSizerHelp );
	ButtonsSizer->Realize();
	RightSizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	ContentSizer->Add( RightSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( ContentSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	ExecutableFilePicker->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( CliCommandEditDialogGeneratedClass::OnFileChanged ), NULL, this );
	Description->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CliCommandEditDialogGeneratedClass::OnDescriptionText ), NULL, this );
	ButtonsSizerHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandEditDialogGeneratedClass::OnHelpButton ), NULL, this );
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandEditDialogGeneratedClass::OnOkButton ), NULL, this );
}

CliCommandEditDialogGeneratedClass::~CliCommandEditDialogGeneratedClass()
{
	// Disconnect Events
	ExecutableFilePicker->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( CliCommandEditDialogGeneratedClass::OnFileChanged ), NULL, this );
	Description->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CliCommandEditDialogGeneratedClass::OnDescriptionText ), NULL, this );
	ButtonsSizerHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandEditDialogGeneratedClass::OnHelpButton ), NULL, this );
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandEditDialogGeneratedClass::OnOkButton ), NULL, this );
	
}

CliCommandListDialogGeneratedClass::CliCommandListDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* ColumnSizer;
	ColumnSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	ColumnSizer->AddGrowableCol( 0 );
	ColumnSizer->AddGrowableRow( 1 );
	ColumnSizer->SetFlexibleDirection( wxBOTH );
	ColumnSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	SavedLabel = new wxStaticText( this, wxID_ANY, _("Saved Console Commands"), wxDefaultPosition, wxDefaultSize, 0 );
	SavedLabel->Wrap( -1 );
	ColumnSizer->Add( SavedLabel, 0, wxALL, 5 );
	
	CommandsList = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE ); 
	ColumnSizer->Add( CommandsList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* ListButtonsSizer;
	ListButtonsSizer = new wxBoxSizer( wxHORIZONTAL );
	
	MoveUpButton = new wxButton( this, wxID_ANY, _("Up"), wxDefaultPosition, wxDefaultSize, 0 );
	ListButtonsSizer->Add( MoveUpButton, 0, wxALL, 5 );
	
	MoveDownButton = new wxButton( this, wxID_ANY, _("Down"), wxDefaultPosition, wxDefaultSize, 0 );
	ListButtonsSizer->Add( MoveDownButton, 0, wxALL, 5 );
	
	AddButton = new wxButton( this, wxID_ANY, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	ListButtonsSizer->Add( AddButton, 0, wxALL, 5 );
	
	DeleteButton = new wxButton( this, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	ListButtonsSizer->Add( DeleteButton, 0, wxALL, 5 );
	
	EditButton = new wxButton( this, wxID_ANY, _("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
	ListButtonsSizer->Add( EditButton, 0, wxALL, 5 );
	
	ColumnSizer->Add( ListButtonsSizer, 1, wxEXPAND, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonsSizer->AddButton( ButtonsSizerCancel );
	ButtonsSizer->Realize();
	ColumnSizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	BoxSizer->Add( ColumnSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	CommandsList->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( CliCommandListDialogGeneratedClass::OnListDoubleClick ), NULL, this );
	MoveUpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandListDialogGeneratedClass::OnUpButton ), NULL, this );
	MoveDownButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandListDialogGeneratedClass::OnDownButton ), NULL, this );
	AddButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandListDialogGeneratedClass::OnAddButton ), NULL, this );
	DeleteButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandListDialogGeneratedClass::OnDeleteButton ), NULL, this );
	EditButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandListDialogGeneratedClass::OnEditButton ), NULL, this );
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandListDialogGeneratedClass::OnOkButton ), NULL, this );
}

CliCommandListDialogGeneratedClass::~CliCommandListDialogGeneratedClass()
{
	// Disconnect Events
	CommandsList->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( CliCommandListDialogGeneratedClass::OnListDoubleClick ), NULL, this );
	MoveUpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandListDialogGeneratedClass::OnUpButton ), NULL, this );
	MoveDownButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandListDialogGeneratedClass::OnDownButton ), NULL, this );
	AddButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandListDialogGeneratedClass::OnAddButton ), NULL, this );
	DeleteButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandListDialogGeneratedClass::OnDeleteButton ), NULL, this );
	EditButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandListDialogGeneratedClass::OnEditButton ), NULL, this );
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CliCommandListDialogGeneratedClass::OnOkButton ), NULL, this );
	
}
