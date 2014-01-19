
#include <actions/CallStackActionClass.h>
#include <language/TagFinderList.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <soci/sqlite3/soci-sqlite3.h>

mvceditor::CallStackActionClass::CallStackActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: GlobalActionClass(runningThreads, eventId) 
	, TagCache()
	, CallStack(TagCache)
	, DetectorDbFileName()
	, StartFileName() 
	, StartClassName() 
	, StartMethodName() 
	, Version(pelet::PHP_53) {
		
}

wxString mvceditor::CallStackActionClass::GetLabel() const {
	return wxT("Call stack generation");
}

bool mvceditor::CallStackActionClass::Init(mvceditor::GlobalsClass& globals) {
	SetStatus(_("Call Stack Gen"));
	bool ret = false;
	Version = globals.Environment.Php.Version;
	std::vector<wxString> otherFileExtensions = globals.FileTypes.GetNonPhpFileExtensions();

	// register the project tag DB file now so that it is available for code completion
	// the tag cache will own these pointers
	mvceditor::TagFinderListClass* projectCache = new mvceditor::TagFinderListClass;
	projectCache->InitGlobalTag(globals.TagCacheDbFileName, globals.FileTypes.GetPhpFileExtensions(), otherFileExtensions, Version);

	// initialize the detected tag cache too so that more methods can be resolved
	projectCache->InitDetectorTag(globals.DetectorCacheDbFileName);
	TagCache.RegisterGlobal(projectCache);
	ret = true;
	SetStatus(_("Call Stack Gen ") + mvceditor::IcuToWx(StartClassName) + wxT("::") + 
		mvceditor::IcuToWx(StartMethodName));
	return ret;
}

bool mvceditor::CallStackActionClass::SetCallStackStart(const wxFileName& startFileName, const UnicodeString& className, 
														const UnicodeString& methodName, const wxFileName& detectorDbFileName) {

	// make sure to set these BEFORE calling CreateSingleInstance
	// in order to prevent Entry from reading them while we write to them
	StartFileName.Assign(startFileName);
	StartClassName = className;
	StartMethodName = methodName;
	DetectorDbFileName.Assign(detectorDbFileName);
	return StartFileName.FileExists();
}

void mvceditor::CallStackActionClass::BackgroundWork() {
	mvceditor::CallStackClass::Errors lastError = mvceditor::CallStackClass::NONE;
	
	// build the call stack then save it to a temp file
	if (CallStack.Build(StartFileName, StartClassName, StartMethodName, Version, lastError)) {
		if (!DetectorDbFileName.IsOk()) {
			mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER, _("Could not create call stack file in ") + DetectorDbFileName.GetFullPath());
		}
		else {
			std::string stdDbName = mvceditor::WxToChar(DetectorDbFileName.GetFullPath());

			// we should be able to open this since it has been created by
			// the DetectorCacheDbVersionActionClass
			soci::session session(*soci::factory_sqlite3(), stdDbName);
			if (!CallStack.Persist(session)) {
				mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER, _("Could not persist call stack file in ") + DetectorDbFileName.GetFullPath());
			}
		}
	}
}