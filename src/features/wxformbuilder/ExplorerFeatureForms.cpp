///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 17 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ExplorerFeatureForms.h"

///////////////////////////////////////////////////////////////////////////

ModalExplorerGeneratedPanel::ModalExplorerGeneratedPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* BoxSizer;
	BoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* GridSizer;
	GridSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	GridSizer->AddGrowableCol( 0 );
	GridSizer->AddGrowableRow( 1 );
	GridSizer->SetFlexibleDirection( wxBOTH );
	GridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	FilterButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	TopSizer->Add( FilterButton, 0, wxALL, 5 );
	
	ParentButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	ParentButton->SetToolTip( wxT("Parent directory") );
	
	ParentButton->SetToolTip( wxT("Parent directory") );
	
	TopSizer->Add( ParentButton, 0, wxALL, 5 );
	
	Directory = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER ); 
	TopSizer->Add( Directory, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	GridSizer->Add( TopSizer, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* BodyGridSizer;
	BodyGridSizer = new wxFlexGridSizer( 1, 2, 0, 0 );
	BodyGridSizer->AddGrowableCol( 0 );
	BodyGridSizer->AddGrowableRow( 0 );
	BodyGridSizer->SetFlexibleDirection( wxBOTH );
	BodyGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	Splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER );
	Splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( ModalExplorerGeneratedPanel::SplitterOnIdle ), NULL, this );
	
	LeftPanel = new wxPanel( Splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* LeftPanelSizer;
	LeftPanelSizer = new wxBoxSizer( wxVERTICAL );
	
	List = new wxListCtrl( LeftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_EDIT_LABELS|wxLC_LIST );
	LeftPanelSizer->Add( List, 1, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	ListLabel = new wxStaticText( LeftPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	ListLabel->Wrap( -1 );
	LeftPanelSizer->Add( ListLabel, 0, wxALL|wxEXPAND, 5 );
	
	LeftPanel->SetSizer( LeftPanelSizer );
	LeftPanel->Layout();
	LeftPanelSizer->Fit( LeftPanel );
	RightPanel = new wxPanel( Splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* RightPanelSizer;
	RightPanelSizer = new wxBoxSizer( wxVERTICAL );
	
	Report = new wxListCtrl( RightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	RightPanelSizer->Add( Report, 1, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	ReportLabel = new wxStaticText( RightPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	ReportLabel->Wrap( -1 );
	RightPanelSizer->Add( ReportLabel, 0, wxALL|wxEXPAND, 5 );
	
	RightPanel->SetSizer( RightPanelSizer );
	RightPanel->Layout();
	RightPanelSizer->Fit( RightPanel );
	Splitter->SplitVertically( LeftPanel, RightPanel, 323 );
	BodyGridSizer->Add( Splitter, 1, wxEXPAND, 5 );
	
	GridSizer->Add( BodyGridSizer, 1, wxEXPAND, 5 );
	
	BoxSizer->Add( GridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( BoxSizer );
	this->Layout();
	
	// Connect Events
	FilterButton->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( ModalExplorerGeneratedPanel::OnFilterButtonLeftDown ), NULL, this );
	ParentButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ModalExplorerGeneratedPanel::OnParentButtonClick ), NULL, this );
	Directory->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ModalExplorerGeneratedPanel::OnDirectoryEnter ), NULL, this );
	Splitter->Connect( wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED, wxSplitterEventHandler( ModalExplorerGeneratedPanel::OnSashChanged ), NULL, this );
	List->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ModalExplorerGeneratedPanel::OnListKeyDown ), NULL, this );
	List->Connect( wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler( ModalExplorerGeneratedPanel::OnListEndLabelEdit ), NULL, this );
	List->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ModalExplorerGeneratedPanel::OnListItemActivated ), NULL, this );
	List->Connect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( ModalExplorerGeneratedPanel::OnListItemRightClick ), NULL, this );
	List->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ModalExplorerGeneratedPanel::OnListItemSelected ), NULL, this );
	Report->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ModalExplorerGeneratedPanel::OnReportItemActivated ), NULL, this );
}

ModalExplorerGeneratedPanel::~ModalExplorerGeneratedPanel()
{
	// Disconnect Events
	FilterButton->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( ModalExplorerGeneratedPanel::OnFilterButtonLeftDown ), NULL, this );
	ParentButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ModalExplorerGeneratedPanel::OnParentButtonClick ), NULL, this );
	Directory->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ModalExplorerGeneratedPanel::OnDirectoryEnter ), NULL, this );
	Splitter->Disconnect( wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED, wxSplitterEventHandler( ModalExplorerGeneratedPanel::OnSashChanged ), NULL, this );
	List->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( ModalExplorerGeneratedPanel::OnListKeyDown ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler( ModalExplorerGeneratedPanel::OnListEndLabelEdit ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ModalExplorerGeneratedPanel::OnListItemActivated ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( ModalExplorerGeneratedPanel::OnListItemRightClick ), NULL, this );
	List->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ModalExplorerGeneratedPanel::OnListItemSelected ), NULL, this );
	Report->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ModalExplorerGeneratedPanel::OnReportItemActivated ), NULL, this );
	
}
