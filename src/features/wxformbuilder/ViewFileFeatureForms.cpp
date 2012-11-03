///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ViewFileFeatureForms.h"

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
	FlexSizer = new wxFlexGridSizer( 4, 1, 0, 0 );
	FlexSizer->AddGrowableCol( 0 );
	FlexSizer->AddGrowableRow( 2 );
	FlexSizer->AddGrowableRow( 3 );
	FlexSizer->SetFlexibleDirection( wxVERTICAL );
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
	
	wxFlexGridSizer* TemplatesSizer;
	TemplatesSizer = new wxFlexGridSizer( 1, 1, 0, 0 );
	TemplatesSizer->AddGrowableCol( 0 );
	TemplatesSizer->AddGrowableRow( 0 );
	TemplatesSizer->SetFlexibleDirection( wxBOTH );
	TemplatesSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	FileTree = new wxTreeCtrl( this, ID_FILETREE, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_SINGLE );
	TemplatesSizer->Add( FileTree, 1, wxALL|wxEXPAND, 5 );
	
	FlexSizer->Add( TemplatesSizer, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* TemplateVariableSizer;
	TemplateVariableSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	TemplateVariableSizer->AddGrowableCol( 0 );
	TemplateVariableSizer->AddGrowableRow( 1 );
	TemplateVariableSizer->SetFlexibleDirection( wxBOTH );
	TemplateVariableSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	TemplateVariablesLabel = new wxStaticText( this, ID_TEMPLATEVARIABLESLABEL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	TemplateVariablesLabel->Wrap( -1 );
	TemplateVariableSizer->Add( TemplateVariablesLabel, 0, wxALL, 5 );
	
	TemplateVariablesTree = new wxTreeCtrl( this, ID_VARIABLETREE, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	TemplateVariableSizer->Add( TemplateVariablesTree, 1, wxALL|wxALIGN_BOTTOM|wxEXPAND, 5 );
	
	FlexSizer->Add( TemplateVariableSizer, 1, wxEXPAND, 5 );
	
	PanelSizer->Add( FlexSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( PanelSizer );
	this->Layout();
}

ViewFilePanelGeneratedClass::~ViewFilePanelGeneratedClass()
{
}
