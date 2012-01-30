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

#include <environment/DatabaseInfoClass.h>
#include <environment/SqlResourceFinderClass.h>
#include <wx/string.h>
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
