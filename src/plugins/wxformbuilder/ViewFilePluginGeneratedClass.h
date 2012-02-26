///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ViewFilePluginGeneratedClass__
#define __ViewFilePluginGeneratedClass__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/treectrl.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class ViewFilePanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class ViewFilePanelGeneratedClass : public wxPanel 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnControllerChoice( wxCommandEvent& event ){ OnControllerChoice( event ); }
		void _wxFB_OnActionChoice( wxCommandEvent& event ){ OnActionChoice( event ); }
		void _wxFB_OnHelpButton( wxCommandEvent& event ){ OnHelpButton( event ); }
		void _wxFB_OnCurrentButton( wxCommandEvent& event ){ OnCurrentButton( event ); }
		void _wxFB_OnTreeItemActivated( wxTreeEvent& event ){ OnTreeItemActivated( event ); }
		
	
	protected:
		enum
		{
			ID_VIEWFILESPANELGENERATEDCLASS = 1000,
			ID_CONTROLLERLABEL,
			ID_CONTROLLER,
			ID_ACTIONLABEL,
			ID_ACTION,
			ID_HELPBUTTON,
			ID_CURRENTBUTTON,
			ID_ERRORLABEL,
			ID_FILETREE,
		};
		
		wxStaticText* ControllerLabel;
		wxChoice* Controller;
		wxStaticText* ActionLabel;
		wxChoice* Action;
		wxBitmapButton* HelpButton;
		wxButton* CurrentButton;
		wxStaticText* StatusLabel;
		wxTreeCtrl* FileTree;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnControllerChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnActionChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCurrentButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTreeItemActivated( wxTreeEvent& event ) { event.Skip(); }
		
	
	public:
		
		ViewFilePanelGeneratedClass( wxWindow* parent, wxWindowID id = ID_VIEWFILESPANELGENERATEDCLASS, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 409,444 ), long style = wxTAB_TRAVERSAL );
		~ViewFilePanelGeneratedClass();
	
};

#endif //__ViewFilePluginGeneratedClass__
