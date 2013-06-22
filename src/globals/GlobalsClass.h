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

#include <globals/EnvironmentClass.h>
#include <globals/UrlTagClass.h>
#include <globals/ProjectClass.h>
#include <globals/SqlResourceFinderClass.h>
#include <globals/TemplateFileTagClass.h>
#include <language/TagCacheClass.h>
#include <vector>

namespace mvceditor {

/**
 * Class that groups together all of the Analysis source code; ie
 * the source code's resources, URLs, and templates.
 */
class GlobalsClass {

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
	TagCacheClass TagCache;

	/**
	 * The URLs (entry points to the current project) that have been detected so far. Also holds the 
	 * "current" URL.
	 */
	UrlTagFinderClass UrlTagFinder;
	
	/**
	 * To grab SQL table meta data
	 */
	SqlResourceFinderClass SqlResourceFinder;
	
	/**
	 * The datatabase connections. These are all of the connections, there may be a 
	 * mix of connections created by the user and connections that were detected
	 * by the DatabaseTagDetector scripts.
	 */ 
	std::vector<DatabaseTagClass> DatabaseTags;

	/**
	 * All of the projects defined by the user.
	 */
	std::vector<mvceditor::ProjectClass> Projects;

	/**
	 * the URL that the user chose to view files from
	 */
	UrlTagClass CurrentUrl;

	/**
	 * The name of the browser that is selected
	 */
	wxString ChosenBrowser;

	/**
	 * Serialized PHP file filters string from the config
	 */
	wxString PhpFileExtensionsString;

	/**
	 * Serialized CSS file filters string from the config
	 */
	wxString CssFileExtensionsString;

	/**
	 * Serialized SQL file filters string from the config
	 */
	wxString SqlFileExtensionsString;

	/**
	 * Serialized miscalleneous file filters string from the config
	 * basically any files that we want to open in the editor, like
	 * XML files, YML files, etc...
	 */
	wxString MiscFileExtensionsString;

	/**
	 * The location of the tag cache db. The tag DB file contains all of
	 * the parsed resources for all of defined projects' resources.
	 *
	 * The SQL schema for this cache can be found in resources/sql/resource.sql
	 * Schema management will be done by TagCacheDbVersionClass. We will check the version
	 * of the schema in the db against the schema the code expects and will re-create the schema
	 * if the versions differ.  This check will be done at app start, so that in most of the code
	 * we can assume that the schema is up-to-date.
	 *
	 * @see mvceditor::ParsedTagFinderClass
	 * @see mvceditor::TagCacheDbVersionActionClass
	 *
	 * the full path to the tag database that stores tags for all defined projects
	 */
	wxFileName TagCacheDbFileName;

	/**
	 * The location of the detectors cache for all defined projects. 
	 *
	 * The SQL schema for this cache can be found in resources/sql/detectors.sql
	 * Schema management will be done by DetectorCacheDbVersionClass. We will check the version
	 * of the schema in the db against the schema the code expects and will re-create the schema
	 * if the versions differ.  This check will be done at app start, so that in most of the code
	 * we can assume that the schema is up-to-date.
	 *
	 * @see mvceditor::DetectorCacheDbVersionClass
	 *
	 * @return the full path to the tag database that stores detected tags for all defined projects
	 */
	wxFileName DetectorCacheDbFileName;

	/**
	 * the opened connection to the detector tags db
	 * @var session
	 */
	soci::session DetectorCacheSession;

	GlobalsClass();

	/**
	 * @return vector of all directories of all source files in all enabled
	 * projects. For example, if there are 3 enabled projects, each with 2 sources
	 * directories, then this method returns 4 source instances. Each returned source
	 * will have the same include/exclude wildcards as the original source.
	 */
	std::vector<mvceditor::SourceClass> AllEnabledSources() const;

	/**
	 * Same as AllEnabledSources() but each returned source
	 * will have the the PHP file filters as its include wildcards, not the same wildcards 
	 * as the original source.
	 *
	 * @return vector of all directories of all source files in all enabled
	 * projects. For example, if there are 3 enabled projects, each with 2 sources
	 * directories, then this method returns 4 source instances.
	 */
	std::vector<mvceditor::SourceClass> AllEnabledPhpSources() const;

	/**
	 * @return vector of all enabled projects
	 */
	std::vector<mvceditor::ProjectClass> AllEnabledProjects() const;

	/**
	 * @return bool TRUE if there is at least 1 enabled project that has AT LEAST 1 source
	 */
	bool HasSources() const;

	/**
	 * Returns the valid PHP file extensions
	 * @return std::vector<wxString> a copy of the file extensions
	 */
	std::vector<wxString> GetPhpFileExtensions() const;

	/**
	 * Returns the valid CSS file extensions
	 * @return std::vector<wxString> a copy of the file extensions
	 */
	std::vector<wxString> GetCssFileExtensions() const;

	/**
	 * Returns the valid SQL file extensions 
	 * @return std::vector<wxString> a copy of the file extensions
	 */
	std::vector<wxString> GetSqlFileExtensions() const;

	/**
	 * Returns the valid Misc. file extensions
	 * @return std::vector<wxString> a copy of the file extensions
	 */
	std::vector<wxString> GetMiscFileExtensions() const;
	
	/**
	 * Returns the all file extensions exception PHP file extensions
	 * @return std::vector<wxString> a copy of the file extensions
	 */
	std::vector<wxString> GetNonPhpFileExtensions() const;

	/**
	 * Returns the all file extensions that we want the editor to open
	 *   php, css, sql, and misc
	 * @return std::vector<wxString> a copy of the file extensions
	 */
	std::vector<wxString> GetAllSourceFileExtensions() const;

	/**
	 * @return TRUE if given full path is a PHP file, as determined by
	 * the sources directories and the php file 
	 * extensions wilcard. Careful, This method will return FALSE 
	 * for php files that are NOT in a project. 
	 */
	bool IsAPhpSourceFile(const wxString& fullPath) const;

	/**
	 * @return TRUE if given full path is a PHP file, as determined only by
	 * the php file extensions wilcard. This method will return TRUE 
	 * for php files that are NOT in a project.
	 */
	bool HasAPhpExtension(const wxString& fullPath) const;

	/**
	 * @return TRUE if given full path is a SQL file, as determined only by
	 * the sql file extensions wilcard. This method will return TRUE 
	 * for sql files that are NOT in a project.
	 */
	bool HasASqlExtension(const wxString& fullPath) const;

	/**
	 * @return TRUE if given full path is a CSS file, as determined only by
	 * the css file extensions wilcard. This method will return TRUE 
	 * for css files that are NOT in a project.
	 */
	bool HasACssExtension(const wxString& fullPath) const;

	/**
	 * @return TRUE if given full path is a misc file, as determined only by
	 * the misc file extensions wilcard. This method will return TRUE 
	 * for misc files that are NOT in a project.
	 */
	bool HasAMiscExtension(const wxString& fullPath) const;

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

	/**
	 * copies the file filters into the project; we 
	 * let the user define the file filters globally and we use
	 * this method to propagate the setting to all projects
	 *
	 * @param project set the file filters on the given project
	 */
	void AssignFileExtensions(mvceditor::ProjectClass& project) const;

	
	/**
	 * The current template files that have been calculated to be used
	 * by the CurrentUrl.
	 */
	// TODO clarify what url is used to calulate these template files
	// since this method just reads from the detector db, the templates
	// are for the url as picked in the template files panel and NOT
	// the URL dialog.
	std::vector<mvceditor::TemplateFileTagClass> CurrentTemplates() const;

};

}
#endif
