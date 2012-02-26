///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ViewFilePluginGeneratedClass.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( ViewFilePanelGeneratedClass, wxPanel )
	EVT_CHOICE( ID_CONTROLLER, ViewFilePanelGeneratedClass::_wxFB_OnControllerChoice )
	EVT_CHOICE( ID_ACTION, ViewFilePanelGeneratedClass::_wxFB_OnActionChoice )
	EVT_BUTTON( ID_HELPBUTTON, ViewFilePanelGeneratedClass::_wxFB_OnHelpButton )
	EVT_BUTTON( ID_CURRENTBUTTON, ViewFilePanelGeneratedClass::_wxFB_OnCurrentButton )
	EVT_TREE_ITEM_ACTIVATED( ID_FILETREE, ViewFilePanelGeneratedClass::_wxFB_OnTreeItemActivated )
END_EVENT_TABLE()

ViewFilePanelGeneratedClass::ViewFilePanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* PanelSizer;
	PanelSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexSizer;
	FlexSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	FlexSizer->AddGrowableCol( 0 );
	FlexSizer->AddGrowableRow( 2 );
	FlexSizer->SetFlexibleDirection( wxBOTH );
	FlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* ChoicesSizer;
	ChoicesSizer = new wxFlexGridSizer( 2, 2, 0, 0 );
	ChoicesSizer->AddGrowableCol( 1 );
	ChoicesSizer->SetFlexibleDirection( wxBOTH );
	ChoicesSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	ControllerLabel = new wxStaticText( this, ID_CONTROLLERLABEL, _("Controller"), wxDefaultPosition, wxDefaultSize, 0 );
	ControllerLabel->Wrap( -1 );
	ChoicesSizer->Add( ControllerLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString ControllerChoices;
	Controller = new wxChoice( this, ID_CONTROLLER, wxDefaultPosition, wxDefaultSize, ControllerChoices, 0 );
	Controller->SetSelection( 0 );
	ChoicesSizer->Add( Controller, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	ActionLabel = new wxStaticText( this, ID_ACTIONLABEL, _("Action"), wxDefaultPosition, wxDefaultSize, 0 );
	ActionLabel->Wrap( -1 );
	ChoicesSizer->Add( ActionLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString ActionChoices;
	Action = new wxChoice( this, ID_ACTION, wxDefaultPosition, wxDefaultSize, ActionChoices, 0 );
	Action->SetSelection( 0 );
	ChoicesSizer->Add( Action, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	FlexSizer->Add( ChoicesSizer, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* MiddleSizer;
	MiddleSizer = new wxFlexGridSizer( 1, 3, 0, 0 );
	MiddleSizer->AddGrowableCol( 2 );
	MiddleSizer->SetFlexibleDirection( wxBOTH );
	MiddleSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HelpButton = new wxBitmapButton( this, ID_HELPBUTTON, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	MiddleSizer->Add( HelpButton, 0, wxALL, 5 );
	
	CurrentButton = new wxButton( this, ID_CURRENTBUTTON, _("Current"), wxDefaultPosition, wxDefaultSize, 0 );
	MiddleSizer->Add( CurrentButton, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	StatusLabel = new wxStaticText( this, ID_ERRORLABEL, _("Error:"), wxDefaultPosition, wxDefaultSize, 0 );
	StatusLabel->Wrap( -1 );
	MiddleSizer->Add( StatusLabel, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	FlexSizer->Add( MiddleSizer, 1, wxEXPAND, 5 );
	
	FileTree = new wxTreeCtrl( this, ID_FILETREE, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT|wxTR_SINGLE );
	FlexSizer->Add( FileTree, 1, wxALL|wxEXPAND, 5 );
	
	PanelSizer->Add( FlexSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( PanelSizer );
	this->Layout();
}

ViewFilePanelGeneratedClass::~ViewFilePanelGeneratedClass()
{
}
