/**
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#include <actions/SqlMetaDataActionClass.h>
#include <globals/Errors.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

const wxEventType t4p::EVENT_SQL_META_DATA_COMPLETE = wxNewEventType();

t4p::SqlMetaDataEventClass::SqlMetaDataEventClass(int eventId,
														const std::vector<UnicodeString>& errors)
	: wxEvent(eventId, t4p::EVENT_SQL_META_DATA_COMPLETE)
	, Errors(errors) {
}

wxEvent* t4p::SqlMetaDataEventClass::Clone() const {
	t4p::SqlMetaDataEventClass* evt = new
		t4p::SqlMetaDataEventClass(GetId(), Errors);
	return evt;
}

t4p::SqlMetaDataInitActionClass::SqlMetaDataInitActionClass(t4p::RunningThreadsClass& runningThreads, int eventId)
: InitializerGlobalActionClass(runningThreads, eventId) {
}

void t4p::SqlMetaDataInitActionClass::Work(t4p::GlobalsClass& globals) {
	// prime the sql resource finder
	// the tag db may not exist if the user screwed up and pointed their settings
	// dir to a non-existing location.
	if (!globals.TagCacheDbFileName.Exists()) {
		return;
	}
	globals.ResourceCacheSession.open(
		*soci::factory_sqlite3(),
		t4p::WxToChar(globals.TagCacheDbFileName.GetFullPath())
	);
}

wxString t4p::SqlMetaDataInitActionClass::GetLabel() const {
	return wxT("SQL Metadata Init");
}


t4p::SqlMetaDataActionClass::SqlMetaDataActionClass(t4p::RunningThreadsClass& runningThreads, int eventId)
	: GlobalActionClass(runningThreads, eventId)
	, DatabaseTags()
	, CacheDbFileName() {
}

bool t4p::SqlMetaDataActionClass::Init(t4p::GlobalsClass& globals) {
	SetStatus(_("SQL Meta"));

	DatabaseTags = globals.DatabaseTags;

	// this will prime the sql connections from the php detectors
	// this is being done here so that we can guarantee that SqlMetaData has the
	// most up-to-date database tags. The GlobalsChangeHandlerClass also reads
	// the database tags from the detector db, but since it also works on events
	// we cannot gurantee that the GlobalsChangeHandlerClass EVENT_WORK_COMPLETE handler will get called
	// before the SequenceClass EVENT_WORK_COMPLETE handler.

	// first remove all detected connections that were previously detected
	std::vector<t4p::DatabaseTagClass>::iterator info;
	info = DatabaseTags.begin();
	while(info != DatabaseTags.end()) {
		if (info->IsDetected) {
			info = DatabaseTags.erase(info);
		} else {
			info++;
		}
	}

	std::vector<wxFileName> sourceDirectories = globals.AllEnabledSourceDirectories();

	// initialize the detected tag cache only the enabled projects
	t4p::DatabaseTagFinderClass finder(globals.DetectorCacheSession);

	std::vector<t4p::DatabaseTagClass> detected = finder.All(sourceDirectories);
	std::vector<t4p::DatabaseTagClass>::const_iterator tag;
	for (tag = detected.begin(); tag != detected.end(); ++tag) {
		if (!tag->Host.isEmpty() && !tag->Schema.isEmpty()) {
			DatabaseTags.push_back(*tag);
		}
	}

	// clone of filename, for thread safety
	CacheDbFileName.Assign(globals.TagCacheDbFileName.GetFullPath());

	return !DatabaseTags.empty();
}

void t4p::SqlMetaDataActionClass::BackgroundWork() {
	std::vector<UnicodeString> errors;
	soci::session session(*soci::factory_sqlite3(), t4p::WxToChar(CacheDbFileName.GetFullPath()));
	t4p::SqlResourceFetchClass fetcher(session);

	fetcher.Wipe();
	for (std::vector<t4p::DatabaseTagClass>::iterator it = DatabaseTags.begin(); it != DatabaseTags.end(); ++it) {
		if (!IsCancelled()) {
			if (it->IsEnabled) {
				wxString wxLabel = _("SQL Meta / ");  // t4p::IcuToWx(it->Label);
				SetStatus(wxLabel);
				UnicodeString error;
				if (!fetcher.Fetch(*it, error)) {
					errors.push_back(it->Label + UNICODE_STRING_SIMPLE(": ") + error);
				}
			}
		} else {
			break;
		}
	}
	if (!IsCancelled()) {
		// PostEvent() will set the correct event Id
		t4p::SqlMetaDataEventClass evt(wxID_ANY, errors);
		PostEvent(evt);
	}
}

wxString t4p::SqlMetaDataActionClass::GetLabel() const {
	return _("SQL metadata detection");
}
