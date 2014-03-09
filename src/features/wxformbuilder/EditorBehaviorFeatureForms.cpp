///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "EditorBehaviorFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

EditorBehaviorPanelGeneratedClass::EditorBehaviorPanelGeneratedClass( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* StaticBoxSizer;
	StaticBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Editor Behaviors") ), wxVERTICAL );
	
	wxFlexGridSizer* FlexGridSizer;
	FlexGridSizer = new wxFlexGridSizer( 17, 2, 0, 0 );
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
	
	EnableCallTipOnMouseHover = new wxCheckBox( this, ID_ENABLECALLTIPONMOUSEHOVER, _("Enable CallTip On ALT+ Mouse Hover"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( EnableCallTipOnMouseHover, 0, wxALL, 5 );
	
	EnableMultipleSelection = new wxCheckBox( this, ID_ENABLEMULTIPLESELECTION, _("Enable Multiple Selections"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( EnableMultipleSelection, 0, wxALL, 5 );
	
	EnableVirtualSpace = new wxCheckBox( this, ID_ENABLEVIRTUALSPACE, _("Enable Virtual Space"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( EnableVirtualSpace, 0, wxALL, 5 );
	
	EnableRectangularSelection = new wxCheckBox( this, ID_ENABLERECTANGULARSELECTION, _("Enable Rectangular Selection"), wxDefaultPosition, wxDefaultSize, 0 );
	FlexGridSizer->Add( EnableRectangularSelection, 0, wxALL, 5 );
	
	ShowWhitespace = new wxCheckBox( this, ID_SHOWWHITESPACE, _("Show Whitespace"), wxDefaultPosition, wxDefaultSize, 0 );
	ShowWhitespace->SetValue(true); 
	FlexGridSizer->Add( ShowWhitespace, 0, wxALL, 5 );
	
	wxBoxSizer* ZoomSizer;
	ZoomSizer = new wxBoxSizer( wxHORIZONTAL );
	
	ZoomLabel = new wxStaticText( this, ID_ZOOMLABEL, _("Default Zoom"), wxDefaultPosition, wxDefaultSize, 0 );
	ZoomLabel->Wrap( -1 );
	ZoomSizer->Add( ZoomLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	Zoom = new wxSpinCtrl( this, ID_ZOOM, wxT("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -35, 50, 0 );
	ZoomSizer->Add( Zoom, 0, wxALL, 5 );
	
	FlexGridSizer->Add( ZoomSizer, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
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
	BoxSizer->Fit( this );
	
	// Connect Events
	EnableRightMargin->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( EditorBehaviorPanelGeneratedClass::OnCheckRightMargin ), NULL, this );
	IndentUsingTabs->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( EditorBehaviorPanelGeneratedClass::OnIndentUsingSpaces ), NULL, this );
}

EditorBehaviorPanelGeneratedClass::~EditorBehaviorPanelGeneratedClass()
{
	// Disconnect Events
	EnableRightMargin->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( EditorBehaviorPanelGeneratedClass::OnCheckRightMargin ), NULL, this );
	IndentUsingTabs->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( EditorBehaviorPanelGeneratedClass::OnIndentUsingSpaces ), NULL, this );
	
}
