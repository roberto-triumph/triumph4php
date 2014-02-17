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

namespace t4p {

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
	wxString Label;

	/**
	 * The directories where source files are located in. 
	 */
	std::vector<t4p::SourceClass> Sources;
	
	/**
	 * The wildcard patterns that will be used to find PHP files in this
	 * project.
	 * Each item in the array will be one wildcard expression; where each
	 * expression can contain either a '*' or a '?' for use in the
	 * wxMatchWild() function.
	 */
	std::vector<wxString> PhpFileExtensions;

	/**
	 * The wildcard patterns that will be used to find CSS files in this
	 * project.
	 * Each item in the array will be one wildcard expression; where each
	 * expression can contain either a '*' or a '?' for use in the
	 * wxMatchWild() function.
	 */
	std::vector<wxString> CssFileExtensions;

	/**
	 * The wildcard patterns that will be used to find SQL files in this
	 * project.
	 * Each item in the array will be one wildcard expression; where each
	 * expression can contain either a '*' or a '?' for use in the
	 * wxMatchWild() function.
	 */
	std::vector<wxString> SqlFileExtensions;

	/**
	 * The wildcard patterns that will be used to find JS files in this
	 * project.
	 * Each item in the array will be one wildcard expression; where each
	 * expression can contain either a '*' or a '?' for use in the
	 * wxMatchWild() function.
	 */
	std::vector<wxString> JsFileExtensions;

	/**
	 * The wildcard patterns that will be used to find miscalleneous files in this
	 * project. Misc. files are files that do not contain code but still want
	 * to open them in the editor.
	 *
	 * Each item in the array will be one wildcard expression; where each
	 * expression can contain either a '*' or a '?' for use in the
	 * wxMatchWild() function.
	 */
	std::vector<wxString> MiscFileExtensions;

	/**
	 * If TRUE, this project is enabled and is used by Triumph.
	 */
	bool IsEnabled;
	
	/**
	 * Construct a ProjectClass object from the given options
	 * 
	 * @param ProjectOptionsClass options the new project's options
	 */
	ProjectClass();

	ProjectClass(const t4p::ProjectClass& src);

	void operator=(const t4p::ProjectClass& src);

	/**
	 * Add a source directory to this project.
	 */
	void AddSource(const t4p::SourceClass& src);

	/**
	 * Removes all of the sources from this project.
	 */
	void ClearSources();
	
	/**
	 * This method will return a list of sources suitable
	 * for recursing into this project to find all of the PHP Source
	 * code files in this project.
	 *
	 * @return all of this project's source directories
	 * but with the PHP extensions added to each source.
	 */
	std::vector<t4p::SourceClass> AllPhpSources() const;

	/**
	 * This method will return a list of sources suitable for recursing into
	 * this project to find all relevant files; ie PHP, CSS, SQL,
	 * and all misc file extensions.
	 *
	 * @return all of this project's sources with all of the wildcards (PHP, CSS, SQL, JS,
	 * and misc file extensions) added to each sources. 
	 */
	std::vector<t4p::SourceClass> AllSources() const;

	/**
	 * @return TRUE if given full path is a PHP file, as determined by
	 * the sources directories and the php file 
	 * extensions wilcard.
	 */
	bool IsAPhpSourceFile(const wxString& fullPath) const;

	
	/**
	 * @return TRUE if given full path is a file, as determined by
	 * the sources directories and the php file 
	 * extensions wilcard.
	 */
	bool IsASourceFile(const wxString& fullPath) const;

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
	 *
	 * return the CssFileExtensions + SqlFileExtensions + MiscFileExtensions for this project.
	 * @return all extension wilcards except for PHP wildcards
	 */
	std::vector<wxString> GetNonPhpExtensions() const;

private:
	
};

}
#endif /*PROJECTCLASS_H_*/
