///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __NewUserFeatureForms__
#define __NewUserFeatureForms__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/filepicker.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class NewUserDialogGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class NewUserDialogGeneratedClass : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* MoreLabel;
		wxStaticText* SettingsHelpLabel;
		wxRadioButton* UserDataDirectory;
		wxRadioButton* ApplicationDirectory;
		wxRadioButton* CustomDirectory;
		wxDirPickerCtrl* SettingsDirectory;
		wxStaticText* PhpHelpLabel;
		wxCheckBox* NoPhp;
		wxFilePickerCtrl* PhpExecutable;
		wxStaticText* FileExtensionsHelpLabel;
		wxStaticText* PhpLabel;
		wxTextCtrl* PhpFileExtensions;
		wxStaticText* SqlLabel;
		wxTextCtrl* SqlFileExtensions;
		wxStaticText* CssLabel;
		wxTextCtrl* CssFileExtensions;
		wxStaticText* MiscLabel;
		wxTextCtrl* MiscFileExtensions;
		wxStdDialogButtonSizer* ButtonsSizer;
		wxButton* ButtonsSizerOK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOkButton( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		NewUserDialogGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Welcome New User"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION );
		~NewUserDialogGeneratedClass();
	
};

#endif //__NewUserFeatureForms__
