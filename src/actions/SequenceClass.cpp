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
#include <actions/ProjectTagActionClass.h>
#include <actions/SqlMetaDataActionClass.h>
#include <actions/TagWipeActionClass.h>
#include <actions/UrlTagDetectorActionClass.h>
#include <actions/TagDetectorActionClass.h>
#include <actions/DatabaseTagDetectorActionClass.h>
#include <actions/CacheDbVersionActionClass.h>
#include <actions/ConfigTagDetectorActionClass.h>
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
	SourceCheck();

	// before we do anything else, make sure that the cache files are the same version as the
	// code expects them to be
	AddStep(new mvceditor::TagCacheDbVersionActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_TAG_CACHE_VERSION_CHECK));
	AddStep(new mvceditor::DetectorCacheDbVersionActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_DETECTOR_CACHE_VERSION_CHECK));

	// this will load the cache from the hard disk
	// load the cache from hard disk so that code completion and 
	// tag searching is available immediately after the app starts
	AddStep(new mvceditor::ProjectTagInitActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_INIT));

	// this will load the url entry point cache
	// do this before the rest so the urls become available asap
	AddStep(new mvceditor::UrlTagFinderInitActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_URL_TAG_DETECTOR_INIT));

	// this will prime the sql connections from the php detectors
	AddStep(new mvceditor::DatabaseTagDetectorInitActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR_INIT));

	// this will prime the detected tags cache
	AddStep(new mvceditor::TagDetectorInitActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_TAG_DETECTOR_INIT));

	// this will detect all of the config files for projects
	AddStep(new mvceditor::ConfigTagDetectorActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_CONFIG_TAG_DETECTOR));

	// this will attempt to detect new sql connections from the php detectors
	// this can go here because it does not need the tag cache to be up-to-date
	AddStep(new mvceditor::DatabaseTagDetectorActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR));

	// this will update the tag cache by parsing newly modified files
	AddStep(new mvceditor::ProjectTagActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE));

	// this will discover the db schema info (tables, columns)
	AddStep(new mvceditor::SqlMetaDataActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_SQL_METADATA));

	// this will detect the urls (entry points) that a project has
	AddStep(new mvceditor::UrlTagDetectorActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_URL_TAG_DETECTOR));

	// this will discover any new detected tags 
	AddStep(new mvceditor::TagDetectorActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_TAG_DETECTOR));
	
	Run();
	return true;
}

bool mvceditor::SequenceClass::ProjectDefinitionsUpdated(const std::vector<mvceditor::ProjectClass>& touchedProjects) {
	if (Running()) {
		return false;
	}
	SourceCheck();

	// before we do anything else, make sure that the cache files are the same version as the
	// code expects them to be
	// we need to do this here too because the version checker only checks active projects
	// and an old project might be enabled
	AddStep(new mvceditor::TagCacheDbVersionActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_TAG_CACHE_VERSION_CHECK));
	AddStep(new mvceditor::DetectorCacheDbVersionActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_DETECTOR_CACHE_VERSION_CHECK));

	// this step will wipe the global cache; this is needed in case a project has
	// new exclude wilcards or a source directory has been removed.  the ProjectTagActionClass
	// will just recurse directories and update the cache, it does not recurse the index
	// hence it won't remove items that don't need to be there
	// don't wipe, just remove tags from touchedProjects
	std::vector<wxFileName> dirsToDelete;
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	std::vector<mvceditor::SourceClass>::const_iterator source;
	for (project = touchedProjects.begin(); project != touchedProjects.end(); ++project) {
		for (source = project->Sources.begin(); source != project->Sources.end(); ++source) {
			dirsToDelete.push_back(source->RootDirectory);
		}
	}
	AddStep(new mvceditor::TagDeleteActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_WIPE, dirsToDelete));

	// this will load the cache from the hard disk
	// load the cache from hard disk so that code completion and 
	// tag searching is available immediately after the app starts
	AddStep(new mvceditor::ProjectTagInitActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_INIT));

	// this will load the url entry point cache
	// do this before the rest so the urls become available asap
	AddStep(new mvceditor::UrlTagFinderInitActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_URL_TAG_DETECTOR_INIT));

	// this will prime the detected tags cache
	AddStep(new mvceditor::TagDetectorInitActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_TAG_DETECTOR_INIT));

	// this will prime the sql connections from the php detectors
	AddStep(new mvceditor::DatabaseTagDetectorInitActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR_INIT));

	// this will detect all of the config files for projects
	AddStep(new mvceditor::ConfigTagDetectorActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_CONFIG_TAG_DETECTOR));

	// this will attempt to detect new sql connections from the php detectors
	// this can go here because it does not need the tag cache to be up-to-date
	AddStep(new mvceditor::DatabaseTagDetectorActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR));

	// this will update the tag cache by parsing newly modified files
	mvceditor::ProjectTagActionClass* action = 
		new mvceditor::ProjectTagActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE);
	action->SetTouchedProjects(touchedProjects);
	AddStep(action);
	
	// this will discover the db schema info (tables, columns)
	AddStep(new mvceditor::SqlMetaDataActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_SQL_METADATA));

	// this will detect the urls (entry points) that a project has
	AddStep(new mvceditor::UrlTagDetectorActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_URL_TAG_DETECTOR));

	// this will discover any new detected tags 
	AddStep(new mvceditor::TagDetectorActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_TAG_DETECTOR));

	Run();
	return true;
}

bool mvceditor::SequenceClass::TagCacheWipeAndIndex() {
	if (Running()) {
		return false;
	}
	SourceCheck();

	// this step will wipe the global cache from all projects
	AddStep(new mvceditor::TagWipeActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_WIPE));

	// this will recurse though all directories and parse the source code
	AddStep(new mvceditor::ProjectTagActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE));

	// this will detect the urls (entry points) that a project has
	AddStep(new mvceditor::UrlTagDetectorActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_URL_TAG_DETECTOR));

	// this will discover any new detected tags 
	AddStep(new mvceditor::TagDetectorActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_TAG_DETECTOR));

	Run();
	return true;
}

bool mvceditor::SequenceClass::Build(std::vector<mvceditor::ActionClass*> actions) {
	if (Running()) {
		return false;
	}
	SourceCheck();
	for (size_t i = 0; i < actions.size(); i++) {
		AddStep(actions[i]);
	}
	Run();
	return true;
}

bool mvceditor::SequenceClass::DatabaseDetection() {
	if (Running()) {
		return false;
	}

	// this will attempt to detect new sql connections from the php detectors
	AddStep(new mvceditor::DatabaseTagDetectorActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR));

	// this will prime the sql connections from the php detectors
	// this is being done here so that we can guarantee that SqlMetaData has the
	// most up-to-date database tags. The GlobalsChangeHandlerClass also reads
	// the database tags from the detector db, but since it also works on events
	// we cannot gurantee that the GlobalsChangeHandlerClass EVENT_WORK_COMPLETE handler will get called
	// before the SequenceClass EVENT_WORK_COMPLETE handler.
	AddStep(new mvceditor::DatabaseTagDetectorInitActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR_INIT));

	// this will discover the db schema info (tables, columns)
	AddStep(new mvceditor::SqlMetaDataActionClass(RunningThreads, mvceditor::ID_EVENT_ACTION_SQL_METADATA));

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
		sequenceEvent.SetId(wxID_ANY);
		RunningThreads.PostEvent(sequenceEvent);
	}
}

void mvceditor::SequenceClass::RunNextStep() {
	if (Steps.empty()) {
		return;
	}
	IsRunning = false;
	IsCurrentStepAsync = false;
	bool isSequenceDone = false;
	while (!Steps.empty()) {
		bool isInit = false;
		if (!Steps.front()->DoAsync()) {

			// if the step is not asynchronous, it means that it does not
			// need us to start a new thread
			isInit = Steps.front()->Init(Globals);
			IsCurrentStepAsync = false;

			if (!isInit) {

				// if the step is not initialized, move on to the next step.
				// we need to delete the step since OnActionComplete will not
				// get called for this action
				delete Steps.front();
				Steps.pop();
				if (Steps.empty()) {
					isSequenceDone = true;
				}
			}
			else {
				// step does not involve async; we are done with this step
				// even though we dont start a background thread, the 
				// sync actions still generate EVT_WORK_COMPLETE events. let's
				// wait for it.
				break;
			}
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
				isSequenceDone = true;				
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
				if (Steps.empty()) {
					isSequenceDone = true;
				}
			}
		}
	}
	if (isSequenceDone) {
		
		// this can happen when the last step could not be 
		// initialized; then no EVT_WORK_COMPLETE will be generated
		wxCommandEvent sequenceEvent(mvceditor::EVENT_SEQUENCE_COMPLETE);
		sequenceEvent.SetId(wxID_ANY);
		RunningThreads.PostEvent(sequenceEvent);
	}
}

bool mvceditor::SequenceClass::Running() const {
	return IsRunning;
}

void mvceditor::SequenceClass::OnActionInProgress(wxCommandEvent &event) {
	wxCommandEvent sequenceEvent(mvceditor::EVENT_SEQUENCE_IN_PROGRESS);
	sequenceEvent.SetId(wxID_ANY);
	RunningThreads.PostEvent(sequenceEvent);
}

void mvceditor::SequenceClass::SourceCheck() {
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	std::vector<mvceditor::SourceClass>::const_iterator source;
	for (project = Globals.Projects.begin(); project != Globals.Projects.end(); ++ project) {
		if (project->IsEnabled) {
			for (source = project->Sources.begin(); source != project->Sources.end(); ++source) {
				if (!source->Exists()) {
					mvceditor::EditorLogError(
						mvceditor::ERR_INVALID_DIRECTORY, source->RootDirectory.GetPath()
					);
				}
			}
		}
	}
}

const wxEventType mvceditor::EVENT_SEQUENCE_START = wxNewEventType();
const wxEventType mvceditor::EVENT_SEQUENCE_IN_PROGRESS = wxNewEventType();
const wxEventType mvceditor::EVENT_SEQUENCE_COMPLETE = wxNewEventType();

BEGIN_EVENT_TABLE(mvceditor::SequenceClass, wxEvtHandler)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_INIT, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_SQL_METADATA_INIT, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_SQL_METADATA, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_URL_TAG_DETECTOR_INIT, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_URL_TAG_DETECTOR, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_TAG_DETECTOR_INIT, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_TAG_DETECTOR, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_WIPE, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_CALL_STACK, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_TEMPLATE_FILE_TAG_DETECTOR, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR_INIT, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_CONFIG_TAG_DETECTOR, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_TAG_CACHE_VERSION_CHECK, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_DETECTOR_CACHE_VERSION_CHECK, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SequenceClass::OnActionComplete)

	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_INIT, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_SQL_METADATA_INIT, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_SQL_METADATA, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_URL_TAG_DETECTOR_INIT, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_URL_TAG_DETECTOR, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_TAG_DETECTOR_INIT, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_TAG_DETECTOR, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_WIPE, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_CALL_STACK, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_TEMPLATE_FILE_TAG_DETECTOR, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR_INIT, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_CONFIG_TAG_DETECTOR, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_TAG_CACHE_VERSION_CHECK, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_DETECTOR_CACHE_VERSION_CHECK, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SequenceClass::OnActionInProgress)
END_EVENT_TABLE()
