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
#include <plugins/wxformbuilder/LintPluginGeneratedClass.h>
#include <unicode/unistr.h>
#include <memory>

namespace mvceditor {
	
/**
 * Generates the call stack file in a background thread. This class
 * needs as input the starting entry point from which to start
 * recursing down the call stack.
 * 
 * All public members are NOT thread safe. Do not attempt access
 * until the EVENT_WORK_COMPLETE event has been received.
 */
class CallStackThreadClass : public ThreadWithHeartbeatClass {

public:

	/**
	 * Used to generate the call stack file (file of all function calls of a URL); call stack 
	 * file is required by the ViewFiles detector
	 * This pointer is owned by this class
	 */
	std::auto_ptr<CallStackClass> CallStack;
	
	/**
	 * error will be filled when a call stack file could not be generated
	 */
	CallStackClass::Errors LastError;
	
	/**
	 * The results of the call stack will be written to this file.
	 * This class will pick the file name (it will be a temporary file).
	 */
	wxFileName StackFile;
	
	/**
	 * if TRUE an error occurred while attempting to write the call stack file to
	 * the hard disk.
	 */
	bool WriteError;

	/**
	 * @param handler will be notified of EVENT_WORK_* events
	 */
	CallStackThreadClass(wxEvtHandler& handler);
	
	void InitCallStack(ResourceCacheClass& resourceCache);
	
	bool InitThread(const wxFileName& fileName, const UnicodeString& className, const UnicodeString& methodName);
	
protected:
	
	void* Entry();
	
private:

	wxFileName StartFileName;
	
	UnicodeString ClassName;
	
	UnicodeString MethodName;
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

	ViewFilePluginClass();
	
	/**
	 * A 'local' detector that will be used to get all of the
	 * files for the current URL.
	 */
	std::auto_ptr<PhpFrameworkDetectorClass> FrameworkDetector;
	
	/**
	 * used to generate the call stack for the current URL
	 */
	CallStackThreadClass CallStackThread;
	
	/**
	 * the final result; the view files that correspond to the
	 * current URL
	 */
	std::vector<wxFileName> CurrentViewFiles;
	
	void AddToolsMenuItems(wxMenu* toolsMenu);
	
private:
	
	/**
	 * show (or create) the view files window and start the calculations if needed
	 */
	void OnViewFilesMenu(wxCommandEvent& event);
	
	void OnWorkComplete(wxCommandEvent& event);
	
	void OnViewFilesDetectionComplete(ViewFilesDetectedEventClass& event);
	
	void OnViewFilesDetectionFailed(wxCommandEvent& event);
	
	DECLARE_EVENT_TABLE()
};

// TODO: make a panel specific for this class
class ViewFilePanelClass : public LintResultsGeneratedPanelClass {
	
public:

	ViewFilePluginClass& Plugin;
	
	ViewFilePanelClass(wxWindow* parent, int id, ViewFilePluginClass& plugin);

	void UpdateLabels();

	void UpdateTitle(const UrlResourceClass& chosenUrl);
	
};
	
}

#endif
