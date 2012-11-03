///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SqlBrowserPluginGeneratedClass__
#define __SqlBrowserPluginGeneratedClass__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/grid.h>
#include <wx/panel.h>
#include <wx/checklst.h>
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
			ID_BOTTOMPANLE,
			ID_CONNECTIONLABEL,
			ID_RESULTSLABEL,
			ID_DATAGRID,
		};
		
		wxPanel* BottomPanel;
		wxStaticText* ConnectionLabel;
		wxStaticText* ResultsLabel;
		wxGrid* ResultsGrid;
	
	public:
		
		SqlBrowserPanelGeneratedClass( wxWindow* parent, wxWindowID id = ID_PANEL, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 830,490 ), long style = wxTAB_TRAVERSAL );
		~SqlBrowserPanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SqlConnectionDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class SqlConnectionDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		enum
		{
			ID_SQLCONNECTIONDIALOGCLASS = 1000,
			ID_GLOBALCONNECTIONSLABEL,
			ID_NAMELABEL,
			ID_LABEL,
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
			ID_WARNINGLABEL,
			ID_TESTBUTTON,
			ID_SQLADDBUTTON,
			ID_SQLDELETEBUTTON,
		};
		
		wxStaticText* ConnectionsLabel;
		wxCheckListBox* List;
		wxStaticText* NameLabel;
		wxTextCtrl* Label;
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
		
		wxStaticText* WarningLabel;
		wxButton* TestButton;
		wxButton* AddButton;
		wxButton* DeleteButton;
		wxStdDialogButtonSizer* StdButtonsSizer;
		wxButton* StdButtonsSizerOK;
		wxButton* StdButtonsSizerCancel;
		wxButton* StdButtonsSizerHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnChecklistSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnChecklistToggled( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLabelText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTestButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		SqlConnectionDialogGeneratedClass( wxWindow* parent, wxWindowID id = ID_SQLCONNECTIONDIALOGCLASS, const wxString& title = wxT("SQL Connections"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 667,319 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SqlConnectionDialogGeneratedClass();
	
};

#endif //__SqlBrowserPluginGeneratedClass__
