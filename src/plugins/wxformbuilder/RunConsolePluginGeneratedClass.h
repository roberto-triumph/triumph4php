///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __RunConsolePluginGeneratedClass__
#define __RunConsolePluginGeneratedClass__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/listbox.h>
#include <wx/statbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class RunConsolePanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class RunConsolePanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxTextCtrl* Command;
		wxButton* RunButton;
		wxButton* ClearButton;
		wxTextCtrl* OutputWindow;
		
		// Virtual event handlers, overide them in your derived class
		virtual void RunCommand( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClear( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		RunConsolePanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 696,210 ), long style = wxTAB_TRAVERSAL );
		~RunConsolePanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ChooseUrlDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class ChooseUrlDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* OpenLabel;
		wxListBox* UrlList;
		wxTextCtrl* Extra;
		wxStaticText* CompleteLabel;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnUpdateUi( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ChooseUrlDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Choose URL To Open"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 557,344 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSTAY_ON_TOP );
		~ChooseUrlDialogGeneratedClass();
	
};

#endif //__RunConsolePluginGeneratedClass__
