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
#include <wx/string.h>
#include <wx/thread.h>
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
 * The Project class is the class that detects various artifacts for PHP frameworks.
 */
class ProjectClass {
	
public:

	wxMutex SqlResourceFinderMutex;
	
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
	 * Get the SQL ResourceFinder. 
	 * WARNING: NEED TO ACQUIRE THE LOCK FIRST!! You need to acquire the sql resource finder mutex of this object
	 * 
	 * wxMutexLocker locker(Project.SqlResourceFinderMutex)
	 * if (locker.IsOk()) {
	 *   //... your logic
	 * }
	 * 
	 */
	SqlResourceFinderClass* GetSqlResourceFinder();
	
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
	 * To grab SQL table meta data
	 */
	SqlResourceFinderClass SqlResourceFinder;
	
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
