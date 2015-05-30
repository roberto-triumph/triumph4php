///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __MainFrameForms__
#define __MainFrameForms__

#include <wx/intl.h>

namespace t4p{ class NotebookClass; }

#include <wx/gdicmn.h>
#include <wx/aui/auibook.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/stattext.h>
#include <wx/hyperlink.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/radiobut.h>
#include <wx/filepicker.h>
#include <wx/statbox.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrameGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class MainFrameGeneratedClass : public wxFrame 
{
	DECLARE_EVENT_TABLE()
	private:
		wxMenuItem* m_separator3;
		
		// Private event handlers
		void _wxFB_OnClose( wxCloseEvent& event ){ OnClose( event ); }
		void _wxFB_OnContentNotebookPageChanged( wxAuiNotebookEvent& event ){ OnContentNotebookPageChanged( event ); }
		void _wxFB_OnContentNotebookPageClose( wxAuiNotebookEvent& event ){ OnContentNotebookPageClose( event ); }
		void _wxFB_OnFileExit( wxCommandEvent& event ){ OnFileExit( event ); }
		void _wxFB_OnEditCut( wxCommandEvent& event ){ OnEditCut( event ); }
		void _wxFB_OnEditCopy( wxCommandEvent& event ){ OnEditCopy( event ); }
		void _wxFB_OnEditPaste( wxCommandEvent& event ){ OnEditPaste( event ); }
		void _wxFB_OnEditSelectAll( wxCommandEvent& event ){ OnEditSelectAll( event ); }
		void _wxFB_OnEditContentAssist( wxCommandEvent& event ){ OnEditContentAssist( event ); }
		void _wxFB_OnEditCallTip( wxCommandEvent& event ){ OnEditCallTip( event ); }
		void _wxFB_OnEditPreferences( wxCommandEvent& event ){ OnEditPreferences( event ); }
		void _wxFB_OnViewToggleTools( wxCommandEvent& event ){ OnViewToggleTools( event ); }
		void _wxFB_OnViewToggleOutline( wxCommandEvent& event ){ OnViewToggleOutline( event ); }
		void _wxFB_OnHelpManual( wxCommandEvent& event ){ OnHelpManual( event ); }
		void _wxFB_OnHelpAbout( wxCommandEvent& event ){ OnHelpAbout( event ); }
		void _wxFB_OnHelpCredits( wxCommandEvent& event ){ OnHelpCredits( event ); }
		void _wxFB_OnHelpLicense( wxCommandEvent& event ){ OnHelpLicense( event ); }
		
	
	protected:
		enum
		{
			ID_MAINFRAMEGENERATEDCLASS = 1000,
			ID_NOTEBOOK,
			ID_EDIT_CONTENT_ASSIST,
			ID_EDIT_CALL_TIP,
			ID_VIEW_TOGGLE_TOOLS,
			ID_VIEW_TOGGLE_OUTLINE,
			ID_MENUITEMMANUAL,
			ID_MENUITEMCREDITS,
			ID_MENUITEMLICENSE,
		};
		
		t4p::NotebookClass* Notebook;
		wxMenuBar* MenuBar;
		wxMenu* FileMenu;
		wxMenu* EditMenu;
		wxMenu* ViewMenu;
		wxMenu* SearchMenu;
		wxMenu* HelpMenu;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnContentNotebookPageChanged( wxAuiNotebookEvent& event ) { event.Skip(); }
		virtual void OnContentNotebookPageClose( wxAuiNotebookEvent& event ) { event.Skip(); }
		virtual void OnFileExit( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditCut( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditCopy( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditPaste( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditSelectAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditContentAssist( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditCallTip( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditPreferences( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnViewToggleTools( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnViewToggleOutline( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpManual( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpAbout( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpCredits( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpLicense( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		MainFrameGeneratedClass( wxWindow* parent, wxWindowID id = ID_MAINFRAMEGENERATEDCLASS, const wxString& title = _("Triumph 4 PHP"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~MainFrameGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CreditsDialogClass
///////////////////////////////////////////////////////////////////////////////
class CreditsDialogClass : public wxDialog 
{
	private:
	
	protected:
		enum
		{
			ID_CREDITSDIALOGGENERATEDCLASS = 1000,
		};
		
		wxStaticText* m_staticText8;
		wxStaticText* m_staticText12;
		wxHyperlinkCtrl* m_hyperlink3;
		wxStaticText* m_staticText13;
		wxHyperlinkCtrl* m_hyperlink4;
		wxStaticText* m_staticText15;
		wxHyperlinkCtrl* m_hyperlink6;
		wxStaticText* m_staticText18;
		wxHyperlinkCtrl* m_hyperlink9;
		wxStaticText* m_staticText17;
		wxHyperlinkCtrl* m_hyperlink8;
		wxStaticText* m_staticText16;
		wxHyperlinkCtrl* m_hyperlink7;
		wxStaticText* m_staticText14;
		wxHyperlinkCtrl* m_hyperlink5;
		wxStaticText* m_staticText11;
		wxHyperlinkCtrl* m_hyperlink2;
		wxStaticText* m_staticText10;
		wxHyperlinkCtrl* m_hyperlink1;
		wxStaticText* m_staticText19;
		wxHyperlinkCtrl* m_hyperlink10;
		wxHyperlinkCtrl* m_hyperlink11;
		wxStdDialogButtonSizer* m_sdbSizer2;
		wxButton* m_sdbSizer2OK;
	
	public:
		
		CreditsDialogClass( wxWindow* parent, wxWindowID id = ID_CREDITSDIALOGGENERATEDCLASS, const wxString& title = _("Credits"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~CreditsDialogClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class LicenseDialogClass
///////////////////////////////////////////////////////////////////////////////
class LicenseDialogClass : public wxDialog 
{
	private:
	
	protected:
		enum
		{
			ID_LICENSEDIAOGCLASS = 1000,
			ID_TEXT,
			ID_LICENSELINK,
		};
		
		wxStaticText* Text;
		wxHyperlinkCtrl* LicenseLink;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;
	
	public:
		
		LicenseDialogClass( wxWindow* parent, wxWindowID id = ID_LICENSEDIAOGCLASS, const wxString& title = _("License"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~LicenseDialogClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SettingsDirectoryGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class SettingsDirectoryGeneratedPanelClass : public wxPanel 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnUserDataDir( wxCommandEvent& event ){ OnUserDataDir( event ); }
		void _wxFB_OnAppDir( wxCommandEvent& event ){ OnAppDir( event ); }
		void _wxFB_OnCustomDir( wxCommandEvent& event ){ OnCustomDir( event ); }
		
	
	protected:
		enum
		{
			ID_SETTINGSDIRECTORYGENERATEDPANELCLASS = 1000,
			ID_SETTINGSSIZER,
			ID_HELPLALBEL,
			ID_USERDATADIRECTORY,
			ID_APPLICATIONDIRECTORY,
			ID_CUSTOMDIRECTORY,
			ID_SETTINGSDIRECTORY,
		};
		
		wxStaticText* HelpLabel;
		wxRadioButton* UserDataDirectory;
		wxRadioButton* ApplicationDirectory;
		wxRadioButton* CustomDirectory;
		wxDirPickerCtrl* SettingsDirectory;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnUserDataDir( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAppDir( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCustomDir( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		SettingsDirectoryGeneratedPanelClass( wxWindow* parent, wxWindowID id = ID_SETTINGSDIRECTORYGENERATEDPANELCLASS, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~SettingsDirectoryGeneratedPanelClass();
	
};

#endif //__MainFrameForms__
