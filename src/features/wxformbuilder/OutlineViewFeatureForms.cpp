///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "OutlineViewFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

OutlineViewGeneratedPanelClass::OutlineViewGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 1 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* NavigationSizer;
	NavigationSizer = new wxFlexGridSizer( 1, 8, 0, 0 );
	NavigationSizer->AddGrowableCol( 0 );
	NavigationSizer->AddGrowableRow( 0 );
	NavigationSizer->SetFlexibleDirection( wxBOTH );
	NavigationSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	StatusLabel = new wxStaticText( this, ID_STATUSLABEL, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	StatusLabel->Wrap( -1 );
	NavigationSizer->Add( StatusLabel, 1, wxALL|wxALIGN_BOTTOM, 5 );
	
	ShowPublicOnlyButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	NavigationSizer->Add( ShowPublicOnlyButton, 0, wxALL, 5 );
	
	ConstantsButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	ConstantsButton->SetToolTip( _("Toggle Class Constants on and off") );
	
	ConstantsButton->SetToolTip( _("Toggle Class Constants on and off") );
	
	NavigationSizer->Add( ConstantsButton, 0, wxALL, 5 );
	
	PropertiesButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	PropertiesButton->SetToolTip( _("Toggle Class members on and off") );
	
	PropertiesButton->SetToolTip( _("Toggle Class members on and off") );
	
	NavigationSizer->Add( PropertiesButton, 0, wxALL, 5 );
	
	MethodsButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	MethodsButton->SetToolTip( _("Toggle Class methods on and off") );
	
	MethodsButton->SetToolTip( _("Toggle Class methods on and off") );
	
	NavigationSizer->Add( MethodsButton, 0, wxALL, 5 );
	
	AddButton = new wxBitmapButton( this, ID_ADDBUTTON, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	AddButton->SetToolTip( _("Add a class to the outline tree") );
	
	AddButton->SetToolTip( _("Add a class to the outline tree") );
	
	NavigationSizer->Add( AddButton, 0, wxALL, 5 );
	
	SyncButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	SyncButton->SetToolTip( _("Sync Outline With Opened Files") );
	
	SyncButton->SetToolTip( _("Sync Outline With Opened Files") );
	
	NavigationSizer->Add( SyncButton, 0, wxALL, 5 );
	
	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	HelpButton->SetToolTip( _("Help") );
	
	HelpButton->SetToolTip( _("Help") );
	
	NavigationSizer->Add( HelpButton, 1, wxALL|wxEXPAND, 5 );
	
	GridSizer->Add( NavigationSizer, 1, wxEXPAND, 5 );
	
	Tree = new wxTreeCtrl( this, ID_TREE, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	GridSizer->Add( Tree, 1, wxALL|wxEXPAND, 5 );
	
	BoxSizer->Add( GridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	ShowPublicOnlyButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OutlineViewGeneratedPanelClass::OnShowPublicOnlyClick ), NULL, this );
	ConstantsButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OutlineViewGeneratedPanelClass::OnConstantsClick ), NULL, this );
	PropertiesButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OutlineViewGeneratedPanelClass::OnPropertiesClick ), NULL, this );
	MethodsButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OutlineViewGeneratedPanelClass::OnMethodsClick ), NULL, this );
	AddButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OutlineViewGeneratedPanelClass::OnAddButton ), NULL, this );
	SyncButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OutlineViewGeneratedPanelClass::OnSyncButton ), NULL, this );
	HelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OutlineViewGeneratedPanelClass::OnHelpButton ), NULL, this );
	Tree->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( OutlineViewGeneratedPanelClass::OnTreeItemActivated ), NULL, this );
	Tree->Connect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( OutlineViewGeneratedPanelClass::OnTreeItemRightClick ), NULL, this );
}

OutlineViewGeneratedPanelClass::~OutlineViewGeneratedPanelClass()
{
	// Disconnect Events
	ShowPublicOnlyButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OutlineViewGeneratedPanelClass::OnShowPublicOnlyClick ), NULL, this );
	ConstantsButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OutlineViewGeneratedPanelClass::OnConstantsClick ), NULL, this );
	PropertiesButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OutlineViewGeneratedPanelClass::OnPropertiesClick ), NULL, this );
	MethodsButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OutlineViewGeneratedPanelClass::OnMethodsClick ), NULL, this );
	AddButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OutlineViewGeneratedPanelClass::OnAddButton ), NULL, this );
	SyncButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OutlineViewGeneratedPanelClass::OnSyncButton ), NULL, this );
	HelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OutlineViewGeneratedPanelClass::OnHelpButton ), NULL, this );
	Tree->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( OutlineViewGeneratedPanelClass::OnTreeItemActivated ), NULL, this );
	Tree->Disconnect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( OutlineViewGeneratedPanelClass::OnTreeItemRightClick ), NULL, this );
	
}

FileSearchDialogGeneratedClass::FileSearchDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexSizer;
	FlexSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	FlexSizer->AddGrowableCol( 0 );
	FlexSizer->AddGrowableRow( 2 );
	FlexSizer->SetFlexibleDirection( wxBOTH );
	FlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* InputsSizer;
	InputsSizer = new wxFlexGridSizer( 2, 2, 0, 0 );
	InputsSizer->AddGrowableCol( 1 );
	InputsSizer->AddGrowableRow( 0 );
	InputsSizer->AddGrowableRow( 1 );
	InputsSizer->SetFlexibleDirection( wxBOTH );
	InputsSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	SearchLabel = new wxStaticText( this, ID_SEARCHLABEL, _("Search"), wxDefaultPosition, wxDefaultSize, 0 );
	SearchLabel->Wrap( -1 );
	InputsSizer->Add( SearchLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	SearchText = new wxTextCtrl( this, ID_SEARCHTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	InputsSizer->Add( SearchText, 1, wxALL|wxEXPAND, 5 );
	
	ProjectLabel = new wxStaticText( this, ID_PROJECTLABEL, _("Project"), wxDefaultPosition, wxDefaultSize, 0 );
	ProjectLabel->Wrap( -1 );
	InputsSizer->Add( ProjectLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString ProjectChoiceChoices;
	ProjectChoice = new wxChoice( this, ID_PROJECTCHOICE, wxDefaultPosition, wxDefaultSize, ProjectChoiceChoices, 0 );
	ProjectChoice->SetSelection( 0 );
	InputsSizer->Add( ProjectChoice, 0, wxEXPAND|wxALL, 5 );
	
	FlexSizer->Add( InputsSizer, 1, wxEXPAND, 5 );
	
	MatchesLabel = new wxStaticText( this, ID_MATCHESLABEL, _("Matches"), wxDefaultPosition, wxDefaultSize, 0 );
	MatchesLabel->Wrap( -1 );
	FlexSizer->Add( MatchesLabel, 1, wxALL|wxEXPAND, 5 );
	
	wxArrayString MatchesListChoices;
	MatchesList = new wxCheckListBox( this, ID_MATCHESLIST, wxDefaultPosition, wxDefaultSize, MatchesListChoices, 0 );
	FlexSizer->Add( MatchesList, 1, wxALL|wxEXPAND, 5 );
	
	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonsSizer->AddButton( ButtonsSizerCancel );
	ButtonsSizer->Realize();
	FlexSizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );
	
	BoxSizer->Add( FlexSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	SearchText->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( FileSearchDialogGeneratedClass::OnSearchKeyDown ), NULL, this );
	SearchText->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( FileSearchDialogGeneratedClass::OnSearchText ), NULL, this );
	SearchText->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FileSearchDialogGeneratedClass::OnSearchEnter ), NULL, this );
	ProjectChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( FileSearchDialogGeneratedClass::OnProjectChoice ), NULL, this );
	MatchesList->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( FileSearchDialogGeneratedClass::OnMatchesListDoubleClick ), NULL, this );
	MatchesList->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( FileSearchDialogGeneratedClass::OnMatchesListKeyDown ), NULL, this );
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileSearchDialogGeneratedClass::OnOkButton ), NULL, this );
}

FileSearchDialogGeneratedClass::~FileSearchDialogGeneratedClass()
{
	// Disconnect Events
	SearchText->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( FileSearchDialogGeneratedClass::OnSearchKeyDown ), NULL, this );
	SearchText->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( FileSearchDialogGeneratedClass::OnSearchText ), NULL, this );
	SearchText->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FileSearchDialogGeneratedClass::OnSearchEnter ), NULL, this );
	ProjectChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( FileSearchDialogGeneratedClass::OnProjectChoice ), NULL, this );
	MatchesList->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( FileSearchDialogGeneratedClass::OnMatchesListDoubleClick ), NULL, this );
	MatchesList->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( FileSearchDialogGeneratedClass::OnMatchesListKeyDown ), NULL, this );
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileSearchDialogGeneratedClass::OnOkButton ), NULL, this );
	
}
