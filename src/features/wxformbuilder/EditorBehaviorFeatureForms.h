///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __EditorBehaviorFeatureForms__
#define __EditorBehaviorFeatureForms__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/radiobox.h>
#include <wx/statbox.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class EditorBehaviorPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class EditorBehaviorPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		enum
		{
			ID_RIGHT_MARGIN_CHECK = 1000,
			ID_ENABLEWORDWRAP,
			ID_AUTOCOMPLETION,
			ID_SPACES_INSTEAD_OF_TABS,
			ID_ENABLEDYNAMICAUTOCOMPLETION,
			ID_TRIMTRAILINGSPACEBEFORESAVE,
			ID_REMOVETRAILINGBLANKLINES,
			ID_ENABLECALLTIPONMOUSEHOVER,
			ID_ENABLEMULTIPLESELECTION,
			ID_ENABLEVIRTUALSPACE,
			ID_ENABLERECTANGULARSELECTION,
			ID_SHOWWHITESPACE,
			ID_ZOOMLABEL,
			ID_ZOOM,
		};
		
		wxCheckBox* EnableCodeFolding;
		wxCheckBox* EnableLineNumbers;
		wxCheckBox* EnableAutomaticLineIndentation;
		wxCheckBox* EnableRightMargin;
		wxSpinCtrl* RightMargin;
		wxCheckBox* EnableIndentationGuides;
		wxCheckBox* EnableWordWrap;
		wxCheckBox* EnableLineEndings;
		wxCheckBox* EnableAutoCompletion;
		wxCheckBox* IndentUsingTabs;
		wxCheckBox* EnableDynamicAutoCompletion;
		wxCheckBox* TrimTrailingSpaceBeforeSave;
		wxCheckBox* RemoveTrailingBlankLines;
		wxCheckBox* EnableCallTipOnMouseHover;
		wxCheckBox* EnableMultipleSelection;
		wxCheckBox* EnableVirtualSpace;
		wxCheckBox* EnableRectangularSelection;
		wxCheckBox* ShowWhitespace;
		wxStaticText* ZoomLabel;
		wxSpinCtrl* Zoom;
		wxStaticText* TabWidthLabel;
		wxSpinCtrl* TabWidth;
		wxStaticText* SpacesPerIndentLabel;
		wxSpinCtrl* SpacesPerIndent;
		wxRadioBox* LineEndingMode;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCheckRightMargin( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnIndentUsingSpaces( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		EditorBehaviorPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~EditorBehaviorPanelGeneratedClass();
	
};

#endif //__EditorBehaviorFeatureForms__
