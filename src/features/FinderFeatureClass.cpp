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
#include <features/FinderFeatureClass.h>
#include <globals/String.h>
#include <widgets/UnicodeStringValidatorClass.h>
#include <widgets/RegularExpressionValidatorClass.h>
#include <Triumph.h>

static const int ID_FIND_PANEL = wxNewId();
static const int ID_REPLACE_PANEL = wxNewId();
const int t4p::ID_FINDER_ACTION = wxNewId();

t4p::FinderFeatureClass::FinderFeatureClass(t4p::AppClass& app)
	: FeatureClass(app)
	, Finder()
	, FinderReplace() {
}

void t4p::FinderFeatureClass::LoadPreferences(wxConfigBase* config) {
	config->Read(wxT("/Finder/Wrap"), &Finder.Wrap);
	config->Read(wxT("/Finder/Mode"), &Finder.Mode);
	config->Read(wxT("/Finder/ReplaceWrap"), &FinderReplace.Wrap);
	config->Read(wxT("/Finder/ReplaceMode"), &FinderReplace.Mode);
}

void t4p::FinderFeatureClass::StartFinderAction(UnicodeString word, char* buf, int documentLength) {
	t4p::FinderActionClass* action = new t4p::FinderActionClass(App.RunningThreads, ID_FINDER_ACTION, word,
		buf, documentLength);
	App.RunningThreads.Queue(action);
}

t4p::FinderActionClass::FinderActionClass(t4p::RunningThreadsClass& runningThreads, int eventId,
		const UnicodeString& search, char* utf8Buf, int bufLength)
: ActionClass(runningThreads, eventId)
, Finder()
, Code()
, Utf8Buf(utf8Buf)
, BufferLength(bufLength) {
	Finder.Expression = search;
	Finder.Mode = t4p::FinderClass::EXACT;
}

void t4p::FinderActionClass::BackgroundWork() {
	if (!Finder.Prepare()) {
		return;
	}
	Code = t4p::CharToIcu(Utf8Buf);
	int32_t nextIndex(0);
	bool found = Finder.FindNext(Code, nextIndex);
	int32_t matchStart(0);
	int32_t matchLength(0);
	while (found) {
		if (Finder.GetLastMatch(matchStart, matchLength)) {
			// convert match back to UTF-8 ugh
			int utf8Start = t4p::CharToUtf8Pos(Utf8Buf, BufferLength, matchStart);
			int utf8End = t4p::CharToUtf8Pos(Utf8Buf, BufferLength, matchStart + matchLength);

			t4p::FinderHitEventClass hit(GetEventId(), utf8Start, utf8End - utf8Start);
			PostEvent(hit);
			nextIndex = matchStart + matchLength + 1; // prevent infinite find next's
		} else {
			break;
		}
		found = Finder.FindNext(Code, nextIndex);
	}
	delete[] Utf8Buf;
}

wxString t4p::FinderActionClass::GetLabel() const {
	return wxT("Finder Search");
}

t4p::FinderHitEventClass::FinderHitEventClass(int id, int start, int length)
: wxEvent(id, t4p::EVENT_FINDER_ACTION)
, Start(start)
, Length(length) {
}

wxEvent* t4p::FinderHitEventClass::Clone() const {
	return new t4p::FinderHitEventClass(GetId(), Start, Length);
}

const wxEventType t4p::EVENT_FINDER_ACTION = wxNewEventType();

