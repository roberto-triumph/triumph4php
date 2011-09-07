///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "SqlBrowserPluginGeneratedClass.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( SqlBrowserPanelGeneratedClass, wxPanel )
	EVT_BUTTON( ID_RUNBUTTON, SqlBrowserPanelGeneratedClass::_wxFB_OnRunButton )
END_EVENT_TABLE()

SqlBrowserPanelGeneratedClass::SqlBrowserPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetExtraStyle( wxWS_EX_VALIDATE_RECURSIVELY );
	
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxHORIZONTAL );
	
	Splitter = new wxSplitterWindow( this, ID_SPLITTER, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	Splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( SqlBrowserPanelGeneratedClass::SplitterOnIdle ), NULL, this );
	
	TopPanel = new wxPanel( Splitter, ID_TOPPANEL, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxRAISED_BORDER|wxTAB_TRAVERSAL );
	TopPanelSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	TopPanelSizer->AddGrowableCol( 0 );
	TopPanelSizer->AddGrowableRow( 1 );
	TopPanelSizer->SetFlexibleDirection( wxBOTH );
	TopPanelSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	ButtonPanel = new wxPanel( TopPanel, ID_GRIDPANEL, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* ButtonPanelSizer;
	ButtonPanelSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxFlexGridSizer* ButtonFlexGrid;
	ButtonFlexGrid = new wxFlexGridSizer( 2, 1, 0, 0 );
	ButtonFlexGrid->AddGrowableCol( 0 );
	ButtonFlexGrid->AddGrowableRow( 0 );
	ButtonFlexGrid->SetFlexibleDirection( wxBOTH );
	ButtonFlexGrid->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* InputSizer;
	InputSizer = new wxBoxSizer( wxHORIZONTAL );
	
	HostLabel = new wxStaticText( ButtonPanel, ID_HOSTLABEL, wxT("Host"), wxDefaultPosition, wxDefaultSize, 0 );
	HostLabel->Wrap( -1 );
	InputSizer->Add( HostLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	Host = new wxTextCtrl( ButtonPanel, ID_HOST, wxT("localhost"), wxDefaultPosition, wxDefaultSize, 0 );
	InputSizer->Add( Host, 1, wxALL|wxEXPAND, 5 );
	
	PortLabel = new wxStaticText( ButtonPanel, ID_PORTLABEL, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	PortLabel->Wrap( -1 );
	InputSizer->Add( PortLabel, 0, wxALIGN_CENTER|wxALL, 5 );
	
	Port = new wxSpinCtrl( ButtonPanel, ID_PORT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 3306 );
	InputSizer->Add( Port, 0, wxALL, 5 );
	
	DatabaseLabel = new wxStaticText( ButtonPanel, ID_DATABASELABEL, wxT("Database"), wxDefaultPosition, wxDefaultSize, 0 );
	DatabaseLabel->Wrap( -1 );
	InputSizer->Add( DatabaseLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	Database = new wxComboBox( ButtonPanel, ID_DATABASE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	Database->Append( wxT("db1") );
	Database->Append( wxT("db2") );
	Database->Append( wxT("mysql") );
	InputSizer->Add( Database, 0, wxALL, 5 );
	
	UserLabel = new wxStaticText( ButtonPanel, ID_USERLABEL, wxT("User"), wxDefaultPosition, wxDefaultSize, 0 );
	UserLabel->Wrap( -1 );
	InputSizer->Add( UserLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	User = new wxTextCtrl( ButtonPanel, ID_USER, wxT("root"), wxDefaultPosition, wxDefaultSize, 0 );
	InputSizer->Add( User, 1, wxALL|wxEXPAND, 5 );
	
	PasswordLabel = new wxStaticText( ButtonPanel, ID_PASSWORDLABEL, wxT("Password"), wxDefaultPosition, wxDefaultSize, 0 );
	PasswordLabel->Wrap( -1 );
	InputSizer->Add( PasswordLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	Password = new wxTextCtrl( ButtonPanel, ID_PASSWORD, wxT("fdfd"), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	InputSizer->Add( Password, 1, wxALL|wxEXPAND, 5 );
	
	ButtonFlexGrid->Add( InputSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* ButtonSizer;
	ButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	RunButton = new wxButton( ButtonPanel, ID_RUNBUTTON, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( RunButton, 0, wxALL, 5 );
	
	ButtonFlexGrid->Add( ButtonSizer, 1, wxEXPAND, 5 );
	
	ButtonPanelSizer->Add( ButtonFlexGrid, 1, wxEXPAND, 0 );
	
	ButtonPanel->SetSizer( ButtonPanelSizer );
	ButtonPanel->Layout();
	ButtonPanelSizer->Fit( ButtonPanel );
	TopPanelSizer->Add( ButtonPanel, 1, wxEXPAND | wxALL, 5 );
	
	CodeControlPanel = new wxPanel( TopPanel, ID_CODECONTROLPANEL, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
	CodeControlPanelSizer = new wxBoxSizer( wxHORIZONTAL );
	
	CodeControlPanel->SetSizer( CodeControlPanelSizer );
	CodeControlPanel->Layout();
	CodeControlPanelSizer->Fit( CodeControlPanel );
	TopPanelSizer->Add( CodeControlPanel, 1, wxEXPAND, 5 );
	
	TopPanel->SetSizer( TopPanelSizer );
	TopPanel->Layout();
	TopPanelSizer->Fit( TopPanel );
	BottomPanel = new wxPanel( Splitter, ID_BOTTOMPANLE, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* BottomGridSizer;
	BottomGridSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 1 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	ResultsLabel = new wxStaticText( BottomPanel, ID_RESULTSLABEL, wxT("20 rows returned"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	ResultsLabel->Wrap( -1 );
	FlexGridSizer->Add( ResultsLabel, 0, wxALL|wxEXPAND, 5 );
	
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
	Splitter->SplitHorizontally( TopPanel, BottomPanel, 251 );
	BoxSizer->Add( Splitter, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
}

SqlBrowserPanelGeneratedClass::~SqlBrowserPanelGeneratedClass()
{
}
