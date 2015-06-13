///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __RunConsoleFeatureForms__
#define __RunConsoleFeatureForms__

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
#include <wx/frame.h>
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
		wxStaticText* WorkingDirectoryLabel;
		wxDirPickerCtrl* WorkingDirectory;
		wxStaticText* ArgumentsLabel;
		wxTextCtrl* Arguments;
		wxStaticText* DescriptionLabel;
		wxTextCtrl* Description;
		wxCheckBox* WaitForArguments;
		wxCheckBox* ShowInToolbar;
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
		
		CliCommandEditDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Add/Edit Commands"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 422,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~CliCommandEditDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class MyFrame1
///////////////////////////////////////////////////////////////////////////////
class MyFrame1 : public wxFrame 
{
	private:
	
	protected:
	
	public:
		
		MyFrame1( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~MyFrame1();
	
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

#endif //__RunConsoleFeatureForms__
