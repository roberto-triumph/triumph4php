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