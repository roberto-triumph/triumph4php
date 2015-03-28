///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LintFeatureForms__
#define __LintFeatureForms__

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
#include <wx/sizer.h>
#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/filepicker.h>
#include <wx/dialog.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/hyperlink.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class LintResultsGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class LintResultsGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		enum
		{
			ID_ERRORS_LIST = 1000,
		};
		
		wxBitmapButton* RunButton;
		wxBitmapButton* HelpButton;
		wxBitmapButton* SuppressionButton;
		wxStaticText* Label;
		wxDataViewListCtrl* ErrorsList;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRunButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSuppressionButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		LintResultsGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 774,371 ), long style = wxTAB_TRAVERSAL );
		~LintResultsGeneratedPanelClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class LintSuppressionsGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class LintSuppressionsGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		enum
		{
			ID_SUPPRESSIONS_LIST = 1000,
		};
		
		wxBitmapButton* HelpButton;
		wxBitmapButton* AddButton;
		wxBitmapButton* EditButton;
		wxBitmapButton* DeleteButton;
		wxBitmapButton* DeleteAllButton;
		wxStaticText* Label;
		wxDataViewListCtrl* SuppressionsList;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteAllButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnKeyDown( wxKeyEvent& event ) { event.Skip(); }
		
	
	public:
		
		LintSuppressionsGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 774,371 ), long style = wxTAB_TRAVERSAL );
		~LintSuppressionsGeneratedPanelClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class LintSuppressionRuleGeneratedDialogClass
///////////////////////////////////////////////////////////////////////////////
class LintSuppressionRuleGeneratedDialogClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* HelpLabel;
		wxStaticText* TypeLabel;
		wxChoice* Types;
		wxStaticText* TargetLabel;
		wxTextCtrl* Target;
		wxStaticText* LocationTypeLabel;
		wxRadioButton* DirectoryRadio;
		wxRadioButton* FileRadio;
		wxStaticText* DirectoryLabel;
		wxDirPickerCtrl* Directory;
		wxStaticText* FileLabel;
		wxFilePickerCtrl* File;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTypeChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDirectoryRadio( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFileRadio( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		LintSuppressionRuleGeneratedDialogClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Lint Suppression Rule"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~LintSuppressionRuleGeneratedDialogClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class LintPreferencesGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class LintPreferencesGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* Help;
		wxCheckBox* CheckOnSave;
		wxCheckBox* CheckUnitializedVariables;
		wxCheckBox* CheckUnitializedGlobalVariables;
		wxCheckBox* CheckUnknownIdentifiers;
		wxCheckBox* CheckFunctionArgumentCount;
	
	public:
		
		LintPreferencesGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 560,560 ), long style = wxTAB_TRAVERSAL );
		~LintPreferencesGeneratedPanelClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class LintErrorGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class LintErrorGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* ErrorLabel;
		wxStaticText* HelpLabel;
		wxHyperlinkCtrl* GoToLink;
		wxHyperlinkCtrl* DismissLink;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnGoToLink( wxHyperlinkEvent& event ) { event.Skip(); }
		virtual void OnDismissLink( wxHyperlinkEvent& event ) { event.Skip(); }
		
	
	public:
		
		LintErrorGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 488,77 ), long style = wxSIMPLE_BORDER|wxTAB_TRAVERSAL|wxWANTS_CHARS );
		~LintErrorGeneratedPanelClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class LintHelpDialogGeneratedDialogClass
///////////////////////////////////////////////////////////////////////////////
class LintHelpDialogGeneratedDialogClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* HelpText;
		wxHyperlinkCtrl* HelpLink;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;
	
	public:
		
		LintHelpDialogGeneratedDialogClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("PHP Lint Check Help"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~LintHelpDialogGeneratedDialogClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class LintSuppressionsHelpGeneratedDialogClass
///////////////////////////////////////////////////////////////////////////////
class LintSuppressionsHelpGeneratedDialogClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* HelpLabel;
		wxHyperlinkCtrl* HelpLink;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
	
	public:
		
		LintSuppressionsHelpGeneratedDialogClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("PHP Lint Suppressions Help"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~LintSuppressionsHelpGeneratedDialogClass();
	
};

#endif //__LintFeatureForms__
