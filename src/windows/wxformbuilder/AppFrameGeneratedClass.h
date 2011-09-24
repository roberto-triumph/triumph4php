///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __AppFrameGeneratedClass__
#define __AppFrameGeneratedClass__

#include <wx/intl.h>

namespace mvceditor{ class NotebookClass; }

#include <wx/gdicmn.h>
#include <wx/aui/auibook.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/statusbr.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/choice.h>
#include <wx/statbox.h>
#include <wx/listbox.h>
#include <wx/stattext.h>
#include <wx/fontpicker.h>
#include <wx/clrpicker.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/spinctrl.h>
#include <wx/radiobox.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class AppFrameGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class AppFrameGeneratedClass : public wxFrame 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnClose( wxCloseEvent& event ){ OnClose( event ); }
		void _wxFB_OnContentNotebookPageChanged( wxAuiNotebookEvent& event ){ OnContentNotebookPageChanged( event ); }
		void _wxFB_OnFileNew( wxCommandEvent& event ){ OnFileNew( event ); }
		void _wxFB_OnFileOpen( wxCommandEvent& event ){ OnFileOpen( event ); }
		void _wxFB_OnFileSave( wxCommandEvent& event ){ OnFileSave( event ); }
		void _wxFB_OnFileSaveAs( wxCommandEvent& event ){ OnFileSaveAs( event ); }
		void _wxFB_OnFileRevert( wxCommandEvent& event ){ OnFileRevert( event ); }
		void _wxFB_OnFileClose( wxCommandEvent& event ){ OnFileClose( event ); }
		void _wxFB_OnFileExit( wxCommandEvent& event ){ OnFileExit( event ); }
		void _wxFB_OnEditCut( wxCommandEvent& event ){ OnEditCut( event ); }
		void _wxFB_OnEditCopy( wxCommandEvent& event ){ OnEditCopy( event ); }
		void _wxFB_OnEditPaste( wxCommandEvent& event ){ OnEditPaste( event ); }
		void _wxFB_OnEditSelectAll( wxCommandEvent& event ){ OnEditSelectAll( event ); }
		void _wxFB_OnEditContentAssist( wxCommandEvent& event ){ OnEditContentAssist( event ); }
		void _wxFB_OnEditCallTip( wxCommandEvent& event ){ OnEditCallTip( event ); }
		void _wxFB_OnEditPreferences( wxCommandEvent& event ){ OnEditPreferences( event ); }
		void _wxFB_OnProjectOpen( wxCommandEvent& event ){ OnProjectOpen( event ); }
		void _wxFB_OnHelpAbout( wxCommandEvent& event ){ OnHelpAbout( event ); }
		
	
	protected:
		enum
		{
			ID_NOTEBOOK = 1000,
			ID_FILE_REVERT,
			ID_FILE_CLOSE,
			ID_EDIT_CONTENT_ASSIST,
			ID_EDIT_CALL_TIP,
			ID_EDIT_PREFERENCES,
			ID_PROJECT_OPEN,
			ID_ABOUT,
		};
		
		mvceditor::NotebookClass* Notebook;
		wxStatusBar* StatusBar;
		wxMenuBar* MenuBar;
		wxMenu* FileMenu;
		wxMenu* EditMenu;
		wxMenu* ProjectMenu;
		wxMenu* ToolsMenu;
		wxMenu* HelpMenu;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnContentNotebookPageChanged( wxAuiNotebookEvent& event ) { event.Skip(); }
		virtual void OnFileNew( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFileOpen( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFileSave( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFileSaveAs( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFileRevert( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFileClose( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFileExit( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditCut( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditCopy( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditPaste( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditSelectAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditContentAssist( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditCallTip( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditPreferences( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnProjectOpen( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpAbout( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		AppFrameGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("MVC Editor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~AppFrameGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class EditColorsPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class EditColorsPanelGeneratedClass : public wxPanel 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnThemeChoice( wxCommandEvent& event ){ OnThemeChoice( event ); }
		void _wxFB_OnListBox( wxCommandEvent& event ){ OnListBox( event ); }
		void _wxFB_OnFontChanged( wxFontPickerEvent& event ){ OnFontChanged( event ); }
		void _wxFB_OnColorChanged( wxColourPickerEvent& event ){ OnColorChanged( event ); }
		void _wxFB_OnCheck( wxCommandEvent& event ){ OnCheck( event ); }
		
	
	protected:
		enum
		{
			ID_FOREGROUND_COLOR = 1000,
			ID_BACKGROUND_COLOR,
			ID_BOLD,
			ID_ITALIC,
		};
		
		wxChoice* Theme;
		wxListBox* Styles;
		wxStaticText* StyleFontLabel;
		wxFontPickerCtrl* Font;
		wxStaticText* ForegroundColorLabel;
		wxColourPickerCtrl* ForegroundColor;
		wxStaticText* BackgroundColorLabel;
		wxColourPickerCtrl* BackgroundColor;
		wxCheckBox* Bold;
		wxCheckBox* Italic;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnThemeChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnListBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFontChanged( wxFontPickerEvent& event ) { event.Skip(); }
		virtual void OnColorChanged( wxColourPickerEvent& event ) { event.Skip(); }
		virtual void OnCheck( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		EditColorsPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 526,361 ), long style = wxTAB_TRAVERSAL );
		~EditColorsPanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class EditorBehaviorPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class EditorBehaviorPanelGeneratedClass : public wxPanel 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnCheckRightMargin( wxCommandEvent& event ){ OnCheckRightMargin( event ); }
		void _wxFB_OnIndentUsingSpaces( wxCommandEvent& event ){ OnIndentUsingSpaces( event ); }
		
	
	protected:
		enum
		{
			ID_RIGHT_MARGIN_CHECK = 1000,
			ID_SPACES_INSTEAD_OF_TABS,
			ID_AUTOCOMPLETION,
		};
		
		wxCheckBox* EnableCodeFolding;
		wxCheckBox* EnableLineNumbers;
		wxCheckBox* EnableAutomaticLineIndentation;
		wxCheckBox* EnableRightMargin;
		wxSpinCtrl* RightMargin;
		wxCheckBox* EnableIndentationGuides;
		wxCheckBox* EnableLineEndings;
		wxCheckBox* IndentUsingTabs;
		wxCheckBox* EnableAutoCompletion;
		wxStaticText* TabWidthLabel;
		wxSpinCtrl* TabWidth;
		wxStaticText* SpacesPerIndentLabel;
		wxSpinCtrl* SpacesPerIndent;
		wxRadioBox* LineEndingMode;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCheckRightMargin( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnIndentUsingSpaces( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		EditorBehaviorPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 572,295 ), long style = wxTAB_TRAVERSAL );
		~EditorBehaviorPanelGeneratedClass();
	
};

#endif //__AppFrameGeneratedClass__
