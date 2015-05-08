///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __EditorMessagesFeatureForms__
#define __EditorMessagesFeatureForms__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/grid.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class EditorMessagesGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class EditorMessagesGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		wxFlexGridSizer* GridSizer;
		wxStaticText* Label;
		wxButton* ClearButton;
		wxGrid* Grid;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClearButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		EditorMessagesGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 760,401 ), long style = wxTAB_TRAVERSAL );
		~EditorMessagesGeneratedPanelClass();
	
};

#endif //__EditorMessagesFeatureForms__
