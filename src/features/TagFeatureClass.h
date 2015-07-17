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
#ifndef __RESOURCEFEATURECLASS_H
#define __RESOURCEFEATURECLASS_H

#include <features/FeatureClass.h>
#include <features/BackgroundFileReaderClass.h>
#include <language_php/ParsedTagFinderClass.h>
#include <actions/ProjectTagActionClass.h>
#include <actions/TagCacheSearchActionClass.h>
#include <code_control/ResourceCacheBuilderClass.h>
#include <globals/Events.h>
#include <wx/string.h>
#include <queue>

namespace t4p {

/**
 * The tag feature will parse PHP files that have been 
 * edited and will record the files' classes, methods, and
 * functions.  All parsing is done in a background thread.
 * The tag feature also listens for events from the file watcher
 * so that files that have been modified outside the editor are
 * re-parsed.  For example, if the user performs a 'git pull'
 * then once the files are updated on disk, file watcher will
 * generate file events, which the tag feature listens for
 * and takes the appropriate action without needing the user to
 * re-tag files.
 * 
 * Also, another cool thing about this feature is that directory
 * renames are efficiently handled; if the user renames a directory
 * then we just update the directory in the tag cache, we don't
 * acutally re-parse the new directory. 
 */
class TagFeatureClass : public FeatureClass {

public:

	/**
	 * when a 'jump to tag' is done and we need to index a project, we
	 * need to keep the search string so that after indexing we can
	 * search the index.
	 */
	wxString JumpToText;
	
	TagFeatureClass(t4p::AppClass& app);

	/** 
	 * returns a short string describing the status of the cache.
	 */
	wxString CacheStatus();
	
private:

	void OnAppStartSequenceComplete(wxCommandEvent& event);

	/**
	 * when a file is deleted remove it from the tag cache
	 */
	void OnAppFileDeleted(wxCommandEvent& event);
	
	/**
	 * when a file is renamed then update the tag cache
	 */
	void OnAppFileRenamed(t4p::RenameEventClass& event);
	
	/**
	 * when a directory has been created tag the directory
	 */
	void OnAppDirCreated(wxCommandEvent& event);

	/**
	 * when a directory has been delete clear the tags from that directory
	 */
	void OnAppDirDeleted(wxCommandEvent& event);

	/**
	 * when a dir is renamed then update the tag cache
	 */
	void OnAppDirRenamed(t4p::RenameEventClass& event);

	/**
	 * if a file is modified externally re-tag it
	 */
	void OnAppFileExternallyModified(wxCommandEvent& event);
	
	void OnProjectsUpdated(wxCommandEvent& event);

	void OnAppFileClosed(t4p::CodeControlEventClass& event);


	void OnAppExit(wxCommandEvent& event);
	
	
	/**
	 * prepare to iterate through the given file. The name part of the given file must match the wildcard.
	 * This method can be used to update the resources once a file has been modified on disk.
	 *
	 * @param project the project that holds the file
	 * @param fullPath file to be scanned (full path, including name).
	 * @param version the version of PHP to check against
	 * @return bool false file does not exist
	 */
	bool InitForFile(const t4p::ProjectClass& project, const wxString& fullPath, pelet::Versions version);

	/**
	 * cache will be considered stale at app start. once all projects have
	 * been indexed then it will be considered as good; though this is a real
	 * naive status as it does not take file system changes from external processes
	 */
	enum CacheStatus {
		CACHE_STALE,
		CACHE_OK
	} CacheState;
	
	DECLARE_EVENT_TABLE()
};

}
#endif // __RESOURCEFEATURECLASS_H
