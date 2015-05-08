///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __TagFeatureForms__
#define __TagFeatureForms__

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
#include <wx/checklst.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class TagSearchDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class TagSearchDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		enum
		{
			ID_SEARCHLABEL = 1000,
			ID_SEARCHTEXT,
		};
		
		wxStaticText* SearchLabel;
		wxTextCtrl* SearchText;
		wxStaticText* m_staticText4;
		wxChoice* ProjectChoice;
		wxStaticText* MatchesLabel;
		wxStaticText* CacheStatusLabel;
		wxCheckListBox* MatchesList;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
		wxButton* ButtonsSizerHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSearchKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnSearchText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSearchEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnProjectChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMatchesListDoubleClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMatchesListKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnCancelButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		TagSearchDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Search For PHP Tags"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 634,333 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~TagSearchDialogGeneratedClass();
	
};

#endif //__TagFeatureForms__
