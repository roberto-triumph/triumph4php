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
#include <actions/DetectorDbInitActionClass.h>
#include <globals/Errors.h>

t4p::SequenceClass::SequenceClass(t4p::GlobalsClass& globals, t4p::RunningThreadsClass& runningThreads)
	: wxEvtHandler()
	, Globals(globals)
	, RunningThreads(runningThreads)
	, Steps()
	, IsRunning(false)
	, IsCurrentStepAsync(false) {
	RunningThreads.AddEventHandler(this);
}

t4p::SequenceClass::~SequenceClass() {
	RunningThreads.RemoveEventHandler(this);
	Stop();
}

void t4p::SequenceClass::Stop() {
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
	wxCommandEvent sequenceEvent(t4p::EVENT_SEQUENCE_COMPLETE);
	sequenceEvent.SetId(wxID_ANY);
	RunningThreads.PostEvent(sequenceEvent);
}

bool t4p::SequenceClass::AppStart() {
	if (Running()) {
		return false;
	}
	SourceCheck();

	// before we do anything else, make sure that the cache files are the same version as the
	// code expects them to be
	AddStep(new t4p::TagCacheDbVersionActionClass(RunningThreads, t4p::ID_EVENT_ACTION_TAG_CACHE_VERSION_CHECK));
	AddStep(new t4p::DetectorCacheDbVersionActionClass(RunningThreads, t4p::ID_EVENT_ACTION_DETECTOR_CACHE_VERSION_CHECK));

	// open the detector tags db file
	AddStep(new t4p::DetectorDbInitActionClass(RunningThreads, t4p::ID_EVENT_ACTION_DETECTOR_DB_INIT));

	// this will load the cache from the hard disk
	// load the cache from hard disk so that code completion and
	// tag searching is available immediately after the app starts
	AddStep(new t4p::ProjectTagInitActionClass(RunningThreads, t4p::ID_EVENT_ACTION_TAG_FINDER_LIST_INIT));

	// this will load the discovered the db schema info (tables, columns)
	AddStep(new t4p::SqlMetaDataInitActionClass(RunningThreads, t4p::ID_EVENT_ACTION_SQL_METADATA));

	Run();
	return true;
}

bool t4p::SequenceClass::ProjectDefinitionsUpdated(const std::vector<t4p::ProjectClass>& touchedProjects,
	const std::vector<t4p::ProjectClass>& removedProjects) {
	if (Running()) {
		return false;
	}

	// in case newly enabled projects are no longer in the file system
	SourceCheck();

	// this step will wipe the global cache; this is needed in case a project has
	// new exclude wilcards or a source directory has been removed.  the ProjectTagActionClass
	// will just recurse directories and update the cache, it does not recurse the index
	// hence it won't remove items that don't need to be there
	// don't wipe, just remove tags from touchedProjects
	std::vector<wxFileName> sourceDirsToDelete;
	std::vector<t4p::ProjectClass>::const_iterator project;
	std::vector<t4p::SourceClass>::const_iterator source;
	for (project = touchedProjects.begin(); project != touchedProjects.end(); ++project) {
		for (source = project->Sources.begin(); source != project->Sources.end(); ++source) {
			sourceDirsToDelete.push_back(source->RootDirectory);
		}
	}

	// remove tags from the deleted projects since we will never use them
	for (project = removedProjects.begin(); project != removedProjects.end(); ++project) {
		for (source = project->Sources.begin(); source != project->Sources.end(); ++source) {
			sourceDirsToDelete.push_back(source->RootDirectory);
		}
	}
	AddStep(new t4p::TagDeleteSourceActionClass(RunningThreads, t4p::ID_EVENT_ACTION_TAG_FINDER_LIST_WIPE, sourceDirsToDelete));

	// this will detect all of the config files for projects
	AddStep(new t4p::ConfigTagDetectorActionClass(RunningThreads, t4p::ID_EVENT_ACTION_CONFIG_TAG_DETECTOR));

	// this will attempt to detect new sql connections from the php detectors
	// this can go here because it does not need the tag cache to be up-to-date
	AddStep(new t4p::DatabaseTagDetectorActionClass(RunningThreads, t4p::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR));

	// this will update the tag cache by parsing newly modified files
	t4p::ProjectTagActionClass* action =
		new t4p::ProjectTagActionClass(RunningThreads, t4p::ID_EVENT_ACTION_TAG_FINDER_LIST);
	action->SetTouchedProjects(touchedProjects);
	AddStep(action);

	// this will discover the db schema info (tables, columns)
	AddStep(new t4p::SqlMetaDataActionClass(RunningThreads, t4p::ID_EVENT_ACTION_SQL_METADATA));

	// this will detect the urls (entry points) that a project has
	AddStep(new t4p::UrlTagDetectorActionClass(RunningThreads, t4p::ID_EVENT_ACTION_URL_TAG_DETECTOR));

	// this will discover any new detected tags
	AddStep(new t4p::TagDetectorActionClass(RunningThreads, t4p::ID_EVENT_ACTION_TAG_DETECTOR));

	Run();
	return true;
}

bool t4p::SequenceClass::TagCacheWipeAndIndex(const std::vector<t4p::ProjectClass>& enabledProjects) {
	if (Running()) {
		return false;
	}
	SourceCheck();

	// do not wipe the entire db, for now, just delete the enabled projects
	// that way, if a user re-enables a project, the existing tags can be used
	std::vector<t4p::ProjectClass>::const_iterator project;
	std::vector<t4p::SourceClass>::const_iterator source;
	std::vector<wxFileName> sourceDirsToDelete;
	for (project = enabledProjects.begin(); project != enabledProjects.end(); ++project) {
		for (source = project->Sources.begin(); source != project->Sources.end(); ++source) {
			sourceDirsToDelete.push_back(source->RootDirectory);
		}
	}

	AddStep(new t4p::TagDeleteSourceActionClass(RunningThreads, t4p::ID_EVENT_ACTION_TAG_FINDER_LIST_WIPE, sourceDirsToDelete));

	// this will detect all of the config files for projects
	AddStep(new t4p::ConfigTagDetectorActionClass(RunningThreads, t4p::ID_EVENT_ACTION_CONFIG_TAG_DETECTOR));

	// this will attempt to detect new sql connections from the php detectors
	// this can go here because it does not need the tag cache to be up-to-date
	AddStep(new t4p::DatabaseTagDetectorActionClass(RunningThreads, t4p::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR));

	// this will recurse though all directories and parse the source code
	AddStep(new t4p::ProjectTagActionClass(RunningThreads, t4p::ID_EVENT_ACTION_TAG_FINDER_LIST));

	// this will detect the urls (entry points) that a project has
	AddStep(new t4p::UrlTagDetectorActionClass(RunningThreads, t4p::ID_EVENT_ACTION_URL_TAG_DETECTOR));

	// this will discover any new detected tags
	AddStep(new t4p::TagDetectorActionClass(RunningThreads, t4p::ID_EVENT_ACTION_TAG_DETECTOR));

	// this will discover the db schema info (tables, columns)
	AddStep(new t4p::SqlMetaDataActionClass(RunningThreads, t4p::ID_EVENT_ACTION_SQL_METADATA));

	Run();
	return true;
}

bool t4p::SequenceClass::Build(std::vector<t4p::GlobalActionClass*> actions) {
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

bool t4p::SequenceClass::DatabaseDetection() {
	if (Running()) {
		return false;
	}

	// this will attempt to detect new sql connections from the php detectors
	AddStep(new t4p::DatabaseTagDetectorActionClass(RunningThreads, t4p::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR));

	// this will discover the db schema info (tables, columns)
	AddStep(new t4p::SqlMetaDataActionClass(RunningThreads, t4p::ID_EVENT_ACTION_SQL_METADATA));

	Run();
	return true;
}

void t4p::SequenceClass::AddStep(t4p::GlobalActionClass* step) {
	Steps.push(step);
}

void t4p::SequenceClass::Run() {
	wxCommandEvent sequenceEvent(t4p::EVENT_SEQUENCE_START);
	RunningThreads.PostEvent(sequenceEvent);

	IsRunning = false;
	RunNextStep();
}

void t4p::SequenceClass::OnActionComplete(t4p::ActionEventClass& event) {
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
		wxCommandEvent sequenceEvent(t4p::EVENT_SEQUENCE_COMPLETE);
		sequenceEvent.SetId(wxID_ANY);
		RunningThreads.PostEvent(sequenceEvent);
	}
}

void t4p::SequenceClass::RunNextStep() {
	if (Steps.empty()) {
		return;
	}
	IsRunning = false;
	IsCurrentStepAsync = false;
	bool isSequenceDone = false;
	while (!Steps.empty()) {
		bool isInit = false;
		if (!Steps.front()->DoAsync()) {
			IsCurrentStepAsync = false;

			// if the step is not asynchronous, it means that it does not
			// need us to start a new thread
			isInit = Steps.front()->Init(Globals);
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
				IsRunning = true;
				break;
			}
		}
		else {
			IsCurrentStepAsync = true;

			// if the step is to be performed in a background thread, then check the
			// return value of the init method, that way we dont start a new thread
			// if there is no work to be done
			isInit = Steps.front()->Init(Globals);
			if (isInit) {
				t4p::ActionClass* action = Steps.front();
				RunningThreads.Queue(action);

				// now wait for the background thread to finish
				IsRunning = true;
				break;
			}
			else {

				// no need to start a background thread since Init failed
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
		wxCommandEvent sequenceEvent(t4p::EVENT_SEQUENCE_COMPLETE);
		sequenceEvent.SetId(wxID_ANY);
		RunningThreads.PostEvent(sequenceEvent);
	}
}

bool t4p::SequenceClass::Running() const {
	return IsRunning;
}

void t4p::SequenceClass::OnActionProgress(t4p::ActionProgressEventClass& event) {

	// if there is an action that is running then send an in-progress event
	// for it
	t4p::SequenceProgressEventClass sequenceEvt(wxID_ANY, event.Mode, event.PercentComplete, event.Message);
	RunningThreads.PostEvent(sequenceEvt);
}

void t4p::SequenceClass::SourceCheck() {
	std::vector<t4p::ProjectClass>::const_iterator project;
	std::vector<t4p::SourceClass>::const_iterator source;
	for (project = Globals.Projects.begin(); project != Globals.Projects.end(); ++project) {
		if (project->IsEnabled) {
			for (source = project->Sources.begin(); source != project->Sources.end(); ++source) {
				if (!source->Exists()) {
					t4p::EditorLogError(
						t4p::ERR_INVALID_DIRECTORY, source->RootDirectory.GetPath());
				}
			}
		}
	}
}

t4p::SequenceProgressEventClass::SequenceProgressEventClass(int id, t4p::ActionClass::ProgressMode mode, int percentComplete, const wxString& msg)
: ActionProgressEventClass(id, mode, percentComplete, msg) {
	SetEventType(t4p::EVENT_SEQUENCE_PROGRESS);
}

wxEvent* t4p::SequenceProgressEventClass::Clone() const {
	return new t4p::SequenceProgressEventClass(GetId(), Mode, PercentComplete, Message);
}

const wxEventType t4p::EVENT_SEQUENCE_START = wxNewEventType();
const wxEventType t4p::EVENT_SEQUENCE_PROGRESS = wxNewEventType();
const wxEventType t4p::EVENT_SEQUENCE_COMPLETE = wxNewEventType();

BEGIN_EVENT_TABLE(t4p::SequenceClass, wxEvtHandler)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_TAG_FINDER_LIST_INIT, t4p::SequenceClass::OnActionComplete)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_TAG_FINDER_LIST, t4p::SequenceClass::OnActionComplete)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_SQL_METADATA_INIT, t4p::SequenceClass::OnActionComplete)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_SQL_METADATA, t4p::SequenceClass::OnActionComplete)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_URL_TAG_DETECTOR, t4p::SequenceClass::OnActionComplete)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_TAG_DETECTOR_INIT, t4p::SequenceClass::OnActionComplete)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_TAG_DETECTOR, t4p::SequenceClass::OnActionComplete)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_TAG_FINDER_LIST_WIPE, t4p::SequenceClass::OnActionComplete)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_CALL_STACK, t4p::SequenceClass::OnActionComplete)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_TEMPLATE_FILE_TAG_DETECTOR, t4p::SequenceClass::OnActionComplete)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR, t4p::SequenceClass::OnActionComplete)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_CONFIG_TAG_DETECTOR, t4p::SequenceClass::OnActionComplete)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_TAG_CACHE_VERSION_CHECK, t4p::SequenceClass::OnActionComplete)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_DETECTOR_CACHE_VERSION_CHECK, t4p::SequenceClass::OnActionComplete)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_DETECTOR_DB_INIT, t4p::SequenceClass::OnActionComplete)

	EVT_ACTION_PROGRESS(t4p::ID_EVENT_ACTION_TAG_FINDER_LIST_INIT, t4p::SequenceClass::OnActionProgress)
	EVT_ACTION_PROGRESS(t4p::ID_EVENT_ACTION_TAG_FINDER_LIST, t4p::SequenceClass::OnActionProgress)
	EVT_ACTION_PROGRESS(t4p::ID_EVENT_ACTION_SQL_METADATA_INIT, t4p::SequenceClass::OnActionProgress)
	EVT_ACTION_PROGRESS(t4p::ID_EVENT_ACTION_SQL_METADATA, t4p::SequenceClass::OnActionProgress)
	EVT_ACTION_PROGRESS(t4p::ID_EVENT_ACTION_URL_TAG_DETECTOR, t4p::SequenceClass::OnActionProgress)
	EVT_ACTION_PROGRESS(t4p::ID_EVENT_ACTION_TAG_DETECTOR_INIT, t4p::SequenceClass::OnActionProgress)
	EVT_ACTION_PROGRESS(t4p::ID_EVENT_ACTION_TAG_DETECTOR, t4p::SequenceClass::OnActionProgress)
	EVT_ACTION_PROGRESS(t4p::ID_EVENT_ACTION_TAG_FINDER_LIST_WIPE, t4p::SequenceClass::OnActionProgress)
	EVT_ACTION_PROGRESS(t4p::ID_EVENT_ACTION_CALL_STACK, t4p::SequenceClass::OnActionProgress)
	EVT_ACTION_PROGRESS(t4p::ID_EVENT_ACTION_TEMPLATE_FILE_TAG_DETECTOR, t4p::SequenceClass::OnActionProgress)
	EVT_ACTION_PROGRESS(t4p::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR, t4p::SequenceClass::OnActionProgress)
	EVT_ACTION_PROGRESS(t4p::ID_EVENT_ACTION_CONFIG_TAG_DETECTOR, t4p::SequenceClass::OnActionProgress)
	EVT_ACTION_PROGRESS(t4p::ID_EVENT_ACTION_TAG_CACHE_VERSION_CHECK, t4p::SequenceClass::OnActionProgress)
	EVT_ACTION_PROGRESS(t4p::ID_EVENT_ACTION_DETECTOR_CACHE_VERSION_CHECK, t4p::SequenceClass::OnActionProgress)
	EVT_ACTION_PROGRESS(t4p::ID_EVENT_ACTION_DETECTOR_DB_INIT, t4p::SequenceClass::OnActionProgress)
END_EVENT_TABLE()
