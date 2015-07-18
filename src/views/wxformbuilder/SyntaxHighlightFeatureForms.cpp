///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "SyntaxHighlightFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( SyntaxHighlightPanelGeneratedClass, wxPanel )
	EVT_CHOICE( wxID_ANY, SyntaxHighlightPanelGeneratedClass::_wxFB_OnThemeChoice )
	EVT_LISTBOX( wxID_ANY, SyntaxHighlightPanelGeneratedClass::_wxFB_OnListBox )
	EVT_FONTPICKER_CHANGED( wxID_ANY, SyntaxHighlightPanelGeneratedClass::_wxFB_OnFontChanged )
	EVT_COLOURPICKER_CHANGED( ID_FOREGROUND_COLOR, SyntaxHighlightPanelGeneratedClass::_wxFB_OnColorChanged )
	EVT_COLOURPICKER_CHANGED( ID_BACKGROUND_COLOR, SyntaxHighlightPanelGeneratedClass::_wxFB_OnColorChanged )
	EVT_CHECKBOX( ID_BOLD, SyntaxHighlightPanelGeneratedClass::_wxFB_OnCheck )
	EVT_CHECKBOX( ID_ITALIC, SyntaxHighlightPanelGeneratedClass::_wxFB_OnCheck )
END_EVENT_TABLE()

SyntaxHighlightPanelGeneratedClass::SyntaxHighlightPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* Sizer;
	Sizer = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 2 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticBoxSizer* ThemeSizer;
	ThemeSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Theme") ), wxVERTICAL );

	ThemeHelp = new wxStaticText( this, ID_THEMEHELP, _("Selecting one of the pre-defined themes will change all styles according to the selected theme."), wxDefaultPosition, wxDefaultSize, 0 );
	ThemeHelp->Wrap( -1 );
	ThemeSizer->Add( ThemeHelp, 1, wxALL|wxEXPAND, 5 );

	wxString ThemeChoices[] = { _("Dark On Light"), _("Light On Dark") };
	int ThemeNChoices = sizeof( ThemeChoices ) / sizeof( wxString );
	Theme = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, ThemeNChoices, ThemeChoices, 0 );
	Theme->SetSelection( 0 );
	ThemeSizer->Add( Theme, 0, wxALL|wxEXPAND, 5 );

	FlexGridSizer->Add( ThemeSizer, 2, wxEXPAND|wxALL, 10 );

	wxStaticBoxSizer* StaticBoxSizer;
	StaticBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Defined Styles") ), wxHORIZONTAL );

	Styles = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE );
	StaticBoxSizer->Add( Styles, 1, wxEXPAND|wxALL, 8 );

	wxFlexGridSizer* FlexSizer;
	FlexSizer = new wxFlexGridSizer( 4, 2, 0, 0 );
	FlexSizer->AddGrowableCol( 1 );
	FlexSizer->SetFlexibleDirection( wxHORIZONTAL );
	FlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	StyleFontLabel = new wxStaticText( this, wxID_ANY, _("Style Font"), wxDefaultPosition, wxDefaultSize, 0 );
	StyleFontLabel->Wrap( -1 );
	FlexSizer->Add( StyleFontLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	Font = new wxFontPickerCtrl( this, wxID_ANY, wxNullFont, wxDefaultPosition, wxDefaultSize, wxFNTP_DEFAULT_STYLE );
	Font->SetMaxPointSize( 100 );
	FlexSizer->Add( Font, 1, wxALL|wxEXPAND, 5 );

	ForegroundColorLabel = new wxStaticText( this, wxID_ANY, _("Foreground Color"), wxDefaultPosition, wxDefaultSize, 0 );
	ForegroundColorLabel->Wrap( -1 );
	FlexSizer->Add( ForegroundColorLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	ForegroundColor = new wxColourPickerCtrl( this, ID_FOREGROUND_COLOR, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	FlexSizer->Add( ForegroundColor, 1, wxALL|wxEXPAND, 5 );

	BackgroundColorLabel = new wxStaticText( this, wxID_ANY, _("Background Color"), wxDefaultPosition, wxDefaultSize, 0 );
	BackgroundColorLabel->Wrap( -1 );
	FlexSizer->Add( BackgroundColorLabel, 0, wxALL, 5 );

	BackgroundColor = new wxColourPickerCtrl( this, ID_BACKGROUND_COLOR, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	FlexSizer->Add( BackgroundColor, 1, wxALL|wxEXPAND, 5 );

	Bold = new wxCheckBox( this, ID_BOLD, _("Bold"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexSizer->Add( Bold, 0, wxALL, 5 );

	Italic = new wxCheckBox( this, ID_ITALIC, _("Italic"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexSizer->Add( Italic, 0, wxALL, 5 );

	StaticBoxSizer->Add( FlexSizer, 1, wxEXPAND|wxLEFT, 5 );

	FlexGridSizer->Add( StaticBoxSizer, 6, wxEXPAND|wxALL, 10 );

	StyleEditSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	StyleEditSizer->AddGrowableCol( 0 );
	StyleEditSizer->AddGrowableRow( 1 );
	StyleEditSizer->SetFlexibleDirection( wxBOTH );
	StyleEditSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	PreviewLabel = new wxStaticText( this, ID_PREVIEWLABEL, _("Style Preview"), wxDefaultPosition, wxDefaultSize, 0 );
	PreviewLabel->Wrap( -1 );
	StyleEditSizer->Add( PreviewLabel, 1, wxALL|wxEXPAND, 5 );

	PreviewNotebook = new wxAuiNotebook( this, ID_PREVIEWNOTEBOOK, wxDefaultPosition, wxDefaultSize, 0 );

	StyleEditSizer->Add( PreviewNotebook, 1, wxEXPAND | wxALL, 5 );

	FlexGridSizer->Add( StyleEditSizer, 2, wxEXPAND, 5 );

	Sizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );

	this->SetSizer( Sizer );
	this->Layout();
}

SyntaxHighlightPanelGeneratedClass::~SyntaxHighlightPanelGeneratedClass()
{
}
