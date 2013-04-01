/**
 * The MIT License
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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <actions/ActionClass.h>
#include <search/DirectorySearchClass.h>
#include <pelet/TokenClass.h>
#include <vector>

#ifndef __MVCEDITOR_RESOURCECACHEACTIONCLASS_H__
#define __MVCEDITOR_RESOURCECACHEACTIONCLASS_H__

namespace mvceditor {

/**
 * This class will take care of iterating through all files in a project
 * and parsing the resources so that queries to ParsedTagFinderClass
 * will work.
 * The ProjectTagActionClass will generate a GlobalCacheCompleteEventClass 
 * event once each project  has been parsed.
 */
class ProjectTagActionClass : public ActionClass {

public:

	/**
	 * @param runningThreads will receive EVENT_FILE_* and EVENT_WORK_* events when all 
	 * files have been iterated through.
	 */
	ProjectTagActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * prepare to iterate through the given file. The name part of the given file must match the wildcard.
	 * This method can be used to update the resources once a file has been modified on disk.
	 *
	 * @param globals to get the tag cache location
	 * @param fullPath file to be scanned (full path, including name).
	 * @return bool false file does not exist
	 */
	bool InitForFile(mvceditor::GlobalsClass& globals, const wxString& fullPath);

	/**
	 * prepare to iterate through all files of the given projects
	 * that match the given wildcard. Only projects that are enabled will
	 * scanned.
	 *
	 * @param globals to get the projects' directories to be scanned (recursively scan all sources in all projects)
	 * @param version the version of PHP to check against
	 * @return bool false if none of the projects are enabled or none of the projects have a PHP source directory
	 */
	bool Init(mvceditor::GlobalsClass& globals);

	/**
	 * This method can be used to set the projects to be scanned.  By default, all of the projects in the
	 * globals list are scanned.  Sometimes, like for example when a single project is edited, we dont want
	 * to scan all of the projects since they have not been changed. This method must be called the Init method.
	 */
	void SetTouchedProjects(const std::vector<mvceditor::ProjectClass>& touchedProjects);

	/**
	 * Files will be parsed for resouces in a background thread.
	 */
	void BackgroundWork();

	wxString GetLabel() const;

private:

	/**
	 * Queue of projects to be indexed.
	 */
	std::vector<mvceditor::ProjectClass> Projects;

	/**
	 * The object that will be used to traverse the file system.
	 */
	DirectorySearchClass DirectorySearch;

	/**
	 * the location of the tag cache; the sqlite file where the tags are stored
	 */
	wxFileName TagCacheDbFileName;

	/**
	 * the version of PHP to parse against
	 */
	pelet::Versions Version;

	/**
	 * This object will perform the parsing and storing of the tags
	 */
	mvceditor::GlobalCacheClass GlobalCache;

	/**
	 * TRUE if we should iterate though the touched projects and not all projects.
	 */
	bool DoTouchedProjects;

	/**
	 * recurse through all sources in a single project
	 */
	void IterateDirectory();

	/**
	 * recurse though all of the queued projects
	 */
	void IterateProjects();

};

/**
 * This class will prime the tag cache with all of the
 * enabled projects. The tag cache will be primed; although
 * it will be primed with the existing cache file which may be
 * stale. 
 */
class ProjectTagInitActionClass : public mvceditor::InitializerActionClass {

public:

	ProjectTagInitActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	void Work(mvceditor::GlobalsClass& globals);

	wxString GetLabel() const;
};

}

#endif