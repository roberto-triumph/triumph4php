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
#include <actions/SequenceClass.h>
#include <actions/ProjectFrameworkDetectionActionClass.h>
#include <actions/ProjectResourceActionClass.h>
#include <actions/SqlMetaDataActionClass.h>
#include <actions/CodeIgniterInitializerActionClass.h>
#include <actions/UrlResourceActionClass.h>
#include <actions/ResourceWipeActionClass.h>
#include <globals/Errors.h>

mvceditor::SequenceClass::SequenceClass(mvceditor::GlobalsClass& globals, mvceditor::RunningThreadsClass& runningThreads)
	: wxEvtHandler()
	, Globals(globals)
	, RunningThreads(runningThreads)
	, Steps()
	, IsRunning(false) 
	, IsCurrentStepAsync(false) {
	RunningThreads.AddEventHandler(this);
}

mvceditor::SequenceClass::~SequenceClass() {
	RunningThreads.RemoveEventHandler(this);
	Stop();
}

void mvceditor::SequenceClass::Stop() {
	if (!Steps.empty()) {

		// remove the step that just finished
		// note that async steps automatically delete themselves since they
		// are run using wxThread::run so we take care to not double-delete those
		if (!IsCurrentStepAsync) {
			delete Steps.front();
		}
		Steps.pop();
	}

	while (!Steps.empty()) {

		// the remaining steps will not delete themselves since they
		// will never be run.
		// do this so that mem checkers don't complain
		delete Steps.front();
		Steps.pop();
	}
	IsRunning = false;
}

bool mvceditor::SequenceClass::AppStart() {
	if (Running()) {
		return false;
	}

	// this will check to see if any source directories use PHP frameworks
	// we need to do this before all others
	AddStep(new mvceditor::ProjectFrameworkDetectionActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_FRAMEWORK_DETECTION));

	// this will load the cache from the hard disk
	// load the cache from hard disk so that code completion and 
	// resource searching is available immediately after the app starts
	AddStep(new mvceditor::ResourceCacheInitActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_INIT));

	// this will update the resource cache by parsing newly modified files
	AddStep(new mvceditor::ProjectResourceActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE));

	// this will prime the sql connections
	AddStep(new mvceditor::SqlMetaDataInitActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_SQL_METADATA_INIT));
	
	// this will discover the db schema info (tables, columns)
	AddStep(new mvceditor::SqlMetaDataActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_SQL_METADATA));

	// this will initialize the code igniter specific features if code igniter was
	// detected
	AddStep(new mvceditor::CodeIgniterInitializerActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_CODE_IGNITER_DETECTED));

	// after frameworks have been detected AND the project resources have been parsed we
	// can look for URLs.  we need the project to be parsed so that we know all of the 
	// project's classes.
	AddStep(new mvceditor::UrlResourceActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_URL_RESOURCES));

	Run();
	return true;
}

bool mvceditor::SequenceClass::ProjectDefinitionsUpdated(const std::vector<mvceditor::ProjectClass>& touchedProjects) {
	if (Running()) {
		return false;
	}

	// this will check to see if any source directories use PHP frameworks
	// we need to do this before all others
	AddStep(new mvceditor::ProjectFrameworkDetectionActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_FRAMEWORK_DETECTION));

	// this step will wipe the global cache; this is needed in case a project has
	// new exclude wilcards or a source directory has been removed.  the ProjectResourceActionClass
	// will just recurse directories and update the cache, it does not recurse the index
	// hence it won't remove items that don't need to be there
	AddStep(new mvceditor::ResourceWipeActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_WIPE, touchedProjects));

	// this will load the cache from the hard disk
	// load the cache from hard disk so that code completion and 
	// resource searching is available immediately after the app starts
	AddStep(new mvceditor::ResourceCacheInitActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_INIT));

	// this will update the resource cache by parsing newly modified files
	mvceditor::ProjectResourceActionClass* action = 
		new mvceditor::ProjectResourceActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE);
	action->SetTouchedProjects(touchedProjects);
	AddStep(action);

	// this will prime the sql connections
	AddStep(new mvceditor::SqlMetaDataInitActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_SQL_METADATA_INIT));
	
	// this will discover the db schema info (tables, columns)
	AddStep(new mvceditor::SqlMetaDataActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_SQL_METADATA));

	// this will initialize the code igniter specific features if code igniter was
	// detected
	AddStep(new mvceditor::CodeIgniterInitializerActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_CODE_IGNITER_DETECTED));

	// after frameworks have been detected AND the project resources have been parsed we
	// can look for URLs.  we need the project to be parsed so that we know all of the 
	// project's classes.
	AddStep(new mvceditor::UrlResourceActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_URL_RESOURCES));

	Run();
	return true;
}

bool mvceditor::SequenceClass::ResourceCacheWipeAndIndex() {
	if (Running()) {
		return false;
	}

	// this step will wipe the global cache from all projects
	AddStep(new mvceditor::ResourceWipeActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_WIPE, Globals.Projects));

	// this will recurse though all directories and parse the source code
	AddStep(new mvceditor::ProjectResourceActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE));

	// after project resources have been parsed we
	// can look for URLs.  we need the project to be parsed so that we know all of the 
	// project's classes (controllers).
	AddStep(new mvceditor::UrlResourceActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_URL_RESOURCES));

	Run();
	return true;
}

void mvceditor::SequenceClass::AddStep(mvceditor::ActionClass* step) {
	Steps.push(step);
}

void mvceditor::SequenceClass::Run() {
	wxCommandEvent sequenceEvent(mvceditor::EVENT_SEQUENCE_START);
	RunningThreads.PostEvent(sequenceEvent);

	IsRunning = false;
	RunNextStep();
}

void mvceditor::SequenceClass::OnActionComplete(wxCommandEvent& event) {
	if (!Steps.empty()) {

		// remove the step that just finished
		// note that async steps automatically delete themselves since they
		// are run using wxThread::run so we take care to not double-delete those
		if (!IsCurrentStepAsync) {
			delete Steps.front();
		}
		Steps.pop();
	}

	// if there are more steps to run, start the next one
	IsRunning = !Steps.empty();
	if (IsRunning) {
		RunNextStep();	
	}
	else {
		wxCommandEvent sequenceEvent(mvceditor::EVENT_SEQUENCE_COMPLETE);
		RunningThreads.PostEvent(sequenceEvent);
	}
}

void mvceditor::SequenceClass::RunNextStep() {
	if (Steps.empty()) {
		return;
	}
	IsRunning = false;
	IsCurrentStepAsync = false;
	while (!Steps.empty()) {
		if (!Steps.front()->DoAsync()) {

			// if the step is not asynchronous, it means that it does not
			// need us to start a new thread
			Steps.front()->Init(Globals);
			IsCurrentStepAsync = false;

			// step does not involve async; we are done with this step
			// even though we dont start a background thread, the 
			// sync actions still generate EVT_WORK_COMPLETE events. let's
			// wait for it.
			break;
		}
		else {

			// if the step is to be performed in a background thread, then check the
			// return value of the init method, that way we dont start a new thread
			// if there is no work to be done
			bool isInit = Steps.front()->Init(Globals);
			wxThreadError err = wxTHREAD_NO_ERROR;
			wxThreadIdType runningThreadId;
			if (isInit) {
				err = Steps.front()->CreateSingleInstance(runningThreadId);
			}
			if (wxTHREAD_NO_RESOURCE == err) {

				// if we cannot start a thread due to low resoures, then other steps will
				// probably fail too. just end the sequence
				while (!Steps.empty()) {
					delete Steps.front();
					Steps.pop();
				}
				mvceditor::EditorLogError(mvceditor::WARNING_OTHER, _("The system is low on resources. Close some programs and try again."));
			}
			else if (isInit && wxTHREAD_NO_ERROR == err) {

				// everything went OK. wait for the background thread to finish
				IsRunning = true;
				IsCurrentStepAsync = true;
				break;
			}
			else {

				// no need to start a background thread or a misc error starting up a thread.
				// lets move on to the next step
				delete Steps.front();
				Steps.pop();
			}
		}
	}
}

wxString mvceditor::SequenceClass::GetStatus() {
	wxString status;
	if (!Steps.empty()) {
		status = Steps.front()->GetStatus();
	}
	return status;
}

bool mvceditor::SequenceClass::Running() const {
	return IsRunning;
}

void mvceditor::SequenceClass::OnActionInProgress(wxCommandEvent &event) {
	wxCommandEvent sequenceEvent(mvceditor::EVENT_SEQUENCE_IN_PROGRESS);
	RunningThreads.PostEvent(sequenceEvent);
}

const wxEventType mvceditor::EVENT_SEQUENCE_START = wxNewEventType();
const wxEventType mvceditor::EVENT_SEQUENCE_IN_PROGRESS = wxNewEventType();
const wxEventType mvceditor::EVENT_SEQUENCE_COMPLETE = wxNewEventType();

BEGIN_EVENT_TABLE(mvceditor::SequenceClass, wxEvtHandler)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_FRAMEWORK_DETECTION, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_INIT, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_SQL_METADATA_INIT, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_SQL_METADATA, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_CODE_IGNITER_DETECTED, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_URL_RESOURCES, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_WIPE, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)

	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_FRAMEWORK_DETECTION, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_INIT, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_SQL_METADATA_INIT, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_SQL_METADATA, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_CODE_IGNITER_DETECTED, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_URL_RESOURCES, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_WIPE, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
END_EVENT_TABLE()
