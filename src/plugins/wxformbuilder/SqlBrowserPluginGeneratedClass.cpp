///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "SqlBrowserPluginGeneratedClass.h"

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
	
	ConnectionLabel = new wxStaticText( BottomPanel, ID_CONNECTIONLABEL, wxT("mysql: root@localhost:3306"), wxDefaultPosition, wxDefaultSize, 0 );
	ConnectionLabel->Wrap( -1 );
	bSizer13->Add( ConnectionLabel, 1, wxALL, 5 );
	
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
}

SqlBrowserPanelGeneratedClass::~SqlBrowserPanelGeneratedClass()
{
}

SqlConnectionDialogGeneratedClass::SqlConnectionDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BodySizer;
	BodySizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FormSizer;
	FormSizer = new wxFlexGridSizer( 1, 2, 0, 0 );
	FormSizer->AddGrowableCol( 0 );
	FormSizer->AddGrowableCol( 1 );
	FormSizer->AddGrowableRow( 0 );
	FormSizer->SetFlexibleDirection( wxBOTH );
	FormSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* ConnectionsSizer;
	ConnectionsSizer = new wxFlexGridSizer( 4, 1, 0, 0 );
	ConnectionsSizer->AddGrowableCol( 0 );
	ConnectionsSizer->AddGrowableRow( 1 );
	ConnectionsSizer->AddGrowableRow( 3 );
	ConnectionsSizer->SetFlexibleDirection( wxBOTH );
	ConnectionsSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	ConnectionsLabel = new wxStaticText( this, ID_GLOBALCONNECTIONSLABEL, wxT("Choose a connection to use"), wxDefaultPosition, wxDefaultSize, 0 );
	ConnectionsLabel->Wrap( -1 );
	ConnectionsSizer->Add( ConnectionsLabel, 0, wxALL, 5 );
	
	List = new wxListBox( this, ID_LIST, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE ); 
	ConnectionsSizer->Add( List, 1, wxALL|wxEXPAND, 5 );
	
	FormSizer->Add( ConnectionsSizer, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* CredentialsSizer;
	CredentialsSizer = new wxFlexGridSizer( 6, 2, 0, 0 );
	CredentialsSizer->AddGrowableCol( 1 );
	CredentialsSizer->SetFlexibleDirection( wxBOTH );
	CredentialsSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	NameLabel = new wxStaticText( this, ID_NAMELABEL, wxT("Label"), wxDefaultPosition, wxDefaultSize, 0 );
	NameLabel->Wrap( -1 );
	CredentialsSizer->Add( NameLabel, 0, wxALL, 5 );
	
	Label = new wxTextCtrl( this, ID_LABEL, wxT("Untitled"), wxDefaultPosition, wxDefaultSize, 0 );
	CredentialsSizer->Add( Label, 0, wxALL|wxEXPAND, 5 );
	
	HostLabel = new wxStaticText( this, ID_HOSTLABEL, wxT("Host"), wxDefaultPosition, wxDefaultSize, 0 );
	HostLabel->Wrap( -1 );
	CredentialsSizer->Add( HostLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	Host = new wxTextCtrl( this, ID_HOST, wxT("localhost"), wxDefaultPosition, wxDefaultSize, 0 );
	CredentialsSizer->Add( Host, 1, wxALL|wxEXPAND, 5 );
	
	PortLabel = new wxStaticText( this, ID_PORTLABEL, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	PortLabel->Wrap( -1 );
	CredentialsSizer->Add( PortLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	Port = new wxSpinCtrl( this, ID_PORT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 3306 );
	CredentialsSizer->Add( Port, 0, wxALL, 5 );
	
	DatabaseLabel = new wxStaticText( this, ID_DATABASELABEL, wxT("Database"), wxDefaultPosition, wxDefaultSize, 0 );
	DatabaseLabel->Wrap( -1 );
	CredentialsSizer->Add( DatabaseLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	Database = new wxTextCtrl( this, ID_DATABASE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	Database->SetMaxLength( 100 ); 
	CredentialsSizer->Add( Database, 0, wxALL|wxEXPAND, 5 );
	
	UserLabel = new wxStaticText( this, ID_USERLABEL, wxT("User"), wxDefaultPosition, wxDefaultSize, 0 );
	UserLabel->Wrap( -1 );
	CredentialsSizer->Add( UserLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	User = new wxTextCtrl( this, ID_USER, wxT("root"), wxDefaultPosition, wxDefaultSize, 0 );
	CredentialsSizer->Add( User, 1, wxALL|wxEXPAND, 5 );
	
	PasswordLabel = new wxStaticText( this, ID_PASSWORDLABEL, wxT("Password"), wxDefaultPosition, wxDefaultSize, 0 );
	PasswordLabel->Wrap( -1 );
	CredentialsSizer->Add( PasswordLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	Password = new wxTextCtrl( this, ID_PASSWORD, wxT("fdfd"), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	CredentialsSizer->Add( Password, 1, wxALL|wxEXPAND, 5 );
	
	
	CredentialsSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	WarningLabel = new wxStaticText( this, ID_WARNINGLABEL, wxT("WARNING: ALL DATABASE PASSWORDS \nWILL BE SAVED IN PLAIN TEXT!"), wxDefaultPosition, wxDefaultSize, 0 );
	WarningLabel->Wrap( -1 );
	CredentialsSizer->Add( WarningLabel, 1, wxALL|wxEXPAND, 5 );
	
	FormSizer->Add( CredentialsSizer, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	BodySizer->Add( FormSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* BottomSizer;
	BottomSizer = new wxBoxSizer( wxHORIZONTAL );
	
	TestButton = new wxButton( this, ID_TESTBUTTON, wxT("Test"), wxDefaultPosition, wxDefaultSize, 0 );
	BottomSizer->Add( TestButton, 0, wxALL, 5 );
	
	AddButton = new wxButton( this, ID_SQLADDBUTTON, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	BottomSizer->Add( AddButton, 0, wxALL, 5 );
	
	DeleteButton = new wxButton( this, ID_SQLDELETEBUTTON, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	BottomSizer->Add( DeleteButton, 0, wxALL, 5 );
	
	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( this, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer2->AddButton( m_sdbSizer2Cancel );
	m_sdbSizer2->Realize();
	BottomSizer->Add( m_sdbSizer2, 1, wxEXPAND, 5 );
	
	BodySizer->Add( BottomSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	List->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SqlConnectionDialogGeneratedClass::OnListboxSelected ), NULL, this );
	Label->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SqlConnectionDialogGeneratedClass::OnLabelText ), NULL, this );
	TestButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionDialogGeneratedClass::OnTestButton ), NULL, this );
	AddButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionDialogGeneratedClass::OnAddButton ), NULL, this );
	DeleteButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionDialogGeneratedClass::OnDeleteButton ), NULL, this );
	m_sdbSizer2OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionDialogGeneratedClass::OnOkButton ), NULL, this );
}

SqlConnectionDialogGeneratedClass::~SqlConnectionDialogGeneratedClass()
{
	// Disconnect Events
	List->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SqlConnectionDialogGeneratedClass::OnListboxSelected ), NULL, this );
	Label->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SqlConnectionDialogGeneratedClass::OnLabelText ), NULL, this );
	TestButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionDialogGeneratedClass::OnTestButton ), NULL, this );
	AddButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionDialogGeneratedClass::OnAddButton ), NULL, this );
	DeleteButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionDialogGeneratedClass::OnDeleteButton ), NULL, this );
	m_sdbSizer2OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SqlConnectionDialogGeneratedClass::OnOkButton ), NULL, this );
	
}
