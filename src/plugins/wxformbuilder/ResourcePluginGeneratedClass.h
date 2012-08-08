///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ResourcePluginGeneratedClass__
#define __ResourcePluginGeneratedClass__

#include <wx/intl.h>

#include <wx/gauge.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checklst.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class IndexingDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class IndexingDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		enum
		{
			ID_INDEXINGDIALOGGENERATEDCLASS = 1000,
			ID_GAUGE,
			ID_HIDEBUTTON,
		};
		
		wxGauge* Gauge;
		wxButton* HideButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnHideButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		IndexingDialogGeneratedClass( wxWindow* parent, wxWindowID id = ID_INDEXINGDIALOGGENERATEDCLASS, const wxString& title = _("Indexing ..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 392,107 ), long style = wxCAPTION|wxSTAY_ON_TOP );
		~IndexingDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ResourceSearchDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class ResourceSearchDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		enum
		{
			ID_RESOURCESEARCHDIALOGGENERATEDCLASS = 1000,
			ID_SEARCHLABEL,
			ID_SEARCHTEXT,
		};
		
		wxStaticText* SearchLabel;
		wxTextCtrl* SearchText;
		wxStaticText* MatchesLabel;
		wxStaticText* CacheStatusLabel;
		wxCheckListBox* MatchesList;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
		wxButton* ButtonsSizerHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSearchKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnSearchText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSearchEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnResultsDoubleClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancelButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ResourceSearchDialogGeneratedClass( wxWindow* parent, wxWindowID id = ID_RESOURCESEARCHDIALOGGENERATEDCLASS, const wxString& title = _("Search For Resources"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 419,270 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~ResourceSearchDialogGeneratedClass();
	
};

#endif //__ResourcePluginGeneratedClass__
