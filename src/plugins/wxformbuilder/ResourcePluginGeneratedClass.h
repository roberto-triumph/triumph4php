///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ResourcePluginGeneratedClass__
#define __ResourcePluginGeneratedClass__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class ResourcePluginGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class ResourcePluginGeneratedPanelClass : public wxPanel 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnFilesComboCombobox( wxCommandEvent& event ){ OnFilesComboCombobox( event ); }
		void _wxFB_OnFilesComboTextEnter( wxCommandEvent& event ){ OnFilesComboTextEnter( event ); }
		void _wxFB_OnHelpButtonClick( wxCommandEvent& event ){ OnHelpButtonClick( event ); }
		
	
	protected:
		enum
		{
			ID_RESOURCEPLUGINPANELCLASS = 1000,
			ID_FILES_COMBO,
			ID_HELP_BUTTON,
		};
		
		wxComboBox* FilesCombo;
		wxBitmapButton* HelpButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFilesComboCombobox( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFilesComboTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButtonClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ResourcePluginGeneratedPanelClass( wxWindow* parent, wxWindowID id = ID_RESOURCEPLUGINPANELCLASS, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 889,45 ), long style = wxTAB_TRAVERSAL );
		~ResourcePluginGeneratedPanelClass();
	
};

#endif //__ResourcePluginGeneratedClass__
