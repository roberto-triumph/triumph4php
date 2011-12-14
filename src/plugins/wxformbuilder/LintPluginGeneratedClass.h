///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LintPluginGeneratedClass__
#define __LintPluginGeneratedClass__

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
#include <wx/textctrl.h>

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
/// Class LintPluginPreferencesGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class LintPluginPreferencesGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* CheckOnSave;
		wxStaticText* SettingsLabel;
		wxTextCtrl* IgnoreFiles;
	
	public:
		
		LintPluginPreferencesGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 583,340 ), long style = wxTAB_TRAVERSAL );
		~LintPluginPreferencesGeneratedPanelClass();
	
};

#endif //__LintPluginGeneratedClass__
