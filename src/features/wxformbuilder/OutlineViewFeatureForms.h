///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 17 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __OutlineViewFeatureForms__
#define __OutlineViewFeatureForms__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/checklst.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class OutlineViewGeneratedPanelClass
///////////////////////////////////////////////////////////////////////////////
class OutlineViewGeneratedPanelClass : public wxPanel 
{
	private:
	
	protected:
		enum
		{
			ID_OUTLINEVIEWGENERATEDPANELCLASS = 1000,
			ID_STATUSLABEL,
			ID_ADDBUTTON,
			ID_TREE,
		};
		
		wxStaticText* StatusLabel;
		wxBitmapButton* AddButton;
		wxBitmapButton* SyncButton;
		wxBitmapButton* HelpButton;
		wxTreeCtrl* Tree;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnAddButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSyncButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTreeItemActivated( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnTreeItemRightClick( wxTreeEvent& event ) { event.Skip(); }
		
	
	public:
		
		OutlineViewGeneratedPanelClass( wxWindow* parent, wxWindowID id = ID_OUTLINEVIEWGENERATEDPANELCLASS, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 502,657 ), long style = wxTAB_TRAVERSAL );
		~OutlineViewGeneratedPanelClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class FileSearchDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class FileSearchDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		enum
		{
			ID_FILESEARCHDIALOGGENERATEDCLASS = 1000,
			ID_SEARCHLABEL,
			ID_SEARCHTEXT,
			ID_PROJECTLABEL,
			ID_PROJECTCHOICE,
			ID_MATCHESLABEL,
			ID_MATCHESLIST,
		};
		
		wxStaticText* SearchLabel;
		wxTextCtrl* SearchText;
		wxStaticText* ProjectLabel;
		wxChoice* ProjectChoice;
		wxStaticText* MatchesLabel;
		wxCheckListBox* MatchesList;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		wxButton* ButtonsSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSearchKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnSearchText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSearchEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnProjectChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMatchesListDoubleClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMatchesListKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		FileSearchDialogGeneratedClass( wxWindow* parent, wxWindowID id = ID_FILESEARCHDIALOGGENERATEDCLASS, const wxString& title = _("File Search"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 559,295 ), long style = wxDEFAULT_DIALOG_STYLE );
		~FileSearchDialogGeneratedClass();
	
};

#endif //__OutlineViewFeatureForms__
