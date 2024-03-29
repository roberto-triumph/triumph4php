///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __FinderFeatureForms__
#define __FinderFeatureForms__

#include <wx/intl.h>

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
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/hyperlink.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class FinderPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class FinderPanelGeneratedClass : public wxPanel
{
	private:

	protected:
		enum
		{
			ID_FIND_CLOSE = 1000,
			ID_FINDTEXT,
			ID_WRAP,
		};

		wxBitmapButton* CloseButton;
		wxBitmapButton* PreviousButton;
		wxBitmapButton* NextButton;
		wxBitmapButton* HelpButton;
		wxStaticText* FindLabel;
		wxButton* RegExFindHelpButton;
		wxComboBox* FindText;
		wxCheckBox* Wrap;
		wxRadioBox* FinderMode;
		wxStaticText* ResultText;

		// Virtual event handlers, overide them in your derived class
		virtual void OnCloseButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPreviousButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNextButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRegExFindHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFindKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnFindKillFocus( wxFocusEvent& event ) { event.Skip(); }
		virtual void OnFindEnter( wxCommandEvent& event ) { event.Skip(); }


	public:

		FinderPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 995,58 ), long style = wxTAB_TRAVERSAL );
		~FinderPanelGeneratedClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class ReplacePanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class ReplacePanelGeneratedClass : public wxPanel
{
	private:

	protected:
		enum
		{
			ID_FINDTEXT = 1000,
			ID_REPLACEWITHTEXT,
			ID_REPLACE_CLOSE,
			ID_WRAP,
		};

		wxStaticText* FindLabel;
		wxButton* RegExFindHelpButton;
		wxComboBox* FindText;
		wxStaticText* ReplaceWithLabel;
		wxButton* RegExReplaceHelpButton;
		wxComboBox* ReplaceWithText;
		wxBitmapButton* CloseButton;
		wxBitmapButton* PreviousButton;
		wxBitmapButton* NextButton;
		wxBitmapButton* HelpButton;
		wxBitmapButton* ReplaceButton;
		wxBitmapButton* ReplaceAllButton;
		wxBitmapButton* UndoButton;
		wxCheckBox* Wrap;
		wxStaticText* ResultText;
		wxRadioBox* FinderMode;

		// Virtual event handlers, overide them in your derived class
		virtual void OnRegExFindHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFindKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnFindKillFocus( wxFocusEvent& event ) { event.Skip(); }
		virtual void OnFindEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReplaceRegExFindHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReplaceKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnReplaceKillFocus( wxFocusEvent& event ) { event.Skip(); }
		virtual void OnReplaceEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCloseButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPreviousButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNextButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReplaceButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReplaceAllButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUndoButton( wxCommandEvent& event ) { event.Skip(); }


	public:

		ReplacePanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1003,105 ), long style = wxTAB_TRAVERSAL );
		~ReplacePanelGeneratedClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class FindHelpDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class FindHelpDialogGeneratedClass : public wxDialog
{
	private:

	protected:
		wxStaticText* HelpText;
		wxHyperlinkCtrl* RegexHelpLink;
		wxHyperlinkCtrl* HelpLink;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;

	public:

		FindHelpDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Help On Find"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~FindHelpDialogGeneratedClass();

};

#endif //__FinderFeatureForms__
