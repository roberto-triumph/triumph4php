///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ChooseUrlDialogWidget__
#define __ChooseUrlDialogWidget__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

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
		wxStaticText* UrlLabel;
		wxTextCtrl* Filter;
		wxStaticText* ProjectLabel;
		wxChoice* ProjectChoice;
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
		virtual void OnProjectChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnListItemSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnListItemDoubleClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExtraChar( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnExtraText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ChooseUrlDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Choose URL To Open"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 638,503 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~ChooseUrlDialogGeneratedClass();
	
};

#endif //__ChooseUrlDialogWidget__
