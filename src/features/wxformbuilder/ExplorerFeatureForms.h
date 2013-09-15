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
#include <wx/filepicker.h>
#include <wx/statbox.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ModalExplorerGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class ModalExplorerGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		wxBitmapButton* FilterButton;
		wxBitmapButton* ParentButton;
		wxBitmapButton* RefreshButton;
		wxComboBox* Directory;
		wxPanel* LeftPanel;
		wxListCtrl* List;
		wxStaticText* ListLabel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFilterButtonLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnParentButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRefreshClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDirectoryEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnListKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnListEndLabelEdit( wxListEvent& event ) { event.Skip(); }
		virtual void OnListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnListItemRightClick( wxListEvent& event ) { event.Skip(); }
		virtual void OnListItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnListRightDown( wxMouseEvent& event ) { event.Skip(); }
		
	
	public:
		
		ModalExplorerGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 722,332 ), long style = wxTAB_TRAVERSAL );
		~ModalExplorerGeneratedPanelClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ExplorerOptionsGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class ExplorerOptionsGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* HelpLabel;
		wxStaticText* FileManagerLabel;
		wxFilePickerCtrl* FileManager;
		wxStaticText* ShellLabel;
		wxFilePickerCtrl* Shell;
	
	public:
		
		ExplorerOptionsGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~ExplorerOptionsGeneratedPanelClass();
	
};

#endif //__ExplorerFeatureForms__
