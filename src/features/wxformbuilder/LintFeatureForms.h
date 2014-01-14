///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LintFeatureForms__
#define __LintFeatureForms__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/hyperlink.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class LintResultsGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class LintResultsGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		enum
		{
			ID_ERRORS_LIST = 1000,
		};
		
		wxStaticText* Label;
		wxListBox* ErrorsList;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnListDoubleClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		LintResultsGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 774,371 ), long style = wxTAB_TRAVERSAL );
		~LintResultsGeneratedPanelClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class LintPreferencesGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class LintPreferencesGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* Help;
		wxCheckBox* CheckOnSave;
		wxCheckBox* CheckUnitializedVariables;
		wxCheckBox* CheckUnitializedGlobalVariables;
		wxCheckBox* CheckUnknownIdentifiers;
	
	public:
		
		LintPreferencesGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,408 ), long style = wxTAB_TRAVERSAL );
		~LintPreferencesGeneratedPanelClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class LintErrorGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class LintErrorGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* ErrorLabel;
		wxStaticText* HelpLabel;
		wxHyperlinkCtrl* GoToLink;
		wxHyperlinkCtrl* DismissLink;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnGoToLink( wxHyperlinkEvent& event ) { event.Skip(); }
		virtual void OnDismissLink( wxHyperlinkEvent& event ) { event.Skip(); }
		
	
	public:
		
		LintErrorGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 488,77 ), long style = wxSIMPLE_BORDER|wxTAB_TRAVERSAL );
		~LintErrorGeneratedPanelClass();
	
};

#endif //__LintFeatureForms__
