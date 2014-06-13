///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DebuggerFeatureForms__
#define __DebuggerFeatureForms__

#include <wx/gdicmn.h>
#include <wx/aui/auibook.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/listctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/textctrl.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DebuggerPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class DebuggerPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxAuiNotebook* Notebook;
	
	public:
		
		DebuggerPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~DebuggerPanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class DebuggerStackPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class DebuggerStackPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* Label;
		wxListCtrl* StackList;
	
	public:
		
		DebuggerStackPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~DebuggerStackPanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class DebuggerLogPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class DebuggerLogPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxBitmapButton* ClearButton;
		wxStaticText* LogLabel;
		wxTextCtrl* Text;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClearButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DebuggerLogPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~DebuggerLogPanelGeneratedClass();
	
};

#endif //__DebuggerFeatureForms__
