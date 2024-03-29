///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DetectorFeatureForms__
#define __DetectorFeatureForms__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/hyperlink.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define ID_PROJECT_CHOICE 1000
#define ID_TEST_DETECTOR_BUTTON 1001
#define ID_ADD_BUTTON 1002

///////////////////////////////////////////////////////////////////////////////
/// Class UrlDetectorPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class UrlDetectorPanelGeneratedClass : public wxPanel
{
	private:

	protected:
		wxStaticText* ProjectLabel;
		wxChoice* ProjectChoice;
		wxButton* TestButton;
		wxButton* AddButton;
		wxBitmapButton* HelpButton;
		wxStaticText* Label;
		wxTreeCtrl* DetectorTree;

		// Virtual event handlers, overide them in your derived class
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }


	public:

		UrlDetectorPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~UrlDetectorPanelGeneratedClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class TemplateFilesDetectorPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class TemplateFilesDetectorPanelGeneratedClass : public wxPanel
{
	private:

	protected:
		wxStaticText* ProjectLabel;
		wxChoice* ProjectChoice;
		wxButton* TestButton;
		wxButton* AddButton;
		wxBitmapButton* HelpButton;
		wxStaticText* UrlLabel;
		wxTextCtrl* UrlToTest;
		wxButton* ChooseUrlButton;
		wxStaticText* Label;
		wxTreeCtrl* DetectorTree;

		// Virtual event handlers, overide them in your derived class
		virtual void OnTestButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnChooseUrlButton( wxCommandEvent& event ) { event.Skip(); }


	public:

		TemplateFilesDetectorPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~TemplateFilesDetectorPanelGeneratedClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class TagDetectorPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class TagDetectorPanelGeneratedClass : public wxPanel
{
	private:

	protected:
		wxStaticText* ProjectLabel;
		wxChoice* ProjectChoice;
		wxButton* TestButton;
		wxButton* AddButton;
		wxBitmapButton* HelpButton;
		wxStaticText* Label;
		wxTreeCtrl* DetectorTree;

		// Virtual event handlers, overide them in your derived class
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }


	public:

		TagDetectorPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~TagDetectorPanelGeneratedClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class DatabaseDetectorPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class DatabaseDetectorPanelGeneratedClass : public wxPanel
{
	private:

	protected:
		wxStaticText* ProjectLabel;
		wxChoice* ProjectChoice;
		wxButton* TestButton;
		wxButton* AddButton;
		wxBitmapButton* HelpButton;
		wxStaticText* Label;
		wxTreeCtrl* DetectorTree;

		// Virtual event handlers, overide them in your derived class
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }


	public:

		DatabaseDetectorPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~DatabaseDetectorPanelGeneratedClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class ConfigDetectorPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class ConfigDetectorPanelGeneratedClass : public wxPanel
{
	private:

	protected:
		wxStaticText* ProjectLabel;
		wxChoice* ProjectChoice;
		wxButton* TestButton;
		wxButton* AddButton;
		wxBitmapButton* HelpButton;
		wxStaticText* Label;
		wxTreeCtrl* DetectorTree;

		// Virtual event handlers, overide them in your derived class
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }


	public:

		ConfigDetectorPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~ConfigDetectorPanelGeneratedClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class UrlDetectorHelpDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class UrlDetectorHelpDialogGeneratedClass : public wxDialog
{
	private:

	protected:
		wxStaticText* HelpText;
		wxHyperlinkCtrl* HelpLink;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;

	public:

		UrlDetectorHelpDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("URL Detectors Help"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~UrlDetectorHelpDialogGeneratedClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class TemplateFilesDetectorHelpGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class TemplateFilesDetectorHelpGeneratedClass : public wxDialog
{
	private:

	protected:
		wxStaticText* HelpText;
		wxHyperlinkCtrl* HelpLink;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;

	public:

		TemplateFilesDetectorHelpGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Template File Detectors Help"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~TemplateFilesDetectorHelpGeneratedClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class ConfigDetectorsHelpDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class ConfigDetectorsHelpDialogGeneratedClass : public wxDialog
{
	private:

	protected:
		wxStaticText* HelpText;
		wxHyperlinkCtrl* HelpLink;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;

	public:

		ConfigDetectorsHelpDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Config Detectors Help"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~ConfigDetectorsHelpDialogGeneratedClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class DatabaseDetectorHelpDialogGenereatedClass
///////////////////////////////////////////////////////////////////////////////
class DatabaseDetectorHelpDialogGenereatedClass : public wxDialog
{
	private:

	protected:
		wxStaticText* HelpText;
		wxHyperlinkCtrl* HelpLink;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;

	public:

		DatabaseDetectorHelpDialogGenereatedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Database Detectors Help"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~DatabaseDetectorHelpDialogGenereatedClass();

};

#endif //__DetectorFeatureForms__
