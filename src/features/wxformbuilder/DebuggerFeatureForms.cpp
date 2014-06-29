///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "DebuggerFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

DebuggerPanelGeneratedClass::DebuggerPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BodySizer;
	BodySizer = new wxBoxSizer( wxVERTICAL );
	
	Notebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	BodySizer->Add( Notebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
}

DebuggerPanelGeneratedClass::~DebuggerPanelGeneratedClass()
{
}

DebuggerOptionsPanelGeneratedClass::DebuggerOptionsPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* BodySizer;
	BodySizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	BodySizer->AddGrowableCol( 0 );
	BodySizer->AddGrowableRow( 2 );
	BodySizer->SetFlexibleDirection( wxBOTH );
	BodySizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxVERTICAL );
	
	HelpLabel = new wxStaticText( this, wxID_ANY, wxT("Debugger Options\n\nTriumph is a full-fledged Xdebug client.  It can listen for incoming Xdebug connections, and can be used to step through code.  In this form, you can configure a few Xdebug settings."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpLabel->Wrap( 450 );
	TopSizer->Add( HelpLabel, 0, wxALL, 5 );
	
	HelpLink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("Debugger Help Page"), wxT("http://docs.triumph4php.com/debugger"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	TopSizer->Add( HelpLink, 0, wxALL, 5 );
	
	BodySizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* FormSizer;
	FormSizer = new wxFlexGridSizer( 5, 2, 0, 0 );
	FormSizer->SetFlexibleDirection( wxBOTH );
	FormSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	PortLabel = new wxStaticText( this, wxID_ANY, wxT("XdebugPort"), wxDefaultPosition, wxDefaultSize, 0 );
	PortLabel->Wrap( -1 );
	FormSizer->Add( PortLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	Port = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 65535, 9000 );
	FormSizer->Add( Port, 0, wxALL, 5 );
	
	MaxChildrenLabel = new wxStaticText( this, wxID_ANY, wxT("Max Children On Inital Load"), wxDefaultPosition, wxDefaultSize, 0 );
	MaxChildrenLabel->Wrap( -1 );
	FormSizer->Add( MaxChildrenLabel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	MaxChildren = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 500, 100 );
	FormSizer->Add( MaxChildren, 0, wxALL, 5 );
	
	MaxDepthLabel = new wxStaticText( this, wxID_ANY, wxT("Max Depth On Initial Load"), wxDefaultPosition, wxDefaultSize, 0 );
	MaxDepthLabel->Wrap( -1 );
	FormSizer->Add( MaxDepthLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	MaxDepth = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 10, 1 );
	FormSizer->Add( MaxDepth, 0, wxALL, 5 );
	
	DoListenOnAppReadyLabel = new wxStaticText( this, wxID_ANY, wxT("Listen On IDE Start"), wxDefaultPosition, wxDefaultSize, 0 );
	DoListenOnAppReadyLabel->Wrap( -1 );
	FormSizer->Add( DoListenOnAppReadyLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	DoListenOnAppReady = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	FormSizer->Add( DoListenOnAppReady, 0, wxALL, 5 );
	
	DoBreakOnStartLabel = new wxStaticText( this, wxID_ANY, wxT("Break On Script Start"), wxDefaultPosition, wxDefaultSize, 0 );
	DoBreakOnStartLabel->Wrap( -1 );
	FormSizer->Add( DoBreakOnStartLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	DoBreakOnStart = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	FormSizer->Add( DoBreakOnStart, 0, wxALL, 5 );
	
	BodySizer->Add( FormSizer, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* GroupSizer;
	GroupSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Debugger Source Code Mappings") ), wxVERTICAL );
	
	wxFlexGridSizer* MappingsSizer;
	MappingsSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	MappingsSizer->SetFlexibleDirection( wxBOTH );
	MappingsSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* MappingsTopSizer;
	MappingsTopSizer = new wxBoxSizer( wxVERTICAL );
	
	MappingsHelpLabel = new wxStaticText( this, wxID_ANY, wxT("Debugger source mappings are useful when your local copy of your source code is in a different directory in the web  server."), wxDefaultPosition, wxDefaultSize, 0 );
	MappingsHelpLabel->Wrap( 450 );
	MappingsTopSizer->Add( MappingsHelpLabel, 1, wxALL|wxEXPAND, 5 );
	
	MappingsSizer->Add( MappingsTopSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* ButtonsSizer;
	ButtonsSizer = new wxBoxSizer( wxHORIZONTAL );
	
	AddMapping = new wxButton( this, wxID_ANY, wxT("Add Mapping"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonsSizer->Add( AddMapping, 0, wxALL, 5 );
	
	EditMapping = new wxButton( this, wxID_ANY, wxT("Edit Mapping"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonsSizer->Add( EditMapping, 0, wxALL, 5 );
	
	DeleteMapping = new wxButton( this, wxID_ANY, wxT("Delete Mapping"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonsSizer->Add( DeleteMapping, 0, wxALL, 5 );
	
	MappingsSizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	SourceCodeMappings = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_NO_SORT_HEADER|wxLC_REPORT|wxLC_VRULES );
	MappingsSizer->Add( SourceCodeMappings, 1, wxALL|wxEXPAND, 5 );
	
	GroupSizer->Add( MappingsSizer, 1, wxEXPAND, 5 );
	
	BodySizer->Add( GroupSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
	
	// Connect Events
	AddMapping->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerOptionsPanelGeneratedClass::OnAddMapping ), NULL, this );
	EditMapping->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerOptionsPanelGeneratedClass::OnEditMapping ), NULL, this );
	DeleteMapping->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerOptionsPanelGeneratedClass::OnDeleteMapping ), NULL, this );
}

DebuggerOptionsPanelGeneratedClass::~DebuggerOptionsPanelGeneratedClass()
{
	// Disconnect Events
	AddMapping->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerOptionsPanelGeneratedClass::OnAddMapping ), NULL, this );
	EditMapping->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerOptionsPanelGeneratedClass::OnEditMapping ), NULL, this );
	DeleteMapping->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerOptionsPanelGeneratedClass::OnDeleteMapping ), NULL, this );
	
}

DebuggerStackPanelGeneratedClass::DebuggerStackPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* BodySizer;
	BodySizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	BodySizer->AddGrowableCol( 0 );
	BodySizer->AddGrowableRow( 1 );
	BodySizer->SetFlexibleDirection( wxBOTH );
	BodySizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	Label = new wxStaticText( this, wxID_ANY, wxT("Stack"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	TopSizer->Add( Label, 1, wxALL, 5 );
	
	StatusLabel = new wxStaticText( this, wxID_ANY, wxT("Status: Debugging session not active"), wxDefaultPosition, wxDefaultSize, 0 );
	StatusLabel->Wrap( -1 );
	TopSizer->Add( StatusLabel, 0, wxALL, 5 );
	
	BodySizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	StackList = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT );
	BodySizer->Add( StackList, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
}

DebuggerStackPanelGeneratedClass::~DebuggerStackPanelGeneratedClass()
{
}

DebuggerLogPanelGeneratedClass::DebuggerLogPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 1 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	ClearButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	TopSizer->Add( ClearButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	LogLabel = new wxStaticText( this, wxID_ANY, wxT("XDebug Log"), wxDefaultPosition, wxDefaultSize, 0 );
	LogLabel->Wrap( -1 );
	TopSizer->Add( LogLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	GridSizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	Text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	GridSizer->Add( Text, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( GridSizer );
	this->Layout();
	
	// Connect Events
	ClearButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerLogPanelGeneratedClass::OnClearButton ), NULL, this );
}

DebuggerLogPanelGeneratedClass::~DebuggerLogPanelGeneratedClass()
{
	// Disconnect Events
	ClearButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerLogPanelGeneratedClass::OnClearButton ), NULL, this );
	
}

DebuggerVariablePanelGeneratedClass::DebuggerVariablePanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 1 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	VariablesLabel = new wxStaticText( this, wxID_ANY, wxT("Local Variables"), wxDefaultPosition, wxDefaultSize, 0 );
	VariablesLabel->Wrap( -1 );
	TopSizer->Add( VariablesLabel, 1, wxALL|wxEXPAND, 5 );
	
	StatusLabel = new wxStaticText( this, wxID_ANY, wxT("Status: Debugger session active"), wxDefaultPosition, wxDefaultSize, 0 );
	StatusLabel->Wrap( -1 );
	TopSizer->Add( StatusLabel, 0, wxALL, 5 );
	
	GridSizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	VariablesList = new wxDataViewCtrl( this, wxID_ANY );
	GridSizer->Add( VariablesList, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( GridSizer );
	this->Layout();
}

DebuggerVariablePanelGeneratedClass::~DebuggerVariablePanelGeneratedClass()
{
}

DebuggerBreakpointPanelGeneratedClass::DebuggerBreakpointPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* BodySizer;
	BodySizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	BodySizer->AddGrowableCol( 0 );
	BodySizer->AddGrowableRow( 2 );
	BodySizer->SetFlexibleDirection( wxBOTH );
	BodySizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	DeleteBreakpointButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	DeleteBreakpointButton->SetHelpText( wxT("Delete seleted breakpoint") );
	
	DeleteBreakpointButton->SetHelpText( wxT("Delete seleted breakpoint") );
	
	TopSizer->Add( DeleteBreakpointButton, 0, wxALL, 5 );
	
	ToggleAllBreakpointsButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	ToggleAllBreakpointsButton->SetHelpText( wxT("Enable or disable all breakpoints") );
	
	ToggleAllBreakpointsButton->SetHelpText( wxT("Enable or disable all breakpoints") );
	
	TopSizer->Add( ToggleAllBreakpointsButton, 0, wxALL, 5 );
	
	BodySizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* LabelSizer;
	LabelSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Breakpoints"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	LabelSizer->Add( m_staticText6, 0, wxALL, 5 );
	
	BodySizer->Add( LabelSizer, 1, wxEXPAND, 5 );
	
	BreakpointsList = new wxDataViewListCtrl( this, wxID_ANY );
	BodySizer->Add( BreakpointsList, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
	
	// Connect Events
	DeleteBreakpointButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerBreakpointPanelGeneratedClass::OnDeleteBreakpoint ), NULL, this );
	ToggleAllBreakpointsButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerBreakpointPanelGeneratedClass::OnToggleAllBreakpoints ), NULL, this );
}

DebuggerBreakpointPanelGeneratedClass::~DebuggerBreakpointPanelGeneratedClass()
{
	// Disconnect Events
	DeleteBreakpointButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerBreakpointPanelGeneratedClass::OnDeleteBreakpoint ), NULL, this );
	ToggleAllBreakpointsButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerBreakpointPanelGeneratedClass::OnToggleAllBreakpoints ), NULL, this );
	
}

DebuggerFullViewDialogGeneratedClass::DebuggerFullViewDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* Sizer;
	Sizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	Sizer->AddGrowableCol( 0 );
	Sizer->AddGrowableRow( 1 );
	Sizer->SetFlexibleDirection( wxBOTH );
	Sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	Label = new wxStaticText( this, wxID_ANY, wxT("Variable Contents"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	Sizer->Add( Label, 1, wxALL|wxEXPAND, 5 );
	
	Text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	Sizer->Add( Text, 1, wxALL|wxEXPAND, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizer->Realize();
	Sizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( Sizer );
	this->Layout();
	
	this->Centre( wxBOTH );
}

DebuggerFullViewDialogGeneratedClass::~DebuggerFullViewDialogGeneratedClass()
{
}
