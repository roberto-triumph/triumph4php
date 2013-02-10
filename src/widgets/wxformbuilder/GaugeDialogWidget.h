///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __GaugeDialogWidget__
#define __GaugeDialogWidget__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/gauge.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class GaugeDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class GaugeDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		enum
		{
			ID_GAUGE = 1000,
			ID_HIDEBUTTON,
		};
		
		wxStaticText* Label;
		wxGauge* Gauge;
		wxButton* HideButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnHideButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		GaugeDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Indexing ..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 448,156 ), long style = wxCAPTION|wxSTAY_ON_TOP );
		~GaugeDialogGeneratedClass();
	
};

#endif //__GaugeDialogWidget__
