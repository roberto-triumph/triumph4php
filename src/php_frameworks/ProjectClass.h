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
#include <wx/string.h>
#include <wx/thread.h>
#include <vector>
#include <map>

namespace mvceditor {

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
		UNIMPLEMENTED
	};

	/**
	 * Holds the location of the PHP binary.
	 */
	const EnvironmentClass& Environment;
	
	/**
	 * Construct a ProjectClass object from the given options
	 * 
	 * @param ProjectOptionsClass options the new project's options
	 * @param EnvironmentClass the system binary locations
	 */
	ProjectClass(const ProjectOptionsClass& options, const EnvironmentClass& environment);
	
	/**
	 * creates the detection command that will figure out which frameworks
	 * the project uses.  Project "detection" means figuring out what framework a
	 * project uses (could be multiple). The caller of this method should execute the returned
	 * command asynchronosuly and give the results to DetectFrameworkResponse() method.
	 *
	 * @return wxString the command (operating system command line) that will calculate the 
	 *         framework that this project uses.
	 */
	wxString DetectFrameworkCommand();

	/**
	 * creates the detection command that will figure out which database connections
	 * the project uses.
	 * @return wxString the command (operating system command line) that will calculate the 
	 *         database connections that this project uses (for the given framework).
	 */
	wxString DetectDatabaseCommand(const wxString& framework);

	/**
	 * Creates the detection command that will figure out the configuration files 
	 * for the given framework (routes file, database file, etc ...)
	 *
	 * @param framework the identifier of the framework. One of the strings returned by
	 *        the framework detection command
	 * @return wxString the command (operating system command line) that will calculate the 
	 *         configuration files for the framework that this project uses.
	 */
	wxString DetectConfigFilesCommand(const wxString& framework);

	/**
	 * Handle the results of the framework detect command.
	 */
	bool DetectFrameworkResponse(const wxString& resultString, DetectError& error);

	/**
	 * Handle the results of the database detection command. After a call to this method, DatabaseInfo() will return
	 * any new database connections (that were identified).
	 */
	bool DetectDatabaseResponse(const wxString& resultString, DetectError& error);

	/**
	 *  Handle the results of the config files detection.
	 */
	bool DetectConfigFilesResponse(const wxString& resultString, DetectError& error, std::map<wxString, wxString>& configFiles);
	
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
	wxString GetPhpKeywords() const;
	
	/**
	 * returns the location of the PHP executable
	 */
	wxString GetPhpExecutable() const;
	
	/**
	 * @return the project's parsed resources (class, method, function names).
	 * This object will still own the returned pointer. Do NOT delete it.
	 */
	ResourceFinderClass* GetResourceFinder();
	
	/**
	 *  This object will still own the returned pointer. Do NOT delete it.
	 * 
	 */
	SqlResourceFinderClass* GetSqlResourceFinder();
	
	/**
	 * Returns the detected database connection infos.
	 * Filled by DetectDatabaseResponse();
	 */
	std::vector<DatabaseInfoClass> DatabaseInfo() const;

	/**
	 * @return the detected frameworks. Filled by DetectFrameworkResponse()
	 */
	std::vector<wxString> FrameworkIdentifiers() const;

private:
	
	/**
	 * sanitize a string to be suitable as an argument to a command 
	 */
	wxString Sanitize(const wxString& arg) const;
	
	/**
	 * Create the command line to run the PHP detection code
	 * returns the command line string 
	 * @param action the 'query' to pass
	 * @param identifier the detected framework identifier
	 * @return wxString the command (operating system command line) that will run the PHP detection code.
	 */
	wxString Ask(const wxString& action, const wxString& identifier);

	/**
	 * 'clean' the process output; output may contain some text before the INI response; this method
	 * will remove the offending text.
	 * @return the  output  (valid INI format) of the last process that was run
	 */
	wxString GetProcessOutput(const wxString& allOutput);
	
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
	 * the detected frameworks
	 */
	std::vector<wxString> Frameworks;
	
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
