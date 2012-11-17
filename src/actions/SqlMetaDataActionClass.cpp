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
	: ActionClass(runningThreads, eventId)
	, Infos() {
		
}

bool mvceditor::SqlMetaDataActionClass::Init(mvceditor::GlobalsClass& globals) {
	Infos = globals.Infos;
	return !Infos.empty();
}

void mvceditor::SqlMetaDataActionClass::BackgroundWork() {
	std::vector<UnicodeString> errors;
	mvceditor::SqlResourceFinderClass newResources;
	for (std::vector<mvceditor::DatabaseInfoClass>::iterator it = Infos.begin(); it != Infos.end(); ++it) {
		if (!IsCancelled()) {
			if (it->IsEnabled) {
				UnicodeString error;
				if (!newResources.Fetch(*it, error)) {
					errors.push_back(error);
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

mvceditor::SqlMetaDataInitActionClass::SqlMetaDataInitActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: InitializerActionClass(runningThreads, eventId) {

}

void mvceditor::SqlMetaDataInitActionClass::Work(mvceditor::GlobalsClass& globals) {
	
	// remove any connections previously detected
	std::vector<mvceditor::DatabaseInfoClass>::iterator it = globals.Infos.begin();
	while (it != globals.Infos.end()) {
		if (it->IsDetected) {
			it = globals.Infos.erase(it);
		}
		else {
			++it;
		}
	}
	
	// add the detected connections to the infos list
	// this makes it easier; that way we always work with one list only
	for (size_t i = 0; i < globals.Frameworks.size(); ++i) {
		globals.Infos.insert(globals.Infos.end(), globals.Frameworks[i].Databases.begin(), globals.Frameworks[i].Databases.end());
	}
}
