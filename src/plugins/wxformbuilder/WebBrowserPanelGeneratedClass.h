///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __WebBrowserPanelGeneratedClass__
#define __WebBrowserPanelGeneratedClass__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include "wx/image.h" //webcontrol does not include it
#include "webcontrol.h"
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class WebBrowserPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class WebBrowserPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		enum
		{
			ID_URL = 1000,
			ID_GO,
			ID_WEB_BROWSER,
		};
		
		wxTextCtrl* Url;
		wxButton* GoButton;
		wxWebControl* WebControl;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnGoButton( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		WebBrowserPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 770,676 ), long style = wxTAB_TRAVERSAL );
		~WebBrowserPanelGeneratedClass();
	
};

#endif //__WebBrowserPanelGeneratedClass__
