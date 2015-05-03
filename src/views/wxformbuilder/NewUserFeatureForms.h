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
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/radiobut.h>
#include <wx/filepicker.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/hyperlink.h>
#include <wx/choice.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class NewUserSettingsPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class NewUserSettingsPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* MoreLabel;
		wxStaticText* VersionUpdateHelp;
		wxCheckBox* CheckForUpdates;
		wxStaticText* SettingsHelpLabel;
		wxRadioButton* UserDataDirectory;
		wxRadioButton* ApplicationDirectory;
		wxRadioButton* CustomDirectory;
		wxDirPickerCtrl* SettingsDirectory;
	
	public:
		
		NewUserSettingsPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~NewUserSettingsPanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class NewUserAssociationsPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class NewUserAssociationsPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* HelpLabel;
		wxStaticText* PhpLabel;
		wxTextCtrl* PhpFileExtensions;
		wxStaticText* JsLabel;
		wxTextCtrl* JsFileExtensions;
		wxStaticText* CssLabel;
		wxTextCtrl* CssFileExtensions;
		wxStaticText* SqlLabel;
		wxTextCtrl* SqlFileExtensions;
		wxStaticText* ConfigLabel;
		wxTextCtrl* ConfigFileExtensions;
		wxStaticText* YamlLabel;
		wxTextCtrl* YamlFileExtensions;
		wxStaticText* XmlLabel;
		wxTextCtrl* XmlFileExtensions;
		wxStaticText* MarkdownLabel;
		wxTextCtrl* MarkdownFileExtensions;
		wxStaticText* BashLabel;
		wxTextCtrl* BashFileExtensions;
		wxStaticText* MiscLabel;
		wxTextCtrl* MiscFileExtensions;
	
	public:
		
		NewUserAssociationsPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 640,480 ), long style = wxTAB_TRAVERSAL );
		~NewUserAssociationsPanelGeneratedClass();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class NewUserPhpSettingsPanelGeneratedClass
///////////////////////////////////////////////////////////////////////////////
class NewUserPhpSettingsPanelGeneratedClass : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* PhpHelpLabel;
		wxHyperlinkCtrl* PhpDetectorsLink;
		wxCheckBox* Installed;
		wxFilePickerCtrl* PhpExecutable;
		wxStaticText* VersionLabel;
		wxChoice* Version;
	
	public:
		
		NewUserPhpSettingsPanelGeneratedClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~NewUserPhpSettingsPanelGeneratedClass();
	
};

#endif //__NewUserFeatureForms__
