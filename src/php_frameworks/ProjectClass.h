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
#include <wx/string.h>
#include <vector>

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
 * This is the database connection information used by the framework.
 * The information will actually be located via a PHP script
 * (MvcEditorFrameworkApp.php) and not the C++ code. this way we
 * can extend / modifify the code for different frameworks.
 */
class DatabaseInfoClass {

public:

	/**
	 * The RDBMS systems that both the frameworks and MVC Editor supports.
	 */
	enum Drivers {
		MYSQL,
		POSTGRESQL,
		SQLITE
	};
	
	/**
	 * The database host to connect to
	 */
	wxString Host;
	
	/**
	 * The database user to connect as
	 */
	wxString User;
	
	/**
	 * The database password
	 */
	wxString Password;
	
	/**
	 * The database (schema name) to connect to
	 */
	wxString DatabaseName;
	
	/**
	 * The full path to the database (in case of SQLite)
	 */
	wxString FileName;
	
	/**
	 * A human friendly name for this info
	 */
	wxString Name;
	
	/**
	 * the system that is used.
	 */
	Drivers Driver;
	
	/**
	 * The port to connect to
	 */
	int Port;
	
	DatabaseInfoClass();
	
	/**
	 * copy the attributes from src to this object.
	 */
	DatabaseInfoClass(const DatabaseInfoClass& other);
	
	/**
	 * copy the attributes from src to this object.
	 */
	void Copy(const DatabaseInfoClass& src);
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
	 * run the detection code against the root path to figure out which frameworks
	 * the project uses.
	 * @param useTest quick hack for unit testing; use TRUE 
	 * if false the test framework will be omitted
	 */
	void Detect(bool useTest = false);
	
	/**
	 * Returns the root path of this project
	 */
	wxString GetRootPath() const;
	
	/**
	 * Returns the valid PHP file extensions for this project
	 * @return wxString file extensions
	 */
	wxString GetPhpFileExtensions() const;
	
	/**
	 * Returns the PHP keywords, according to the PHP version of this project.
	 */
	wxString GetPhpKeywords() const;
	
	/**
	 * returns the location of the PHP executable
	 */
	wxString GetPhpExecutable() const;
	
	/**
	 * This object will still own the returned pointer. Do NOT delete it.
	 */
	ResourceFinderClass* GetResourceFinder();
	
	/**
	 * Returns the detected database connection infos
	 */
	std::vector<DatabaseInfoClass> DatabaseInfo();

private:
	
	/**
	 * sanitize a string to be suitable as an argument to a command 
	 */
	wxString Sanitize(const wxString& arg) const;
	
	/**
	 * Send a query to the PHP detection code
	 * returns the parsed response
	 */
	wxString Ask(const wxString& action, const wxString& identifier) const;
	
	wxString AskDatabaseInfo(const wxString& identifier) const;
	
	/*
	 * Holds project attributes.
	 */
	ProjectOptionsClass Options;

	/**
	 * to look for classes and methods
	 */
	ResourceFinderClass ResourceFinder;
	
	/**
	 * the detected frameworks
	 */
	std::vector<wxString> FrameworkIdentifiers;
	
	/**
	 * The detected database connection info
	 */
	std::vector<DatabaseInfoClass> Databases;
};

}
#endif /*PROJECTCLASS_H_*/
