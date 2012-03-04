///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ResourcePluginGeneratedClass.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( ResourcePluginGeneratedPanelClass, wxPanel )
	EVT_COMBOBOX( ID_FILES_COMBO, ResourcePluginGeneratedPanelClass::_wxFB_OnFilesComboCombobox )
	EVT_TEXT_ENTER( ID_FILES_COMBO, ResourcePluginGeneratedPanelClass::_wxFB_OnFilesComboTextEnter )
	EVT_BUTTON( ID_HELP_BUTTON, ResourcePluginGeneratedPanelClass::_wxFB_OnHelpButtonClick )
END_EVENT_TABLE()

ResourcePluginGeneratedPanelClass::ResourcePluginGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 1, 3, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	FilesCombo = new wxComboBox( this, ID_FILES_COMBO, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER ); 
	GridSizer->Add( FilesCombo, 0, wxALIGN_TOP|wxALL|wxEXPAND, 5 );
	
	HelpButton = new wxBitmapButton( this, ID_HELP_BUTTON, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	GridSizer->Add( HelpButton, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( GridSizer );
	this->Layout();
}

ResourcePluginGeneratedPanelClass::~ResourcePluginGeneratedPanelClass()
{
}

BEGIN_EVENT_TABLE( IndexingDialogGeneratedClass, wxDialog )
	EVT_BUTTON( ID_HIDEBUTTON, IndexingDialogGeneratedClass::_wxFB_OnHideButton )
END_EVENT_TABLE()

IndexingDialogGeneratedClass::IndexingDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* BodySizer;
	BodySizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexSizer;
	FlexSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexSizer->AddGrowableCol( 0 );
	FlexSizer->AddGrowableRow( 0 );
	FlexSizer->SetFlexibleDirection( wxBOTH );
	FlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	Gauge = new wxGauge( this, ID_GAUGE, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	FlexSizer->Add( Gauge, 1, wxALL|wxEXPAND, 5 );
	
	HideButton = new wxButton( this, ID_HIDEBUTTON, _("Hide"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexSizer->Add( HideButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	BodySizer->Add( FlexSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
	
	this->Centre( wxBOTH );
}

IndexingDialogGeneratedClass::~IndexingDialogGeneratedClass()
{
}
