///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __RunBrowserFeatureForms__
#define __RunBrowserFeatureForms__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/choice.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/treectrl.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

#define ID_FILTERSIZER 1000
#define ID_FILTER 1001
#define ID_PROJECT_CHOICE 1002
#define ID_TEST_DETECTOR_BUTTON 1003
#define ID_ADD_BUTTON 1004
#define ID_DETECTOR_TREE 1005

///////////////////////////////////////////////////////////////////////////////
/// Class ChooseUrlDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class ChooseUrlDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxTextCtrl* Filter;
		wxStaticText* OpenLabel;
		wxListBox* UrlList;
		wxStaticText* ExtraLabel;
		wxTextCtrl* ExtraText;
		wxStaticText* FinalLabel;
		wxStaticText* FinalUrlLabel;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;
		wxButton* ButtonSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFilterKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnFilterText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFilterTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnListItemSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExtraChar( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnExtraText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ChooseUrlDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Choose URL To Open"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 638,461 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~ChooseUrlDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class UrlDetectorPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class UrlDetectorPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* ProjectLabel;
		wxChoice* ProjectChoice;
		wxButton* TestButton;
		wxButton* AddButton;
		wxBitmapButton* HelpButton;
		wxStaticText* Label;
		wxTreeCtrl* UrlDetectorTree;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTestButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTreeItemDelete( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnTreeItemEndLabelEdit( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnTreeItemActivated( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnTreeItemRightClick( wxTreeEvent& event ) { event.Skip(); }
		
	
	public:
		
		UrlDetectorPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~UrlDetectorPanelGeneratedClass();
	
};

#endif //__RunBrowserFeatureForms__
