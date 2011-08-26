///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ProjectPluginGeneratedPanelClass__
#define __ProjectPluginGeneratedPanelClass__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/filepicker.h>
#include <wx/gbsizer.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ProjectPluginGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class ProjectPluginGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* HelpText;
		wxStaticText* Label;
		wxTextCtrl* ExplorerExecutable;
		wxFilePickerCtrl* FilePicker;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFileChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		
	
	public:
		
		ProjectPluginGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 863,280 ), long style = wxTAB_TRAVERSAL );
		~ProjectPluginGeneratedPanelClass();
	
};

#endif //__ProjectPluginGeneratedPanelClass__
