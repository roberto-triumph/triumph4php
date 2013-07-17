///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
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
	EVT_MENU( ID_FILE_PHP_NEW, MainFrameGeneratedClass::_wxFB_OnFilePhpNew )
	EVT_MENU( ID_FILE_SQL_NEW, MainFrameGeneratedClass::_wxFB_OnFileSqlNew )
	EVT_MENU( ID_FILE_CSS_NEW, MainFrameGeneratedClass::_wxFB_OnFileCssNew )
	EVT_MENU( ID_FILE_TEXT_NEW_FILE, MainFrameGeneratedClass::_wxFB_OnFileTextNew )
	EVT_MENU( wxID_OPEN, MainFrameGeneratedClass::_wxFB_OnFileOpen )
	EVT_MENU( wxID_SAVE, MainFrameGeneratedClass::_wxFB_OnFileSave )
	EVT_MENU( wxID_SAVEAS, MainFrameGeneratedClass::_wxFB_OnFileSaveAs )
	EVT_MENU( ID_FILE_SAVE_ALL, MainFrameGeneratedClass::_wxFB_OnFileSaveAll )
	EVT_MENU( ID_FILE_REVERT, MainFrameGeneratedClass::_wxFB_OnFileRevert )
	EVT_MENU( ID_FILE_CLOSE, MainFrameGeneratedClass::_wxFB_OnFileClose )
	EVT_MENU( wxID_EXIT, MainFrameGeneratedClass::_wxFB_OnFileExit )
	EVT_MENU( wxID_CUT, MainFrameGeneratedClass::_wxFB_OnEditCut )
	EVT_MENU( wxID_COPY, MainFrameGeneratedClass::_wxFB_OnEditCopy )
	EVT_MENU( wxID_PASTE, MainFrameGeneratedClass::_wxFB_OnEditPaste )
	EVT_MENU( wxID_SELECTALL, MainFrameGeneratedClass::_wxFB_OnEditSelectAll )
	EVT_MENU( ID_EDIT_CONTENT_ASSIST, MainFrameGeneratedClass::_wxFB_OnEditContentAssist )
	EVT_MENU( ID_EDIT_CALL_TIP, MainFrameGeneratedClass::_wxFB_OnEditCallTip )
	EVT_MENU( ID_EDIT_PREFERENCES, MainFrameGeneratedClass::_wxFB_OnEditPreferences )
	EVT_MENU( ID_ABOUT, MainFrameGeneratedClass::_wxFB_OnHelpAbout )
	EVT_MENU( ID_MENUITEMCREDITS, MainFrameGeneratedClass::_wxFB_OnHelpCredits )
	EVT_MENU( ID_MENUITEMLICENSE, MainFrameGeneratedClass::_wxFB_OnHelpLicense )
END_EVENT_TABLE()

MainFrameGeneratedClass::MainFrameGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* Sizer;
	Sizer = new wxBoxSizer( wxVERTICAL );
	
	Notebook = new mvceditor::NotebookClass( this, ID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxAUI_NB_CLOSE_ON_ACTIVE_TAB|wxAUI_NB_SCROLL_BUTTONS|wxAUI_NB_TAB_MOVE|wxAUI_NB_WINDOWLIST_BUTTON );
	
	Sizer->Add( Notebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( Sizer );
	this->Layout();
	MenuBar = new wxMenuBar( 0 );
	FileMenu = new wxMenu();
	wxMenuItem* MenuItemFilePhpNew;
	MenuItemFilePhpNew = new wxMenuItem( FileMenu, ID_FILE_PHP_NEW, wxString( _("New &PHP File") ) + wxT('\t') + wxT("CTRL+N"), _("Create a new PHP File"), wxITEM_NORMAL );
	FileMenu->Append( MenuItemFilePhpNew );
	
	wxMenuItem* MenuItemFileSqlNew;
	MenuItemFileSqlNew = new wxMenuItem( FileMenu, ID_FILE_SQL_NEW, wxString( _("New S&QL File") ) , _("Create a new SQL File"), wxITEM_NORMAL );
	FileMenu->Append( MenuItemFileSqlNew );
	
	wxMenuItem* MenuItemFileCssNew;
	MenuItemFileCssNew = new wxMenuItem( FileMenu, ID_FILE_CSS_NEW, wxString( _("New &CSS File") ) , _("Create a new CSS File"), wxITEM_NORMAL );
	FileMenu->Append( MenuItemFileCssNew );
	
	wxMenuItem* MenuItemFileTextNew;
	MenuItemFileTextNew = new wxMenuItem( FileMenu, ID_FILE_TEXT_NEW_FILE, wxString( _("New Te&xt File") ) , _("Create a new text file"), wxITEM_NORMAL );
	FileMenu->Append( MenuItemFileTextNew );
	
	m_separator4 = FileMenu->AppendSeparator();
	
	wxMenuItem* MenuItemFileOpen;
	MenuItemFileOpen = new wxMenuItem( FileMenu, wxID_OPEN, wxString( _("&Open") ) , _("Open a file"), wxITEM_NORMAL );
	FileMenu->Append( MenuItemFileOpen );
	
	wxMenuItem* MenuItemFileSave;
	MenuItemFileSave = new wxMenuItem( FileMenu, wxID_SAVE, wxString( _("Save") ) + wxT('\t') + wxT("CTRL+S"), _("Save the current file to disk"), wxITEM_NORMAL );
	FileMenu->Append( MenuItemFileSave );
	MenuItemFileSave->Enable( false );
	
	wxMenuItem* MenuItemFileSaveAs;
	MenuItemFileSaveAs = new wxMenuItem( FileMenu, wxID_SAVEAS, wxString( _("Save &As") ) , wxEmptyString, wxITEM_NORMAL );
	FileMenu->Append( MenuItemFileSaveAs );
	MenuItemFileSaveAs->Enable( false );
	
	wxMenuItem* MenuItemSaveAll;
	MenuItemSaveAll = new wxMenuItem( FileMenu, ID_FILE_SAVE_ALL, wxString( _("Save A&ll") ) + wxT('\t') + wxT("CTRL+SHIFT+S"), wxEmptyString, wxITEM_NORMAL );
	FileMenu->Append( MenuItemSaveAll );
	MenuItemSaveAll->Enable( false );
	
	wxMenuItem* MenuItemRevert;
	MenuItemRevert = new wxMenuItem( FileMenu, ID_FILE_REVERT, wxString( _("Revert") ) , _("Reload the file from Disk"), wxITEM_NORMAL );
	FileMenu->Append( MenuItemRevert );
	MenuItemRevert->Enable( false );
	
	wxMenuItem* MenuItemFileClose;
	MenuItemFileClose = new wxMenuItem( FileMenu, ID_FILE_CLOSE, wxString( _("Close File") ) + wxT('\t') + wxT("CTRL+F4"), _("Close the current file"), wxITEM_NORMAL );
	FileMenu->Append( MenuItemFileClose );
	MenuItemFileClose->Enable( false );
	
	wxMenuItem* MenuItemFileExit;
	MenuItemFileExit = new wxMenuItem( FileMenu, wxID_EXIT, wxString( _("Exit") ) + wxT('\t') + wxT("ALT+F4"), _("Exit this program"), wxITEM_NORMAL );
	FileMenu->Append( MenuItemFileExit );
	
	MenuBar->Append( FileMenu, _("&File") ); 
	
	EditMenu = new wxMenu();
	wxMenuItem* MenuItemEditCut;
	MenuItemEditCut = new wxMenuItem( EditMenu, wxID_CUT, wxString( _("Cut") ) + wxT('\t') + wxT("CTRL+X"), _("Cut the selected text"), wxITEM_NORMAL );
	EditMenu->Append( MenuItemEditCut );
	
	wxMenuItem* MenuItemEditCopy;
	MenuItemEditCopy = new wxMenuItem( EditMenu, wxID_COPY, wxString( _("Copy") ) + wxT('\t') + wxT("CTRL+C"), _("Copy the selected text to the clipboard"), wxITEM_NORMAL );
	EditMenu->Append( MenuItemEditCopy );
	
	wxMenuItem* MenuItemEditPaste;
	MenuItemEditPaste = new wxMenuItem( EditMenu, wxID_PASTE, wxString( _("Paste") ) + wxT('\t') + wxT("CTRL+V"), _("Paste the text from the clipboard"), wxITEM_NORMAL );
	EditMenu->Append( MenuItemEditPaste );
	
	wxMenuItem* MenuItemSelectAll;
	MenuItemSelectAll = new wxMenuItem( EditMenu, wxID_SELECTALL, wxString( _("Select All") ) + wxT('\t') + wxT("CTRL+A"), _("Select Entire Text"), wxITEM_NORMAL );
	EditMenu->Append( MenuItemSelectAll );
	
	m_separator3 = EditMenu->AppendSeparator();
	
	wxMenuItem* MenuItemEditContentAssist;
	MenuItemEditContentAssist = new wxMenuItem( EditMenu, ID_EDIT_CONTENT_ASSIST, wxString( _("Content Assist") ) + wxT('\t') + wxT("CTRL+Space"), wxEmptyString, wxITEM_NORMAL );
	EditMenu->Append( MenuItemEditContentAssist );
	MenuItemEditContentAssist->Enable( false );
	
	wxMenuItem* MenuItemEditCallTip;
	MenuItemEditCallTip = new wxMenuItem( EditMenu, ID_EDIT_CALL_TIP, wxString( _("Display Call Tip") ) + wxT('\t') + wxT("CTRL+SHIFT+SPACE"), wxEmptyString, wxITEM_NORMAL );
	EditMenu->Append( MenuItemEditCallTip );
	MenuItemEditCallTip->Enable( false );
	
	wxMenuItem* MenuItemEditPreferences;
	MenuItemEditPreferences = new wxMenuItem( EditMenu, ID_EDIT_PREFERENCES, wxString( _("Preferences") ) , wxEmptyString, wxITEM_NORMAL );
	EditMenu->Append( MenuItemEditPreferences );
	
	MenuBar->Append( EditMenu, _("&Edit") ); 
	
	ViewMenu = new wxMenu();
	MenuBar->Append( ViewMenu, _("&View") ); 
	
	SearchMenu = new wxMenu();
	MenuBar->Append( SearchMenu, _("&Search") ); 
	
	HelpMenu = new wxMenu();
	wxMenuItem* MenuItemAbout;
	MenuItemAbout = new wxMenuItem( HelpMenu, ID_ABOUT, wxString( _("About") ) , _("About this program"), wxITEM_NORMAL );
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

BEGIN_EVENT_TABLE( EditColorsPanelGeneratedClass, wxPanel )
	EVT_CHOICE( wxID_ANY, EditColorsPanelGeneratedClass::_wxFB_OnThemeChoice )
	EVT_LISTBOX( wxID_ANY, EditColorsPanelGeneratedClass::_wxFB_OnListBox )
	EVT_FONTPICKER_CHANGED( wxID_ANY, EditColorsPanelGeneratedClass::_wxFB_OnFontChanged )
	EVT_COLOURPICKER_CHANGED( ID_FOREGROUND_COLOR, EditColorsPanelGeneratedClass::_wxFB_OnColorChanged )
	EVT_COLOURPICKER_CHANGED( ID_BACKGROUND_COLOR, EditColorsPanelGeneratedClass::_wxFB_OnColorChanged )
	EVT_CHECKBOX( ID_BOLD, EditColorsPanelGeneratedClass::_wxFB_OnCheck )
	EVT_CHECKBOX( ID_ITALIC, EditColorsPanelGeneratedClass::_wxFB_OnCheck )
END_EVENT_TABLE()

EditColorsPanelGeneratedClass::EditColorsPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* Sizer;
	Sizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableRow( 1 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* ThemeSizer;
	ThemeSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Theme") ), wxVERTICAL );
	
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
	
	Sizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( Sizer );
	this->Layout();
}

EditColorsPanelGeneratedClass::~EditColorsPanelGeneratedClass()
{
}

BEGIN_EVENT_TABLE( EditorBehaviorPanelGeneratedClass, wxPanel )
	EVT_CHECKBOX( ID_RIGHT_MARGIN_CHECK, EditorBehaviorPanelGeneratedClass::_wxFB_OnCheckRightMargin )
	EVT_CHECKBOX( ID_SPACES_INSTEAD_OF_TABS, EditorBehaviorPanelGeneratedClass::_wxFB_OnIndentUsingSpaces )
END_EVENT_TABLE()

EditorBehaviorPanelGeneratedClass::EditorBehaviorPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* StaticBoxSizer;
	StaticBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Editor Behaviors") ), wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 15, 2, 0, 0 );
	FlexGridSizer->AddGrowableCol( 0 );
	FlexGridSizer->AddGrowableCol( 1 );
	FlexGridSizer->SetFlexibleDirection( wxBOTH );
	FlexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	EnableCodeFolding = new wxCheckBox( this, wxID_ANY, _("Enable code folding"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( EnableCodeFolding, 0, wxALL, 5 );
	
	EnableLineNumbers = new wxCheckBox( this, wxID_ANY, _("Show line numbers"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( EnableLineNumbers, 0, wxALL, 5 );
	
	EnableAutomaticLineIndentation = new wxCheckBox( this, wxID_ANY, _("Enable automatic indentation"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( EnableAutomaticLineIndentation, 0, wxALL, 5 );
	
	wxBoxSizer* RightMarginSizer;
	RightMarginSizer = new wxBoxSizer( wxHORIZONTAL );
	
	EnableRightMargin = new wxCheckBox( this, ID_RIGHT_MARGIN_CHECK, _("Show Right Margin at Col"), wxDefaultPosition, wxDefaultSize, 0 );
	RightMarginSizer->Add( EnableRightMargin, 0, wxALL, 5 );
	
	RightMargin = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1000, 0 );
	RightMarginSizer->Add( RightMargin, 0, wxALL, 5 );
	
	FlexGridSizer->Add( RightMarginSizer, 1, wxEXPAND, 5 );
	
	EnableIndentationGuides = new wxCheckBox( this, wxID_ANY, _("Show Indentation Guides"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( EnableIndentationGuides, 0, wxALL, 5 );
	
	EnableWordWrap = new wxCheckBox( this, ID_ENABLEWORDWRAP, _("Enable Word Wrap"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( EnableWordWrap, 0, wxALL, 5 );
	
	EnableLineEndings = new wxCheckBox( this, wxID_ANY, _("Show Line Endings"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( EnableLineEndings, 0, wxALL, 5 );
	
	EnableAutoCompletion = new wxCheckBox( this, ID_AUTOCOMPLETION, _("Enable automatic code completion"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( EnableAutoCompletion, 0, wxALL, 5 );
	
	IndentUsingTabs = new wxCheckBox( this, ID_SPACES_INSTEAD_OF_TABS, _("Indent Using Tabs"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( IndentUsingTabs, 0, wxALL, 5 );
	
	EnableDynamicAutoCompletion = new wxCheckBox( this, ID_ENABLEDYNAMICAUTOCOMPLETION, _("Enable Dynamic code completion"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( EnableDynamicAutoCompletion, 0, wxALL, 5 );
	
	TrimTrailingSpaceBeforeSave = new wxCheckBox( this, ID_TRIMTRAILINGSPACEBEFORESAVE, _("Trim Trailing Space Before Save"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( TrimTrailingSpaceBeforeSave, 0, wxALL, 5 );
	
	RemoveTrailingBlankLines = new wxCheckBox( this, ID_REMOVETRAILINGBLANKLINES, _("Remove Trailing Blank Lines (PHP)"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( RemoveTrailingBlankLines, 0, wxALL, 5 );
	
	EnableCallTipOnMouseHover = new wxCheckBox( this, ID_ENABLECALLTIPONMOUSEHOVER, _("Enable CallTip On Mouse Hover"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( EnableCallTipOnMouseHover, 0, wxALL, 5 );
	
	
	FlexGridSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* NumbersSizer;
	NumbersSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* TabWidthSizer;
	TabWidthSizer = new wxBoxSizer( wxVERTICAL );
	
	TabWidthLabel = new wxStaticText( this, wxID_ANY, _("Tab width"), wxDefaultPosition, wxDefaultSize, 0 );
	TabWidthLabel->Wrap( -1 );
	TabWidthSizer->Add( TabWidthLabel, 0, wxALL, 5 );
	
	TabWidth = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	TabWidthSizer->Add( TabWidth, 0, wxALL, 5 );
	
	NumbersSizer->Add( TabWidthSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* SpacesPerIndentSizer;
	SpacesPerIndentSizer = new wxBoxSizer( wxVERTICAL );
	
	SpacesPerIndentLabel = new wxStaticText( this, wxID_ANY, _("Spaces per tab"), wxDefaultPosition, wxDefaultSize, 0 );
	SpacesPerIndentLabel->Wrap( -1 );
	SpacesPerIndentSizer->Add( SpacesPerIndentLabel, 0, wxALL, 5 );
	
	SpacesPerIndent = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	SpacesPerIndentSizer->Add( SpacesPerIndent, 0, wxALL, 5 );
	
	NumbersSizer->Add( SpacesPerIndentSizer, 1, wxEXPAND, 5 );
	
	FlexGridSizer->Add( NumbersSizer, 1, wxEXPAND, 5 );
	
	wxString LineEndingModeChoices[] = { _("Windows"), _("Mac"), _("Unix") };
	int LineEndingModeNChoices = sizeof( LineEndingModeChoices ) / sizeof( wxString );
	LineEndingMode = new wxRadioBox( this, wxID_ANY, _("EOL Mode"), wxDefaultPosition, wxDefaultSize, LineEndingModeNChoices, LineEndingModeChoices, 1, wxRA_SPECIFY_ROWS );
	LineEndingMode->SetSelection( 0 );
	FlexGridSizer->Add( LineEndingMode, 0, wxALL|wxEXPAND, 5 );
	
	StaticBoxSizer->Add( FlexGridSizer, 1, wxEXPAND, 5 );
	
	BoxSizer->Add( StaticBoxSizer, 1, wxEXPAND|wxALL, 10 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
}

EditorBehaviorPanelGeneratedClass::~EditorBehaviorPanelGeneratedClass()
{
}

CreditsDialogClass::CreditsDialogClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* Sizer;
	Sizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 4, 1, 0, 0 );
	fgSizer9->SetFlexibleDirection( wxBOTH );
	fgSizer9->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText8 = new wxStaticText( this, wxID_ANY, _("MVC Editor makes uses of the following open source projects. None of these projects are affiliated with MVC Editor in any way."), wxDefaultPosition, wxDefaultSize, 0 );
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
	
	m_hyperlink5 = new wxHyperlinkCtrl( this, wxID_ANY, _("http://www.gnu.org/software/bison/,"), wxT("http://www.gnu.org/software/bison/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer17->Add( m_hyperlink5, 0, wxALL, 5 );
	
	GridSizer->Add( bSizer17, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText11 = new wxStaticText( this, wxID_ANY, _("ICU: A library that provides unicode support.\nICU License"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer19->Add( m_staticText11, 0, wxALL, 5 );
	
	m_hyperlink2 = new wxHyperlinkCtrl( this, wxID_ANY, _("http://site.icu-project.org/"), wxT("http://site.icu-project.org/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
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
	
	this->Centre( wxBOTH );
}

LicenseDialogClass::~LicenseDialogClass()
{
}
