///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ProjectPluginGeneratedPanelClass.h"

///////////////////////////////////////////////////////////////////////////

ProjectPluginGeneratedPanelClass::ProjectPluginGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxStaticBoxSizer* BoxSizer;
	BoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Explorer") ), wxVERTICAL );
	
	wxGridBagSizer* GridBagSizer;
	GridBagSizer = new wxGridBagSizer( 0, 0 );
	GridBagSizer->AddGrowableCol( 1 );
	GridBagSizer->SetFlexibleDirection( wxBOTH );
	GridBagSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HelpText = new wxStaticText( this, wxID_ANY, _("MVC Editor will use this program whenever the \"Explore\" button is clicked."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( -1 );
	GridBagSizer->Add( HelpText, wxGBPosition( 0, 0 ), wxGBSpan( 1, 2 ), wxALL, 5 );
	
	Label = new wxStaticText( this, wxID_ANY, _("Explorer Executable"), wxDefaultPosition, wxDefaultSize, 0 );
	Label->Wrap( -1 );
	GridBagSizer->Add( Label, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	ExplorerExecutable = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	GridBagSizer->Add( ExplorerExecutable, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
	
	FilePicker = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN );
	GridBagSizer->Add( FilePicker, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALL, 5 );
	
	BoxSizer->Add( GridBagSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	FilePicker->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( ProjectPluginGeneratedPanelClass::OnFileChanged ), NULL, this );
}

ProjectPluginGeneratedPanelClass::~ProjectPluginGeneratedPanelClass()
{
	// Disconnect Events
	FilePicker->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( ProjectPluginGeneratedPanelClass::OnFileChanged ), NULL, this );
	
}
