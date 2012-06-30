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

#include <search/DirectorySearchClass.h>
#include <wx/string.h>
#include <vector>

namespace mvceditor {

/**
 * The Project class represents a single project.
 *
 * This project can hold files from multiple, separate directories.
 */
class ProjectClass {
	
public:

	/**
	 * A friendly label for this project. This is usually set by a user.
	 */
	wxString Description;
	
	/**
	 * Construct a ProjectClass object from the given options
	 * 
	 * @param ProjectOptionsClass options the new project's options
	 */
	ProjectClass();

	/**
	 * Add a source directory to this project.
	 */
	void AddSource(const mvceditor::SourceClass& src);

	/**
	 * Removes all of the sources from this project.
	 */
	void ClearSources();

	/**
	 * @return all of this project's source directories
	 */
	std::vector<mvceditor::SourceClass> AllSources() const;
	
	/**
	 * @return all of this project's source directories
	 * but with the added restriction of including only the
	 * PHP file extension
	 */
	std::vector<mvceditor::SourceClass> AllPhpSources() const;

	/**
	 * @return TRUE if given full path is a PHP file, as determined by
	 * the sources directories and the php file 
	 * extensions wilcard.
	 */
	bool IsAPhpSourceFile(const wxString& fullPath) const;

	/**
	 * @return bool TRUE if this project has AT LEAST 1 source
	 */
	bool HasSources() const;

	/**
	 * removes the source directory from the given full path.
	 * Examples
	 * source directory = /home/roberto/
	 * fullPath = /home/roberto/workspace/now.php
	 * Then this method returns "workspace/now.php"
	 * 
	 * @param full path to a file
	 * @return the part of the file without the source prefix
	 * In the case that fullPath is not contained in any of this
	 * project's sources, then this method returns nothing.
	 */
	wxString RelativeFileName(const wxString& fullPath) const;

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

private:
	
	/**
	 * The directories where source files are located in. 
	 */
	std::vector<mvceditor::SourceClass> Sources;
	
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
