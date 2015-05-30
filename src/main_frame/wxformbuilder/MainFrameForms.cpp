///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "../../widgets/NotebookClass.h"

#include "MainFrameForms.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( MainFrameGeneratedClass, wxFrame )
	EVT_CLOSE( MainFrameGeneratedClass::_wxFB_OnClose )
	EVT_AUINOTEBOOK_PAGE_CHANGED( ID_NOTEBOOK, MainFrameGeneratedClass::_wxFB_OnContentNotebookPageChanged )
	EVT_AUINOTEBOOK_PAGE_CLOSE( ID_NOTEBOOK, MainFrameGeneratedClass::_wxFB_OnContentNotebookPageClose )
	EVT_MENU( wxID_EXIT, MainFrameGeneratedClass::_wxFB_OnFileExit )
	EVT_MENU( wxID_PREFERENCES, MainFrameGeneratedClass::_wxFB_OnEditPreferences )
	EVT_MENU( ID_VIEW_TOGGLE_TOOLS, MainFrameGeneratedClass::_wxFB_OnViewToggleTools )
	EVT_MENU( ID_VIEW_TOGGLE_OUTLINE, MainFrameGeneratedClass::_wxFB_OnViewToggleOutline )
	EVT_MENU( ID_MENUITEMMANUAL, MainFrameGeneratedClass::_wxFB_OnHelpManual )
	EVT_MENU( wxID_ABOUT, MainFrameGeneratedClass::_wxFB_OnHelpAbout )
	EVT_MENU( ID_MENUITEMCREDITS, MainFrameGeneratedClass::_wxFB_OnHelpCredits )
	EVT_MENU( ID_MENUITEMLICENSE, MainFrameGeneratedClass::_wxFB_OnHelpLicense )
END_EVENT_TABLE()

MainFrameGeneratedClass::MainFrameGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* Sizer;
	Sizer = new wxBoxSizer( wxVERTICAL );
	
	Notebook = new t4p::NotebookClass( this, ID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxAUI_NB_CLOSE_ON_ACTIVE_TAB|wxAUI_NB_SCROLL_BUTTONS|wxAUI_NB_TAB_MOVE|wxAUI_NB_WINDOWLIST_BUTTON );
	
	Sizer->Add( Notebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( Sizer );
	this->Layout();
	MenuBar = new wxMenuBar( 0 );
	FileMenu = new wxMenu();
	wxMenuItem* MenuItemFileExit;
	MenuItemFileExit = new wxMenuItem( FileMenu, wxID_EXIT, wxString( _("Exit") ) , _("Exit this program"), wxITEM_NORMAL );
	FileMenu->Append( MenuItemFileExit );
	
	MenuBar->Append( FileMenu, _("&File") ); 
	
	EditMenu = new wxMenu();
	wxMenuItem* MenuItemEditPreferences;
	MenuItemEditPreferences = new wxMenuItem( EditMenu, wxID_PREFERENCES, wxString( _("Preferences") ) , wxEmptyString, wxITEM_NORMAL );
	EditMenu->Append( MenuItemEditPreferences );
	
	MenuBar->Append( EditMenu, _("&Edit") ); 
	
	ViewMenu = new wxMenu();
	wxMenuItem* ToggleTools;
	ToggleTools = new wxMenuItem( ViewMenu, ID_VIEW_TOGGLE_TOOLS, wxString( _("Toggle Tools Pane") ) + wxT('\t') + wxT("CTRL+ALT+T"), wxEmptyString, wxITEM_NORMAL );
	ViewMenu->Append( ToggleTools );
	
	wxMenuItem* ToggleOutline;
	ToggleOutline = new wxMenuItem( ViewMenu, ID_VIEW_TOGGLE_OUTLINE, wxString( _("Toogle Outline Pane") ) + wxT('\t') + wxT("CTRL+ALT+O"), wxEmptyString, wxITEM_NORMAL );
	ViewMenu->Append( ToggleOutline );
	
	MenuBar->Append( ViewMenu, _("&View") ); 
	
	SearchMenu = new wxMenu();
	MenuBar->Append( SearchMenu, _("&Search") ); 
	
	HelpMenu = new wxMenu();
	wxMenuItem* MenuItemManual;
	MenuItemManual = new wxMenuItem( HelpMenu, ID_MENUITEMMANUAL, wxString( _("Online Manual") ) , _("Open a web browser to the online manual"), wxITEM_NORMAL );
	HelpMenu->Append( MenuItemManual );
	
	wxMenuItem* MenuItemAbout;
	MenuItemAbout = new wxMenuItem( HelpMenu, wxID_ABOUT, wxString( _("About") ) , _("About this program"), wxITEM_NORMAL );
	HelpMenu->Append( MenuItemAbout );
	
	wxMenuItem* MenuItemCredits;
	MenuItemCredits = new wxMenuItem( HelpMenu, ID_MENUITEMCREDITS, wxString( _("Credits") ) , wxEmptyString, wxITEM_NORMAL );
	HelpMenu->Append( MenuItemCredits );
	
	wxMenuItem* MenuItemLicense;
	MenuItemLicense = new wxMenuItem( HelpMenu, ID_MENUITEMLICENSE, wxString( _("License") ) , wxEmptyString, wxITEM_NORMAL );
	HelpMenu->Append( MenuItemLicense );
	
	MenuBar->Append( HelpMenu, _("&Help") ); 
	
	this->SetMenuBar( MenuBar );
	
}

MainFrameGeneratedClass::~MainFrameGeneratedClass()
{
}

CreditsDialogClass::CreditsDialogClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* Sizer;
	Sizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer9->AddGrowableCol( 0 );
	fgSizer9->AddGrowableRow( 1 );
	fgSizer9->SetFlexibleDirection( wxBOTH );
	fgSizer9->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText8 = new wxStaticText( this, wxID_ANY, _("Triumph makes uses of the following open source projects. None of these projects are affiliated with Triumph in any way."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer9->Add( m_staticText8, 0, wxALL, 10 );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 9, 2, 0, 0 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText12 = new wxStaticText( this, wxID_ANY, _("wxFormBuilder: A RAD tool for wxWidgets GUI design."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bSizer12->Add( m_staticText12, 0, wxALL, 5 );
	
	m_hyperlink3 = new wxHyperlinkCtrl( this, wxID_ANY, _("http://www.wxformbuilder.org"), wxT("http://www.wxformbuilder.org"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer12->Add( m_hyperlink3, 0, wxALL, 5 );
	
	GridSizer->Add( bSizer12, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText13 = new wxStaticText( this, wxID_ANY, _("re2c: A tool for writing very fast and very flexible scanners.\t"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	bSizer16->Add( m_staticText13, 0, wxALL, 5 );
	
	m_hyperlink4 = new wxHyperlinkCtrl( this, wxID_ANY, _("http://re2c.org/"), wxT("http://re2c.org/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer16->Add( m_hyperlink4, 0, wxALL, 5 );
	
	GridSizer->Add( bSizer16, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText15 = new wxStaticText( this, wxID_ANY, _("SQLite: A library that implements a self-contained SQL database engine."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	bSizer18->Add( m_staticText15, 0, wxALL, 5 );
	
	m_hyperlink6 = new wxHyperlinkCtrl( this, wxID_ANY, _("http://www.sqlite.org/"), wxT("http://www.sqlite.org/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer18->Add( m_hyperlink6, 0, wxALL, 5 );
	
	GridSizer->Add( bSizer18, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText18 = new wxStaticText( this, wxID_ANY, _("Thanks to Google for providing project hosting."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	bSizer13->Add( m_staticText18, 0, wxALL, 5 );
	
	m_hyperlink9 = new wxHyperlinkCtrl( this, wxID_ANY, _("https://code.google.com/"), wxT("https://code.google.com/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer13->Add( m_hyperlink9, 0, wxALL, 5 );
	
	GridSizer->Add( bSizer13, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText17 = new wxStaticText( this, wxID_ANY, _("Premake: Powerfully simple build configuration."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	bSizer14->Add( m_staticText17, 0, wxALL, 5 );
	
	m_hyperlink8 = new wxHyperlinkCtrl( this, wxID_ANY, _("http://industriousone.com/premake"), wxT("http://industriousone.com/premake"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer14->Add( m_hyperlink8, 0, wxALL, 5 );
	
	GridSizer->Add( bSizer14, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText16 = new wxStaticText( this, wxID_ANY, _("SOCI: A database access library for C++."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	bSizer15->Add( m_staticText16, 0, wxALL, 5 );
	
	m_hyperlink7 = new wxHyperlinkCtrl( this, wxID_ANY, _("http://soci.sourceforge.net/"), wxT("http://soci.sourceforge.net/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer15->Add( m_hyperlink7, 0, wxALL, 5 );
	
	GridSizer->Add( bSizer15, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText14 = new wxStaticText( this, wxID_ANY, _("bison: A general-purpose parser generator."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	bSizer17->Add( m_staticText14, 0, wxALL, 5 );
	
	m_hyperlink5 = new wxHyperlinkCtrl( this, wxID_ANY, _("http://www.gnu.org/software/bison/"), wxT("http://www.gnu.org/software/bison/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer17->Add( m_hyperlink5, 0, wxALL, 5 );
	
	GridSizer->Add( bSizer17, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText11 = new wxStaticText( this, wxID_ANY, _("ICU: A library that provides unicode support.\nICU License"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer19->Add( m_staticText11, 0, wxALL, 5 );
	
	m_hyperlink2 = new wxHyperlinkCtrl( this, wxID_ANY, _("http://site.icu-project.org/"), wxT("http://site.icu-project.org/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer19->Add( m_hyperlink2, 0, wxALL, 5 );
	
	GridSizer->Add( bSizer19, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText10 = new wxStaticText( this, wxID_ANY, _("wxWidgets: A C++ crossplatform framework.\nwxWidgets License"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	bSizer20->Add( m_staticText10, 0, wxALL, 5 );
	
	m_hyperlink1 = new wxHyperlinkCtrl( this, wxID_ANY, _("http://www.wxwidgets.org/"), wxT("http://www.wxwidgets.org/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer20->Add( m_hyperlink1, 0, wxALL, 5 );
	
	GridSizer->Add( bSizer20, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText19 = new wxStaticText( this, wxID_ANY, _("Fugue icons copyright Yusuke Kamiyamane under\na Creative Commons Attribution-ShareAlike license"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	bSizer11->Add( m_staticText19, 0, wxALL, 5 );
	
	m_hyperlink10 = new wxHyperlinkCtrl( this, wxID_ANY, _("http://p.yusukekamiyamane.com/"), wxT("http://p.yusukekamiyamane.com/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer11->Add( m_hyperlink10, 0, wxALL, 5 );
	
	m_hyperlink11 = new wxHyperlinkCtrl( this, wxID_ANY, _("http://creativecommons.org/licenses/by/3.0/"), wxT("http://creativecommons.org/licenses/by/3.0/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer11->Add( m_hyperlink11, 0, wxALL, 5 );
	
	GridSizer->Add( bSizer11, 0, wxTOP|wxBOTTOM, 10 );
	
	fgSizer9->Add( GridSizer, 1, wxEXPAND, 5 );
	
	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( this, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2->Realize();
	fgSizer9->Add( m_sdbSizer2, 1, wxEXPAND, 5 );
	
	Sizer->Add( fgSizer9, 1, wxEXPAND, 5 );
	
	this->SetSizer( Sizer );
	this->Layout();
	Sizer->Fit( this );
	
	this->Centre( wxBOTH );
}

CreditsDialogClass::~CreditsDialogClass()
{
}

LicenseDialogClass::LicenseDialogClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* Sizer;
	Sizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer10;
	fgSizer10 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer10->AddGrowableCol( 0 );
	fgSizer10->AddGrowableRow( 0 );
	fgSizer10->SetFlexibleDirection( wxBOTH );
	fgSizer10->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	Text = new wxStaticText( this, ID_TEXT, _("This software is released under the terms of the MIT License. \n\nCopyright (c) 2009 Roberto Perpuly\n\nPermission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n\nThe above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n\nFull licensing terms can be found at"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	Text->Wrap( 375 );
	fgSizer10->Add( Text, 1, wxALL|wxEXPAND, 5 );
	
	LicenseLink = new wxHyperlinkCtrl( this, ID_LICENSELINK, _("http://www.opensource.org/licenses/mit-license.php"), wxT("http://www.opensource.org/licenses/mit-license.php"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	fgSizer10->Add( LicenseLink, 0, wxALL, 5 );
	
	ButtonSizer = new wxStdDialogButtonSizer();
	ButtonSizerOK = new wxButton( this, wxID_OK );
	ButtonSizer->AddButton( ButtonSizerOK );
	ButtonSizer->Realize();
	fgSizer10->Add( ButtonSizer, 1, wxEXPAND, 5 );
	
	Sizer->Add( fgSizer10, 1, wxEXPAND, 5 );
	
	this->SetSizer( Sizer );
	this->Layout();
	Sizer->Fit( this );
	
	this->Centre( wxBOTH );
}

LicenseDialogClass::~LicenseDialogClass()
{
}

BEGIN_EVENT_TABLE( SettingsDirectoryGeneratedPanelClass, wxPanel )
	EVT_RADIOBUTTON( ID_USERDATADIRECTORY, SettingsDirectoryGeneratedPanelClass::_wxFB_OnUserDataDir )
	EVT_RADIOBUTTON( ID_APPLICATIONDIRECTORY, SettingsDirectoryGeneratedPanelClass::_wxFB_OnAppDir )
	EVT_RADIOBUTTON( ID_CUSTOMDIRECTORY, SettingsDirectoryGeneratedPanelClass::_wxFB_OnCustomDir )
END_EVENT_TABLE()

SettingsDirectoryGeneratedPanelClass::SettingsDirectoryGeneratedPanelClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* PanelSizer;
	PanelSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* SettingsSizer;
	SettingsSizer = new wxStaticBoxSizer( new wxStaticBox( this, ID_SETTINGSSIZER, _("Settings Directory") ), wxVERTICAL );
	
	HelpLabel = new wxStaticText( this, ID_HELPLALBEL, _("This is the directory where Triumph will store its settings (projects list, tag cache, and preferences)."), wxDefaultPosition, wxDefaultSize, 0 );
	HelpLabel->Wrap( 450 );
	SettingsSizer->Add( HelpLabel, 0, wxALL, 5 );
	
	UserDataDirectory = new wxRadioButton( this, ID_USERDATADIRECTORY, _("User Data Directory"), wxDefaultPosition, wxDefaultSize, 0 );
	SettingsSizer->Add( UserDataDirectory, 0, wxALL, 5 );
	
	ApplicationDirectory = new wxRadioButton( this, ID_APPLICATIONDIRECTORY, _("Application Directory (Good for portable installations)"), wxDefaultPosition, wxDefaultSize, 0 );
	SettingsSizer->Add( ApplicationDirectory, 0, wxALL, 5 );
	
	CustomDirectory = new wxRadioButton( this, ID_CUSTOMDIRECTORY, _("Custom Directory"), wxDefaultPosition, wxDefaultSize, 0 );
	SettingsSizer->Add( CustomDirectory, 0, wxALL, 5 );
	
	SettingsDirectory = new wxDirPickerCtrl( this, ID_SETTINGSDIRECTORY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_USE_TEXTCTRL, wxDefaultValidator, wxT("Settings directory") );
	SettingsSizer->Add( SettingsDirectory, 0, wxALL|wxEXPAND, 5 );
	
	PanelSizer->Add( SettingsSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( PanelSizer );
	this->Layout();
}

SettingsDirectoryGeneratedPanelClass::~SettingsDirectoryGeneratedPanelClass()
{
}
