///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "../../widgets/NotebookClass.h"

#include "AppFrameGeneratedClass.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( AppFrameGeneratedClass, wxFrame )
	EVT_CLOSE( AppFrameGeneratedClass::_wxFB_OnClose )
	EVT_AUINOTEBOOK_PAGE_CHANGED( ID_NOTEBOOK, AppFrameGeneratedClass::_wxFB_OnContentNotebookPageChanged )
	EVT_AUINOTEBOOK_PAGE_CLOSE( ID_NOTEBOOK, AppFrameGeneratedClass::_wxFB_OnContentNotebookPageClose )
	EVT_MENU( ID_FILE_PHP_NEW, AppFrameGeneratedClass::_wxFB_OnFilePhpNew )
	EVT_MENU( ID_FILE_SQL_NEW, AppFrameGeneratedClass::_wxFB_OnFileSqlNew )
	EVT_MENU( ID_FILE_CSS_NEW, AppFrameGeneratedClass::_wxFB_OnFileCssNew )
	EVT_MENU( ID_FILE_TEXT_NEW_FILE, AppFrameGeneratedClass::_wxFB_OnFileTextNew )
	EVT_MENU( wxID_OPEN, AppFrameGeneratedClass::_wxFB_OnFileOpen )
	EVT_MENU( wxID_SAVE, AppFrameGeneratedClass::_wxFB_OnFileSave )
	EVT_MENU( wxID_SAVEAS, AppFrameGeneratedClass::_wxFB_OnFileSaveAs )
	EVT_MENU( ID_FILE_REVERT, AppFrameGeneratedClass::_wxFB_OnFileRevert )
	EVT_MENU( ID_FILE_CLOSE, AppFrameGeneratedClass::_wxFB_OnFileClose )
	EVT_MENU( wxID_EXIT, AppFrameGeneratedClass::_wxFB_OnFileExit )
	EVT_MENU( wxID_CUT, AppFrameGeneratedClass::_wxFB_OnEditCut )
	EVT_MENU( wxID_COPY, AppFrameGeneratedClass::_wxFB_OnEditCopy )
	EVT_MENU( wxID_PASTE, AppFrameGeneratedClass::_wxFB_OnEditPaste )
	EVT_MENU( wxID_SELECTALL, AppFrameGeneratedClass::_wxFB_OnEditSelectAll )
	EVT_MENU( ID_EDIT_CONTENT_ASSIST, AppFrameGeneratedClass::_wxFB_OnEditContentAssist )
	EVT_MENU( ID_EDIT_CALL_TIP, AppFrameGeneratedClass::_wxFB_OnEditCallTip )
	EVT_MENU( ID_EDIT_PREFERENCES, AppFrameGeneratedClass::_wxFB_OnEditPreferences )
	EVT_MENU( ID_ABOUT, AppFrameGeneratedClass::_wxFB_OnHelpAbout )
END_EVENT_TABLE()

AppFrameGeneratedClass::AppFrameGeneratedClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* Sizer;
	Sizer = new wxBoxSizer( wxVERTICAL );
	
	Notebook = new mvceditor::NotebookClass( this, ID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxAUI_NB_CLOSE_ON_ACTIVE_TAB|wxAUI_NB_SCROLL_BUTTONS|wxAUI_NB_TAB_MOVE|wxAUI_NB_WINDOWLIST_BUTTON );
	
	Sizer->Add( Notebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( Sizer );
	this->Layout();
	StatusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	MenuBar = new wxMenuBar( 0 );
	FileMenu = new wxMenu();
	wxMenuItem* MenuItemFilePhpNew;
	MenuItemFilePhpNew = new wxMenuItem( FileMenu, ID_FILE_PHP_NEW, wxString( _("New &PHP File") ) , _("Create a new PHP File"), wxITEM_NORMAL );
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
	
	wxMenuItem* m_separator4;
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
	
	wxMenuItem* MenuItemRevert;
	MenuItemRevert = new wxMenuItem( FileMenu, ID_FILE_REVERT, wxString( _("Revert") ) , _("Reload the file from Disk"), wxITEM_NORMAL );
	FileMenu->Append( MenuItemRevert );
	MenuItemRevert->Enable( false );
	
	wxMenuItem* MenuItemFileClose;
	MenuItemFileClose = new wxMenuItem( FileMenu, ID_FILE_CLOSE, wxString( _("Close File") ) , _("Close the current file"), wxITEM_NORMAL );
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
	
	wxMenuItem* m_separator3;
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
	
	MenuBar->Append( HelpMenu, _("&Help") ); 
	
	this->SetMenuBar( MenuBar );
	
}

AppFrameGeneratedClass::~AppFrameGeneratedClass()
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
	FlexGridSizer = new wxFlexGridSizer( 5, 2, 0, 0 );
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
