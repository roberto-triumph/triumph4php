///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __VersionUpdateFeatureForms__
#define __VersionUpdateFeatureForms__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/gauge.h>
#include <wx/hyperlink.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class VersionUpdateGeneratedDialogClass
///////////////////////////////////////////////////////////////////////////////
class VersionUpdateGeneratedDialogClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* StatusLabel;
		wxStaticText* Result;
		wxGauge* Gauge;
		wxStaticText* CurrentLabel;
		wxStaticText* CurrentVersion;
		wxStaticText* NewLabel;
		wxStaticText* NewVersion;
		wxHyperlinkCtrl* DownloadLink;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		VersionUpdateGeneratedDialogClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Version Update Check"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 371,209 ), long style = wxCAPTION|wxCLOSE_BOX );
		~VersionUpdateGeneratedDialogClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class VersionUpdatePreferencesGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class VersionUpdatePreferencesGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* HelpText;
		wxCheckBox* CheckForUpdates;
	
	public:
		
		VersionUpdatePreferencesGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~VersionUpdatePreferencesGeneratedPanelClass();
	
};

#endif //__VersionUpdateFeatureForms__
