/**
 * This software is released under the terms of the MIT License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <PreferencesClass.h>
#include <MvcEditorErrors.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/menuutils.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>
#include <wx/frame.h>

#include <wx/wx.h>

// this function was ripped off from wxMenuCmd::CreateNew() but this function
// gracefully handles non-existant menus
static wxCmd* CreateNewMenuCmd(wxMenuBar* menuBar, int id) {

	// search the menuitem which is tied to the given ID
	wxMenuItem* item = menuBar->FindItem(id);
	return item ? new wxMenuCmd(item) : NULL;
}

// this function was ripped off from wxKeyBinder::Load() but this function
// gracefully handles non-existant menus
static int LoadKeyProfileShortcuts(wxMenuBar* menuBar, wxKeyProfile& profile, wxConfigBase* config, const wxString& configKey) {
    wxString str;
    bool cont;
    int total = 0;
    long idx;

    // before starting...
    config->SetPath(configKey);
    profile.Reset();

    cont = config->GetFirstEntry(str, idx);
    while (cont) {

        // try to decode this entry
        if (str.StartsWith(wxCMD_CONFIG_PREFIX)) {

            wxString id(str.BeforeFirst(wxT('-')));
            wxString type(str.AfterFirst(wxT('-')));
            id = id.Right(id.Len()-wxString(wxCMD_CONFIG_PREFIX).Len());
            type = type.Right(type.Len()-wxString(wxT("type")).Len());

            // is this a valid entry ?
            if (id.IsNumber() && type.IsNumber() &&
                config->GetEntryType(str) == wxConfigBase::Type_String) {

                // we will interpret this group as a command ID
                int nid = wxAtoi(id);
                int ntype = wxAtoi(type);

                // create & load this command
                wxCmd *cmd = CreateNewMenuCmd(menuBar, nid);
				if (cmd && cmd->Load(config, str)) {
					profile.AddCmd(cmd);      // add to the array
					total++;
				}
				else if (!cmd) {
					
					// menu no longer exists
					// the normal keybinder code would just error out here but we 
					// dont want to 
					mvceditor::EditorLogWarning(mvceditor::MISSING_KEYBOARD_SHORTCUT, str);
				}
				else {

					// could not load from config ... continue on to next shortcut
					// the normal keybinder code would just error out here but we 
					// dont want to 
					mvceditor::EditorLogWarning(mvceditor::CORRUPT_KEYBOARD_SHORTCUT, str);
				}
            }
        }

        // proceed with next entry (if it does exist)
        cont &= config->GetNextEntry(str, idx);
    }

    return total;
}

// this function was ripped off from wxKeyProfile::Load() but this function
// gracefully handles non-existant menus
static bool LoadKeyProfile(wxMenuBar* menuBar, wxKeyProfile& profile, wxConfigBase* config, const wxString& configKey) {
	bool ret = false;
	config->SetPath(configKey);        // enter into this group
    wxString name;
    wxString desc;

    // do we have valid DESC & NAME entries ?
    if (config->HasEntry(wxT("desc")) && config->HasEntry(wxT("name"))) {
        if (!config->Read(wxT("desc"), &desc))
            return FALSE;
        if (!config->Read(wxT("name"), &name))
            return FALSE;

        // check for valid name & desc
        if (name.IsEmpty())
            return FALSE;

        profile.SetName(name);
        profile.SetDesc(desc);

        // load from current path (we cannot use '.')
        ret = LoadKeyProfileShortcuts(menuBar, profile, config, wxT("../") + configKey) > 0;
	}
	return ret;
}

static bool LoadKeyProfileArray(wxMenuBar* menuBar, wxKeyProfileArray& profiles, wxConfigBase* config, const wxString& configKey) {
    wxKeyProfile tmp;
    wxString str;
    bool cont;
    long idx;

    // before starting...
    config->SetPath(configKey);
	int selected = 0;
    if (!config->Read(wxT("nSelProfile"), &selected))
        return FALSE;
	
    cont = config->GetFirstGroup(str, idx);
    while (cont) {

        // try to decode this group name
        if (str.StartsWith(wxKEYPROFILE_CONFIG_PREFIX)) {

            // is this a valid entry ?
			if (LoadKeyProfile(menuBar, tmp, config, str)) {
				profiles.Add(new wxKeyProfile(tmp));     // yes, it is; add it to the array
			}
        }

        // set the path again (it could have been changed...)
        config->SetPath(configKey);

        // proceed with next entry (if it does exist)
        cont &= config->GetNextGroup(str, idx);
    }
	if (selected < profiles.GetCount()) {
		profiles.SetSelProfile(selected);
	}
	else if (!profiles.IsEmpty()) {
		profiles.SetSelProfile(0);
	}
    return TRUE;
}


mvceditor::PreferencesClass::PreferencesClass()
	: CodeControlOptions()
	, KeyProfiles() {
	
}

mvceditor::PreferencesClass::~PreferencesClass() {
	//KeyProfileArray destructor deletes the pointers
}

void mvceditor::PreferencesClass::Load(wxFrame* frame) {
	CodeControlOptions.StartEditMode();
	CodeControlOptions.SetToLightTheme();
	CodeControlOptions.CommitChanges();
	KeyProfiles.Cleanup();
	wxConfigBase* config = wxConfigBase::Get();
	CodeControlOptions.Load(config);
	wxMenuBar* menuBar = frame->GetMenuBar();
	
	// before loading we must register in wxCmd arrays the various types
	// of commands we want wxCmd::Load to be able to recognize...	
	wxMenuCmd::Register(menuBar);
	int totalCmdCount = 0;

	// call out own key profile load function
	// out function will gracefully handle deleted menus ; the normal keybinder code
	// throws asserts
	LoadKeyProfileArray(menuBar, KeyProfiles, config, wxT(""));
	for (int i = 0; i < KeyProfiles.GetCount(); ++i) {
		totalCmdCount += KeyProfiles.Item(i)->GetCmdCount();
	}
	
	// keybinder sets the config path, must reset it back to normal
	config->SetPath(wxT("/"));

	if (0 == totalCmdCount) {
		wxKeyProfile* profile = new wxKeyProfile(wxT("MVC Editor keyboard shortcuts"), wxT("MVC Editor keyboard shortcuts"));
		profile->ImportMenuBarCmd(menuBar);
		KeyProfiles.Add(profile);
		KeyProfiles.SetSelProfile(0);
	}
	LoadKeyboardShortcuts(frame);
}

void mvceditor::PreferencesClass::Save() {
	wxConfigBase* config = wxConfigBase::Get();
	CodeControlOptions.Save(config);
	KeyProfiles.Save(config, wxT(""), true);
	
	// keybinder sets the config path, must reset it back to normal
	config->SetPath(wxT("/"));
	config->Flush();
}

void mvceditor::PreferencesClass::LoadKeyboardShortcuts(wxWindow* window) {
	KeyProfiles.DetachAll();
	KeyProfiles.GetSelProfile()->Enable(true);

	// not attaching recursively for now; since the child windows (the code editor widgets)
	// are temporary we cannot attach them to the profile otherwise when a code editor
	// is deleted the profile would have a dangling pointer.
	// however this means that the given window must be the frame
	KeyProfiles.GetSelProfile()->Attach(window);
}

void mvceditor::PreferencesClass::InitConfig() {
	wxStandardPaths paths;
	wxString appConfigFileName = paths.GetUserConfigDir() + wxFileName::GetPathSeparator() + wxT(".mvc_editor.ini");
	wxFileConfig* config = new wxFileConfig(wxT("mvc_editor"), wxEmptyString, appConfigFileName, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
	wxConfigBase::Set(config);
	// this config will be automatically deleted by wxWidgets at the end
}