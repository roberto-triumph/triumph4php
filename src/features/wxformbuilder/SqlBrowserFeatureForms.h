///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SqlBrowserFeatureForms__
#define __SqlBrowserFeatureForms__

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/grid.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/dialog.h>
#include <wx/filepicker.h>
#include <wx/hyperlink.h>
#include <wx/checklst.h>
#include <wx/aui/auibook.h>
#include <wx/radiobox.h>

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
			ID_CONNECTIONLABEL,
			ID_RESULTSLABEL,
			ID_DATAGRID,
		};
		
		wxBitmapButton* RefreshButton;
		wxBitmapButton* HelpButton;
		wxStaticText* ConnectionLabel;
		wxChoice* Connections;
		
		wxStaticText* ResultsLabel;
		wxGrid* ResultsGrid;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRefreshButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnConnectionChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnGridRightClick( wxGridEvent& event ) { event.Skip(); }
		
	
	public:
		
		SqlBrowserPanelGeneratedClass( wxWindow* parent, wxWindowID id = ID_PANEL, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 830,490 ), long style = wxTAB_TRAVERSAL );
		~SqlBrowserPanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class MysqlConnectionDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class MysqlConnectionDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		enum
		{
			ID_WARNINGLABEL = 1000,
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
			ID_TESTBUTTON,
		};
		
		wxStaticText* WarningLabel;
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
		
		wxButton* TestButton;
		wxStdDialogButtonSizer* StdButtonsSizer;
		wxButton* StdButtonsSizerOK;
		wxButton* StdButtonsSizerCancel;
		wxButton* StdButtonsSizerHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnLabelText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTestButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancelButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		MysqlConnectionDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("MySQL Connection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~MysqlConnectionDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SqliteConnectionDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class SqliteConnectionDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* HelpLabel;
		wxStaticText* NameLabel;
		wxTextCtrl* Label;
		wxStaticText* FileLabel;
		wxFilePickerCtrl* File;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
	
	public:
		
		SqliteConnectionDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("SQLite Connection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 491,208 ), long style = wxDEFAULT_DIALOG_STYLE );
		~SqliteConnectionDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SqlConnectionListDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class SqlConnectionListDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		enum
		{
			ID_TESTBUTTON = 1000,
		};
		
		wxStaticText* HelpLabel;
		wxHyperlinkCtrl* HelpLink;
		wxCheckListBox* List;
		wxButton* AdMysqlButton;
		wxButton* AddSqliteButton;
		wxButton* CloneButton;
		wxButton* TestButton;
		wxButton* RemoveSelected;
		wxButton* RemoveAllButton;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;
		wxButton* ButtonSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnListDoubleClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckToggled( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddMysqlButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddSqliteButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCloneButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTestSelectedButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveSelectedButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveAllButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		SqlConnectionListDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("SQL Connections"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SqlConnectionListDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class TableDefinitionPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class TableDefinitionPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxBitmapButton* RefreshButton;
		wxButton* ShowSqlButton;
		wxStaticText* ConnectionLabel;
		wxChoice* Connections;
		wxStaticText* TableLabel;
		wxTextCtrl* TableName;
		wxAuiNotebook* Notebook;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRefreshButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSqlButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTableNameEnter( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		TableDefinitionPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 719,415 ), long style = wxTAB_TRAVERSAL );
		~TableDefinitionPanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class DefinitionIndicesPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class DefinitionIndicesPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxGrid* IndicesGrid;
	
	public:
		
		DefinitionIndicesPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 666,260 ), long style = wxTAB_TRAVERSAL );
		~DefinitionIndicesPanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class DefinitionColumnsPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class DefinitionColumnsPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxGrid* ColumnsGrid;
	
	public:
		
		DefinitionColumnsPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 690,299 ), long style = wxTAB_TRAVERSAL );
		~DefinitionColumnsPanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SqlCopyDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class SqlCopyDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* ColumnDelimLabel;
		wxTextCtrl* ColumnDelim;
		wxStaticText* ColumnEnclosureLabel;
		wxTextCtrl* ColumnEnclosure;
		wxStaticText* RowDelimLabel;
		wxTextCtrl* RowDelim;
		wxStaticText* NullFillterLabel;
		wxTextCtrl* NullFiller;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;
		wxButton* ButtonSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCancelButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		SqlCopyDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Copy Format"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~SqlCopyDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SqlCopyAsInsertDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class SqlCopyAsInsertDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* ColumnsLabel;
		wxCheckListBox* Columns;
		wxRadioBox* LineModeRadio;
		wxButton* CheckAll;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;
		wxButton* ButtonSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCheckAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancelButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		SqlCopyAsInsertDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Copy As SQL Insert"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 421,478 ), long style = wxDEFAULT_DIALOG_STYLE );
		~SqlCopyAsInsertDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SqlCopyAsPhpDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class SqlCopyAsPhpDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* ColumnsLabel;
		wxCheckListBox* Columns;
		wxRadioBox* CopyValues;
		wxRadioBox* ArraySyntaxRadio;
		wxButton* CheckAll;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;
		wxButton* ButtonSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCheckAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancelButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		SqlCopyAsPhpDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Copy As PHP Array"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 421,510 ), long style = wxDEFAULT_DIALOG_STYLE );
		~SqlCopyAsPhpDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SqlBrowserHelpDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class SqlBrowserHelpDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* HelpText;
		wxHyperlinkCtrl* HelpLink;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;
	
	public:
		
		SqlBrowserHelpDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("SQL Browser Help"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~SqlBrowserHelpDialogGeneratedClass();
	
};

#endif //__SqlBrowserFeatureForms__
