///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __EnvironmentGeneratedDialogClass__
#define __EnvironmentGeneratedDialogClass__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/filepicker.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/statbox.h>
#include <wx/gauge.h>
#include <wx/button.h>
#include <wx/gbsizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define ID_PHP_EXECUTABLE 1000
#define ID_PHP_FILE 1001
#define ID_SCAN 1002

///////////////////////////////////////////////////////////////////////////////
/// Class EnvironmentGeneratedDialogClass
///////////////////////////////////////////////////////////////////////////////
class EnvironmentGeneratedDialogClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* HelpText;
		wxStaticText* ConfigurationDirectoryLabel;
		wxDirPickerCtrl* ApacheConfigurationDirectory;
		wxStaticText* VirtualHostsLabel;
		wxTextCtrl* VirtualHostResults;
		wxStaticText* PhpLabel;
		wxTextCtrl* PhpExecutable;
		wxFilePickerCtrl* PhpExecutableFile;
		wxGauge* Gauge;
		wxButton* ScanButton;
		
		wxButton* OkButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnScanButton( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		EnvironmentGeneratedDialogClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Project Environment"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 555,414 ), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP );
		~EnvironmentGeneratedDialogClass();
	
};

#endif //__EnvironmentGeneratedDialogClass__
