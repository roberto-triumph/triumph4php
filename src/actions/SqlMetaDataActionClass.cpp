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
#include <actions/SqlMetaDataActionClass.h>
#include <globals/Errors.h>

const wxEventType mvceditor::EVENT_SQL_META_DATA_COMPLETE = wxNewEventType();

mvceditor::SqlMetaDataEventClass::SqlMetaDataEventClass(int eventId, const mvceditor::SqlResourceFinderClass& newResources,
														const std::vector<UnicodeString>& errors) 
	: wxEvent(eventId, mvceditor::EVENT_SQL_META_DATA_COMPLETE)
	, NewResources(newResources)
	, Errors(errors) {
}

wxEvent* mvceditor::SqlMetaDataEventClass::Clone() const {
	mvceditor::SqlMetaDataEventClass* evt = new 
		mvceditor::SqlMetaDataEventClass(GetId(), NewResources, Errors);
	return evt;
}

mvceditor::SqlMetaDataActionClass::SqlMetaDataActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: GlobalActionClass(runningThreads, eventId)
	, DatabaseTags() {
		
}

bool mvceditor::SqlMetaDataActionClass::Init(mvceditor::GlobalsClass& globals) {
	SetStatus(_("SQL Meta"));
	DatabaseTags = globals.DatabaseTags;
	
	// this will prime the sql connections from the php detectors
	// this is being done here so that we can guarantee that SqlMetaData has the
	// most up-to-date database tags. The GlobalsChangeHandlerClass also reads
	// the database tags from the detector db, but since it also works on events
	// we cannot gurantee that the GlobalsChangeHandlerClass EVENT_WORK_COMPLETE handler will get called
	// before the SequenceClass EVENT_WORK_COMPLETE handler.
	
	// first remove all detected connections that were previously detected
	std::vector<mvceditor::DatabaseTagClass>::iterator info;
	info = DatabaseTags.begin();
	while(info != DatabaseTags.end()) {
		if (info->IsDetected) {
			info = DatabaseTags.erase(info);
		}
		else {
			info++;
		}
	}

	std::vector<wxFileName> sourceDirectories = globals.AllEnabledSourceDirectories();

	// initialize the detected tag cache only the enabled projects
	mvceditor::DatabaseTagFinderClass finder;
	finder.InitSession(&globals.DetectorCacheSession);
	
	std::vector<mvceditor::DatabaseTagClass> detected = finder.All(sourceDirectories);
	std::vector<mvceditor::DatabaseTagClass>::const_iterator tag;
	for (tag = detected.begin(); tag != detected.end(); ++tag) {
		if (!tag->Host.isEmpty() && !tag->Schema.isEmpty()) {
			DatabaseTags.push_back(*tag);
		}
	}
	
	return !DatabaseTags.empty();
}

void mvceditor::SqlMetaDataActionClass::BackgroundWork() {
	std::vector<UnicodeString> errors;
	mvceditor::SqlResourceFinderClass newResources;
	for (std::vector<mvceditor::DatabaseTagClass>::iterator it = DatabaseTags.begin(); it != DatabaseTags.end(); ++it) {
		if (!IsCancelled()) {
			if (it->IsEnabled) {
				wxString wxLabel = _("SQL Meta / ") ; //mvceditor::IcuToWx(it->Label);
				SetStatus(wxLabel);
				UnicodeString error;
				if (!newResources.Fetch(*it, error)) {
					errors.push_back(it->Label + UNICODE_STRING_SIMPLE(": ") + error);
				}
			}
		}
		else {
			break;
		}
	}
	if (!IsCancelled()) {

		// PostEvent() will set the correct event Id
		mvceditor::SqlMetaDataEventClass evt(wxID_ANY, newResources, errors);
		PostEvent(evt);
	}
}

wxString mvceditor::SqlMetaDataActionClass::GetLabel() const {
	return _("SQL metadata detection");
}