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
#ifndef PROJECTCLASS_H_
#define PROJECTCLASS_H_

#include <search/ResourceFinderClass.h>
#include <environment/DatabaseInfoClass.h>
#include <environment/SqlResourceFinderClass.h>
#include <environment/EnvironmentClass.h>
#include <widgets/ProcessWithHeartbeatClass.h>
#include <wx/string.h>
#include <wx/thread.h>
#include <wx/event.h>
#include <vector>
#include <map>

namespace mvceditor {

class DetectorClass : public wxEvtHandler {

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
	DetectorClass(wxEvtHandler& handler);
	virtual ~DetectorClass();

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
	 * @return wxString the command (operating system command line) that will run the PHP detection code.
	 */
	bool Init(int id, const EnvironmentClass& environment, const wxString& projectRootPath, const wxString& identifier);

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
class FrameworkDetectorClass : public DetectorClass {

public:

	std::vector<wxString> Frameworks;

	FrameworkDetectorClass(wxEvtHandler& handler);

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
class DatabaseDetectorClass : public DetectorClass {

public:

	std::vector<DatabaseInfoClass> Databases;

	DatabaseDetectorClass(wxEvtHandler& handler);
	
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
class ConfigFilesDetectorClass : public DetectorClass {

public: 
	
	std::map<wxString, wxString> ConfigFiles;

	ConfigFilesDetectorClass(wxEvtHandler& handler);

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
class ResourcesDetectorClass : public DetectorClass {

public: 
	
	std::vector<mvceditor::ResourceClass> Resources;

	ResourcesDetectorClass(wxEvtHandler& handler);

protected:

	void Clear();

	wxString GetAction();

	bool Response();
};

/*
 * Data structure that holds project attributes.
 */
class ProjectOptionsClass {
	
public:
	
	/**
	 * Default constructor. Sets name & root path to empty string, and a 
	 * generic project.
	 */
	ProjectOptionsClass();
	
	/**
	 * Create a new project options object from another one. The new
	 * project options will have the same values as other.
	 * 
	 * @param ProjectOptionsClass other the options to copy FROM
	 */
	ProjectOptionsClass(const ProjectOptionsClass& other);
	
	/**
	 * The location of the root directory of the project in
	 * the file system. 
	 */
	wxString RootPath;
};

/**
 * The Project class is the class that detects various artifacts for PHP frameworks.
 */
class ProjectClass {
	
public:
	
	/**
	 * Construct a ProjectClass object from the given options
	 * 
	 * @param ProjectOptionsClass options the new project's options
	 */
	ProjectClass(const ProjectOptionsClass& options);

	/**
	 * Returns the root path of this project
	 */
	wxString GetRootPath() const;
	
	/**
	 * Returns the valid PHP file extensions for this project
	 * @return wxString file extensions. This string will be suitable to
	 * serialize the wildcard list.
	 */
	wxString GetPhpFileExtensionsString() const;

	/**
	 * Returns the valid PHP file extensions for this project
	 * @return std::vector<wxSring> a copy of the file extensions
	 */
	std::vector<wxString> GetPhpFileExtensions() const;

	/**
	 * @param wxString Tokenizes the wildcards in the given string and adds them to the
	 * PhpFileFilters.
	 * Each wildcard is assumed to be tokenized by a semicolon ';'
	 * Existing filters are left intact
	 */
	void SetPhpFileExtensionsString(wxString wildcardString);

	/**
	 * Returns the valid CSS file extensions for this project
	 * @return wxString file extensions. This string will be suitable to
	 * serialize the wildcard list.
	 */
	wxString GetCssFileExtensionsString() const;

	/**
	 * Returns the valid CSS file extensions for this project
	 * @return std::vector<wxSring> a copy of the file extensions
	 */
	std::vector<wxString> GetCssFileExtensions() const;

	/**
	 * @param wxString Tokenizes the wildcards in the given string and adds them to the
	 * CssFileFilters.
	 * Each wildcard is assumed to be tokenized by a semicolon ';'
	 * Existing filters are left intact
	 */
	void SetCssFileExtensionsString(wxString wildcardString);

	/**
	 * Returns the valid SQL file extensions for this project
	 * @return wxString file extensions. This string will be suitable to
	 * serialize the wildcard list.
	 */
	wxString GetSqlFileExtensionsString() const;

	/**
	 * Returns the valid SQL file extensions for this project
	 * @return std::vector<wxSring> a copy of the file extensions
	 */
	std::vector<wxString> GetSqlFileExtensions() const;

	/**
	 * @param wxString Tokenizes the wildcards in the given string and adds them to the
	 * SqlFileFilters.
	 * Each wildcard is assumed to be tokenized by a semicolon ';'
	 * Existing filters are left intact
	 */
	void SetSqlFileExtensionsString(wxString wildcardString);
	
	/**
	 * Returns the PHP keywords, according to the PHP version of this project.
	 */
	// TODO remove from this class and into one of the Language classes
	wxString GetPhpKeywords() const;
	
	/**
	 * @return the project's parsed resources (class, method, function names).
	 * This object will still own the returned pointer. Do NOT delete it.
	 */
	// TODO move this out of here; need to create a ResourceFinderLocker
	ResourceFinderClass* GetResourceFinder();
	
	/**
	 *  This object will still own the returned pointer. Do NOT delete it.
	 */
	SqlResourceFinderClass* GetSqlResourceFinder();

	/**
	* Returns the detected database connection infos.
	* Filled by the Application object
	*/
	std::vector<DatabaseInfoClass> DatabaseInfo() const;

	void PushDatabaseInfo(const DatabaseInfoClass& info);

private:
	
	/*
	 * Holds project attributes.
	 */
	ProjectOptionsClass Options;

	/**
	 * to look for classes and methods
	 */
	ResourceFinderClass ResourceFinder;
	
	/**
	 * To grab SQL table meta data
	 */
	SqlResourceFinderClass SqlResourceFinder;

	/**
	 * The detected database connection info
	 */
	std::vector<DatabaseInfoClass> Databases;
	
	/**
	 * The wildcard patterns that will be used to find PHP files in this
	 * project.
	 * Each item in the array will be one wildcard expression; where each
	 * expression can contain either a '*' or a '?' for use in the
	 * wxMatchWild() function.
	 */
	std::vector<wxString> PhpFileFilters;

	/**
	 * The wildcard patterns that will be used to find CSS files in this
	 * project.
	 * Each item in the array will be one wildcard expression; where each
	 * expression can contain either a '*' or a '?' for use in the
	 * wxMatchWild() function.
	 */
	std::vector<wxString> CssFileFilters;

	/**
	 * The wildcard patterns that will be used to find SQL files in this
	 * project.
	 * Each item in the array will be one wildcard expression; where each
	 * expression can contain either a '*' or a '?' for use in the
	 * wxMatchWild() function.
	 */
	std::vector<wxString> SqlFileFilters;
};

}
#endif /*PROJECTCLASS_H_*/
