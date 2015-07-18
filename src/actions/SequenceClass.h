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
#ifndef T4P_SEQUENCECLASS_H
#define T4P_SEQUENCECLASS_H

#include <globals/GlobalsClass.h>
#include <actions/GlobalActionClass.h>
#include <wx/event.h>
#include <queue>

namespace t4p {

/**
 * this event will be generated when a sequence has begun
 */
extern const wxEventType EVENT_SEQUENCE_START;

/**
 * this event will be generated while a sequence is running; it is 
 * generated consitantly every 200-300 ms.
 */
extern const wxEventType EVENT_SEQUENCE_PROGRESS;

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
	t4p::GlobalsClass& Globals;

	/**
	 * This will be used by the actions to create background threads
	 */
	t4p::RunningThreadsClass& RunningThreads;

	SequenceClass(t4p::GlobalsClass& globals, t4p::RunningThreadsClass& runningThreads);

	~SequenceClass();

	/**
	 * start the application start sequence. The project update sequence will do the following:
	 * - Detect the PHP framework that the project is using
	 * - Load the tag cache if it exists
	 * - Start the tag cache update for the project
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
	 * - Remove the project cache, urls, SQL table metadata for the removed projects
	 * - Detect the PHP framework that the project is using
	 * - Load the tag cache if it exists
	 * - Start the tag cache update for the project
	 * - Load the project's database connections
	 * - Detect the SQL table metadata for the project's database connections
	 *
	 * @param updateProjects the list of projects that were updated ie. new/removed/edited 
	 *       source directories, wildcards, etc...
	 * @param removedProjects the list of projects that were deleted we will delete the tags
	 *        from these projects
	 * @return bool FALSE if there is a sequence already running. if there is an existing
	 *         sequence running then we will not start another sequence as sequences deal with 
	 *         GlobalsClass and running many sequences may cause problems 
	 */
	bool ProjectDefinitionsUpdated(const std::vector<t4p::ProjectClass>& touchedProjects,
		const std::vector<t4p::ProjectClass>& removedProjects);

	/**
	 * Start the full tag cache rebuild sequence.  This will include
	 * - wiping all existing global caches from the given projects
	 * - indexing the given projects
	 * @return bool FALSE if there is a sequence already running. if there is an existing
	 *         sequence running then we will not start another sequence as sequences deal with 
	 *         GlobalsClass and running many sequences may cause problems 
	 */
	bool TagCacheWipeAndIndex(const std::vector<t4p::ProjectClass>& enabledProjects);

	/**
	 * Start the database detection sequence. This will include
	 * - redetect project's database connections
	 * - read metadata from the project's database connections
	 *
	 * This sequence is usually done when the user modifies a framework's
	 * database config file.
	 */
	bool DatabaseDetection();

	/**
	 * start running a sequence of arbritrary actions. actions will run 
	 * in the order that they are given. 
	 *
	 * @param actions the list of actions to run. action pointers will be owned by
	 *        this class.
	 * @return bool FALSE if there is a sequence already running.
	 */
	bool Build(std::vector<t4p::GlobalActionClass*> actions);

	/**
	 * @return bool TRUE if there is a sequence currently running.
	 */
	bool Running() const;

	/**
	 * stops a sequence that is currently running. Note that RunningThreads.StopAll()
	 * must have been called in order for this method to work.
	 */
	void Stop();

protected:

	/**
	 * @param step to be run.  Steps are run in the order they are given. This class
	 *        will own the pointer and will be deleted when the action completes.
	 *        This means that the action must have been allocated in the 
	 *        heap.
	 */
	void AddStep(t4p::GlobalActionClass* action);

	/**
	 * starts the sequence.
	 */
	void Run();

private:

	void OnActionComplete(t4p::ActionEventClass& event);

	void OnActionProgress(t4p::ActionProgressEventClass& event);

	/**
	 * start the next step in the sequence.
	 */
	void RunNextStep();

	/**
	 * will perform a check to make sure that all sources for all enabled projects
	 * exist and if not an error will be logged.  This check will be performed at the
	 * start of all sequences so that we print out at most 1 error for each
	 * missing source, and not 1 error for each missing source and each action.
	 */
	void SourceCheck();
	
	/**
	 * The steps in the current sequence.  
	 *
	 * These pointers are owned by this class, although that if an action
	 * is run as a background thread the pointer will delete itself.
	 */
	std::queue<t4p::GlobalActionClass*> Steps;

	/**
	 * Flag that tells whether the sequence has been started but is not yet complete
	 */
	bool IsRunning;

	/**
	 * Flag that tells if the currently running step is async. we need to
	 * know when a step is not async, because async steps delete themselves
	 * (because they use wxThread::run) while sync steps do not
	 */
	bool IsCurrentStepAsync;

	DECLARE_EVENT_TABLE()
};

class SequenceProgressEventClass : public t4p::ActionProgressEventClass {

public:

	SequenceProgressEventClass(int id, t4p::ActionClass::ProgressMode mode, int percentComplete, const wxString& msg);

	wxEvent* Clone() const;
};

typedef void (wxEvtHandler::*SequenceProgressEventClassFunction)(t4p::SequenceProgressEventClass&);

#define EVT_SEQUENCE_PROGRESS(fn) \
        DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_SEQUENCE_PROGRESS, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( SequenceProgressEventClassFunction, & fn ), (wxObject *) NULL ),


}

#endif
