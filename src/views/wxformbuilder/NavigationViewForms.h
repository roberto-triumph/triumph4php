///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __NavigationViewForms__
#define __NavigationViewForms__

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NavigationViewDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class NavigationViewDialogGeneratedClass : public wxDialog
{
	private:

	protected:
		wxGridSizer* BodySizer;
		wxStaticBoxSizer* FilesSizer;
		wxListCtrl* FilesList;
		wxStaticBoxSizer* PanelsSizer;
		wxListCtrl* PanelsList;

		// Virtual event handlers, overide them in your derived class
		virtual void OnFilesListKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnFileItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnPanelsListKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnPanelItemActivated( wxListEvent& event ) { event.Skip(); }


	public:

		NavigationViewDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Navigate To Pane"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,350 ), long style = wxCAPTION|wxCLOSE_BOX|wxRESIZE_BORDER );
		~NavigationViewDialogGeneratedClass();

};

#endif //__NavigationViewForms__
