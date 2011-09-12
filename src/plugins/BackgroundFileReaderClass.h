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
#ifndef __BACKGROUND_TASK_DRIVER_H__
#define __BACKGROUND_TASK_DRIVER_H__

#include <wx/string.h>
#include <wx/thread.h>
#include <wx/window.h>
#include <search/DirectorySearchClass.h>
#include <widgets/StatusBarWithGaugeClass.h>

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
const wxEventType EVENT_FILE_READ_COMPLETE = wxNewEventType();

/**
 * Dispatched each time a single file has been processed.
 * The event will have the following info:
 * event.getInt() will have the file counter; first file will be 1 then 2 and so on.
 * event.getClientData() will have a boolean value; true /false the value returned by FileRead() or FileMatched()
 */
const wxEventType EVENT_FILE_READ = wxNewEventType();

/**
 * This is a class that will glue together any background task with any "results" type panel.  Since
 * wxThreads cannot access GUI elements we need a way to separate any time-consuming tasks (such as 
 * searching through a project's files) with the updating of a GUI element (such as displaying the
 * found hits).  This class is the "glue" between a background task and GUI element: It will start
 * up a wxThread. Note: Objects of this class will handle at most 1 concurrently running thread at a time.
 */
class BackgroundFileReaderClass : public wxThreadHelper {

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
	 * Possible reasons why Starting a new thread may fail
	 *  NONE: no error, thread started successfully
	 *  NO_RESOURCES: not enough system resources to create the new thread
	 *  ALREADY_RUNNING: there is a previous thread that has not finished it work. This means that 
	 *                   objects of this class will handle at most 1 concurrently running thread at a time.
	 */
	enum StartError {
		NONE,
		NO_RESOURCES,
		ALREADY_RUNNING
	};
	
	/**
	 * @param wxEvtHandler* handler this event handler will receive
	 *        a EVENT_FILE_READ_COMPLETE event when the
	 *        background thread has completed.
	 */
	BackgroundFileReaderClass(wxEvtHandler* handler);

	/**
	 * Prepare the background thread to iterate through all
	 * files in the given directory.
	 * @return bool true if the given path is valid and exists
	 */
	bool Init(const wxString& path);

	/**
	 * prepares the thread to iterate over the last set of matched files
	 * In this case the FileMatch() method will be called.
	 * @return bool true if there are matching files from the previous find
	 * operation.
	 */
	bool InitMatched();

	/**
	 * Will create a new thread and run it. If an existing thread is already running,
	 * it will be destroyed. This means that objects of this class will handle at most 
	 * 1 concurrently running thread at a time.
	 * @return bool true if thread was created
	 */
	bool StartReading(StartError &error);
	
	/**
	 * Stop the file walking.
	 */
	void StopReading();

	/**
	 * returns TRUE if there is a background thread running.
	 */
	bool IsRunning() const;

protected: 

	/**
	 * Returns true if the background thread has been deleted
	 * and is no longer running.
	 */
	bool TestDestroy();

	/**
	 * This method will be executed in it's own thread. Most of the time
	 * this method would be implemented by calling search.Walk() method.
	 */
	virtual bool FileRead(DirectorySearchClass& search) = 0;

	/**
	 * This method will be executed in it's own thread. The method
	 * will be given a file that had a match (the files that
	 * has DirectoryWalker.Walk() method return TRUE).
	 */
	virtual bool FileMatch(const wxString& file) = 0;

	void* Entry();

private:

	/**
	 * Will get notified when the thread completes. This class will
	 * NOT own the pointer (it will not DELETE the pointer).
	 */
	wxEvtHandler* Handler;

	/**
	 * The object that will be used to traverse the file system.
	 */
	DirectorySearchClass DirectorySearch;

	/**
	 * The mode that this instance of the background thread
	 * will run.
	 */
	Mode Mode;
};

}
#endif