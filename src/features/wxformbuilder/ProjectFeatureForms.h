///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ProjectFeatureForms__
#define __ProjectFeatureForms__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/filepicker.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/checklst.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ProjectPreferencesGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class ProjectPreferencesGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* HelpText;
		wxStaticText* Label;
		wxTextCtrl* ExplorerExecutable;
		wxFilePickerCtrl* FilePicker;
		wxStaticText* WildcardHelpText;
		wxStaticText* PhpLabel;
		wxTextCtrl* PhpFileExtensions;
		wxStaticText* CssLabel;
		wxTextCtrl* CssFileExtensions;
		wxStaticText* SqlLabel;
		wxTextCtrl* SqlFileExtensions;
		wxStaticText* MiscLabel;
		wxTextCtrl* MiscFileExtensions;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFileChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		
	
	public:
		
		ProjectPreferencesGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,416 ), long style = wxTAB_TRAVERSAL );
		~ProjectPreferencesGeneratedPanelClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ProjectDefinitionDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class ProjectDefinitionDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* HelpLabel;
		wxStaticText* LabelStatic;
		wxTextCtrl* Label;
		wxStaticText* SourcesLabel;
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
		
		ProjectDefinitionDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Project Definition"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 536,492 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
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
		
		ProjectSourceDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Project Source"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 536,541 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~ProjectSourceDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ProjectListDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class ProjectListDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* HelpLabel;
		wxCheckListBox* ProjectsList;
		wxButton* SelectAll;
		wxButton* AddButton;
		wxButton* AddFromDirectoryButton;
		wxButton* EditButton;
		wxButton* RemoveButton;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
		wxButton* ButtonsSizerHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnProjectsListDoubleClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnProjectsListCheckbox( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectAllButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddFromDirectoryButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancelButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ProjectListDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Defined Projects"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 504,507 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~ProjectListDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class MultipleSelectDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class MultipleSelectDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* Label;
		wxCheckListBox* Checklist;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCancelButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		MultipleSelectDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 355,249 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~MultipleSelectDialogGeneratedClass();
	
};

#endif //__ProjectFeatureForms__
