///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "FinderPluginGeneratedClass.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( FinderPanelGeneratedClass, wxPanel )
	EVT_BUTTON( ID_FIND_CLOSE, FinderPanelGeneratedClass::_wxFB_OnCloseButton )
	EVT_BUTTON( wxID_BACKWARD, FinderPanelGeneratedClass::_wxFB_OnPreviousButton )
	EVT_BUTTON( wxID_FORWARD, FinderPanelGeneratedClass::_wxFB_OnNextButton )
	EVT_BUTTON( wxID_HELP, FinderPanelGeneratedClass::_wxFB_OnHelpButton )
	EVT_BUTTON( ID_FIND_REG_EX_HELP, FinderPanelGeneratedClass::_wxFB_OnRegExFindHelpButton )
	EVT_TEXT_ENTER( ID_FINDTEXT, FinderPanelGeneratedClass::_wxFB_OnFindEnter )
END_EVENT_TABLE()

FinderPanelGeneratedClass::FinderPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* FindSizer;
	FindSizer = new wxBoxSizer( wxHORIZONTAL );
	
	CloseButton = new wxBitmapButton( this, ID_FIND_CLOSE, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	CloseButton->SetToolTip( _("Hide this panel") );
	
	CloseButton->SetToolTip( _("Hide this panel") );
	
	FindSizer->Add( CloseButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	PreviousButton = new wxBitmapButton( this, wxID_BACKWARD, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	PreviousButton->SetToolTip( _("Advance to previous hit") );
	
	PreviousButton->SetToolTip( _("Advance to previous hit") );
	
	FindSizer->Add( PreviousButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	NextButton = new wxBitmapButton( this, wxID_FORWARD, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	NextButton->SetToolTip( _("Advance to next hit") );
	
	NextButton->SetToolTip( _("Advance to next hit") );
	
	FindSizer->Add( NextButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	HelpButton->SetToolTip( _("Help") );
	
	HelpButton->SetToolTip( _("Help") );
	
	FindSizer->Add( HelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	FindLabel = new wxStaticText( this, wxID_ANY, _("Find:"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	FindLabel->Wrap( -1 );
	FindSizer->Add( FindLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	RegExFindHelpButton = new wxButton( this, ID_FIND_REG_EX_HELP, _(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	RegExFindHelpButton->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Sans") ) );
	
	FindSizer->Add( RegExFindHelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	FindText = new wxComboBox( this, ID_FINDTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER ); 
	FindSizer->Add( FindText, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	Wrap = new wxCheckBox( this, ID_WRAP, _("Wrap"), wxDefaultPosition, wxDefaultSize, 0 );
	FindSizer->Add( Wrap, 0, wxALL|wxEXPAND, 5 );
	
	wxString FinderModeChoices[] = { _("Exact"), _("Regular Expression") };
	int FinderModeNChoices = sizeof( FinderModeChoices ) / sizeof( wxString );
	FinderMode = new wxRadioBox( this, wxID_ANY, _("Mode"), wxDefaultPosition, wxDefaultSize, FinderModeNChoices, FinderModeChoices, 3, wxRA_SPECIFY_COLS );
	FinderMode->SetSelection( 0 );
	FindSizer->Add( FinderMode, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	ResultText = new wxStaticText( this, wxID_ANY, _("Status: <OK>"), wxDefaultPosition, wxSize( -1,-1 ), wxALIGN_RIGHT );
	ResultText->Wrap( -1 );
	FindSizer->Add( ResultText, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	BoxSizer->Add( FindSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
}

FinderPanelGeneratedClass::~FinderPanelGeneratedClass()
{
}

BEGIN_EVENT_TABLE( ReplacePanelGeneratedClass, wxPanel )
	EVT_BUTTON( ID_FIND_REG_EX_HELP, ReplacePanelGeneratedClass::_wxFB_OnRegExFindHelpButton )
	EVT_TEXT_ENTER( ID_FINDTEXT, ReplacePanelGeneratedClass::_wxFB_OnFindEnter )
	EVT_BUTTON( ID_REPLACE_REG_EX_HELP, ReplacePanelGeneratedClass::_wxFB_OnReplaceRegExFindHelpButton )
	EVT_TEXT_ENTER( ID_REPLACEWITHTEXT, ReplacePanelGeneratedClass::_wxFB_OnReplaceEnter )
	EVT_BUTTON( ID_REPLACE_CLOSE, ReplacePanelGeneratedClass::_wxFB_OnCloseButton )
	EVT_BUTTON( wxID_BACKWARD, ReplacePanelGeneratedClass::_wxFB_OnPreviousButton )
	EVT_BUTTON( wxID_FORWARD, ReplacePanelGeneratedClass::_wxFB_OnNextButton )
	EVT_BUTTON( wxID_HELP, ReplacePanelGeneratedClass::_wxFB_OnHelpButton )
	EVT_BUTTON( wxID_REPLACE, ReplacePanelGeneratedClass::_wxFB_OnReplaceButton )
	EVT_BUTTON( wxID_REPLACE_ALL, ReplacePanelGeneratedClass::_wxFB_OnReplaceAllButton )
	EVT_BUTTON( wxID_UNDO, ReplacePanelGeneratedClass::_wxFB_OnUndoButton )
END_EVENT_TABLE()

ReplacePanelGeneratedClass::ReplacePanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* FindSizer;
	FindSizer = new wxBoxSizer( wxHORIZONTAL );
	
	FindLabel = new wxStaticText( this, wxID_ANY, _("Find:"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	FindLabel->Wrap( -1 );
	FindSizer->Add( FindLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	RegExFindHelpButton = new wxButton( this, ID_FIND_REG_EX_HELP, _(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	RegExFindHelpButton->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Sans") ) );
	
	FindSizer->Add( RegExFindHelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	FindText = new wxComboBox( this, ID_FINDTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER ); 
	FindSizer->Add( FindText, 1, wxALL|wxEXPAND, 5 );
	
	ReplaceWithLabel = new wxStaticText( this, wxID_ANY, _("Replace With:"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	ReplaceWithLabel->Wrap( -1 );
	FindSizer->Add( ReplaceWithLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	RegExReplaceHelpButton = new wxButton( this, ID_REPLACE_REG_EX_HELP, _(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	RegExReplaceHelpButton->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Sans") ) );
	
	FindSizer->Add( RegExReplaceHelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	ReplaceWithText = new wxComboBox( this, ID_REPLACEWITHTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER ); 
	FindSizer->Add( ReplaceWithText, 1, wxALL, 5 );
	
	BoxSizer->Add( FindSizer, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* MiddleSizer;
	MiddleSizer = new wxFlexGridSizer( 1, 3, 0, 0 );
	MiddleSizer->AddGrowableCol( 1 );
	MiddleSizer->AddGrowableCol( 2 );
	MiddleSizer->SetFlexibleDirection( wxBOTH );
	MiddleSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* StatusSizer;
	StatusSizer = new wxBoxSizer( wxHORIZONTAL );
	
	CloseButton = new wxBitmapButton( this, ID_REPLACE_CLOSE, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	CloseButton->SetToolTip( _("Hide this panel") );
	
	CloseButton->SetToolTip( _("Hide this panel") );
	
	StatusSizer->Add( CloseButton, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	PreviousButton = new wxBitmapButton( this, wxID_BACKWARD, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	PreviousButton->SetToolTip( _("Advance to previous hit") );
	
	PreviousButton->SetToolTip( _("Advance to previous hit") );
	
	StatusSizer->Add( PreviousButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	NextButton = new wxBitmapButton( this, wxID_FORWARD, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	NextButton->SetToolTip( _("Advance to next hit") );
	
	NextButton->SetToolTip( _("Advance to next hit") );
	
	StatusSizer->Add( NextButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	HelpButton->SetToolTip( _("Help") );
	
	HelpButton->SetToolTip( _("Help") );
	
	StatusSizer->Add( HelpButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	ReplaceButton = new wxBitmapButton( this, wxID_REPLACE, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	ReplaceButton->Enable( false );
	ReplaceButton->SetToolTip( _("Replace text and advance to next hit") );
	
	ReplaceButton->Enable( false );
	ReplaceButton->SetToolTip( _("Replace text and advance to next hit") );
	
	StatusSizer->Add( ReplaceButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	ReplaceAllButton = new wxBitmapButton( this, wxID_REPLACE_ALL, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	ReplaceAllButton->SetToolTip( _("Replace all hits") );
	
	ReplaceAllButton->SetToolTip( _("Replace all hits") );
	
	StatusSizer->Add( ReplaceAllButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	UndoButton = new wxBitmapButton( this, wxID_UNDO, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	UndoButton->Enable( false );
	UndoButton->SetToolTip( _("Undo previous replace") );
	
	UndoButton->Enable( false );
	UndoButton->SetToolTip( _("Undo previous replace") );
	
	StatusSizer->Add( UndoButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	Wrap = new wxCheckBox( this, ID_WRAP, _("Wrap"), wxDefaultPosition, wxDefaultSize, 0 );
	StatusSizer->Add( Wrap, 0, wxALL|wxEXPAND, 5 );
	
	ResultText = new wxStaticText( this, wxID_ANY, _("Status: <OK>"), wxDefaultPosition, wxSize( 220,-1 ), wxALIGN_RIGHT );
	ResultText->Wrap( -1 );
	StatusSizer->Add( ResultText, 1, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	MiddleSizer->Add( StatusSizer, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	wxBoxSizer* ModeSizer;
	ModeSizer = new wxBoxSizer( wxVERTICAL );
	
	wxString FinderModeChoices[] = { _("Exact"), _("Regular Expression") };
	int FinderModeNChoices = sizeof( FinderModeChoices ) / sizeof( wxString );
	FinderMode = new wxRadioBox( this, wxID_ANY, _("Mode"), wxDefaultPosition, wxDefaultSize, FinderModeNChoices, FinderModeChoices, 1, wxRA_SPECIFY_ROWS );
	FinderMode->SetSelection( 0 );
	ModeSizer->Add( FinderMode, 1, wxALL|wxEXPAND, 5 );
	
	MiddleSizer->Add( ModeSizer, 1, wxEXPAND, 5 );
	
	BoxSizer->Add( MiddleSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
}

ReplacePanelGeneratedClass::~ReplacePanelGeneratedClass()
{
}
