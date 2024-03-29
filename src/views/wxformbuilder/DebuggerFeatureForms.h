///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
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
#include <wx/hyperlink.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/statbox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/textctrl.h>
#include <wx/treelist.h>
#include <wx/splitter.h>
#include <wx/dataview.h>
#include <wx/dialog.h>
#include <wx/filepicker.h>

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
/// Class DebuggerOptionsPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class DebuggerOptionsPanelGeneratedClass : public wxPanel
{
	private:

	protected:
		wxStaticText* HelpLabel;
		wxHyperlinkCtrl* HelpLink;
		wxStaticText* PortLabel;
		wxSpinCtrl* Port;
		wxStaticText* MaxChildrenLabel;
		wxSpinCtrl* MaxChildren;
		wxStaticText* MaxDepthLabel;
		wxSpinCtrl* MaxDepth;
		wxStaticText* DoListenOnAppReadyLabel;
		wxCheckBox* DoListenOnAppReady;
		wxStaticText* DoBreakOnStartLabel;
		wxCheckBox* DoBreakOnStart;
		wxStaticText* MappingsHelpLabel;
		wxButton* AddMapping;
		wxButton* EditMapping;
		wxButton* DeleteMapping;
		wxListCtrl* SourceCodeMappings;

		// Virtual event handlers, overide them in your derived class
		virtual void OnAddMapping( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditMapping( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteMapping( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnListItemActivated( wxListEvent& event ) { event.Skip(); }


	public:

		DebuggerOptionsPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~DebuggerOptionsPanelGeneratedClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class DebuggerStackPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class DebuggerStackPanelGeneratedClass : public wxPanel
{
	private:

	protected:
		wxStaticText* Label;
		wxStaticText* StatusLabel;
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

///////////////////////////////////////////////////////////////////////////////
/// Class DebuggerVariablePanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class DebuggerVariablePanelGeneratedClass : public wxPanel
{
	private:

	protected:
		wxStaticText* VariablesLabel;
		wxStaticText* StatusLabel;
		wxTreeListCtrl* VariablesList;

	public:

		DebuggerVariablePanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~DebuggerVariablePanelGeneratedClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class DebuggerEvalPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class DebuggerEvalPanelGeneratedClass : public wxPanel
{
	private:

	protected:
		wxBitmapButton* EvalButton;
		wxBitmapButton* ClearButton;

		wxStaticText* StatusLabel;
		wxSplitterWindow* Splitter;
		wxPanel* ExprCodePanel;
		wxBoxSizer* ExprSizer;
		wxPanel* ExprResultPanel;
		wxTextCtrl* ExprResult;

		// Virtual event handlers, overide them in your derived class
		virtual void OnEvalClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		DebuggerEvalPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~DebuggerEvalPanelGeneratedClass();

		void SplitterOnIdle( wxIdleEvent& )
		{
			Splitter->SetSashPosition( 0 );
			Splitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( DebuggerEvalPanelGeneratedClass::SplitterOnIdle ), NULL, this );
		}

};

///////////////////////////////////////////////////////////////////////////////
/// Class DebuggerBreakpointPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class DebuggerBreakpointPanelGeneratedClass : public wxPanel
{
	private:

	protected:
		wxBitmapButton* DeleteBreakpointButton;
		wxBitmapButton* ToggleAllBreakpointsButton;
		wxStaticText* m_staticText6;
		wxDataViewListCtrl* BreakpointsList;

		// Virtual event handlers, overide them in your derived class
		virtual void OnDeleteBreakpoint( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnToggleAllBreakpoints( wxCommandEvent& event ) { event.Skip(); }


	public:

		DebuggerBreakpointPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~DebuggerBreakpointPanelGeneratedClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class DebuggerFullViewDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class DebuggerFullViewDialogGeneratedClass : public wxDialog
{
	private:

	protected:
		wxStaticText* Label;
		wxTextCtrl* Text;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;

	public:

		DebuggerFullViewDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Variable Contents"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 511,360 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~DebuggerFullViewDialogGeneratedClass();

};

///////////////////////////////////////////////////////////////////////////////
/// Class DebuggerMappingDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class DebuggerMappingDialogGeneratedClass : public wxDialog
{
	private:

	protected:
		wxStaticText* HelpLabel;
		wxStaticText* LocalPathLabel;
		wxDirPickerCtrl* LocalPath;
		wxStaticText* RemotePathLabel;
		wxTextCtrl* RemotePath;
		wxStdDialogButtonSizer* ButtonSizer;
		wxButton* ButtonSizerOK;
		wxButton* ButtonSizerCancel;

		// Virtual event handlers, overide them in your derived class
		virtual void OnCancelButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }


	public:

		DebuggerMappingDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Debugger Source Code Mapping"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~DebuggerMappingDialogGeneratedClass();

};

#endif //__DebuggerFeatureForms__
