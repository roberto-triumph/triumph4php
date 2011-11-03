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
#include <wx/wx.h>
#include <wx/dnd.h>
#include <wx/filename.h>
#include <widgets/CodeControlOptionsClass.h>
#include <widgets/CodeControlClass.h>
#include <php_frameworks/ProjectClass.h>

/**
 * This profiler can be used to test the speediness of the code editing control.
 * Interface: Files can be opened by dragging them from the operating system file explorer 
 * There will be a menu created where the user can test the code completions and call tips.
 */
class CodeControlProfilerAppClass : public wxApp {
public:
	virtual bool OnInit();
};

/**
 * This class will handle the dropping of files from the file explorer.
 * When a file is dropped into the control it will be opened.
 */
class FileDropTargetClass : public wxFileDropTarget {

public:

	/**
	 * This class will NOT own the codeControl pointer
	 */
	FileDropTargetClass(mvceditor::CodeControlClass* codeControl);

	/** 
	 * Called by wxWidgets when user drags a file to this application frame. All files dragged in will be opened
	 * into the notebook.
	 */
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& files);

private:

	mvceditor::CodeControlClass* CodeControl;
};

FileDropTargetClass::FileDropTargetClass(mvceditor::CodeControlClass* codeControl) :
	CodeControl(codeControl) {

}

bool FileDropTargetClass::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& files) {
	if (files.Count() != 1) {
		wxMessageBox(_("You can only drop one file at a time into this control"));
		return false;
	}
	wxString fileNameString = files[0];
	wxFileName name(fileNameString);
	if (name.IsDir()) {
		wxMessageBox(_("You can only drop files (not directories) into this control"));
		return false;
	}
	else if (!name.IsOk()) {
		wxMessageBox(_("Invalid file:") + fileNameString);
		return false;
	}
	CodeControl->LoadAndTrackFile(fileNameString);
	return true;
}

/**
 * The application frame will contain the code control only
 */
class CodeControlFrameClass: public wxFrame {
public:
	CodeControlFrameClass();

private:
	
/*
 * Build the menu and add the event handlers
 */

	void CreateMenu();
	
	void OnContentAssist(wxCommandEvent& event);
	
	void OnCallTip(wxCommandEvent& event);
	
	void OnHelp(wxCommandEvent& event);
	
	enum {
		ID_CONTENT_ASSIST,
		ID_CALL_TIP,
		ID_HELP
	};
	
	mvceditor::CodeControlOptionsClass Options;
	mvceditor::ProjectOptionsClass ProjectOptions;
	mvceditor::EnvironmentClass Environment;
	mvceditor::ProjectClass Project;
	mvceditor::CodeControlClass* Ctrl;
	

	DECLARE_EVENT_TABLE()
};

IMPLEMENT_APP(CodeControlProfilerAppClass)

bool CodeControlProfilerAppClass::OnInit() {
	CodeControlFrameClass* frame = new CodeControlFrameClass();
	SetTopWindow(frame);
	frame->Show(true);	
	return true;
}

CodeControlFrameClass::CodeControlFrameClass() 
	: wxFrame(NULL, wxID_ANY, wxT("CodeControlClass profiler"), wxDefaultPosition, 
			wxSize(1024, 768))
	, Options()
	, ProjectOptions()
	, Project(ProjectOptions, Environment) {
	Options.EnableAutomaticLineIndentation = true;
	Options.EnableAutoCompletion = true;
	Ctrl = new mvceditor::CodeControlClass(this, Options, &Project, wxID_ANY);
	Ctrl->SetDropTarget(new FileDropTargetClass(Ctrl));
	CreateMenu();
}

void CodeControlFrameClass::OnCallTip(wxCommandEvent& event) {
	Ctrl->HandleCallTip(0, true);	
}

void CodeControlFrameClass::OnContentAssist(wxCommandEvent& event) {
	Ctrl->HandleAutoCompletion(true);	
}

void CodeControlFrameClass::OnHelp(wxCommandEvent& event) {
	wxString msg = wxT("This is a program that can be used to profile the source code control. Testing of the ");
	msg += wxT("auto completion and call tips can be done by using the menu items, just like in the App.\n\n");
	msg += wxT("Dragging a file into the window will open its contents.");
	wxMessageBox(msg);
}

void CodeControlFrameClass::CreateMenu() {
	wxMenu* edit = new wxMenu(wxT("Edit"));
	edit->Append(ID_CONTENT_ASSIST, wxT("Content Assist\tCTRL+SPACE"), wxT("Content Assist"), false);
	edit->Append(ID_CALL_TIP, wxT("Show Call Tip\tCTRL+SHIFT+SPACE"), wxT("Show Call Tip"), false);
	edit->Append(ID_HELP, wxT("Help"), wxT("Help"), false);
	
	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(edit, wxT("Menu"));
	SetMenuBar(menuBar);
}

BEGIN_EVENT_TABLE(CodeControlFrameClass, wxFrame) 
	EVT_MENU(CodeControlFrameClass::ID_CONTENT_ASSIST, CodeControlFrameClass::OnContentAssist)
	EVT_MENU(CodeControlFrameClass::ID_CALL_TIP, CodeControlFrameClass::OnCallTip)
	EVT_MENU(CodeControlFrameClass::ID_HELP, CodeControlFrameClass::OnHelp)
END_EVENT_TABLE()