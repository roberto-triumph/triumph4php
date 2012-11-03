///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "OutlineViewFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( OutlineViewPluginGeneratedPanelClass, wxPanel )
	EVT_BUTTON( wxID_HELP, OutlineViewPluginGeneratedPanelClass::_wxFB_OnHelpButton )
	EVT_BUTTON( ID_SYNCBUTTON, OutlineViewPluginGeneratedPanelClass::_wxFB_OnSyncButton )
	EVT_CHOICE( ID_CHOICE, OutlineViewPluginGeneratedPanelClass::_wxFB_OnChoice )
	EVT_TREE_ITEM_ACTIVATED( ID_TREE, OutlineViewPluginGeneratedPanelClass::_wxFB_OnTreeItemActivated )
END_EVENT_TABLE()

OutlineViewPluginGeneratedPanelClass::OutlineViewPluginGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
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
	NavigationSizer = new wxFlexGridSizer( 1, 3, 0, 0 );
	NavigationSizer->AddGrowableCol( 2 );
	NavigationSizer->AddGrowableRow( 0 );
	NavigationSizer->SetFlexibleDirection( wxBOTH );
	NavigationSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	NavigationSizer->Add( HelpButton, 1, wxALL|wxEXPAND, 5 );
	
	SyncButton = new wxButton( this, ID_SYNCBUTTON, _("Sync With Editor"), wxDefaultPosition, wxDefaultSize, 0 );
	NavigationSizer->Add( SyncButton, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString ChoiceChoices;
	Choice = new wxChoice( this, ID_CHOICE, wxDefaultPosition, wxDefaultSize, ChoiceChoices, 0 );
	Choice->SetSelection( 0 );
	NavigationSizer->Add( Choice, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	GridSizer->Add( NavigationSizer, 1, wxEXPAND, 5 );
	
	OutlineSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	OutlineSizer->AddGrowableCol( 0 );
	OutlineSizer->AddGrowableRow( 1 );
	OutlineSizer->SetFlexibleDirection( wxBOTH );
	OutlineSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	StatusLabel = new wxStaticText( this, ID_STATUSLABEL, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	StatusLabel->Wrap( -1 );
	OutlineSizer->Add( StatusLabel, 1, wxALL|wxEXPAND, 5 );
	
	Tree = new wxTreeCtrl( this, ID_TREE, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	OutlineSizer->Add( Tree, 1, wxALL|wxEXPAND, 5 );
	
	GridSizer->Add( OutlineSizer, 1, wxEXPAND, 5 );
	
	BoxSizer->Add( GridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
}

OutlineViewPluginGeneratedPanelClass::~OutlineViewPluginGeneratedPanelClass()
{
}
