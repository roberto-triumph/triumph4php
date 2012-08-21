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

#ifndef __MVCEDITOR_STRUCTSCLASS_H__
#define __MVCEDITOR_STRUCTSCLASS_H__

#include <environment/EnvironmentClass.h>
#include <environment/UrlResourceClass.h>
#include <environment/ProjectClass.h>
#include <environment/SqlResourceFinderClass.h>
#include <language/ResourceCacheClass.h>
#include <php_frameworks/FrameworkDetectorClass.h>
#include <vector>

namespace mvceditor {

/**
 * Class that groups together all of the Analysis source code; ie
 * the source code's resources, URLs, and templates.
 */
class StructsClass {

public:

	/**
	 * The environment stack.
	 * 
	 * @var EnvironmentClass
	 */
	EnvironmentClass Environment;
	
	/**
	 * This object will be used to parse the resources of files that are currently open.
	 */
	ResourceCacheClass ResourceCache;

	/**
	 * The URLs (entry points to the current project) that have been detected so far. Also holds the 
	 * "current" URL.
	 */
	UrlResourceFinderClass UrlResourceFinder;
	
	/**
	 * To grab SQL table meta data
	 */
	SqlResourceFinderClass SqlResourceFinder;
	
	/**
	 * The datatabase connections. These are all of the connections, there may be a 
	 * mix of connections created by the user and connections that were detected
	 * by the DatabaseDetector.
	 */ 
	std::vector<DatabaseInfoClass> Infos;

	/**
	 * All of the views of the currently selected controller/action
	 * pair.
	 */
	std::vector<mvceditor::ViewInfoClass> CurrentViewInfos;

	/**
	 * All of the projects defined by the user.
	 */
	std::vector<mvceditor::ProjectClass> Projects;

	/**
	 * the URL that the user chose to view files from
	 */
	UrlResourceClass CurrentUrl;

	/**
	 * Serialized PHP file filters string from the config
	 */
	wxString PhpFileFiltersString;

	/**
	 * Serialized CSS file filters string from the config
	 */
	wxString CssFileFiltersString;

	/**
	 * Serialized SQL file filters string from the config
	 */
	wxString SqlFileFiltersString;

	/**
	 * framework info that was detected
	 */
	std::vector<mvceditor::FrameworkClass> Frameworks;

	StructsClass();

	/**
	 * @return vector of all directories of all source files in all enabled
	 * projects. For example, if there are 3 enabled projects, each with 2 sources
	 * directories, then this method returns 4 source instances.
	 */
	std::vector<mvceditor::SourceClass> AllEnabledSources() const;

	/**
	 * @return bool TRUE if there is at least 1 enabled project that has AT LEAST 1 source
	 */
	bool HasSources() const;

	/**
	 * Returns the valid PHP file extensions for this project
	 * @return std::vector<wxSring> a copy of the file extensions
	 */
	std::vector<wxString> GetPhpFileExtensions() const;

	/**
	 * Returns the valid CSS file extensions for this project
	 * @return std::vector<wxSring> a copy of the file extensions
	 */
	std::vector<wxString> GetCssFileExtensions() const;

	/**
	 * Returns the valid SQL file extensions for this project
	 * @return std::vector<wxSring> a copy of the file extensions
	 */
	std::vector<wxString> GetSqlFileExtensions() const;

	/**
	 * @return TRUE if given full path is a PHP file, as determined by
	 * the sources directories and the php file 
	 * extensions wilcard.
	 */
	bool IsAPhpSourceFile(const wxString& fullPath) const;

	/**
	 * removes the source directory from the given full path.
	 * Examples
	 * source RootDirectory = /home/roberto/
	 * fullPath = /home/roberto/workspace/now.php
	 * Then this method returns "workspace/now.php"
	 * 
	 * @param full path to a file
	 * @param projectLabel will be filled in with the label of the project
	 *       where that contains the given fullPath. Note that this is 
	 *       the first matching project
	 * @return the part of the file without the source prefix
	 * In the case that fullPath is not contained in any 
	 * project's sources, then this method returns nothing.
	 */
	wxString RelativeFileName(const wxString& fullPath, wxString& projectLabel) const;

	/**
	 * Removes all of the DatabaseInfo instances that were detected by
	 * a PHP framework. This should be called before detection is re-attempted.
	 */
	void ClearDetectedInfos();

};

}
#endif
