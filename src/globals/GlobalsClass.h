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

#ifndef __T4P_STRUCTSCLASS_H__
#define __T4P_STRUCTSCLASS_H__

#include <globals/EnvironmentClass.h>
#include <globals/UrlTagClass.h>
#include <globals/ProjectClass.h>
#include <globals/SqlResourceFinderClass.h>
#include <globals/TemplateFileTagClass.h>
#include <language/TagCacheClass.h>
#include <globals/FileTypeClass.h>
#include <vector>

namespace t4p {

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
	std::vector<t4p::ProjectClass> Projects;

	/**
	 * the URL that the user chose to view files from
	 */
	UrlTagClass CurrentUrl;

	/**
	 * The name of the browser that is selected
	 */
	wxString ChosenBrowser;
	
	/**
	 * Holds all of the file type => file extension associations
	 */
	t4p::FileTypeClass FileTypes;

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
	 * @see t4p::ParsedTagFinderClass
	 * @see t4p::TagCacheDbVersionActionClass
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
	 * @see t4p::DetectorCacheDbVersionClass
	 *
	 * @return the full path to the tag database that stores detected tags for all defined projects
	 */
	wxFileName DetectorCacheDbFileName;
	
	/**
	 * the opened connection to the resources tags db
	 * @var session
	 */
	soci::session ResourceCacheSession;

	/**
	 * the opened connection to the detector tags db
	 * @var session
	 */
	soci::session DetectorCacheSession;
	
	/**
	 * List of the local volumes that are mounted and writable.
	 * if any source directories
	 * are not in one of these voluimes, it means that they are in network drives
	 * we will not add them to the watch, as watches on network
	 * directories fail to notify of file changes inside of sub-directories.
	 */
	std::vector<wxString> LocalVolumes;

	GlobalsClass();

	void Close();

	/**
	 * @return vector of all directories of all source files in all enabled
	 * projects. For example, if there are 3 enabled projects, each with 2 sources
	 * directories, then this method returns 4 source instances. Each returned source
	 * will have the same include/exclude wildcards as the original source.
	 */
	std::vector<t4p::SourceClass> AllEnabledSources() const;

	/**
	 * Same as AllEnabledSources() but each returned source
	 * will have the the PHP file filters as its include wildcards, not the same wildcards 
	 * as the original source.
	 *
	 * @return vector of all directories of all source files in all enabled
	 * projects. For example, if there are 3 enabled projects, each with 2 sources
	 * directories, then this method returns 4 source instances.
	 */
	std::vector<t4p::SourceClass> AllEnabledPhpSources() const;

	/**
	 * Same as AllEnabledSources() but it returns the root directories of all enabled
	 * sources.
	 *
	 * @return vector of all root directories of all sources.
	 */
	std::vector<wxFileName> AllEnabledSourceDirectories() const;

	/**
	 * @return vector of all enabled projects
	 */
	std::vector<t4p::ProjectClass> AllEnabledProjects() const;

	/**
	 * @return bool TRUE if there is at least 1 enabled project that has AT LEAST 1 source
	 */
	bool HasSources() const;

	/**
	 * @return TRUE if given full path is a PHP file, as determined by
	 * the sources directories and the php file 
	 * extensions wilcard. Careful, This method will return FALSE 
	 * for php files that are NOT in a project. 
	 */
	bool IsAPhpSourceFile(const wxString& fullPath) const;
	
	/**
	 * @return TRUE if given full path is a file that triumph tracks, as determined by
	 * the sources directories and any of the configure file extensions.
	 * Careful, This method will return FALSE 
	 * for php files that are NOT in a project, or files that triumph
	 * is not set to look at.
	 */
	bool IsASourceFile(const wxString& fullPath) const;

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
	void AssignFileExtensions(t4p::ProjectClass& project) const;

	
	/**
	 * The current template files that have been calculated to be used
	 * by the CurrentUrl.
	 */
	// TODO clarify what url is used to calulate these template files
	// since this method just reads from the detector db, the templates
	// are for the url as picked in the template files panel and NOT
	// the URL dialog.
	std::vector<t4p::TemplateFileTagClass> CurrentTemplates() const;
	
	/**
	 * @return all of the database tags that are enabled.
	 */
	std::vector<t4p::DatabaseTagClass> AllEnabledDatabaseTags() const;
	
	/**
	 * find a db tag by hash
	 * will find even tags that are not enabled
	 */
	bool FindDatabaseTagByHash(const wxString& connectionHash, t4p::DatabaseTagClass& tag) const;
	
	/**
	 * check to see if the given file is located in a local volume.
	 * Note that this check is quick because the volumes
	 * are scanned only once at app start.
	 * 
	 * @param the file to check
	 * @return bool if TRUE, it means that the given file is located in one of
	 *         the system's local volumes (hard drives). If false, then
	 *         file is located in a network share, read-only, or removable
	 *         media.
	 */
	bool IsInLocalVolume(const wxFileName& fileName) const;
	
};

}
#endif
