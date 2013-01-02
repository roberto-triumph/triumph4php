///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DetectorFeatureForms__
#define __DetectorFeatureForms__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

#define ID_PROJECT_CHOICE 1000
#define ID_TEST_DETECTOR_BUTTON 1001
#define ID_ADD_BUTTON 1002

///////////////////////////////////////////////////////////////////////////////
/// Class DetectorPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class DetectorPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* ProjectLabel;
		wxChoice* ProjectChoice;
		wxButton* TestButton;
		wxButton* AddButton;
		wxBitmapButton* HelpButton;
		wxStaticText* Label;
		wxTreeCtrl* DetectorTree;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTestButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTreeItemDelete( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnTreeItemEndLabelEdit( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnTreeItemActivated( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnTreeItemRightClick( wxTreeEvent& event ) { event.Skip(); }
		
	
	public:
		
		DetectorPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~DetectorPanelGeneratedClass();
	
};

#endif //__DetectorFeatureForms__
