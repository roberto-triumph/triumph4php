///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __FindInFilesPluginDialogGeneratedClass__
#define __FindInFilesPluginDialogGeneratedClass__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/filepicker.h>
#include <wx/statbox.h>
#include <wx/radiobox.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/listbox.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class FindInFilesDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class FindInFilesDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		enum
		{
			ID_FINDTEXT = 1000,
			ID_REPLACEWITHTEXT,
		};
		
		wxStaticText* FindLabel;
		wxButton* RegexFindHelpButton;
		wxComboBox* FindText;
		wxStaticText* ReplaceWithLabel;
		wxButton* RegexReplaceWithHelpButton;
		wxComboBox* ReplaceWithText;
		wxComboBox* Directory;
		wxDirPickerCtrl* DirectoryDirPicker;
		wxComboBox* FilesFilter;
		wxRadioBox* FinderMode;
		wxCheckBox* CaseSensitive;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRegExFindHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRegExReplaceHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDirChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void OnCancelButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		FindInFilesDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Find In Files"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 599,402 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~FindInFilesDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class FindInFilesResultsPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class FindInFilesResultsPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		enum
		{
			ID_REPLACE_BUTTON = 1000,
			ID_REPLACE_ALL_IN_FILE_BUTTON,
			ID_REPLACE_ALL_BUTTON,
			ID_STOP_BUTTON,
			ID_COPY_SELECTED_BUTTON,
			ID_COPY_ALL_BUTTON,
			ID_FIND_IN_FILES_RESULTS,
		};
		
		wxButton* ReplaceButton;
		wxButton* ReplaceAllInFileButton;
		wxButton* ReplaceInAllFilesButton;
		wxButton* StopButton;
		wxButton* CopySelectedButton;
		wxButton* CopyAllButton;
		wxStaticText* ResultText;
		wxListBox* ResultsList;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnReplaceButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReplaceAllInFileButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReplaceInAllFilesButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStopButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCopySelectedButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCopyAllButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDoubleClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		FindInFilesResultsPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 810,255 ), long style = wxTAB_TRAVERSAL );
		~FindInFilesResultsPanelGeneratedClass();
	
};

#endif //__FindInFilesPluginDialogGeneratedClass__
