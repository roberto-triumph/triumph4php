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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <language/TagFinderList.h>
#include <search/TagFinderClass.h>
#include <soci/sqlite3/soci-sqlite3.h>

std::vector<UnicodeString> mvceditor::TagFinderListClassParents(UnicodeString className, UnicodeString methodName, 
												 const std::vector<mvceditor::ParsedTagFinderClass*>& allTagFinders) {
	std::vector<UnicodeString> parents;
	bool found = false;
	UnicodeString classToLookup = className;
	do {

		// each parent class may be located in any of the finders. in practice this code is not as slow
		// as it looks; class hierarchies are usually not very deep (1-4 parents)
		found = false;
		for (size_t i = 0; i < allTagFinders.size(); ++i) {
			UnicodeString parentClass = allTagFinders[i]->ParentClassName(classToLookup);
			if (!parentClass.isEmpty()) {
				found = true;
				parents.push_back(parentClass);
				classToLookup = parentClass;

				// a class can have at most 1 parent, no need to look at other finders
				break;
			}
		}
	} while (found);
	return parents;
}

std::vector<UnicodeString> mvceditor::TagFinderListClassUsedTraits(const UnicodeString& className, 
												  const std::vector<UnicodeString>& parentClassNames, 
												  const UnicodeString& methodName, 
												  const std::vector<mvceditor::ParsedTagFinderClass*>& allTagFinders) {

	// trait support; a class can use multiple traits; hence the different logic 
	std::vector<UnicodeString> classesToLookup;
	classesToLookup.push_back(className);
	classesToLookup.insert(classesToLookup.end(), parentClassNames.begin(), parentClassNames.end());
	std::vector<UnicodeString> usedTraits;

	// TODO propagate from enabled projects
	std::vector<wxFileName> emptyVector;
	bool found = false;
	do {
		found = false;
		std::vector<UnicodeString> nextTraitsToLookup;
		for (std::vector<UnicodeString>::iterator it = classesToLookup.begin(); it != classesToLookup.end(); ++it) {
			for (size_t i = 0; i < allTagFinders.size(); ++i) {
				std::vector<UnicodeString> traits = allTagFinders[i]->GetResourceTraits(*it, methodName, emptyVector);
				if (!traits.empty()) {
					found = true;
					nextTraitsToLookup.insert(nextTraitsToLookup.end(), traits.begin(), traits.end());
					usedTraits.insert(usedTraits.end(), traits.begin(), traits.end());
				}
			}
		}

		// next, look for traits used by the traits themselves
		classesToLookup = nextTraitsToLookup;
	} while (found);

	return usedTraits;
}

mvceditor::TagFinderListClass::TagFinderListClass()
	: TagParser() 
	, TagFinder()
	, NativeTagFinder()
	, DetectedTagFinder()
	, IsNativeTagFinderInit(false)
	, IsTagFinderInit(false)
	, IsDetectedTagFinderInit(false)
	, TagDbSession(NULL)
	, NativeDbSession(NULL)
	, DetectedTagDbSession(NULL)
{

}
mvceditor::TagFinderListClass::~TagFinderListClass() {
	TagParser.Close();
	if (TagDbSession) {
		TagDbSession->close();
		delete TagDbSession;
	}
	if (DetectedTagDbSession) {
		DetectedTagDbSession->close();
		delete DetectedTagDbSession;
	}
	if (NativeDbSession) {
		NativeDbSession->close();
		delete NativeDbSession;
	}
}

void mvceditor::TagFinderListClass::InitGlobalTag(const wxFileName& tagDbFileName, 
									   const std::vector<wxString>& phpFileExtensions, 
									   const std::vector<wxString>& miscFileExtensions,
									   pelet::Versions version) {
	wxASSERT_MSG(!IsTagFinderInit, wxT("tag finder can only be initialized once"));
	TagParser.PhpFileExtensions = phpFileExtensions;
	TagParser.MiscFileExtensions = miscFileExtensions;
	TagDbSession = new soci::session;
	IsTagFinderInit = Open(TagDbSession, tagDbFileName.GetFullPath());
	if (IsTagFinderInit) {
		TagParser.SetVersion(version);
		TagParser.Init(TagDbSession);
		TagFinder.InitSession(TagDbSession);
	}
}

void mvceditor::TagFinderListClass::AdoptGlobalTag(soci::session* globalSession,
												 const std::vector<wxString>& phpFileExtensions, 
												 const std::vector<wxString>& miscFileExtensions,
												 pelet::Versions version) {
	wxASSERT_MSG(!IsTagFinderInit, wxT("tag finder can only be initialized once"));
	TagParser.PhpFileExtensions = phpFileExtensions;
	TagParser.MiscFileExtensions = miscFileExtensions;
	TagDbSession = globalSession;
	IsTagFinderInit = NULL != globalSession;
	if (IsTagFinderInit) {
		TagParser.SetVersion(version);
		TagParser.Init(TagDbSession);
		TagFinder.InitSession(TagDbSession);
	}
}

void mvceditor::TagFinderListClass::InitDetectorTag(const wxFileName& detectorDbFileName) {
	wxASSERT_MSG(!IsDetectedTagFinderInit, wxT("tag finder can only be initialized once"));
	DetectedTagDbSession = new soci::session;
	IsDetectedTagFinderInit = Open(DetectedTagDbSession, detectorDbFileName.GetFullPath());
	if (IsDetectedTagFinderInit) {
		DetectedTagFinder.InitSession(DetectedTagDbSession);
	}
}

void mvceditor::TagFinderListClass::AdoptDetectorTag(soci::session* session) {
	wxASSERT_MSG(!IsDetectedTagFinderInit, wxT("tag finder can only be initialized once"));
	IsDetectedTagFinderInit = NULL != session;
	DetectedTagDbSession = session;
	if (IsDetectedTagFinderInit) {
		DetectedTagFinder.InitSession(DetectedTagDbSession);
	}
}

void mvceditor::TagFinderListClass::InitNativeTag(const wxFileName& nativeDbFileName) {
	wxASSERT_MSG(!IsNativeTagFinderInit, wxT("tag finder can only be initialized once"));
	NativeDbSession = new soci::session;
	IsNativeTagFinderInit = Open(NativeDbSession, nativeDbFileName.GetFullPath());
	if (IsNativeTagFinderInit) {
		NativeTagFinder.InitSession(NativeDbSession);
	}
}

void mvceditor::TagFinderListClass::AdoptNativeTag(soci::session* session) {
	wxASSERT_MSG(!IsNativeTagFinderInit, wxT("tag finder can only be initialized once"));
	IsNativeTagFinderInit = NULL != session;
	NativeDbSession = session;
	if (IsNativeTagFinderInit) {
		NativeTagFinder.InitSession(NativeDbSession);
	}
}

bool mvceditor::TagFinderListClass::Open(soci::session* session, const wxString& dbName) {
	bool ret = false;
	try {
		std::string stdDbName = mvceditor::WxToChar(dbName);
		
		// we should be able to open this since it has been created by
		// the TagCacheDbVersionActionClass
		session->open(*soci::factory_sqlite3(), stdDbName);

		// set a busy handler so that if we attempt to query while the file is locked, we 
		// sleep for a bit then try again
		mvceditor::SqliteSetBusyTimeout(*session, 100);
		ret = true;
	} catch(std::exception const& e) {
		session->close();
		wxString msg = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
	return ret;
}

void mvceditor::TagFinderListClass::Walk(mvceditor::DirectorySearchClass& search) {
	search.Walk(TagParser);
}
