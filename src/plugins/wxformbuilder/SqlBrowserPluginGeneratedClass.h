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
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/splitter.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class SqlBrowserPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class SqlBrowserPanelGeneratedClass : public wxPanel 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnRunButton( wxCommandEvent& event ){ OnRunButton( event ); }
		
	
	protected:
		enum
		{
			ID_PANEL = 1000,
			ID_SPLITTER,
			ID_TOPPANEL,
			ID_GRIDPANEL,
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
			ID_RUNBUTTON,
			ID_BOTTOMPANLE,
			ID_RESULTSLABEL,
			ID_DATAGRID,
		};
		
		wxSplitterWindow* Splitter;
		wxPanel* TopPanel;
		wxFlexGridSizer* TopPanelSizer;
		wxPanel* ButtonPanel;
		wxStaticText* HostLabel;
		wxTextCtrl* Host;
		wxStaticText* PortLabel;
		wxSpinCtrl* Port;
		wxStaticText* DatabaseLabel;
		wxComboBox* Database;
		wxStaticText* UserLabel;
		wxTextCtrl* User;
		wxStaticText* PasswordLabel;
		wxTextCtrl* Password;
		wxButton* RunButton;
		wxPanel* BottomPanel;
		wxStaticText* ResultsLabel;
		wxGrid* ResultsGrid;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRunButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		SqlBrowserPanelGeneratedClass( wxWindow* parent, wxWindowID id = ID_PANEL, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 830,490 ), long style = wxTAB_TRAVERSAL );
		~SqlBrowserPanelGeneratedClass();
		
		void SplitterOnIdle( wxIdleEvent& )
		{
			Splitter->SetSashPosition( 251 );
			Splitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( SqlBrowserPanelGeneratedClass::SplitterOnIdle ), NULL, this );
		}
	
};

#endif //__SqlBrowserPluginGeneratedClass__
