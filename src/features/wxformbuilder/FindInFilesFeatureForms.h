///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __FindInFilesFeatureForms__
#define __FindInFilesFeatureForms__

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
#include <wx/hyperlink.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/dataview.h>
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
			ID_REGEXREPLACEHELPBUTTON,
			ID_REPLACEWITHTEXT,
			ID_FILESFILTER,
		};
		
		wxStaticText* FindLabel;
		wxButton* RegExFindHelpButton;
		wxComboBox* FindText;
		wxStaticText* ReplaceWithLabel;
		wxButton* RegexReplaceWithHelpButton;
		wxComboBox* ReplaceWithText;
		wxComboBox* Directory;
		wxDirPickerCtrl* DirectoryDirPicker;
		wxComboBox* FilesFilter;
		wxRadioBox* FinderMode;
		wxCheckBox* DoHiddenFiles;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRegExFindHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnKillFocusFindText( wxFocusEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRegExReplaceHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnKillFocusReplaceText( wxFocusEvent& event ) { event.Skip(); }
		virtual void OnDirChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void OnCancelButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		FindInFilesDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Find In Files"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~FindInFilesDialogGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class FindInFilesHelpDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class FindInFilesHelpDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* HelpText;
		wxHyperlinkCtrl* HelpLink;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;
		wxButton* ButtonSizerCancel;
	
	public:
		
		FindInFilesHelpDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Find In Files Help"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~FindInFilesHelpDialogGeneratedClass();
	
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
			ID_REPLACEBUTTON = 1000,
			ID_REPLACEALLINFILEBUTTON,
			ID_REPLACEINALLFILESBUTTON,
			ID_COPYSELETEDBUTTON,
			ID_COPYALLBUTTON,
			ID_STOPBUTTON,
			ID_REPLACEREGEXHELPBUTTON,
			ID_REPLACETEXT,
			ID_RESULTS_LIST,
		};
		
		wxBitmapButton* ReplaceButton;
		wxBitmapButton* ReplaceAllInFileButton;
		wxBitmapButton* ReplaceInAllFilesButton;
		wxBitmapButton* PreviousHitButton;
		wxBitmapButton* NextHitButton;
		wxBitmapButton* CopySelectedButton;
		wxBitmapButton* CopyAllButton;
		wxBitmapButton* StopButton;
		wxStaticText* ReplaceLabel;
		wxButton* RegexReplaceWithHelpButton;
		wxComboBox* ReplaceWithText;
		wxStaticText* FindLabel;
		wxStaticText* ResultText;
		wxDataViewListCtrl* ResultsList;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnReplaceButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReplaceAllInFileButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReplaceInAllFilesButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPreviousHitButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNextHitButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCopySelectedButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCopyAllButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStopButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRegExReplaceHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnKillFocusReplaceText( wxFocusEvent& event ) { event.Skip(); }
		
	
	public:
		
		FindInFilesResultsPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 810,255 ), long style = wxTAB_TRAVERSAL );
		~FindInFilesResultsPanelGeneratedClass();
	
};

#endif //__FindInFilesFeatureForms__
