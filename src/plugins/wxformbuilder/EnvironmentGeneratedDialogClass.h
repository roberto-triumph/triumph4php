///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
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
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/filepicker.h>
#include <wx/statbox.h>
#include <wx/textctrl.h>
#include <wx/gauge.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class WebBrowserEditPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class WebBrowserEditPanelGeneratedClass : public wxPanel 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnResize( wxSizeEvent& event ){ OnResize( event ); }
		void _wxFB_OnRemoveSelectedWebBrowser( wxCommandEvent& event ){ OnRemoveSelectedWebBrowser( event ); }
		void _wxFB_OnEditSelectedWebBrowser( wxCommandEvent& event ){ OnEditSelectedWebBrowser( event ); }
		void _wxFB_OnAddWebBrowser( wxCommandEvent& event ){ OnAddWebBrowser( event ); }
		
	
	protected:
		enum
		{
			ID_HELPTEXT = 1000,
			ID_REMOVE_BROWSER,
			ID_EDITSELECTEDWEBBROWSER,
			ID_ADD_BROWSER,
			ID_BROWSERLIST,
		};
		
		wxStaticText* HelpText;
		
		wxButton* RemoveSelectedBrowser;
		wxButton* EditSelectedWebBrowser;
		wxButton* AddBrowserButton;
		wxListCtrl* BrowserList;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnResize( wxSizeEvent& event ) { event.Skip(); }
		virtual void OnRemoveSelectedWebBrowser( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditSelectedWebBrowser( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddWebBrowser( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		WebBrowserEditPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 550,400 ), long style = wxTAB_TRAVERSAL );
		~WebBrowserEditPanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ApacheEnvironmentPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class ApacheEnvironmentPanelGeneratedClass : public wxPanel 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnResize( wxSizeEvent& event ){ OnResize( event ); }
		void _wxFB_OnScanButton( wxCommandEvent& event ){ OnScanButton( event ); }
		
	
	protected:
		enum
		{
			ID_CONFIG_DIRECTORY = 1000,
			ID_SCAN,
		};
		
		wxStaticText* HelpText;
		wxDirPickerCtrl* ApacheConfigurationDirectory;
		wxStaticText* VirtualHostsLabel;
		wxTextCtrl* VirtualHostResults;
		wxGauge* Gauge;
		wxButton* ScanButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnResize( wxSizeEvent& event ) { event.Skip(); }
		virtual void OnScanButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ApacheEnvironmentPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 550,400 ), long style = wxTAB_TRAVERSAL );
		~ApacheEnvironmentPanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class PhpEnvironmentPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class PhpEnvironmentPanelGeneratedClass : public wxPanel 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnResize( wxSizeEvent& event ){ OnResize( event ); }
		void _wxFB_OnPhpFileChanged( wxFileDirPickerEvent& event ){ OnPhpFileChanged( event ); }
		
	
	protected:
		enum
		{
			ID_HELPTEXT = 1000,
			ID_PHP_EXECUTABLE,
			ID_PHP_FILE,
		};
		
		wxStaticText* HelpText;
		wxStaticText* PhpLabel;
		wxTextCtrl* PhpExecutable;
		wxFilePickerCtrl* PhpExecutableFile;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnResize( wxSizeEvent& event ) { event.Skip(); }
		virtual void OnPhpFileChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		
	
	public:
		
		PhpEnvironmentPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~PhpEnvironmentPanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class WebBrowserCreateDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class WebBrowserCreateDialogGeneratedClass : public wxDialog 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnFileChanged( wxFileDirPickerEvent& event ){ OnFileChanged( event ); }
		void _wxFB_OnOkButton( wxCommandEvent& event ){ OnOkButton( event ); }
		
	
	protected:
		enum
		{
			ID_WEBBROWSERCREATEDIALOGGENERATEDCLASS = 1000,
			ID_NAMELABEL,
			ID_NAME,
			ID_PATHLABEL,
			ID_WEBBROWSERPATH,
		};
		
		wxStaticText* NameLabel;
		wxTextCtrl* WebBrowserLabel;
		wxStaticText* PathLabel;
		wxFilePickerCtrl* WebBrowserPath;
		wxStdDialogButtonSizer* Buttons;
		wxButton* ButtonsOK;
		wxButton* ButtonsCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFileChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		WebBrowserCreateDialogGeneratedClass( wxWindow* parent, wxWindowID id = ID_WEBBROWSERCREATEDIALOGGENERATEDCLASS, const wxString& title = _("Add a new Web Browser"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 458,168 ), long style = wxCLOSE_BOX|wxDEFAULT_DIALOG_STYLE );
		~WebBrowserCreateDialogGeneratedClass();
	
};

#endif //__EnvironmentGeneratedDialogClass__
