/**
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#include <pelet/TokenClass.h>
#include <vector>
#include "actions/GlobalActionClass.h"
#include "language_php/TagFinderList.h"
#include "search/DirectorySearchClass.h"

#ifndef SRC_ACTIONS_PROJECTTAGACTIONCLASS_H_
#define SRC_ACTIONS_PROJECTTAGACTIONCLASS_H_

namespace t4p {
/**
 * This class will take care of iterating through all files in a project
 * and parsing the resources so that queries to ParsedTagFinderClass
 * will work.
 * The ProjectTagActionClass will generate a TagFinderListCompleteEventClass
 * event once each project  has been parsed.
 */
class ProjectTagActionClass : public GlobalActionClass {
	public:
	/**
	 * @param runningThreads will receive EVENT_FILE_* and EVENT_WORK_* events when all
	 * files have been iterated through.
	 */
	ProjectTagActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * prepare to iterate through all files of the given projects
	 * that match the given wildcard. Only projects that are enabled will
	 * scanned.
	 *
	 * @param globals to get the projects' directories to be scanned (recursively scan all sources in all projects)
	 * @param version the version of PHP to check against
	 * @return bool false if none of the projects are enabled or none of the projects have a PHP source directory
	 */
	bool Init(t4p::GlobalsClass& globals);

	/**
	 * This method can be used to set the projects to be scanned.  By default, all of the projects in the
	 * globals list are scanned.  Sometimes, like for example when a single project is edited, we dont want
	 * to scan all of the projects since they have not been changed. This method must be called the Init method.
	 */
	void SetTouchedProjects(const std::vector<t4p::ProjectClass>& touchedProjects);

	/**
	 * Files will be parsed for resouces in a background thread.
	 */
	void BackgroundWork();

	wxString GetLabel() const;

	private:
	/**
	 * Queue of projects to be indexed.
	 */
	std::vector<t4p::ProjectClass> Projects;

	/**
	 * the relevant file extensions that we want to look at
	 */
	t4p::FileTypeClass FileTypes;

	/**
	 * The object that will be used to traverse the file system.
	 */
	DirectorySearchClass DirectorySearch;

	/**
	 * This object will perform the parsing and storing of the tags
	 */
	t4p::TagFinderListClass TagFinderList;

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

	/** the number of files that have been parsed in the current project */
	int FilesCompleted;

	/** the number of files to be parsed in the current project */
	int FilesTotal;
};

/**
 * This class will prime the tag cache with all of the
 * enabled projects. The tag cache will be primed; although
 * it will be primed with the existing cache file which may be
 * stale.
 */
class ProjectTagInitActionClass : public t4p::InitializerGlobalActionClass {
	public:
	ProjectTagInitActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	void Work(t4p::GlobalsClass& globals);

	wxString GetLabel() const;
};

/**
 * this action will re-tag a single directory (and its subdirs)
 */
class ProjectTagDirectoryActionClass : public t4p::GlobalActionClass {
	public:
	ProjectTagDirectoryActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * Set the directory to be parsed
	 * @param fullPath directory to be scanned (full path).
	 */
	void SetDirToParse(const wxString& path);

	/**
	 * prepare to iterate through the file given in SetDirToParse.
	 * This method can be used to update the resources once a die has been created on disk.
	 * Note that directory must be inside an enabled project
	 *
	 * @param globals to get the tag cache location
	 * @return bool false if file given in DirToParse does not exist or if the directory is
	 *        not inside any enabled projects
	 */
	bool Init(t4p::GlobalsClass& globals);

	protected:
	void BackgroundWork();

	wxString GetLabel() const;

	/**
	 * the project in which the directory is located in
	 */
	t4p::ProjectClass Project;

	/**
	 * the directory to re-parse
	 */
	wxFileName Dir;

	/**
	 * This object will perform the parsing and storing of the tags
	 */
	t4p::TagFinderListClass TagFinderList;
};

/**
 * this action will re-tag a single file only
 */
class ProjectTagSingleFileActionClass : public t4p::GlobalActionClass {
	public:
	ProjectTagSingleFileActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	/**
	 *
	 * Set the file to be parsed
	 * @param fullPath file to be scanned (full path, including name).
	 */
	void SetFileToParse(const wxString& fullPath);

	/**
	 * prepare to iterate through the file given in SetFileToParse. The name part of the given file must match one
	 * od the wildcards that has been set in globals.
	 * This method can be used to update the resources once a file has been modified on disk.
	 * Note that the file MUST be part of a project.
	 *
	 * @param globals to get the tag cache location
	 * @return bool false if file given in FileToParse does not exist, or if file is not inside any enabled projects
	 */
	bool Init(t4p::GlobalsClass& globals);


	protected:
	void BackgroundWork();

	wxString GetLabel() const;

	/**
	 * the project in which the file is located in
	 */
	t4p::ProjectClass Project;

	/**
	 * the file to re-parse
	 */
	wxFileName FileName;

	/**
	 * This object will perform the parsing and storing of the tags
	 */
	t4p::TagFinderListClass TagFinderList;
};

/**
 * this action will rename the file tag for a single file only
 */
class ProjectTagSingleFileRenameActionClass : public t4p::GlobalActionClass {
	public:
	ProjectTagSingleFileRenameActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	/**
	 *
	 * Set the file to be parsed
	 * @param fullPath file to be scanned (full path, including name).
	 */
	void SetPaths(const wxString& oldPath, const wxString& newPath);

	/**
	 *
	 * @param globals to get the tag cache location
	 * @return bool false tag cache file does not exist
	 */
	bool Init(t4p::GlobalsClass& globals);


	protected:
	void BackgroundWork();

	wxString GetLabel() const;

	/**
	 * the files
	 */
	wxFileName OldFileName;
	wxFileName NewFileName;

	/**
	 * This object will perform the tag updates
	 */
	t4p::TagFinderListClass TagFinderList;

	/**
	 * the project, needed in case we need to parse the tags in the new file.
	 * we need to parse the tags when a file is renamed from a non-php extension to
	 * a php extension
	 */
	t4p::ProjectClass Project;
};

/**
 * this action will rename the file tag for an entire directory
 */
class ProjectTagDirectoryRenameActionClass : public t4p::GlobalActionClass {
	public:
	ProjectTagDirectoryRenameActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	/**
	 *
	 * Set the file to be parsed
	 * @param fullPath file to be scanned (full path, including name).
	 */
	void SetPaths(const wxString& oldPath, const wxString& newPath);

	/**
	 *
	 * @param globals to get the tag cache location
	 * @return bool false tag cache file does not exist
	 */
	bool Init(t4p::GlobalsClass& globals);


	protected:
	void BackgroundWork();

	wxString GetLabel() const;

	/**
	 * the directories
	 */
	wxFileName OldDirectory;
	wxFileName NewDirectory;

	/**
	 * This object will perform the tag updates
	 */
	t4p::TagFinderListClass TagFinderList;
};
}  // namespace t4p

#endif  // SRC_ACTIONS_PROJECTTAGACTIONCLASS_H_
