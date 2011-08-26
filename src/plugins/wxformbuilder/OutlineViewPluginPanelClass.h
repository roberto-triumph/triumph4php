///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __OutlineViewPluginPanelClass__
#define __OutlineViewPluginPanelClass__

#include <wx/intl.h>

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
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class OutlineViewPluginGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class OutlineViewPluginGeneratedPanelClass : public wxPanel 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnHelpButton( wxCommandEvent& event ){ OnHelpButton( event ); }
		void _wxFB_OnSyncButton( wxCommandEvent& event ){ OnSyncButton( event ); }
		void _wxFB_OnLookupButton( wxCommandEvent& event ){ OnLookupButton( event ); }
		
	
	protected:
		enum
		{
			ID_OUTLINEVIEWPLUGINGENERATEDPANELCLASS = 1000,
			ID_SYNCBUTTON,
			ID_LOOKUPBUTTON,
			ID_LOOKUP,
		};
		
		wxBitmapButton* HelpButton;
		wxButton* SyncButton;
		wxButton* LookupButton;
		wxTextCtrl* Lookup;
		wxFlexGridSizer* OutlineSizer;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSyncButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLookupButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		OutlineViewPluginGeneratedPanelClass( wxWindow* parent, wxWindowID id = ID_OUTLINEVIEWPLUGINGENERATEDPANELCLASS, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 813,356 ), long style = wxTAB_TRAVERSAL );
		~OutlineViewPluginGeneratedPanelClass();
	
};

#endif //__OutlineViewPluginPanelClass__
