///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ExplorerFeatureForms__
#define __ExplorerFeatureForms__

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
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/stattext.h>
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/hyperlink.h>
#include <wx/dialog.h>
#include <wx/filepicker.h>
#include <wx/statbox.h>
#include <wx/textctrl.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ModalExplorerGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class ModalExplorerGeneratedPanelClass : public wxPanel
{
	private:

	protected:
		wxBitmapButton* FilterButton;
		wxBitmapButton* ParentButton;
		wxBitmapButton* RefreshButton;
		wxBitmapButton* HelpButton;
		wxComboBox* Directory;
		wxSplitterWindow* Splitter;
		wxPanel* SourcesPanel;
		wxListCtrl* SourcesList;
		wxStaticText* SourcesLabel;
		wxPanel* FilesPanel;
		wxListCtrl* List;
		wxStaticText* ListLabel;

		// Virtual event handlers, overide them in your derived class
		virtual void OnFilterButtonLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnParentButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRefreshClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDirectoryEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSourceActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnListKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnListEndLabelEdit( wxListEvent& event ) { event.Skip(); }
		virtual void OnListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnListItemRightClick( wxListEvent& event ) { event.Skip(); }
		virtual void OnListItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnListRightDown( wxMouseEvent& event ) { event.Skip(); }


	public:

		ModalExplorerGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 722,332 ), long style = wxTAB_TRAVERSAL );
		~ModalExplorerGeneratedPanelClass();

		void SplitterOnIdle( wxIdleEvent& )
		{
			Splitter->SetSashPosition( 164 );
			Splitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( ModalExplorerGeneratedPanelClass::SplitterOnIdle ), NULL, this );
		}

};

///////////////////////////////////////////////////////////////////////////////
/// Class ExplorerOutlineGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class ExplorerOutlineGeneratedPanelClass : public wxPanel
{
	private:

	protected:
		wxBitmapButton* FilterButton;
		wxBitmapButton* ParentButton;
		wxBitmapButton* RefreshButton;
		wxBitmapButton* HelpButton;
		wxComboBox* Directory;
		wxListCtrl* List;
		wxStaticText* ListLabel;

		// Virtual event handlers, overide them in your derived class
		virtual void OnFilterButtonLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnParentButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRefreshClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDirectorySelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDirectoryEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnListKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnListEndLabelEdit( wxListEvent& event ) { event.Skip(); }
		virtual void OnListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnListItemRightClick( wxListEvent& event ) { event.Skip(); }
		virtual void OnListItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnListRightDown( wxMouseEvent& event ) { event.Skip(); }


	public:

		ExplorerOutlineGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 374,756 ), long style = wxTAB_TRAVERSAL );
		~ExplorerOutlineGeneratedPanelClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class ExplorerHelpGeneratedDialogClass
///////////////////////////////////////////////////////////////////////////////
class ExplorerHelpGeneratedDialogClass : public wxDialog
{
	private:

	protected:
		wxStaticText* HelpText;
		wxHyperlinkCtrl* HelpLink;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;

	public:

		ExplorerHelpGeneratedDialogClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Explorer Help"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~ExplorerHelpGeneratedDialogClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class ExplorerOptionsGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class ExplorerOptionsGeneratedPanelClass : public wxPanel
{
	private:

	protected:
		wxStaticText* HelpLabel;
		wxStaticText* FileManagerLabel;
		wxFilePickerCtrl* FileManager;
		wxStaticText* ShellLabel;
		wxFilePickerCtrl* Shell;

	public:

		ExplorerOptionsGeneratedPanelClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~ExplorerOptionsGeneratedPanelClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class ExplorerNewFileGeneratedDialogClass
///////////////////////////////////////////////////////////////////////////////
class ExplorerNewFileGeneratedDialogClass : public wxDialog
{
	private:

	protected:
		wxStaticText* Label;
		wxTextCtrl* FileNameText;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;
		wxButton* ButtonSizerCancel;

		// Virtual event handlers, overide them in your derived class
		virtual void OnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }


	public:

		ExplorerNewFileGeneratedDialogClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Create a new file"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 450,142 ), long style = wxDEFAULT_DIALOG_STYLE );
		~ExplorerNewFileGeneratedDialogClass();

};

#endif //__ExplorerFeatureForms__
