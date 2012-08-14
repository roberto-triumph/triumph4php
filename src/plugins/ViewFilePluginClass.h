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
#ifndef __MVCEDITORVIEWFILEPLUGINCLASS_H__
#define __MVCEDITORVIEWFILEPLUGINCLASS_H__

#include <PluginClass.h>
#include <php_frameworks/FrameworkDetectorClass.h>
#include <php_frameworks/CallStackClass.h>
#include <widgets/ThreadWithHeartbeatClass.h>
#include <plugins/wxformbuilder/ViewFilePluginGeneratedClass.h>
#include <unicode/unistr.h>
#include <memory>

namespace mvceditor {

// defined  at the bottom of this file
class ViewFilePanelClass;

extern const wxEventType EVENT_CALL_STACK_COMPLETE;

class CallStackCompleteEventClass : public wxEvent {

public:

	/**
	 * error will be filled when a call stack file could not be generated
	 */
	CallStackClass::Errors LastError;
	
	/**
	 * if TRUE an error occurred while attempting to write the call stack file to
	 * the hard disk.
	 */
	bool WriteError;
	
	CallStackCompleteEventClass(mvceditor::CallStackClass::Errors error, bool writeError);
	
	wxEvent* Clone() const;
	
};

typedef void (wxEvtHandler::*CallStackCompleteEventClassFunction)(CallStackCompleteEventClass&);

#define EVT_CALL_STACK_COMPLETE(fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_CALL_STACK_COMPLETE, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( CallStackCompleteEventClassFunction, & fn ), (wxObject *) NULL ),

	
/**
 * Generates the call stack file in a background thread. This class
 * needs as input the starting entry point from which to start
 * recursing down the call stack. The class will generate a
 * EVENT_CALL_STACK_COMPLETE event as well
 * 
 */
class CallStackThreadClass : public ThreadWithHeartbeatClass {

public:

	/**
	 * @param handler will be notified of EVENT_WORK_* events and EVENT_CALL_STACK_COMPLETE events as well
	 */
	CallStackThreadClass(wxEvtHandler& handler, mvceditor::RunningThreadsClass& runningThreads);
	
	/**
	 * one-time initialization, call this before InitThread
	 */
	void InitCallStack(ResourceCacheClass& resourceCache);
	
	
	/**
	 * starts the background thread
	 */
	bool InitThread(const wxFileName& fileName, const UnicodeString& className, const UnicodeString& methodName, pelet::Versions version,
		const wxFileName& persistFileName);
	
protected:
	
	void BackgroundWork();
	
	private:
	
	/**
	 * Used to generate the call stack file (file of all function calls of a URL); call stack 
	 * file is required by the ViewInfos detector
	 * This pointer is owned by this class
	 */
	std::auto_ptr<CallStackClass> CallStack;
	
	/**
	 * The results of the call stack will be written to this file.
	 * This class will pick the file name (it will be a temporary file).
	 */
	wxFileName PersistFile;

	wxFileName StartFileName;
	
	UnicodeString ClassName;
	
	UnicodeString MethodName;

	pelet::Versions Version;
};

/**
 * This is a class that will show the user any view files that are
 * associated with the current URL (the URL that the user chose in
 * the toolbar).
 * 
 * The class will do the following:
 * (1) generate the call stack file in a background thread
 * (2) call the PHP view file detector (in an external process)
 * 
 * After all of this, it will populate a window with the view files.
 */
class ViewFilePluginClass : public PluginClass {
	
public:

	/**
	 * A 'local' detector that will be used to get all of the
	 * files for the current URL.
	 */
	mvceditor::PhpFrameworkDetectorClass FrameworkDetector;
	
	/**
	 *  The file where the call stack was written to. This file is a 
	 * temporary file, it needs to be deleted when we are done using it
	 */
	wxFileName CallStackPersistFile;
	
	/**
	 * error will be filled when a call stack file could not be generated
	 */
	CallStackClass::Errors LastError;
	
	/**
	 * if TRUE an error occurred while attempting to write the call stack file to
	 * the hard disk.
	 */
	bool WriteError;
	
	ViewFilePluginClass(mvceditor::AppClass& app);

	void SetCurrentUrl(mvceditor::UrlResourceClass url);
	
	void AddViewMenuItems(wxMenu* viewMenu);

	UrlResourceFinderClass& Urls();

	/**
	 * the final result; the view files that correspond to the
	 * current URL
	 */
	std::vector<mvceditor::ViewInfoClass> CurrentViewInfos();

	/**
	 * starts the view file detection process. This is an asynchronous operation.
	 * When view files are detected, the plugin will update the
	 * panel appropriately.
	 */
	void StartDetection();

	/**
	 * @return the currently opened file. Note that this may not be
	 * a valid file (new, untitled files).
	 */
	wxString CurrentFile();

	/**
	 * Opens the given file. file must be a full path.
	 */
	void OpenFile(wxString file);
	
private:
	
	/**
	 * show (or create) the view files window and start the calculations if needed
	 */
	void OnViewInfosMenu(wxCommandEvent& event);
	
	void OnWorkComplete(wxCommandEvent& event);
	
	void OnCallStackComplete(mvceditor::CallStackCompleteEventClass& event);
	
	void OnViewInfosDetectionComplete(ViewInfosDetectedEventClass& event);
	
	void OnViewInfosDetectionFailed(wxCommandEvent& event);
	
	void OnProjectUrls(wxCommandEvent& event);
	
	void ShowPanel();
	
	enum States {
		FREE,
		INDEXING
	};
	
	/**
	 * used to generate the call stack for the current URL
	 * This pointer will automatically delete itself
	 */
	CallStackThreadClass* CallStackThread;
	
	
	States State;
	
	DECLARE_EVENT_TABLE()
};

class ViewFilePanelClass : public ViewFilePanelGeneratedClass {
	
public:

	ViewFilePluginClass& Plugin;
	
	ViewFilePanelClass(wxWindow* parent, int id, ViewFilePluginClass& plugin);

	void UpdateResults();

	void ClearResults();

	void UpdateControllers();
	
protected:

	void OnHelpButton(wxCommandEvent& event);

	void OnCurrentButton(wxCommandEvent& event);

	void OnControllerChoice(wxCommandEvent& event);

	void OnActionChoice(wxCommandEvent& event);

	void OnTreeItemActivated(wxTreeEvent& event);
};

/**
 * Class used to put the full path of a view file inside of the tree control
 * so that we can get the full path at a later time. This is because
 * the tree label will be the relative file.
 */
class StringTreeItemDataClass : public wxTreeItemData {

public:

	wxString Str;

	StringTreeItemDataClass(const wxString& str);
};

}

#endif
