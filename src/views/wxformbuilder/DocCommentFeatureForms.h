///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DocCommentFeatureForms__
#define __DocCommentFeatureForms__

#include <wx/string.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/hyperlink.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/sizer.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

#define ID_PHP_SITE_LINK 1000
#define ID_CLOSE_LINK 1001

///////////////////////////////////////////////////////////////////////////////
/// Class DocCommentPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class DocCommentPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxHyperlinkCtrl* PhpSiteDocs;
		wxHyperlinkCtrl* CloseLink;
		wxRichTextCtrl* Text;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnPhpSiteDocs( wxHyperlinkEvent& event ) { event.Skip(); }
		virtual void OnClose( wxHyperlinkEvent& event ) { event.Skip(); }
		
	
	public:
		
		DocCommentPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,252 ), long style = wxNO_BORDER|wxTAB_TRAVERSAL );
		~DocCommentPanelGeneratedClass();
	
};

#endif //__DocCommentFeatureForms__
