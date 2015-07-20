
#include <actions/CallStackActionClass.h>
#include <language_php/TagFinderList.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <soci/sqlite3/soci-sqlite3.h>

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
