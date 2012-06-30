///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ProjectPluginGeneratedPanelClass__
#define __ProjectPluginGeneratedPanelClass__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/filepicker.h>
#include <wx/gbsizer.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ProjectPluginGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class ProjectPluginGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* HelpText;
		wxStaticText* Label;
		wxTextCtrl* ExplorerExecutable;
		wxFilePickerCtrl* FilePicker;
		wxStaticText* WildcardHelpText;
		wxStaticText* PhpLabel;
		wxTextCtrl* PhpFileFilters;
		wxStaticText* CssLabel;
		wxTextCtrl* CssFileFilters;
		wxStaticText* SqlLabel;
		wxTextCtrl* SqlFileFilters;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFileChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		
	
	public:
		
		ProjectPluginGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,387 ), long style = wxTAB_TRAVERSAL );
		~ProjectPluginGeneratedPanelClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ProjectDefinitionDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class ProjectDefinitionDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* Label;
		wxListBox* SourcesList;
		wxButton* AddButton;
		wxButton* RemoveButton;
		wxButton* EditButton;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSourcesListDoubleClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddSource( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveSource( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditSource( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ProjectDefinitionDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Project Definition"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 592,475 ), long style = wxDEFAULT_DIALOG_STYLE );
		~ProjectDefinitionDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ProjectSourceDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class ProjectSourceDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* HelpLabel;
		wxStaticText* RootDirectoryLabel;
		wxDirPickerCtrl* RootDirectory;
		wxStaticText* IncludeWildcardsLabel;
		wxTextCtrl* IncludeWildcards;
		wxStaticText* ExcludeWildcardsLabel;
		wxTextCtrl* ExcludeWildcards;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ProjectSourceDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Project Source"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 513,438 ), long style = wxDEFAULT_DIALOG_STYLE );
		~ProjectSourceDialogGeneratedClass();
	
};

#endif //__ProjectPluginGeneratedPanelClass__
