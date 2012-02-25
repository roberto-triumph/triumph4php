///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __RunBrowserPluginGeneratedClass__
#define __RunBrowserPluginGeneratedClass__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ChooseUrlDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class ChooseUrlDialogGeneratedClass : public wxDialog 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnListItemSelected( wxCommandEvent& event ){ OnListItemSelected( event ); }
		void _wxFB_OnText( wxCommandEvent& event ){ OnText( event ); }
		void _wxFB_OnOkButton( wxCommandEvent& event ){ OnOkButton( event ); }
		
	
	protected:
		wxStaticText* OpenLabel;
		wxListBox* UrlList;
		wxTextCtrl* Extra;
		wxStaticText* CompleteLabel;
		wxStdDialogButtonSizer* m_sdbSizer2;
		wxButton* m_sdbSizer2OK;
		wxButton* m_sdbSizer2Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnListItemSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ChooseUrlDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Choose URL To Open"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 638,413 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSTAY_ON_TOP );
		~ChooseUrlDialogGeneratedClass();
	
};

#endif //__RunBrowserPluginGeneratedClass__
