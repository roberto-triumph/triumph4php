///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __EnvironmentFeatureForms__
#define __EnvironmentFeatureForms__

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
#include <wx/gauge.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
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
		void _wxFB_OnMoveDown( wxCommandEvent& event ){ OnMoveDown( event ); }
		void _wxFB_OnMoveUp( wxCommandEvent& event ){ OnMoveUp( event ); }
		void _wxFB_OnRemoveSelectedWebBrowser( wxCommandEvent& event ){ OnRemoveSelectedWebBrowser( event ); }
		void _wxFB_OnEditSelectedWebBrowser( wxCommandEvent& event ){ OnEditSelectedWebBrowser( event ); }
		void _wxFB_OnAddWebBrowser( wxCommandEvent& event ){ OnAddWebBrowser( event ); }
		
	
	protected:
		enum
		{
			ID_HELPTEXT = 1000,
			ID_MOVEDOWNBUTTON,
			ID_MOVEUPBUTTON,
			ID_REMOVE_BROWSER,
			ID_EDITSELECTEDWEBBROWSER,
			ID_ADD_BROWSER,
			ID_BROWSERLIST,
		};
		
		wxStaticText* HelpText;
		
		wxButton* MoveDownButton;
		wxButton* MoveUpButton;
		wxButton* RemoveSelectedBrowser;
		wxButton* EditSelectedWebBrowser;
		wxButton* AddBrowserButton;
		wxListCtrl* BrowserList;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnMoveDown( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMoveUp( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveSelectedWebBrowser( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditSelectedWebBrowser( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddWebBrowser( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		WebBrowserEditPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
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
		void _wxFB_OnUpdateUi( wxUpdateUIEvent& event ){ OnUpdateUi( event ); }
		void _wxFB_OnDirChanged( wxFileDirPickerEvent& event ){ OnDirChanged( event ); }
		void _wxFB_OnScanButton( wxCommandEvent& event ){ OnScanButton( event ); }
		void _wxFB_OnRemoveButton( wxCommandEvent& event ){ OnRemoveButton( event ); }
		void _wxFB_OnEditButton( wxCommandEvent& event ){ OnEditButton( event ); }
		void _wxFB_OnAddButton( wxCommandEvent& event ){ OnAddButton( event ); }
		
	
	protected:
		enum
		{
			ID_CONFIG_DIRECTORY = 1000,
			ID_SCAN,
			ID_MANUAL,
			ID_REMOVEBUTTON,
			ID_EDITBUTTON,
			ID_ADDBUTTON,
			ID_VIRTUALHOSTLIST,
		};
		
		wxStaticText* HelpText;
		wxDirPickerCtrl* ApacheConfigurationDirectory;
		wxButton* ScanButton;
		wxGauge* Gauge;
		wxCheckBox* Manual;
		wxButton* RemoveButton;
		wxButton* EditButton;
		wxButton* AddButton;
		wxStaticText* VirtualHostsLabel;
		wxListCtrl* VirtualHostList;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnUpdateUi( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnDirChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void OnScanButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ApacheEnvironmentPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 564,456 ), long style = wxTAB_TRAVERSAL );
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
		void _wxFB_OnPhpFileChanged( wxFileDirPickerEvent& event ){ OnPhpFileChanged( event ); }
		
	
	protected:
		enum
		{
			ID_HELPTEXT = 1000,
			ID_PHP_EXECUTABLE,
			ID_PHP_FILE,
			ID_VERSIONLABEL,
			ID_VERSION,
		};
		
		wxStaticText* HelpText;
		wxStaticText* PhpLabel;
		wxTextCtrl* PhpExecutable;
		wxFilePickerCtrl* PhpExecutableFile;
		wxStaticText* VersionLabel;
		wxComboBox* Version;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnPhpFileChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		
	
	public:
		
		PhpEnvironmentPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 487,256 ), long style = wxTAB_TRAVERSAL );
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

///////////////////////////////////////////////////////////////////////////////
/// Class VirtualHostCreateDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class VirtualHostCreateDialogGeneratedClass : public wxDialog 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnOkButton( wxCommandEvent& event ){ OnOkButton( event ); }
		
	
	protected:
		enum
		{
			ID_VIRTRUALHOSTCREATEDIALOGGENERATEDCLASS = 1000,
			ID_DIRECTORYLABEL,
			ID_ROOTDIRECTORY,
			ID_HOSTNAMELABEL,
			ID_HOSTNAME,
		};
		
		wxStaticText* DirectoryLabel;
		wxDirPickerCtrl* RootDirectory;
		wxStaticText* HostnameLabel;
		wxTextCtrl* Hostname;
		
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		VirtualHostCreateDialogGeneratedClass( wxWindow* parent, wxWindowID id = ID_VIRTRUALHOSTCREATEDIALOGGENERATEDCLASS, const wxString& title = _("Create a Virtual Host"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 600,144 ), long style = wxDEFAULT_DIALOG_STYLE );
		~VirtualHostCreateDialogGeneratedClass();
	
};

#endif //__EnvironmentFeatureForms__
