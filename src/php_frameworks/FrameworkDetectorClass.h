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
#include <widgets/ThreadWithHeartbeatClass.h>
#include <environment/DatabaseInfoClass.h>
#include <environment/EnvironmentClass.h>
#include <environment/UrlResourceClass.h>
#include <wx/string.h>
#include <wx/event.h>
#include <map>

namespace mvceditor {

// forward declaration; class is defined in this file further down below.
class DetectorActionClass;

/**
 * A small class to parse the PHP detector responses in a background thread.
 * This is done in case the response is big; we don't want to lock up
 * the main thread.
 */
class ResponseThreadWithHeartbeatClass : public ThreadWithHeartbeatClass {

public:

	ResponseThreadWithHeartbeatClass(DetectorActionClass& action);
	
	bool Init(wxFileName outputFile);

protected:

	void* Entry();

private:

	DetectorActionClass& Action;

	wxFileName OutputFile;
};

/**
 * A DetectorAction is a base class that will "communicate" with the PHP detection
 * code by running the MvcEditorFrameworkApp.php script. The base class will make it
 * easy for different kinds of detection (database connections, dynamic resources, etc)
 * to be created.
 * 
 * In a nutshell, the detector is a wrapper for a call to 
 *    php MvcEditorFrameworkApp.php --identifier=xxx --dir=xxx --action=xxx --extra=xxx
 * 
 * The detector runs the external process in an asynchronous manner and requires a handler; the 
 * handler will get called once the detection script finishes running and its output has been parsed.
 * 
 * Note that since detector communicated by creating an external process its methods may only
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
	 * @param moreParams key-value pairs of other arguments; these can vary per detector. The keys do NOT
	 *       need the starting dashes (ie. the key should be "file" instead of "--file").
	 * @return wxString the command (operating system command line) that will run the PHP detection code.
	 */
	bool Init(int id, const EnvironmentClass& environment, const wxString& projectRootPath, const wxString& identifier,
		std::map<wxString, wxString> moreParams);

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
	 * This method will get run in a background thread, make sure that any synchronization
	 * is done accordingly. 
	 */
	virtual bool Response() = 0;

private:

	void OnProcessComplete(wxCommandEvent& event);

	void OnProcessFailed(wxCommandEvent& event);
	
	void OnWorkInProgress(wxCommandEvent& event);

	void OnWorkComplete(wxCommandEvent& event);

public:

	DetectError Error;

	wxString ErrorMessage;

protected:

	/**
	 * Used to start the detector process
	 */
	ProcessWithHeartbeatClass Process;

	/**
	 * Used to parse the detector response in a background thread.
	 */
	ResponseThreadWithHeartbeatClass ResponseThread;

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

	int CurrentId;

	DECLARE_EVENT_TABLE()

	friend class ResponseThreadWithHeartbeatClass;

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

	std::vector<UrlResourceClass> Urls;

	UrlDetectorActionClass(wxEvtHandler& handler);
	
protected:

	void Clear();

	wxString GetAction();

	bool Response();
};

/**
 * Creates the detection command that will figure out the view files 
 * for the given framework (ie. to be able to go from a controller file to its view files)
 * When the external process ends, the ViewFiles list will contain all of the view files
 * for a given controller action for a single framework.
 * Note: currently only one controller action can be requested at a time.
 * Note: the caller must create the Call stack file (using the CallStack class).
 */
class ViewFilesDetectorActionClass : public DetectorActionClass {

public: 
	
	std::vector<wxFileName> ViewFiles;

	ViewFilesDetectorActionClass(wxEvtHandler& handler);

protected:

	void Clear();

	wxString GetAction();

	bool Response();

};

/**
 * This event will get genrated by the PhpFrameworkDetectorClass when the
 * project framework detection has ended.
 */
extern const wxEventType EVENT_FRAMEWORK_DETECTION_COMPLETE;
extern const wxEventType EVENT_FRAMEWORK_DETECTION_FAILED;

/**
 * This event will get generated by the PhpFrameworkDetectorClass when the
 * URL detection has completed.
 */
extern const wxEventType EVENT_FRAMEWORK_URL_COMPLETE;
extern const wxEventType EVENT_FRAMEWORK_URL_FAILED;

/**
 * This event will get generated by the PhPFrameworkDetectorClass when the
 * View files detection has completed
 */
extern const wxEventType EVENT_FRAMEWORK_VIEW_FILES_COMPLETE;
extern const wxEventType EVENT_FRAMEWORK_VIEW_FILES_FAILED;

/**
 * This is the class that will 'detect' various artifacts that are common to PHP
 * projects that use a PHP framework; the artifacts include database connections,
 * configuration files, dynamic resources, etc. Each artifact will be detected
 * using a DetectorActionClass; essentially wrapper to execute 
 * 
 *   php MvcEditorFrameworkApp.php --identifier=xxx --action=xxx --dir=xxx
 * 
 * Since this detection is done via call to wxExecute(), the methods in this class
 * must always be called from the main thread.
 * 
 */
class PhpFrameworkDetectorClass : public wxEvtHandler {
	
public: 

	/**
	 * the frameworks that were detected. 
	 */
	std::vector<wxString>Identifiers;
	
	/**
	 * This is the list of config files that were detected.
	 * 
	 * The key is user-friendly name; the value is the absolute path to the
	 * corresponding config file.
	 */
	std::map<wxString, wxString> ConfigFiles;
	
	/**
	 * The detected database connection info
	 */
	std::vector<DatabaseInfoClass> Databases;
	
	/**
	 * The list of dynamic resources that are provided by the PHP frameworks
	 */
	std::vector<mvceditor::ResourceClass> Resources;
	
	/**
	 * @param event handler that will receive the EVENT_FRAMEWORK_DETECTION_COMPLETE, EVENT_FRAMEWORK_URL
	 * events. Also, the handler will get an EVENT_PROCESS_IN_PROGRESS while the detectors are running
	 * @param the environment; which contains the location of the PHP binary
	 */
	PhpFrameworkDetectorClass(wxEvtHandler& handler, const EnvironmentClass& environment);
	
	/**
	 * deletes any detected data
	 */
	void Clear();

	/**
	 * kicks off the initial framework detection process to find out what framework the project
	 * uses. If the project uses a supported framework, then this method will
	 * detect the database connections, config files, and resources (and put the results in the public
	 * members). At the end of all detections, a EVENT_FRAMEWORK_DETECTION_COMPLETE will be generated; in which
	 * case the handler may access any public members.
	 * 
	 * @param dir the project's root directory
	 * @return bool TRUE if detection started successfully. If false, then it is likely that the PHP executable path
	 * is not correct
	 */
	bool Init(const wxString& dir);
	
	/**
	 * kicks off the URL detector; URL detector is used to ask the PHP framework to
	 * resolve all project URLs.  At the end of all detections, a EVENT_FRAMEWORK_URL_COMPLETE will be generated.
	 * The fileName is the persisted cache file (from ResourceCacheClass)controller file; the php URL detector
	 * will read the cache file to inspect the class names (it wont need to parse the source code).
	 
	 * The result of this would be a list of URLs, one for each action in a controller (this will vary by PHP framework).
	 * 
	 * @param dir the project's root directory
	 * @param fileName the full path to the resource cache file. 
	 * @param baseUrl the base URL of the project (usually calculated by the ApacheClass). This will be the 
	 *        virtual host + the directory within the virtual host where the current project starts.  For example,
	 *        if the virtual host document root is /home/user/public, the project root directory is 
	 *        /home/user/public/secret_project, then the baseUrl should be 
	          http://localhost/secret_project
	 * @return bool TRUE if detection started successfully. If false, then it is likely that the PHP executable path
	 * is not correct or that no PHP frameworks were detected.
	 */
	bool InitUrlDetector(const wxString& dir, const wxString& resourceCacheFileName, const wxString& baseUrl);
	
	/**
	 * kicks off the View files detector, the View files detector is used to ask the PHP framework to
	 * get all of the view files for a particular controller action. When the detection is complete, a
	 * ViewFilesDetectedEventClass event will be generated.
	 * 
	 * @param dir the project's root directory
	 * @param url the URL of the action to query
	 * @param callStackFile the location of the call stack file that contains all of the function calls
	 *        for the given URL.
	 */
	bool InitViewFilesDetector(const wxString& dir, const wxString& url, const wxFileName& callStackFile);
	
private:

	/**
	 * To call the PHP framework detection scripts.
	 */
	FrameworkDetectorActionClass FrameworkDetector;
	ConfigFilesDetectorActionClass ConfigFilesDetector;
	DatabaseDetectorActionClass DatabaseDetector;
	ResourcesDetectorActionClass ResourcesDetector;
	UrlDetectorActionClass UrlDetector;
	ViewFilesDetectorActionClass ViewFilesDetector;
	
	/**
	 * a project may be using more than one framework. This vector
	 * will be used as a 'queue' so that we can know when all framework
	 * info has been discovered. This queue is needed because the
	 * detection process is asynchronous.
	 * The vector holds a string vector; the inner vector will always have 2 items:
	 * item 0 is the framework identifier and item 1 is the detector action
	 */
	std::vector<std::vector<wxString> > FrameworkIdentifiersLeftToDetect;
	
	/**
	 * the list of result of URL detection; these could be from multiple frameworks
	 */
	std::vector<UrlResourceClass> UrlsDetected;
	
	/**
	 * the list of result of view file detection
	 */
	std::vector<wxFileName> ViewFilesDetected;
	
	/**
	 * event handler that will receive the EVENT_FRAMEWORK_DETECTION_COMPLETE, EVENT_FRAMEWORK_URL
	 * events
	 */
	wxEvtHandler& Handler;
	
	/**
	 * The environment; used to get the location of the PHP executable
	 */
	const EnvironmentClass& Environment;
	
	/**
	 * The location of the project to be scanned
	 */
	wxString ProjectRootPath;
	
	/**
	 * methods that get called when one of the external processes finishes
	 */
	void OnFrameworkDetectionComplete(wxCommandEvent& event);
	void OnDatabaseDetectionComplete(wxCommandEvent& event);
	void OnConfigFilesDetectionComplete(wxCommandEvent& event);
	void OnResourcesDetectionComplete(wxCommandEvent& event);
	void OnUrlDetectionComplete(wxCommandEvent& event);
	void OnViewFileDetectionComplete(wxCommandEvent& event);
	
	/**
	 * method that get called when one of the external processes fails
	 */
	void OnDetectionFailed(wxCommandEvent& event);
	void OnUrlDetectionFailed(wxCommandEvent& event);
	void OnViewFileDetectionFailed(wxCommandEvent& event);
	
	/**
	 * these methods will take care of running the next detection in the queue
	 */
	void NextDetection();
	void NextUrlDetection();
	void NextViewFileDetection();
	
	void OnWorkInProgress(wxCommandEvent& event);
	
	DECLARE_EVENT_TABLE()
};

class UrlDetectedEventClass : public wxEvent {

	public:

	/**
	 * the URLs that were detected; these URLs are calculated using framework specific
	 * routing rules. Urls will not contain a hostname
	 */
	std::vector<UrlResourceClass> Urls;
	
	UrlDetectedEventClass(std::vector<UrlResourceClass> urls);
	
	/**
	 * needed by wxPostEvent
	 */
	wxEvent* Clone() const;
};

class ViewFilesDetectedEventClass : public wxEvent {

	public:	
	
	std::vector<wxFileName> ViewFiles;

	ViewFilesDetectedEventClass(std::vector<wxFileName> viewFiles);
	
	/**
	 * needed by wxPostEvent
	 */
	wxEvent* Clone() const;
};

typedef void (wxEvtHandler::*UrlDetectedEventClassFunction)(UrlDetectedEventClass&);
typedef void (wxEvtHandler::*ViewFilesDetectedEventClassFunction)(ViewFilesDetectedEventClass&);

#define EVT_FRAMEWORK_URL_COMPLETE(fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_FRAMEWORK_URL_COMPLETE, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( UrlDetectedEventClassFunction, & fn ), (wxObject *) NULL ),

#define EVT_FRAMEWORK_VIEW_FILES_COMPLETE(fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_FRAMEWORK_VIEW_FILES_COMPLETE, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( ViewFilesDetectedEventClassFunction, & fn ), (wxObject *) NULL ),

}

#endif