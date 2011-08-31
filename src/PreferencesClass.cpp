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
 * @author     $Author: robertop2004@gmail.com $
 * @date       $Date: 2011-08-20 13:19:24 -0700 (Sat, 20 Aug 2011) $
 * @version    $Rev: 598 $ 
 */
#include <PreferencesClass.h>

#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/menuutils.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>
#include <wx/stc/stc.h>

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
		
	// for some reason, must give absolute path else loading will saving & fail
	// COMMENT OUT THIS CODE TO SEE IF IT IS THE CAUSE OF CRASHES
	//KeyProfiles.Load(config, wxT("/Keyboard"));
	KeyProfiles.Load(config);
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
	
	// for some reason, must give absolute path else loading will saving & fail
	// COMMENT OUT THIS CODE TO SEE IF IT IS THE CAUSE OF CRASHES
	//KeyProfiles.Save(config, wxT("/Keyboard"), true);
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
