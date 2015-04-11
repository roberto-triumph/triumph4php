///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "FileWatcherFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

FileWatcherPreferencesPanelGeneratedClass::FileWatcherPreferencesPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* BodySizer;
	BodySizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	BodySizer->AddGrowableCol( 0 );
	BodySizer->AddGrowableRow( 0 );
	BodySizer->AddGrowableRow( 1 );
	BodySizer->SetFlexibleDirection( wxBOTH );
	BodySizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	HelpText = new wxStaticText( this, wxID_ANY, wxT("Triumph will watch all project directories for changes and update its' tag cache whenever a file has been externally modified. An example of this is when new code is fetched from a repository, as soon as you issue 'git pull' in your terminal Triumph will have parsed the updated files for class names an method names. How cool is that!  You will almost always want this enabled, except in the rare case that you encounter a bug with this feature.\n\nThis feature is experimental in Mac OS X, and should not be enabled if you  have large projects.\n\nEven is this is enabled, if your project sources reside in a networked file system, then Triumph will not watch the source directories. This is because file changes from networked drives are not propagated properly."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( 450 );
	BodySizer->Add( HelpText, 1, wxALL|wxEXPAND, 5 );
	
	Enabled = new wxCheckBox( this, wxID_ANY, wxT("Enable File Watcher"), wxDefaultPosition, wxDefaultSize, 0 );
	BodySizer->Add( Enabled, 0, wxALL, 5 );
	
	this->SetSizer( BodySizer );
	this->Layout();
}

FileWatcherPreferencesPanelGeneratedClass::~FileWatcherPreferencesPanelGeneratedClass()
{
}
