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

#endif //__RunConsolePluginGeneratedClass__
