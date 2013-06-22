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
#include <language/ParsedTagFinderClass.h>
#include <language/DetectedTagFinderResultClass.h>
#include <soci/sqlite3/soci-sqlite3.h>

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


std::vector<UnicodeString> mvceditor::TagFinderListClass::ClassParents(UnicodeString className, UnicodeString methodName) {
	std::vector<UnicodeString> parents;
	bool found = false;
	UnicodeString classToLookup = className;
	do {

		// each parent class may be located in any of the finders. in practice this code is not as slow
		// as it looks; class hierarchies are usually not very deep (1-4 parents)
		found = false;		
		UnicodeString parentClass;
		if (IsTagFinderInit) {
			parentClass = TagFinder.ParentClassName(classToLookup);
			if (!parentClass.isEmpty()) {
				found = true;
				parents.push_back(parentClass);
				classToLookup = parentClass;
			}
		}
		if (IsNativeTagFinderInit) {
			parentClass = NativeTagFinder.ParentClassName(classToLookup);
			if (!parentClass.isEmpty()) {
				found = true;
				parents.push_back(parentClass);
				classToLookup = parentClass;
			}
		}
	} while (found);
	return parents;
}

std::vector<UnicodeString> mvceditor::TagFinderListClass::ClassUsedTraits(const UnicodeString& className, 
												  const std::vector<UnicodeString>& parentClassNames, 
												  const UnicodeString& methodName) {

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
			UnicodeString parentClass;
			if (IsTagFinderInit) {
				std::vector<UnicodeString> traits = TagFinder.GetResourceTraits(*it, methodName, emptyVector);
				if (!traits.empty()) {
					found = true;
					nextTraitsToLookup.insert(nextTraitsToLookup.end(), traits.begin(), traits.end());
					usedTraits.insert(usedTraits.end(), traits.begin(), traits.end());
				}
			}
			if (IsNativeTagFinderInit) {
				std::vector<UnicodeString> traits = NativeTagFinder.GetResourceTraits(*it, methodName, emptyVector);
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


UnicodeString mvceditor::TagFinderListClass::ResolveResourceType(UnicodeString resourceToLookup) {
	UnicodeString type;
	mvceditor::TagSearchClass tagSearch(resourceToLookup);
	tagSearch.SetParentClasses(ClassParents(tagSearch.GetClassName(), tagSearch.GetMethodName()));
	tagSearch.SetTraits(ClassUsedTraits(tagSearch.GetClassName(), tagSearch.GetParentClasses(), tagSearch.GetMethodName()));
	
	if (IsDetectedTagFinderInit && !tagSearch.GetClassName().isEmpty()) {
		mvceditor::DetectedTagExactMemberResultClass detectedResult;
		std::vector<UnicodeString> classNames = tagSearch.GetClassHierarchy();
		detectedResult.Set(classNames, tagSearch.GetMethodName());
		if (DetectedTagFinder.Exec(&detectedResult)) {
			detectedResult.Next();
			type = detectedResult.Tag.ReturnType;
		}
	}
	if (type.isEmpty() && IsTagFinderInit) {
		mvceditor::TagResultClass* tagResults = tagSearch.CreateExactResults();
		if (TagFinder.Exec(tagResults)) {

			// since we are doing fully qualified matches, all matches are from the inheritance chain; ie. all methods
			// should have the same signature (return type)
			tagResults->Next();

			// if the given string was a class name, return the class name
			// if the given string was a method, return the method's return type
			type =  mvceditor::TagClass::CLASS == tagResults->Tag.Type ? tagResults->Tag.ClassName : tagResults->Tag.ReturnType;
		}
		delete tagResults;

	}
	if (type.isEmpty() && IsNativeTagFinderInit) {
		mvceditor::TagResultClass* tagResults = tagSearch.CreateExactResults();
		if (NativeTagFinder.Exec(tagResults)) {
			tagResults->Next();
			type =  mvceditor::TagClass::CLASS == tagResults->Tag.Type ? tagResults->Tag.ClassName : tagResults->Tag.ReturnType;
		}
		delete tagResults;

	}
	return type;
}

UnicodeString mvceditor::TagFinderListClass::ParentClassName(UnicodeString className) {
	UnicodeString parent;
	if (IsTagFinderInit) {
		parent = TagFinder.ParentClassName(className);
	}
	if (parent.isEmpty() && IsNativeTagFinderInit) {
		parent = NativeTagFinder.ParentClassName(className);
	}
	return parent;
}

void mvceditor::TagFinderListClass::ExactMatchesFromAll(mvceditor::TagSearchClass& tagSearch, std::vector<mvceditor::TagClass>& matches) {
	mvceditor::TagResultClass* result = tagSearch.CreateExactResults();
	if (IsTagFinderInit && TagFinder.Exec(result)) {
		while (result->More()) {
			result->Next();
			matches.push_back(result->Tag);
		}
	}
	delete result;
	result = tagSearch.CreateExactResults();
	if (IsNativeTagFinderInit && NativeTagFinder.Exec(result)) {
		while (result->More()) {
			result->Next();
			matches.push_back(result->Tag);
		}
	}
	delete result;

	if (IsDetectedTagFinderInit && !tagSearch.GetClassName().isEmpty()) {
		mvceditor::DetectedTagExactMemberResultClass detectedResult;
		detectedResult.Set(tagSearch.GetClassHierarchy(), tagSearch.GetMethodName());
		if (DetectedTagFinder.Exec(&detectedResult)) {
			while (detectedResult.More()) {
				detectedResult.Next();
				matches.push_back(detectedResult.Tag);
			}
		}
	}
}

void mvceditor::TagFinderListClass::NearMatchesFromAll(mvceditor::TagSearchClass& tagSearch, std::vector<mvceditor::TagClass>& matches) {
	mvceditor::TagResultClass* result = tagSearch.CreateNearMatchResults();
	if (IsTagFinderInit && TagFinder.Exec(result)) {
		while (result->More()) {
			result->Next();
			matches.push_back(result->Tag);
		}
	}
	delete result;
	result = tagSearch.CreateNearMatchResults();
	if (IsNativeTagFinderInit && NativeTagFinder.Exec(result)) {
		while (result->More()) {
			result->Next();
			matches.push_back(result->Tag);
		}
	}
	delete result;

	if (IsDetectedTagFinderInit && !tagSearch.GetClassName().isEmpty()) {
		mvceditor::DetectedTagNearMatchMemberResultClass detectedResult;
		detectedResult.Set(tagSearch.GetClassHierarchy(), tagSearch.GetMethodName());
		if (DetectedTagFinder.Exec(&detectedResult)) {
			while (detectedResult.More()) {
				detectedResult.Next();
				matches.push_back(detectedResult.Tag);
			}
		}
	}
}

void mvceditor::TagFinderListClass::ExactTraitAliasesFromAll(mvceditor::TagSearchClass& tagSearch, std::vector<mvceditor::TagClass>& matches) {
	if (tagSearch.GetClassName().isEmpty()) {

		// no class = impossible to have traits
		return;
	}
	mvceditor::TraitTagResultClass traitResult;
	traitResult.Set(tagSearch.GetClassHierarchy(), tagSearch.GetMethodName(), true, tagSearch.GetSourceDirs());
	if (IsTagFinderInit && TagFinder.Exec(&traitResult)) {
		std::vector<mvceditor::TagClass> traitAliases = traitResult.MatchesAsTags();
		for (size_t i = 0; i < traitAliases.size(); ++i) {
			matches.push_back(traitAliases[i]);
		}
	}
	if (IsNativeTagFinderInit && NativeTagFinder.Exec(&traitResult)) {
		std::vector<mvceditor::TagClass> traitAliases = traitResult.MatchesAsTags();
		for (size_t i = 0; i < traitAliases.size(); ++i) {
			matches.push_back(traitAliases[i]);
		}
	}
}

void mvceditor::TagFinderListClass::NearMatchTraitAliasesFromAll(mvceditor::TagSearchClass& tagSearch, std::vector<mvceditor::TagClass>& matches) {
	if (tagSearch.GetClassName().isEmpty()) {

		// no class = impossible to have traits
		return;
	}
	mvceditor::TraitTagResultClass traitResult;
	traitResult.Set(tagSearch.GetClassHierarchy(), tagSearch.GetMethodName(), false, tagSearch.GetSourceDirs());
	if (IsTagFinderInit && TagFinder.Exec(&traitResult)) {
		std::vector<mvceditor::TagClass> traitAliases = traitResult.MatchesAsTags();
		for (size_t i = 0; i < traitAliases.size(); ++i) {
			matches.push_back(traitAliases[i]);
		}
	}
	if (IsNativeTagFinderInit && NativeTagFinder.Exec(&traitResult)) {
		std::vector<mvceditor::TagClass> traitAliases = traitResult.MatchesAsTags();
		for (size_t i = 0; i < traitAliases.size(); ++i) {
			matches.push_back(traitAliases[i]);
		}
	}
}