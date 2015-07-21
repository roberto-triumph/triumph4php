/**
 * @copyright  2009-2011 Roberto Perpuly
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
#include "actions/CallStackActionClass.h"
#include <soci/sqlite3/soci-sqlite3.h>
#include <string>
#include <vector>
#include "globals/Assets.h"
#include "globals/Errors.h"
#include "language_php/TagFinderList.h"

t4p::CallStackActionClass::CallStackActionClass(t4p::RunningThreadsClass& runningThreads, int eventId)
	: GlobalActionClass(runningThreads, eventId)
	, TagCache()
	, CallStack(TagCache)
	, DetectorDbFileName()
	, StartFileName()
	, StartClassName()
	, StartMethodName()
	, Version(pelet::PHP_53) {
}

wxString t4p::CallStackActionClass::GetLabel() const {
	return wxT("Call stack generation");
}

bool t4p::CallStackActionClass::Init(t4p::GlobalsClass& globals) {
	SetStatus(_("Call Stack Gen"));
	bool ret = false;
	Version = globals.Environment.Php.Version;
	std::vector<wxString> otherFileExtensions = globals.FileTypes.GetNonPhpFileExtensions();

	// register the project tag DB file now so that it is available for code completion
	// the tag cache will own these pointers
	t4p::TagFinderListClass* projectCache = new t4p::TagFinderListClass;
	projectCache->InitGlobalTag(globals.TagCacheDbFileName, globals.FileTypes.GetPhpFileExtensions(), otherFileExtensions, Version);

	// initialize the detected tag cache too so that more methods can be resolved
	projectCache->InitDetectorTag(globals.DetectorCacheDbFileName);
	TagCache.RegisterGlobal(projectCache);
	ret = true;
	SetStatus(_("Call Stack Gen ") + t4p::IcuToWx(StartClassName) + wxT("::") +
		t4p::IcuToWx(StartMethodName));
	return ret;
}

bool t4p::CallStackActionClass::SetCallStackStart(const wxFileName& startFileName, const UnicodeString& className,
														const UnicodeString& methodName, const wxFileName& detectorDbFileName) {
	// make sure to set these BEFORE calling CreateSingleInstance
	// in order to prevent Entry from reading them while we write to them
	StartFileName.Assign(startFileName);
	StartClassName = className;
	StartMethodName = methodName;
	DetectorDbFileName.Assign(detectorDbFileName);
	return StartFileName.FileExists();
}

void t4p::CallStackActionClass::BackgroundWork() {
	t4p::CallStackClass::Errors lastError = t4p::CallStackClass::NONE;

	// build the call stack then save it to a temp file
	if (CallStack.Build(StartFileName, StartClassName, StartMethodName, Version, lastError)) {
		if (!DetectorDbFileName.IsOk()) {
			t4p::EditorLogWarning(t4p::ERR_TAG_READ, _("Could not create call stack file in ") + DetectorDbFileName.GetFullPath());
		} else {
			std::string stdDbName = t4p::WxToChar(DetectorDbFileName.GetFullPath());

			// we should be able to open this since it has been created by
			// the DetectorCacheDbVersionActionClass
			soci::session session(*soci::factory_sqlite3(), stdDbName);
			if (!CallStack.Persist(session)) {
				t4p::EditorLogWarning(t4p::ERR_TAG_READ, _("Could not persist call stack file in ") + DetectorDbFileName.GetFullPath());
			}
		}
	}
}
