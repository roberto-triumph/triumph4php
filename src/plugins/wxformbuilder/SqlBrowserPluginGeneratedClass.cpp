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

BEGIN_EVENT_TABLE( SqlConnectionDialogGeneratedClass, wxDialog )
	EVT_LISTBOX( ID_LIST, SqlConnectionDialogGeneratedClass::_wxFB_OnListboxSelected )
	EVT_BUTTON( ID_TEST, SqlConnectionDialogGeneratedClass::_wxFB_OnTestButton )
	EVT_BUTTON( wxID_CANCEL, SqlConnectionDialogGeneratedClass::_wxFB_OnCancelButton )
	EVT_BUTTON( wxID_HELP, SqlConnectionDialogGeneratedClass::_wxFB_OnHelpButton )
	EVT_BUTTON( wxID_OK, SqlConnectionDialogGeneratedClass::_wxFB_OnOkButton )
END_EVENT_TABLE()

SqlConnectionDialogGeneratedClass::SqlConnectionDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->AddGrowableRow( 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	List = new wxListBox( this, ID_LIST, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE ); 
	fgSizer3->Add( List, 1, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 5, 2, 0, 0 );
	fgSizer8->AddGrowableCol( 1 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HostLabel = new wxStaticText( this, ID_HOSTLABEL, wxT("Host"), wxDefaultPosition, wxDefaultSize, 0 );
	HostLabel->Wrap( -1 );
	fgSizer8->Add( HostLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	Host = new wxTextCtrl( this, ID_HOST, wxT("localhost"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( Host, 1, wxALL|wxEXPAND, 5 );
	
	PortLabel = new wxStaticText( this, ID_PORTLABEL, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	PortLabel->Wrap( -1 );
	fgSizer8->Add( PortLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	Port = new wxSpinCtrl( this, ID_PORT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 3306 );
	fgSizer8->Add( Port, 0, wxALL, 5 );
	
	DatabaseLabel = new wxStaticText( this, ID_DATABASELABEL, wxT("Database"), wxDefaultPosition, wxDefaultSize, 0 );
	DatabaseLabel->Wrap( -1 );
	fgSizer8->Add( DatabaseLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	Database = new wxTextCtrl( this, ID_DATABASE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	Database->SetMaxLength( 100 ); 
	fgSizer8->Add( Database, 0, wxALL|wxEXPAND, 5 );
	
	UserLabel = new wxStaticText( this, ID_USERLABEL, wxT("User"), wxDefaultPosition, wxDefaultSize, 0 );
	UserLabel->Wrap( -1 );
	fgSizer8->Add( UserLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	User = new wxTextCtrl( this, ID_USER, wxT("root"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( User, 1, wxALL|wxEXPAND, 5 );
	
	PasswordLabel = new wxStaticText( this, ID_PASSWORDLABEL, wxT("Password"), wxDefaultPosition, wxDefaultSize, 0 );
	PasswordLabel->Wrap( -1 );
	fgSizer8->Add( PasswordLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	Password = new wxTextCtrl( this, ID_PASSWORD, wxT("fdfd"), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	fgSizer8->Add( Password, 1, wxALL|wxEXPAND, 5 );
	
	fgSizer3->Add( fgSizer8, 1, wxEXPAND, 5 );
	
	bSizer11->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button2 = new wxButton( this, ID_TEST, wxT("Test"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_button2, 0, wxALL, 5 );
	
	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( this, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer2->AddButton( m_sdbSizer2Cancel );
	m_sdbSizer2Help = new wxButton( this, wxID_HELP );
	m_sdbSizer2->AddButton( m_sdbSizer2Help );
	m_sdbSizer2->Realize();
	bSizer12->Add( m_sdbSizer2, 1, wxEXPAND, 5 );
	
	bSizer11->Add( bSizer12, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer11 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

SqlConnectionDialogGeneratedClass::~SqlConnectionDialogGeneratedClass()
{
}
