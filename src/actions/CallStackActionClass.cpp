
#include <actions/CallStackActionClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>

mvceditor::CallStackActionClass::CallStackActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ActionClass(runningThreads, eventId) 
	, TagCache()
	, CallStack(TagCache, mvceditor::TagCacheWorkingAsset())
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
	std::vector<wxString> otherFileExtensions = globals.GetNonPhpFileExtensions();

	// register the project tag DB file now so that it is available for code completion
	// the tag cache will own these pointers
	mvceditor::GlobalCacheClass* projectCache = new mvceditor::GlobalCacheClass;
	projectCache->InitGlobalTag(globals.TagCacheDbFileName, globals.GetPhpFileExtensions(), otherFileExtensions, Version);
	TagCache.RegisterGlobal(projectCache);

	// initialize the detected tag cache too so that more methods can be resolved
	projectCache = new mvceditor::GlobalCacheClass;
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
		else if (!CallStack.Persist(DetectorDbFileName)) {
			mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER, _("Could not persist call stack file in ") + DetectorDbFileName.GetFullPath());
		}
	}
}