///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __RunConsolePluginGeneratedClass__
#define __RunConsolePluginGeneratedClass__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/filepicker.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/listbox.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class RunConsolePanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class RunConsolePanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxTextCtrl* Command;
		wxButton* RunButton;
		wxButton* ClearButton;
		wxButton* StoreButton;
		wxTextCtrl* OutputWindow;
		
		// Virtual event handlers, overide them in your derived class
		virtual void RunCommand( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClear( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStoreButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMouseMotion( wxMouseEvent& event ) { event.Skip(); }
		
	
	public:
		
		RunConsolePanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 696,210 ), long style = wxTAB_TRAVERSAL );
		~RunConsolePanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CliCommandEditDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class CliCommandEditDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* ExecutableLabel;
		wxTextCtrl* Executable;
		wxFilePickerCtrl* ExecutableFilePicker;
		wxStaticText* ArgumentsLabel;
		wxTextCtrl* Arguments;
		wxStaticText* DescriptionLabel;
		wxTextCtrl* Description;
		wxCheckBox* WaitForArguments;
		wxCheckBox* ShowInToolbar;
		wxStaticText* HelpLabel;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
		wxButton* ButtonsSizerHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFileChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void OnDescriptionText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		CliCommandEditDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Add/Edit Commands"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 422,396 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~CliCommandEditDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CliCommandListDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class CliCommandListDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* SavedLabel;
		wxListBox* CommandsList;
		wxButton* MoveUpButton;
		wxButton* MoveDownButton;
		wxButton* AddButton;
		wxButton* DeleteButton;
		wxButton* EditButton;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
		wxButton* ButtonsSizerHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnListDoubleClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDownButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		CliCommandListDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Console Commands"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 511,484 ), long style = wxDEFAULT_DIALOG_STYLE );
		~CliCommandListDialogGeneratedClass();
	
};

#endif //__RunConsolePluginGeneratedClass__
