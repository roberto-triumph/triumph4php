///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ExplorerFeatureForms__
#define __ExplorerFeatureForms__

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/stattext.h>
#include <wx/panel.h>
#include <wx/splitter.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ModalExplorerGeneratedPanel
///////////////////////////////////////////////////////////////////////////////
class ModalExplorerGeneratedPanel : public wxPanel 
{
	private:
	
	protected:
		wxBitmapButton* FilterButton;
		wxBitmapButton* ParentButton;
		wxComboBox* Directory;
		wxSplitterWindow* Splitter;
		wxPanel* LeftPanel;
		wxListCtrl* List;
		wxStaticText* ListLabel;
		wxPanel* RightPanel;
		wxListCtrl* Report;
		wxStaticText* ReportLabel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFilterButtonLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnParentButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDirectoryEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSashChanged( wxSplitterEvent& event ) { event.Skip(); }
		virtual void OnListKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnListEndLabelEdit( wxListEvent& event ) { event.Skip(); }
		virtual void OnListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnListItemRightClick( wxListEvent& event ) { event.Skip(); }
		virtual void OnListItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnListRightDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnReportItemActivated( wxListEvent& event ) { event.Skip(); }
		
	
	public:
		
		ModalExplorerGeneratedPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 722,332 ), long style = wxTAB_TRAVERSAL );
		~ModalExplorerGeneratedPanel();
		
		void SplitterOnIdle( wxIdleEvent& )
		{
			Splitter->SetSashPosition( 323 );
			Splitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( ModalExplorerGeneratedPanel::SplitterOnIdle ), NULL, this );
		}
	
};

#endif //__ExplorerFeatureForms__
