///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "SqlBrowserFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

SqlBrowserPanelGeneratedClass::SqlBrowserPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetExtraStyle( wxWS_EX_VALIDATE_RECURSIVELY );
	
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxHORIZONTAL );
	
	BottomPanel = new wxPanel( this, ID_BOTTOMPANLE, wxDefaultPosition, wxDefaultSize, 0 );
	wxBoxSizer* BottomGridSizer;
	BottomGridSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 1 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );
	
	ConnectionLabel = new wxStaticText( BottomPanel, ID_CONNECTIONLABEL, wxT("Connection"), wxDefaultPosition, wxDefaultSize, 0 );
	ConnectionLabel->Wrap( -1 );
	bSizer13->Add( ConnectionLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString ConnectionsChoices;
	Connections = new wxChoice( BottomPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, ConnectionsChoices, 0 );
	Connections->SetSelection( 0 );
	bSizer13->Add( Connections, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer13->Add( 0, 0, 1, wxEXPAND, 5 );
	
	ResultsLabel = new wxStaticText( BottomPanel, ID_RESULTSLABEL, wxT("20 rows returned"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	ResultsLabel->Wrap( -1 );
	bSizer13->Add( ResultsLabel, 0, wxALL|wxEXPAND, 5 );
	
	FlexGridSizer->Add( bSizer13, 1, wxEXPAND, 5 );
	
	ResultsGrid = new wxGrid( BottomPanel, ID_DATAGRID, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	ResultsGrid->CreateGrid( 5, 5 );
	ResultsGrid->EnableEditing( false );
	ResultsGrid->EnableGridLines( true );
	ResultsGrid->EnableDragGridSize( true );
	ResultsGrid->SetMargins( 0, 0 );
	
	// Columns
	ResultsGrid->SetColSize( 0, 80 );
	ResultsGrid->SetColSize( 1, 80 );
	ResultsGrid->SetColSize( 2, 80 );
	ResultsGrid->SetColSize( 3, 195 );
	ResultsGrid->SetColSize( 4, 243 );
	ResultsGrid->EnableDragColMove( false );
	ResultsGrid->EnableDragColSize( true );
	ResultsGrid->SetColLabelSize( 30 );
	ResultsGrid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	ResultsGrid->EnableDragRowSize( true );
	ResultsGrid->SetRowLabelSize( 80 );
	ResultsGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	ResultsGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	FlexGridSizer->Add( ResultsGrid, 1, wxALL|wxEXPAND, 5 );
	
	BottomGridSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	BottomPanel->SetSizer( BottomGridSizer );
	BottomPanel->Layout();
	BottomGridSizer->Fit( BottomPanel );
	BoxSizer->Add( BottomPanel, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	Connections->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SqlBrowserPanelGeneratedClass::OnConnectionChoice ), NULL, this );
}

SqlBrowserPanelGeneratedClass::~SqlBrowserPanelGeneratedClass()
{
	// Disconnect Events
	Connections->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SqlBrowserPanelGeneratedClass::OnConnectionChoice ), NULL, this );
	
}

MysqlConnectionDialogGeneratedClass::MysqlConnectionDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BodySizer;
	BodySizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 1 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	WarningLabel = new wxStaticText( this, ID_WARNINGLABEL, wxT("Here you define the connection credentials for a MySQL database.\n\nWARNING: ALL DATABASE PASSWORDS  WILL BE SAVED IN PLAIN TEXT!"), wxDefaultPosition, wxDefaultSize, 0 );
	WarningLabel->Wrap( 310 );
	GridSizer->Add( WarningLabel, 1, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* CredentialsSizer;
	CredentialsSizer = new wxFlexGridSizer( 7, 2, 0, 0 );
	CredentialsSizer->AddGrowableCol( 1 );
	CredentialsSizer->SetFlexibleDirection( wxBOTH );
	CredentialsSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	NameLabel = new wxStaticText( this, ID_NAMELABEL, wxT("Label"), wxDefaultPosition, wxDefaultSize, 0 );
	NameLabel->Wrap( -1 );
	CredentialsSizer->Add( NameLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	Label = new wxTextCtrl( this, ID_LABEL, wxT("Untitled"), wxDefaultPosition, wxDefaultSize, 0 );
	CredentialsSizer->Add( Label, 0, wxALL|wxEXPAND, 5 );
	
	HostLabel = new wxStaticText( this, ID_HOSTLABEL, wxT("Host"), wxDefaultPosition, wxDefaultSize, 0 );
	HostLabel->Wrap( -1 );
	CredentialsSizer->Add( HostLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	Host = new wxTextCtrl( this, ID_HOST, wxT("localhost"), wxDefaultPosition, wxDefaultSize, 0 );
	CredentialsSizer->Add( Host, 1, wxALL|wxEXPAND, 5 );
	
	PortLabel = new wxStaticText( this, ID_PORTLABEL, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	PortLabel->Wrap( -1 );
	CredentialsSizer->Add( PortLabel, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	Port = new wxSpinCtrl( this, ID_PORT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 3306 );
	CredentialsSizer->Add( Port, 0, wxALL, 5 );
	
	DatabaseLabel = new wxStaticText( this, ID_DATABASELABEL, wxT("Database"), wxDefaultPosition, wxDefaultSize, 0 );
	DatabaseLabel->Wrap( -1 );
	CredentialsSizer->Add( DatabaseLabel, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	Database = new wxTextCtrl( this, ID_DATABASE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	Database->SetMaxLength( 100 ); 
	CredentialsSizer->Add( Database, 0, wxALL|wxEXPAND, 5 );
	
	UserLabel = new wxStaticText( this, ID_USERLABEL, wxT("User"), wxDefaultPosition, wxDefaultSize, 0 );
	UserLabel->Wrap( -1 );
	CredentialsSizer->Add( UserLabel, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	User = new wxTextCtrl( this, ID_USER, wxT("root"), wxDefaultPosition, wxDefaultSize, 0 );
	CredentialsSizer->Add( User, 1, wxALL|wxEXPAND, 5 );
	
	PasswordLabel = new wxStaticText( this, ID_PASSWORDLABEL, wxT("Password"), wxDefaultPosition, wxDefaultSize, 0 );
	PasswordLabel->Wrap( -1 );
	CredentialsSizer->Add( PasswordLabel, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	Password = new wxTextCtrl( this, ID_PASSWORD, wxT("fdfd"), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	CredentialsSizer->Add( Password, 1, wxALL|wxEXPAND, 5 );
	
	
	CredentialsSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	GridSizer->Add( CredentialsSizer, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* BottomSizer;
	BottomSizer = new wxBoxSizer( wxHORIZONTAL );
	
	TestButton = new wxButton( this, ID_TESTBUTTON, wxT("Test"), wxDefaultPosition, wxDefaultSize, 0 );
	BottomSizer->Add( TestButton, 0, wxALL, 5 );
	
	StdButtonsSizer = new wxStdDialogButtonSizer();
	StdButtonsSizerOK = new wxButton( this, wxID_OK );
	StdButtonsSizer->AddButton( StdButtonsSizerOK );
	StdButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	StdButtonsSizer->AddButton( StdButtonsSizerCancel );
	StdButtonsSizerHelp = new wxButton( this, wxID_HELP );
	StdButtonsSizer->AddButton( StdButtonsSizerHelp );
	StdButtonsSizer->Realize();
	BottomSizer->Add( StdButtonsSizer, 1, wxEXPAND, 5 );
	
	GridSizer->Add( BottomSizer, 0, wxEXPAND, 5 );
	
	BodySizer->Add( GridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
	BodySizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	Label->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MysqlConnectionDialogGeneratedClass::OnLabelText ), NULL, this );
	TestButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MysqlConnectionDialogGeneratedClass::OnTestButton ), NULL, this );
	StdButtonsSizerCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MysqlConnectionDialogGeneratedClass::OnCancelButton ), NULL, this );
}

MysqlConnectionDialogGeneratedClass::~MysqlConnectionDialogGeneratedClass()
{
	// Disconnect Events
	Label->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MysqlConnectionDialogGeneratedClass::OnLabelText ), NULL, this );
	TestButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MysqlConnectionDialogGeneratedClass::OnTestButton ), NULL, this );
	StdButtonsSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MysqlConnectionDialogGeneratedClass::OnCancelButton ), NULL, this );
	
}

SqliteConnectionDialogGeneratedClass::SqliteConnectionDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* Sizer;
	Sizer = new wxBoxSizer( wxVERTICAL );
	
	HelpLabel = new wxStaticText( this, wxID_ANY, wxT("Here you define the file that contains a SQLite database"), wxDefaultPosition, wxDefaultSize, 0 );
	HelpLabel->Wrap( -1 );
	Sizer->Add( HelpLabel, 0, wxALL|wxEXPAND, 15 );
	
	wxFlexGridSizer* FormSizer;
	FormSizer = new wxFlexGridSizer( 2, 2, 0, 0 );
	FormSizer->AddGrowableCol( 1 );
	FormSizer->SetFlexibleDirection( wxBOTH );
	FormSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	NameLabel = new wxStaticText( this, wxID_ANY, wxT("Label"), wxDefaultPosition, wxDefaultSize, 0 );
	NameLabel->Wrap( -1 );
	FormSizer->Add( NameLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	Label = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	FormSizer->Add( Label, 1, wxALL|wxEXPAND, 5 );
	
	FileLabel = new wxStaticText( this, wxID_ANY, wxT("File"), wxDefaultPosition, wxDefaultSize, 0 );
	FileLabel->Wrap( -1 );
	FormSizer->Add( FileLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	File = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	FormSizer->Add( File, 1, wxALL|wxEXPAND, 5 );
	
	Sizer->Add( FormSizer, 1, wxEXPAND, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonsSizer->AddButton( ButtonsSizerCancel );
	ButtonsSizer->Realize();
	Sizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( Sizer );
	this->Layout();
	
	this->Centre( wxBOTH );
}

SqliteConnectionDialogGeneratedClass::~SqliteConnectionDialogGeneratedClass()
{
}

SqlConnectionListDialogGeneratedClass::SqlConnectionListDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,450 ), wxDefaultSize );
	
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	HelpLabel = new wxStaticText( this, wxID_ANY, wxT("This is the list of connections that are available to MVC Editor. You can add a new connection, and the table names and column names of each enabled connection will be available for code completion."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpLabel->Wrap( 500 );
	BoxSizer->Add( HelpLabel, 0, wxALL|wxEXPAND, 15 );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 0 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxArrayString ListChoices;
	List = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, ListChoices, wxLB_EXTENDED|wxLB_MULTIPLE|wxLB_NEEDED_SB );
	GridSizer->Add( List, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	AdMysqlButton = new wxButton( this, wxID_ANY, wxT("Add MySQL"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( AdMysqlButton, 0, wxALL, 5 );
	
	AddSqliteButton = new wxButton( this, wxID_ANY, wxT("Add SQLite"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( AddSqliteButton, 0, wxALL, 5 );
	
	CloneButton = new wxButton( this, wxID_ANY, wxT("Clone Selected"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( CloneButton, 0, wxALL, 5 );
	
	TestButton = new wxButton( this, ID_TESTBUTTON, wxT("Test Selected"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( TestButton, 0, wxALL, 5 );
	
	RemoveSelected = new wxButton( this, wxID_ANY, wxT("Remove Selected"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( RemoveSelected, 0, wxALL, 5 );
	
	RemoveAllButton = new wxButton( this, wxID_ANY, wxT("Remove All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( RemoveAllButton, 0, wxALL, 5 );
	
	GridSizer->Add( bSizer7, 1, wxALIGN_RIGHT, 5 );
	
	ButtonSizer = new wxStdDialogButtonSizer();
	ButtonSizerOK = new wxButton( this, wxID_OK );
	ButtonSizer->AddButton( ButtonSizerOK );
	ButtonSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonSizer->AddButton( ButtonSizerCancel );
	ButtonSizer->Realize();
	GridSizer->Add( ButtonSizer, 1, wxEXPAND, 5 );
	
	BoxSizer->Add( GridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	BoxSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	List->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnListDoubleClick ), NULL, this );
	List->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnCheckToggled ), NULL, this );
	AdMysqlButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnAddMysqlButton ), NULL, this );
	AddSqliteButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnAddSqliteButton ), NULL, this );
	CloneButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnCloneButton ), NULL, this );
	TestButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnTestSelectedButton ), NULL, this );
	RemoveSelected->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnRemoveSelectedButton ), NULL, this );
	RemoveAllButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnRemoveAllButton ), NULL, this );
	ButtonSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnOkButton ), NULL, this );
}

SqlConnectionListDialogGeneratedClass::~SqlConnectionListDialogGeneratedClass()
{
	// Disconnect Events
	List->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnListDoubleClick ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnCheckToggled ), NULL, this );
	AdMysqlButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnAddMysqlButton ), NULL, this );
	AddSqliteButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnAddSqliteButton ), NULL, this );
	CloneButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnCloneButton ), NULL, this );
	TestButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnTestSelectedButton ), NULL, this );
	RemoveSelected->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnRemoveSelectedButton ), NULL, this );
	RemoveAllButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnRemoveAllButton ), NULL, this );
	ButtonSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionListDialogGeneratedClass::OnOkButton ), NULL, this );
	
}
