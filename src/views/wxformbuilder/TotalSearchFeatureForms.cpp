///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "TotalSearchFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

TotalSearchDialogGeneratedClass::TotalSearchDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 6, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 4 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	SearchLabel = new wxStaticText( this, ID_SEARCHLABEL, _("Search String"), wxDefaultPosition, wxDefaultSize, 0 );
	SearchLabel->Wrap( -1 );
	FlexGridSizer->Add( SearchLabel, 0, wxALL|wxEXPAND, 5 );

	SearchText = new wxTextCtrl( this, ID_SEARCHTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	FlexGridSizer->Add( SearchText, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	FlexGridSizer->Add( bSizer4, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	MatchesLabel = new wxStaticText( this, wxID_ANY, _("No matches found"), wxDefaultPosition, wxDefaultSize, 0 );
	MatchesLabel->Wrap( -1 );
	bSizer5->Add( MatchesLabel, 1, wxALL, 5 );

	CacheStatusLabel = new wxStaticText( this, wxID_ANY, _("Cache Status: OK       "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	CacheStatusLabel->Wrap( -1 );
	bSizer5->Add( CacheStatusLabel, 1, wxALL|wxEXPAND, 5 );

	FlexGridSizer->Add( bSizer5, 1, wxEXPAND, 5 );

	wxBoxSizer* ChecklistSizer;
	ChecklistSizer = new wxBoxSizer( wxHORIZONTAL );

	wxArrayString MatchesListChoices;
	MatchesList = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, MatchesListChoices, wxLB_SINGLE );
	ChecklistSizer->Add( MatchesList, 1, wxALL|wxEXPAND, 5 );

	FlexGridSizer->Add( ChecklistSizer, 1, wxEXPAND, 5 );

	ButtonsSizer = new wxStdDialogButtonSizer();
	ButtonsSizerOK = new wxButton( this, wxID_OK );
	ButtonsSizer->AddButton( ButtonsSizerOK );
	ButtonsSizerCancel = new wxButton( this, wxID_CANCEL );
	ButtonsSizer->AddButton( ButtonsSizerCancel );
	ButtonsSizerHelp = new wxButton( this, wxID_HELP );
	ButtonsSizer->AddButton( ButtonsSizerHelp );
	ButtonsSizer->Realize();
	FlexGridSizer->Add( ButtonsSizer, 1, wxEXPAND, 5 );

	BoxSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );

	this->SetSizer( BoxSizer );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	SearchText->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( TotalSearchDialogGeneratedClass::OnSearchKeyDown ), NULL, this );
	SearchText->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnSearchEnter ), NULL, this );
	MatchesList->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnMatchesListDoubleClick ), NULL, this );
	MatchesList->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( TotalSearchDialogGeneratedClass::OnMatchesListKeyDown ), NULL, this );
	ButtonsSizerCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnHelpButton ), NULL, this );
	ButtonsSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnOkButton ), NULL, this );
}

TotalSearchDialogGeneratedClass::~TotalSearchDialogGeneratedClass()
{
	// Disconnect Events
	SearchText->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( TotalSearchDialogGeneratedClass::OnSearchKeyDown ), NULL, this );
	SearchText->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnSearchEnter ), NULL, this );
	MatchesList->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnMatchesListDoubleClick ), NULL, this );
	MatchesList->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( TotalSearchDialogGeneratedClass::OnMatchesListKeyDown ), NULL, this );
	ButtonsSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnCancelButton ), NULL, this );
	ButtonsSizerHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnHelpButton ), NULL, this );
	ButtonsSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TotalSearchDialogGeneratedClass::OnOkButton ), NULL, this );

}

TotalSearchHelpDialogGeneratedClass::TotalSearchHelpDialogGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	HelpText = new wxStaticText( this, wxID_ANY, _("Use this dialog to quickly locate a class, file, function, or method in any of your enabled projects.\n\nType in a file name, file name:page number,\nclass name,  or class name::method name. The resulting page will then be opened.\n\nExamples:\n\nuser.php\nuser.php:129\nUser\nUser::login\n\nYou can search with partial files names\n\nser.php (would match user.php)\n\nYou can search with partial class names\n\nUse (would match Use, User, UserClass, ...)\n\nYou can search entire class names\nUser:: (would match all methods including inherited methods, from User class)\n\nYou can search all methods\n::print (would match all methods in all classes that start with 'print' )\n\nYou can also search for any database tables found in any of your configured connections\n\nuse (would match on tables that start with \"use\": \"user\", \"user_roles\", ...)\n\nCache Status:\nA search cannot be made while indexing is taking place. If indexing is taking place\nthen you will see the status as \"indexing\". Please wait until indexing completes"), wxDefaultPosition, wxDefaultSize, 0 );
	HelpText->Wrap( 640 );
	fgSizer2->Add( HelpText, 0, wxALL, 5 );

	HelpLink = new wxHyperlinkCtrl( this, wxID_ANY, _("More about Total Search in Triumph 4 PHP"), wxT("http://docs.triumph4php.com/total-search/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	fgSizer2->Add( HelpLink, 0, wxALL, 5 );

	ButtonSizer = new wxStdDialogButtonSizer();
	ButtonSizerOK = new wxButton( this, wxID_OK );
	ButtonSizer->AddButton( ButtonSizerOK );
	ButtonSizer->Realize();
	fgSizer2->Add( ButtonSizer, 1, wxEXPAND, 5 );

	this->SetSizer( fgSizer2 );
	this->Layout();
	fgSizer2->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	ButtonSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TotalSearchHelpDialogGeneratedClass::OnOkButton ), NULL, this );
}

TotalSearchHelpDialogGeneratedClass::~TotalSearchHelpDialogGeneratedClass()
{
	// Disconnect Events
	ButtonSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TotalSearchHelpDialogGeneratedClass::OnOkButton ), NULL, this );

}
