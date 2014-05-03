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
#include <unicode/uclean.h>
#include <globals/CodeControlOptionsClass.h>
#include <globals/Assets.h>
#include <globals/Events.h>
#include <globals/GlobalsClass.h>
#include <language/TagFinderList.h>
#include <code_control/CodeControlStyles.h>
#include <code_control/CodeControlClass.h>
#include <search/FindInFilesClass.h>

/**
 * This profiler can be used to test the speediness of the code editing control.
 * Interface: Files can be opened by dragging them from the operating system file explorer 
 * There will be a menu created where the user can test the code completions and call tips.
 */
class CodeControlProfilerAppClass : public wxApp {
public:

	CodeControlProfilerAppClass();
	
	virtual bool OnInit();
	
	virtual int OnExit();
	
	t4p::CodeControlOptionsClass Options;
	t4p::GlobalsClass Globals;
	t4p::EventSinkClass EventSink;
};

/**
 * This class will handle the dropping of files from the file explorer.
 * When a file is dropped into the control it will be opened.
 */
class FileDropTargetClass : public wxFileDropTarget {

public:

	/**
	 * This class will NOT own the codeControl or the globals pointer
	 */
	FileDropTargetClass(t4p::CodeControlClass* codeControl, t4p::GlobalsClass* globals);

	/** 
	 * Called by wxWidgets when user drags a file to this application frame. All files dragged in will be opened
	 * into the notebook.
	 */
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& files);

private:

	t4p::CodeControlClass* CodeControl;
	t4p::GlobalsClass* Globals;
};

FileDropTargetClass::FileDropTargetClass(t4p::CodeControlClass* codeControl, t4p::GlobalsClass* globals)
 : CodeControl(codeControl)
 , Globals(globals) {

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
	UnicodeString contents;
	wxString charset;
	bool hasSignature = false;
	t4p::FindInFilesClass::FileContents(fileNameString, contents, charset, hasSignature);
	CodeControl->TrackFile(fileNameString, contents, charset, hasSignature);

	// add the new file to the  tag cache
	t4p::TagFinderListClass* tagFinderlist = new t4p::TagFinderListClass();
	tagFinderlist->InitGlobalTag(
		Globals->TagCacheDbFileName, Globals->FileTypes.GetPhpFileExtensions(),
		Globals->FileTypes.GetMiscFileExtensions(),
		Globals->Environment.Php.Version
	);
	
	t4p::WorkingCacheClass* workingCache = new t4p::WorkingCacheClass();
	workingCache->Init(fileNameString, CodeControl->GetIdString(), false, Globals->Environment.Php.Version, true);
	bool good = workingCache->Update(contents);
	if (good) {
		tagFinderlist->TagParser.BuildResourceCacheForFile(wxT(""), fileNameString, contents, false);
		Globals->TagCache.RegisterWorking(CodeControl->GetIdString(), workingCache);
	}
	else {
		delete workingCache;
	}
	
	delete tagFinderlist;
	return true;
}

/**
 * The application frame will contain the code control only
 */
class CodeControlFrameClass: public wxFrame {
public:
	CodeControlFrameClass(CodeControlProfilerAppClass& app);

private:
	
/*
 * Build the menu and add the event handlers
 */

	void CreateMenu();
	
	void OnContentAssist(wxCommandEvent& event);
	
	void OnCallTip(wxCommandEvent& event);
	
	void OnHelp(wxCommandEvent& event);
	
	void OnClose(wxCloseEvent& event);
	
	enum {
		ID_CONTENT_ASSIST,
		ID_CALL_TIP,
		ID_HELP
	};
	
	t4p::CodeControlClass* Ctrl;
	
	CodeControlProfilerAppClass& App;
	

	DECLARE_EVENT_TABLE()
};

IMPLEMENT_APP(CodeControlProfilerAppClass)

CodeControlProfilerAppClass::CodeControlProfilerAppClass() 
	: wxApp()
	, Options()
	, Globals()
	, EventSink() {
		
}

bool CodeControlProfilerAppClass::OnInit() {
	t4p::CodeControlStylesInit(Options);
	Globals.TagCacheDbFileName = t4p::TagCacheAsset();
	Globals.DetectorCacheDbFileName = t4p::DetectorCacheAsset();
	Globals.Environment.Php.Version = pelet::PHP_54;
	Options.EnableAutomaticLineIndentation = true;
	Options.EnableAutoCompletion = true;
	
	
	t4p::TagFinderListClass* tagFinderlist = new t4p::TagFinderListClass;
	tagFinderlist->InitGlobalTag(
		Globals.TagCacheDbFileName,
		Globals.FileTypes.GetPhpFileExtensions(),
		Globals.FileTypes.GetMiscFileExtensions(),
		Globals.Environment.Php.Version
	);
	tagFinderlist->InitDetectorTag(Globals.DetectorCacheDbFileName);
	tagFinderlist->InitNativeTag(t4p::NativeFunctionsAsset());
	Globals.TagCache.RegisterGlobal(tagFinderlist);
	
	CodeControlFrameClass* frame = new CodeControlFrameClass(*this);
	SetTopWindow(frame);
	frame->Show(true);
	return true;
}

int CodeControlProfilerAppClass::OnExit() {
	u_cleanup();
	return 0;
}

CodeControlFrameClass::CodeControlFrameClass(CodeControlProfilerAppClass& app) 
	: wxFrame(NULL, wxID_ANY, wxT("CodeControlClass profiler"), wxDefaultPosition, 
			wxSize(1024, 768)) 
	, App(app) {
	Ctrl = new t4p::CodeControlClass(this, app.Options, &app.Globals, app.EventSink, wxID_ANY);
	Ctrl->SetDropTarget(new FileDropTargetClass(Ctrl, &app.Globals));
	Ctrl->SetDocumentMode(t4p::CodeControlClass::PHP);
	CreateMenu();
}

void CodeControlFrameClass::OnCallTip(wxCommandEvent& event) {
	Ctrl->HandleCallTip(0, true);	
}

void CodeControlFrameClass::OnContentAssist(wxCommandEvent& event) {
	Ctrl->HandleAutoCompletion();	
}

void CodeControlFrameClass::OnHelp(wxCommandEvent& event) {
	wxString msg = wxT("This is a program that can be used to profile the source code control. ");
	msg += wxT("Unfortunately auto completion & call tips won't work because that code is is separate.\n\n");
	msg += wxT("Dragging a file into the window will open its contents.");
	wxMessageBox(msg);
}

void CodeControlFrameClass::OnClose(wxCloseEvent& event) {
	event.Skip();
}

void CodeControlFrameClass::CreateMenu() {
	wxMenuBar* menuBar = new wxMenuBar(0);
	wxMenu* edit = new wxMenu();
	edit->Append(ID_CONTENT_ASSIST, wxT("Content Assist\tCTRL+SPACE"), wxT("Content Assist"), false);
	edit->Append(ID_CALL_TIP, wxT("Show Call Tip\tCTRL+SHIFT+SPACE"), wxT("Show Call Tip"), false);
	edit->Append(ID_HELP, wxT("Help"), wxT("Help"), false);
	menuBar->Append(edit, wxT("Edit"));
	SetMenuBar(menuBar);
}

BEGIN_EVENT_TABLE(CodeControlFrameClass, wxFrame) 
	EVT_MENU(CodeControlFrameClass::ID_CONTENT_ASSIST, CodeControlFrameClass::OnContentAssist)
	EVT_MENU(CodeControlFrameClass::ID_CALL_TIP, CodeControlFrameClass::OnCallTip)
	EVT_MENU(CodeControlFrameClass::ID_HELP, CodeControlFrameClass::OnHelp)
	EVT_CLOSE(CodeControlFrameClass::OnClose)
END_EVENT_TABLE()