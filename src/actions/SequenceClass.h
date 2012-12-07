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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __MVCEDITORSEQUENCECLASS_H__
#define __MVCEDITORSEQUENCECLASS_H__

#include <globals/GlobalsClass.h>
#include <widgets/ThreadWithHeartbeatClass.h>
#include <actions/ActionClass.h>
#include <wx/event.h>
#include <queue>

namespace mvceditor {

/**
 * this event will be generated when a sequence has begun
 */
extern const wxEventType EVENT_SEQUENCE_START;

/**
 * this event will be generated while a sequence is running
 */
extern const wxEventType EVENT_SEQUENCE_IN_PROGRESS;

/**
 * this event will be generated when the AppStart sequence is complete
 */
extern const wxEventType EVENT_SEQUENCE_COMPLETE;

/**
 * This class runs all of the added steps in sequence, one
 * after another.
 */
class SequenceClass : public wxEvtHandler {

public:

	/**
	 * The global data structures; each action will read these as their
	 * input
	 */
	mvceditor::GlobalsClass& Globals;

	/**
	 * This will be used by the actions to create background threads
	 */
	mvceditor::RunningThreadsClass& RunningThreads;

	SequenceClass(mvceditor::GlobalsClass& globals, mvceditor::RunningThreadsClass& runningThreads);

	~SequenceClass();

	/**
	 * start the application start sequence. The project update sequence will do the following:
	 * - Detect the PHP framework that the project is using
	 * - Load the resource cache if it exists
	 * - Start the resource cache update for the project
	 * - Load the project's database connections
	 * - Detect the SQL table metadata for the project's database connections
	 *
	 * @return bool FALSE if there is a sequence already running. if there is an existing
	 *         sequence running then we will not start another sequence as sequences deal with 
	 *         GlobalsClass and running many sequences may cause problems 
	 */
	bool AppStart();

	/**
	 * start the project refresh sequence.  This sequence should be run when the user
	 * has added / removed a project, or edited a project's sources list. This sequence
	 * will do the following, but only on the given projects, as opposed to all projects
	 * like AppStart sequence.
	 * - Remove the project cache, urls, SQL table metada for the removed projects
	 * - Detect the PHP framework that the project is using
	 * - Load the resource cache if it exists
	 * - Start the resource cache update for the project
	 * - Load the project's database connections
	 * - Detect the SQL table metadata for the project's database connections
	 *
	 * @param updateProjects the list of projects that were updated ie. new/removed/edited 
	 *       source directories, wildcards, etc...
	 * @return bool FALSE if there is a sequence already running. if there is an existing
	 *         sequence running then we will not start another sequence as sequences deal with 
	 *         GlobalsClass and running many sequences may cause problems 
	 */
	bool ProjectDefinitionsUpdated(const std::vector<mvceditor::ProjectClass>& touchedProjects);

	/**
	 * Start the full resource cache rebuild sequence.  This will include
	 * - wiping all existing global caches from all projects
	 * - indexing all enabled projects
	 * @return bool FALSE if there is a sequence already running. if there is an existing
	 *         sequence running then we will not start another sequence as sequences deal with 
	 *         GlobalsClass and running many sequences may cause problems 
	 */
	bool ResourceCacheWipeAndIndex();

	/**
	 * @return wxString a short description of which step is currently being run
	 */
	wxString GetStatus();

	/**
	 * @return bool TRUE if there is a sequence currently running.
	 */
	bool Running() const;

protected:

	/**
	 * @param step to be run.  Steps are run in the order they are given. This class
	 *        will own the pointer and will be deleted when the action completes.
	 *        This means that the action must have been allocated in the 
	 *        heap.
	 */
	void AddStep(mvceditor::ActionClass* action);

	/**
	 * starts the sequence.
	 */
	void Run();

private:

	void OnActionComplete(wxCommandEvent& event);

	void OnActionInProgress(wxCommandEvent& event);

	/**
	 * start the next step in the sequence.
	 */
	void RunNextStep();
	
	/**
	 * The steps in the current sequence.  
	 *
	 * These pointers are owned by this class, although that if an action
	 * is run as a background thread the pointer will delete itself.
	 */
	std::queue<mvceditor::ActionClass*> Steps;

	/**
	 * Flag that tells whether the sequence has been started but is not yet complete
	 */
	bool IsRunning;

	DECLARE_EVENT_TABLE()
};

}

#endif