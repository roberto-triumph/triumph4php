///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SyntaxHighlightFeatureForms__
#define __SyntaxHighlightFeatureForms__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/listbox.h>
#include <wx/fontpicker.h>
#include <wx/clrpicker.h>
#include <wx/checkbox.h>
#include <wx/aui/auibook.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class SyntaxHighlightPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class SyntaxHighlightPanelGeneratedClass : public wxPanel
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
			ID_SYNTAXHIGHLIGHTPANELGENERATEDCLASS = 1000,
			ID_THEMEHELP,
			ID_FOREGROUND_COLOR,
			ID_BACKGROUND_COLOR,
			ID_BOLD,
			ID_ITALIC,
			ID_PREVIEWLABEL,
			ID_PREVIEWNOTEBOOK,
		};

		wxStaticText* ThemeHelp;
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
		wxFlexGridSizer* StyleEditSizer;
		wxStaticText* PreviewLabel;
		wxAuiNotebook* PreviewNotebook;

		// Virtual event handlers, overide them in your derived class
		virtual void OnThemeChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnListBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFontChanged( wxFontPickerEvent& event ) { event.Skip(); }
		virtual void OnColorChanged( wxColourPickerEvent& event ) { event.Skip(); }
		virtual void OnCheck( wxCommandEvent& event ) { event.Skip(); }


	public:

		SyntaxHighlightPanelGeneratedClass( wxWindow* parent, wxWindowID id = ID_SYNTAXHIGHLIGHTPANELGENERATEDCLASS, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 533,480 ), long style = wxTAB_TRAVERSAL );
		~SyntaxHighlightPanelGeneratedClass();

};

#endif //__SyntaxHighlightFeatureForms__
