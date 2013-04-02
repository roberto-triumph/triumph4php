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
#ifndef __BACKGROUNDFILEREADER_H__
#define __BACKGROUNDFILEREADER_H__

#include <wx/string.h>
#include <wx/event.h>
#include <actions/ActionClass.h>
#include <search/DirectorySearchClass.h>
#include <widgets/ThreadWithHeartbeatClass.h>


namespace mvceditor {

/**
 * These event will be dispatched to the handler. Note that the 
 * event won't have a consitent ID; use wxID_ANY when connecting
 * to these events.
 *
 */

/**
 * Dispatched when all files have been read or all matched files have been processed.
 * The event will have the following info:
 * event.getInt() will have the mode that was completed: either Modes::WALK or Modes::MATCHED
 * depending on whether Init() or InitMatched() was called.
 */
extern const wxEventType EVENT_FILE_READ_COMPLETE;

/**
 * Dispatched each time a single file has been processed.
 * The event will have the following info:
 * event.getInt() will have the file counter; first file will be 1 then 2 and so on.
 * event.getClientData() will have a boolean value; true /false the value returned by FileRead() or FileMatched()
 */
extern const wxEventType EVENT_FILE_READ;

/**
 * This is a class that will glue together any background task with any "results" type panel.  Since
 * wxThreads cannot access GUI elements we need a way to separate any time-consuming tasks (such as 
 * searching through a project's files) with the updating of a GUI element (such as displaying the
 * found hits).  This class is the "glue" between a background task and GUI element: It will start
 * up a wxThread. Note: Objects of this class will handle at most 1 concurrently running thread at a time.
 * 
 * ATTN: make it so that multiple logic can be added to this class; as we ideally do not want
 * to iterate through entire projects twice. It is better to open a file once and hand it to
 * multiple workers than multiple workers attempting to open/close the same file multiple times.
 */
class BackgroundFileReaderClass : public mvceditor::ActionClass {

public:

	/**
	 * flags to determine whether to iterate through all files
	 * or only files that were matched in the previous find
	 * operation
	 */
	enum Mode {
		WALK,
		MATCHED
	};
	
	/**
	 * @param wxEvtHandler& handler this event handler will receive
	 *        a EVENT_FILE_READ_COMPLETE event when the
	 *        background thread has completed. Will also receive
	 * 		  mvceditor::WORK_* events 
	 * @see mvceditor::ThreadWithHeartbeatClass
	 */
	BackgroundFileReaderClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * Prepare the background thread to iterate through all
	 * files in the given directory.
	 *
	 * @param const wxString& path the path to recurse
	 * @param one of RECURSIVE or PRECISE.  in PRECISE mode, all files for all sub-directories are enumerated at once, making the 
	 *        total files count available.  In RECURSIVE mode, sub-directories are recursed one at a time.  PRECISE mode
	 *        is useful when the caller needs to know how many total files will be walked over, but it is also more
	 *        memory intensive.  Note that both modes will result in walking of all files.
	 * @return bool doHidden if TRUE then hidden files will be walked as well.
	 * @return bool true of the given path exists
	 */
	bool Init(const wxString& path, DirectorySearchClass::Modes mode = DirectorySearchClass::RECURSIVE, bool doHiddenFiles = false);

	/**
	 * Prepare the background thread to iterate through the given sources.
	 *
	 * @param sources the list of directories to recurse
	 * @param one of RECURSIVE or PRECISE.  in PRECISE mode, all files for all sub-directories are enumerated at once, making the 
	 *        total files count available.  In RECURSIVE mode, sub-directories are recursed one at a time.  PRECISE mode
	 *        is useful when the caller needs to know how many total files will be walked over, but it is also more
	 *        memory intensive.  Note that both modes will result in walking of all files.
	 * @return bool doHidden if TRUE then hidden files will be walked as well.
	 * @return bool true of the given path exists
	 */
	bool Init(std::vector<mvceditor::SourceClass> sources, DirectorySearchClass::Modes mode = DirectorySearchClass::RECURSIVE, bool doHiddenFiles = false);

	/**
	 * prepares the thread to iterate over the given set of files
	 * In this case the FileMatch() method will be called.
	 * @param matchedFiles vector of full paths. Each full path will be given the to the BackgroundFileMatch() method.
	 * @return bool true if there are matching files from the previous find
	 * operation.
	 */
	bool InitMatched(const std::vector<wxString>& matchedFiles);

protected: 

	/**
	 * This method will be executed in it's own thread. Most of the time
	 * this method would be implemented by calling search.Walk() method.
	 * The return value of this method will be set as the event.GetClientData()
	 * of the correspoding FILE_READ event; subclasses can use it in the
	 * event handler if they want / need to.
	 */
	virtual bool BackgroundFileRead(DirectorySearchClass& search) = 0;

	/**
	 * This method will be executed in it's own thread. The method
	 * will be given a file that had a match (the files that
	 * has DirectoryWalker.Walk() method return TRUE).
	 */
	virtual bool BackgroundFileMatch(const wxString& file) = 0;

	void BackgroundWork();

private:

	/**
	 * The object that will be used to traverse the file system.
	 */
	DirectorySearchClass DirectorySearch;
	
	/**
	 * The files to traverse through if the caller gave us a set of files
	 */
	std::vector<wxString> MatchedFiles;

	/**
	 * The mode that this instance of the background thread
	 * will run.
	 */
	Mode Mode;
};

}
#endif