///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "OutlineViewPluginPanelClass.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( OutlineViewPluginGeneratedPanelClass, wxPanel )
	EVT_BUTTON( wxID_HELP, OutlineViewPluginGeneratedPanelClass::_wxFB_OnHelpButton )
	EVT_BUTTON( ID_SYNCBUTTON, OutlineViewPluginGeneratedPanelClass::_wxFB_OnSyncButton )
	EVT_BUTTON( ID_LOOKUPBUTTON, OutlineViewPluginGeneratedPanelClass::_wxFB_OnLookupButton )
	EVT_TEXT_ENTER( ID_LOOKUP, OutlineViewPluginGeneratedPanelClass::_wxFB_OnLookupButton )
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
	NavigationSizer = new wxFlexGridSizer( 1, 4, 0, 0 );
	NavigationSizer->AddGrowableCol( 3 );
	NavigationSizer->AddGrowableRow( 0 );
	NavigationSizer->SetFlexibleDirection( wxBOTH );
	NavigationSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HelpButton = new wxBitmapButton( this, wxID_HELP, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	NavigationSizer->Add( HelpButton, 1, wxALL|wxEXPAND, 5 );
	
	SyncButton = new wxButton( this, ID_SYNCBUTTON, _("Sync Outline"), wxDefaultPosition, wxDefaultSize, 0 );
	NavigationSizer->Add( SyncButton, 1, wxALL|wxEXPAND, 5 );
	
	LookupButton = new wxButton( this, ID_LOOKUPBUTTON, _("Lookup"), wxDefaultPosition, wxDefaultSize, 0 );
	NavigationSizer->Add( LookupButton, 1, wxALL|wxEXPAND, 5 );
	
	Lookup = new wxTextCtrl( this, ID_LOOKUP, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	NavigationSizer->Add( Lookup, 1, wxALL|wxEXPAND, 5 );
	
	GridSizer->Add( NavigationSizer, 1, wxEXPAND, 5 );
	
	OutlineSizer = new wxFlexGridSizer( 1, 2, 0, 0 );
	OutlineSizer->AddGrowableCol( 0 );
	OutlineSizer->AddGrowableCol( 1 );
	OutlineSizer->AddGrowableCol( 2 );
	OutlineSizer->AddGrowableRow( 0 );
	OutlineSizer->SetFlexibleDirection( wxBOTH );
	OutlineSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	GridSizer->Add( OutlineSizer, 1, wxEXPAND, 5 );
	
	BoxSizer->Add( GridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
}

OutlineViewPluginGeneratedPanelClass::~OutlineViewPluginGeneratedPanelClass()
{
}
