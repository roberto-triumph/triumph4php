///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __FileCabinetFeatureForms__
#define __FileCabinetFeatureForms__

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
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/hyperlink.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class FileCabinetPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class FileCabinetPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxBitmapButton* AddFileButton;
		wxBitmapButton* AddDirectoryButton;
		wxBitmapButton* HelpButton;
		wxListCtrl* List;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnAddFileClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddDirectoryClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnListItemRightClick( wxListEvent& event ) { event.Skip(); }
		
	
	public:
		
		FileCabinetPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~FileCabinetPanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class FileCabinetFeatureHelpDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class FileCabinetFeatureHelpDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* Help;
		wxHyperlinkCtrl* MoreLink;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
	
	public:
		
		FileCabinetFeatureHelpDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("File Cabinet Help"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~FileCabinetFeatureHelpDialogGeneratedClass();
	
};

#endif //__FileCabinetFeatureForms__
