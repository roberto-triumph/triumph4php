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
#ifndef __MVCEDITORFRAMEWORKDETECTORCLASS_H__
#define __MVCEDITORFRAMEWORKDETECTORCLASS_H__

#include <search/ResourceFinderClass.h>
#include <widgets/ProcessWithHeartbeatClass.h>
#include <environment/DatabaseInfoClass.h>
#include <environment/EnvironmentClass.h>
#include <wx/event.h>
#include <wx/string.h>


namespace mvceditor {

/**
 * A DetectorAction is a base class that will "communicate" with the PHP detection
 * code by running the MvcEditorFrameworkApp.php script. The base class will make it
 * easy for different kinds of detection (database connections, dynamic resources, etc)
 * to be created.
 * 
 * In a nutshell, the detector is a wrapper for a call to 
 *    php MvcEditorFrameworkApp.php --identifier=xxx --dir=xxx --action=xxx
 * 
 * The detector runs the external process in an asynchronous manner and requires a handler; the 
 * handler will get called once the detection script finishes running and its output has been parsed.
 * 
 * Note that since detector communicated by createing an external process its methods may only
 * be called from the main thread.
 */
class DetectorActionClass : public wxEvtHandler {

public:

	/**
	 * Any errors that occur when reading the Detection process response.
	 */
	enum DetectError {
		NONE,

		/**
		 * detection response is empty or not in the expected format
		 */
		BAD_CONTENT,
		
		/**
		 * detection response has a value that is not yet implemented
		 */
		UNIMPLEMENTED,

		/**
		 * External process returned an non-zero exit code.
		 */
		 PROCESS_FAILED
	};

	/**
	 * @param handler will get notified with EVENT_PROCESS_COMPLETE and EVENT_PROCESS_FAILED
	 * just like if it was connected to a ProcessWithHeartbeat object. The events are posted
	 * after the responses are parsed; it is safe to access the results from the event handlers.
	 */
	DetectorActionClass(wxEvtHandler& handler);
	virtual ~DetectorActionClass();

	/**
	 * Starts the detection process. This method will return 
	 * immediately; the handler given in the constructor will get notified
	 * when the process finishes
	 *
	 * @param int commandId command ID will be used when an EVENT_PROCESS_* is genereated
	 *        this way the caller can correlate a command to an event.
	 * @param environment to get location of PHP binary "php-win.exe" / php
	 * @param projectRootPath location of the project to run detection on ie. the project that
	 *        the user is modifying
	 * @param identifier the detected framework identifier
	 * @param extra any extra parameters that this specific detector needs.
	 * @return wxString the command (operating system command line) that will run the PHP detection code.
	 */
	bool Init(int id, const EnvironmentClass& environment, const wxString& projectRootPath, const wxString& identifier,
		wxString extra = wxEmptyString);

	/**
	 * initialize a detector from an existing file. For now, this is used only to test the
	 * parsing code.
	 */
	void InitFromFile(wxString fileName);

protected:

	/**
	 * sub classes will need to clear their data structures here.
	 */
	virtual void Clear() = 0;

	/**
	 * @return wxString sub classes must return the action that they are handling
	 */
	virtual wxString GetAction() = 0;

	/**
	 * This method will be called when the detector process ends.  The sub classes will
	 * read in the contents of OutputFile and will deserialize the output into
	 * data structures. Sub classes can set the Error property as well.
	 */
	virtual bool Response() = 0;

private:

	void OnProcessComplete(wxCommandEvent& event);

	void OnProcessFailed(wxCommandEvent& event);

public:

	DetectError Error;

	wxString ErrorMessage;

protected:

	/**
	 * Used to start the detector process
	 */
	ProcessWithHeartbeatClass Process;

	/**
	 * This handler will get notified after process has ended and response
	 * has been parsed.
	 */
	wxEvtHandler& Handler;

	/**
	 * The detector process will dump its results to this file.
	 */
	wxFileName OutputFile;

private:

	long CurrentPid;

	DECLARE_EVENT_TABLE()

};
	
/**
 * creates the detection command that will figure out which frameworks
 * the project uses.  Project "detection" means figuring out what framework a
 * project uses (could be multiple). When the external process ends,
 * the Frameworks vector will be populated with all of the frameworks that were
 * detected.
 */
class FrameworkDetectorActionClass : public DetectorActionClass {

public:

	std::vector<wxString> Frameworks;

	FrameworkDetectorActionClass(wxEvtHandler& handler);

protected:

	void Clear();

	wxString GetAction();

	bool Response();

};

/**
 * creates the detection command that will figure out which database connections
 * the project uses. When the external process ends,
 * the Databases vector will be populated with all of the database connections that were
 * detected.
 */
class DatabaseDetectorActionClass : public DetectorActionClass {

public:

	std::vector<DatabaseInfoClass> Databases;

	DatabaseDetectorActionClass(wxEvtHandler& handler);
	
protected:

	void Clear();

	wxString GetAction();

	bool Response();
};

/**
 * Creates the detection command that will figure out the configuration files 
 * for the given framework (routes file, database file, etc ...)
 * When the external process ends, the ConfigFiles map will contain all of the configuration
 * files for a single framework.
 */
class ConfigFilesDetectorActionClass : public DetectorActionClass {

public: 
	
	std::map<wxString, wxString> ConfigFiles;

	ConfigFilesDetectorActionClass(wxEvtHandler& handler);

protected:

	void Clear();

	wxString GetAction();

	bool Response();

};

/**
 * creates the detection command that will figure out any "dynamic" resources
 * that the a framework provides. When the external process ends, the Resources
 * vector will contain all of the dynamic resources.
 */
class ResourcesDetectorActionClass : public DetectorActionClass {

public: 
	
	std::vector<mvceditor::ResourceClass> Resources;

	ResourcesDetectorActionClass(wxEvtHandler& handler);

protected:

	void Clear();

	wxString GetAction();

	bool Response();
};

/**
 * The editor will ask a framework to generate a URL for a specific page.  Frameworks have 
 * their own way of mappings URLs to specific files / classes and methods ("Routing").
 * Since each framework uses a different mechanism; the editor will ask the URL detector
 * to generate all of the URLs that a file maps to.
 */
class UrlDetectorActionClass : public DetectorActionClass {

public:

	std::vector<wxString> Urls;

	UrlDetectorActionClass(wxEvtHandler& handler);
	
protected:

	void Clear();

	wxString GetAction();

	bool Response();
};
	
	
}
#endif