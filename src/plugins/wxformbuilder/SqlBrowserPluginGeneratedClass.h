///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SqlBrowserPluginGeneratedClass__
#define __SqlBrowserPluginGeneratedClass__

#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/panel.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/grid.h>
#include <wx/splitter.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class SqlBrowserPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class SqlBrowserPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		enum
		{
			ID_PANEL = 1000,
			ID_SPLITTER,
			ID_CODECONTROLPANEL,
			ID_BOTTOMPANLE,
			ID_CONNECTIONLABEL,
			ID_RESULTSLABEL,
			ID_DATAGRID,
		};
		
		wxSplitterWindow* Splitter;
		wxPanel* CodeControlPanel;
		wxBoxSizer* CodeControlPanelSizer;
		wxPanel* BottomPanel;
		wxStaticText* ConnectionLabel;
		wxStaticText* ResultsLabel;
		wxGrid* ResultsGrid;
	
	public:
		
		SqlBrowserPanelGeneratedClass( wxWindow* parent, wxWindowID id = ID_PANEL, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 830,490 ), long style = wxTAB_TRAVERSAL );
		~SqlBrowserPanelGeneratedClass();
		
		void SplitterOnIdle( wxIdleEvent& )
		{
			Splitter->SetSashPosition( 251 );
			Splitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( SqlBrowserPanelGeneratedClass::SplitterOnIdle ), NULL, this );
		}
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SqlConnectionDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class SqlConnectionDialogGeneratedClass : public wxDialog 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnListboxSelected( wxCommandEvent& event ){ OnListboxSelected( event ); }
		void _wxFB_OnTestButton( wxCommandEvent& event ){ OnTestButton( event ); }
		void _wxFB_OnCancelButton( wxCommandEvent& event ){ OnCancelButton( event ); }
		void _wxFB_OnHelpButton( wxCommandEvent& event ){ OnHelpButton( event ); }
		void _wxFB_OnOkButton( wxCommandEvent& event ){ OnOkButton( event ); }
		
	
	protected:
		enum
		{
			ID_SQLCONNECTIONDIALOGCLASS = 1000,
			ID_LIST,
			ID_HOSTLABEL,
			ID_HOST,
			ID_PORTLABEL,
			ID_PORT,
			ID_DATABASELABEL,
			ID_DATABASE,
			ID_USERLABEL,
			ID_USER,
			ID_PASSWORDLABEL,
			ID_PASSWORD,
			ID_TEST,
		};
		
		wxListBox* List;
		wxStaticText* HostLabel;
		wxTextCtrl* Host;
		wxStaticText* PortLabel;
		wxSpinCtrl* Port;
		wxStaticText* DatabaseLabel;
		wxTextCtrl* Database;
		wxStaticText* UserLabel;
		wxTextCtrl* User;
		wxStaticText* PasswordLabel;
		wxTextCtrl* Password;
		wxButton* m_button2;
		wxStdDialogButtonSizer* m_sdbSizer2;
		wxButton* m_sdbSizer2OK;
		wxButton* m_sdbSizer2Cancel;
		wxButton* m_sdbSizer2Help;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnListboxSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTestButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancelButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		SqlConnectionDialogGeneratedClass( wxWindow* parent, wxWindowID id = ID_SQLCONNECTIONDIALOGCLASS, const wxString& title = wxT("SQL Connections"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 599,274 ), long style = wxDEFAULT_DIALOG_STYLE );
		~SqlConnectionDialogGeneratedClass();
	
};

#endif //__SqlBrowserPluginGeneratedClass__
