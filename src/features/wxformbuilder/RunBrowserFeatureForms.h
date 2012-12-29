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
#include <wx/treectrl.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

#define ID_FILTERSIZER 1000
#define ID_FILTER 1001

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
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;
		wxButton* ButtonSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnListItemSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ChooseUrlDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Choose URL To Open"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 638,435 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSTAY_ON_TOP );
		~ChooseUrlDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class UrlDetectorPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class UrlDetectorPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* Label;
		wxTreeCtrl* UrlDetectorTree;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTreeItemActivated( wxTreeEvent& event ) { event.Skip(); }
		
	
	public:
		
		UrlDetectorPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~UrlDetectorPanelGeneratedClass();
	
};

#endif //__RunBrowserFeatureForms__
