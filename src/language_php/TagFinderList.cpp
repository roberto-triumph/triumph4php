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
#include <language_php/TagFinderList.h>
#include <language_php/ParsedTagFinderClass.h>
#include <language_php/DetectedTagFinderResultClass.h>
#include <globals/Assets.h>
#include <soci/sqlite3/soci-sqlite3.h>

t4p::TagFinderListClass::TagFinderListClass()
	: TagDbSession()
	, NativeDbSession()
	, DetectedTagDbSession()
	, TagParser()
	, TagFinder(TagDbSession)
	, NativeTagFinder(NativeDbSession)
	, DetectedTagFinder(DetectedTagDbSession)
	, IsNativeTagFinderInit(false)
	, IsTagFinderInit(false)
	, IsDetectedTagFinderInit(false) {
}

t4p::TagFinderListClass::~TagFinderListClass() {
	TagParser.Close();
	TagDbSession.close();
	DetectedTagDbSession.close();
	NativeDbSession.close();
}

void t4p::TagFinderListClass::InitGlobalTag(const wxFileName& tagDbFileName,
									   const std::vector<wxString>& phpFileExtensions,
									   const std::vector<wxString>& miscFileExtensions,
									   pelet::Versions version) {
	wxASSERT_MSG(!IsTagFinderInit, wxT("tag finder can only be initialized once"));

	// make sure to clone files and strings so
	// that this cache object can be used
	// in a background thread
	t4p::DeepCopy(TagParser.PhpFileExtensions, phpFileExtensions);
	t4p::DeepCopy(TagParser.MiscFileExtensions, miscFileExtensions);
	IsTagFinderInit = t4p::SqliteOpen(TagDbSession, tagDbFileName.GetFullPath());
	if (IsTagFinderInit) {
		TagParser.SetVersion(version);
		TagParser.Init(&TagDbSession);
	}
}

void t4p::TagFinderListClass::CreateGlobalTag(const std::vector<wxString>& phpFileExtensions,
												 const std::vector<wxString>& miscFileExtensions,
												 pelet::Versions version) {
	wxASSERT_MSG(!IsTagFinderInit, wxT("tag finder can only be initialized once"));

	// make sure to clone files and strings so
	// that this cache object can be used
	// in a background thread
	t4p::DeepCopy(TagParser.PhpFileExtensions, phpFileExtensions);
	t4p::DeepCopy(TagParser.MiscFileExtensions, miscFileExtensions);
	TagDbSession.open(*soci::factory_sqlite3(), ":memory:");
	wxString error;
	IsTagFinderInit = t4p::SqliteSqlScript(t4p::ResourceSqlSchemaAsset(), TagDbSession, error);
	wxASSERT_MSG(IsTagFinderInit, error);
	if (IsTagFinderInit) {
		TagParser.SetVersion(version);
		TagParser.Init(&TagDbSession);
	}
}

void t4p::TagFinderListClass::InitDetectorTag(const wxFileName& detectorDbFileName) {
	wxASSERT_MSG(!IsDetectedTagFinderInit, wxT("tag finder can only be initialized once"));
	IsDetectedTagFinderInit = t4p::SqliteOpen(DetectedTagDbSession, detectorDbFileName.GetFullPath());
}

void t4p::TagFinderListClass::CreateDetectorTag() {
	wxASSERT_MSG(!IsDetectedTagFinderInit, wxT("tag finder can only be initialized once"));

	DetectedTagDbSession.open(*soci::factory_sqlite3(), ":memory:");
	wxString error;
	IsDetectedTagFinderInit = t4p::SqliteSqlScript(t4p::DetectorSqlSchemaAsset(), DetectedTagDbSession, error);
	wxASSERT_MSG(IsDetectedTagFinderInit, error);
}

void t4p::TagFinderListClass::InitNativeTag(const wxFileName& nativeDbFileName) {
	wxASSERT_MSG(!IsNativeTagFinderInit, wxT("native tag finder can only be initialized once"));
	IsNativeTagFinderInit = t4p::SqliteOpen(NativeDbSession, nativeDbFileName.GetFullPath());
}

void t4p::TagFinderListClass::Walk(t4p::DirectorySearchClass& search) {
	search.Walk(TagParser);
}

std::vector<UnicodeString> t4p::TagFinderListClass::ClassParents(UnicodeString className, UnicodeString methodName) {
	std::vector<UnicodeString> parents;
	bool found = false;
	UnicodeString classToLookup = className;
	do {
		// each parent class may be located in any of the finders. in practice this code is not as slow
		// as it looks; class hierarchies are usually not very deep (1-4 parents)
		found = false;
		UnicodeString parentClass;
		if (IsTagFinderInit) {
			parentClass = TagFinder.ParentClassName(classToLookup, 0);
			if (!parentClass.isEmpty()) {
				found = true;
				parents.push_back(parentClass);
				classToLookup = parentClass;
			}
		}
		if (IsNativeTagFinderInit) {
			parentClass = NativeTagFinder.ParentClassName(classToLookup, 0);
			if (!parentClass.isEmpty()) {
				found = true;
				parents.push_back(parentClass);
				classToLookup = parentClass;
			}
		}
	} while (found);
	return parents;
}

std::vector<UnicodeString> t4p::TagFinderListClass::ClassUsedTraits(const UnicodeString& className,
												  const std::vector<UnicodeString>& parentClassNames,
												  const UnicodeString& methodName,
												  const std::vector<wxFileName>& sourceDirs) {
	// trait support; a class can use multiple traits; hence the different logic
	std::vector<UnicodeString> classesToLookup;
	classesToLookup.push_back(className);
	classesToLookup.insert(classesToLookup.end(), parentClassNames.begin(), parentClassNames.end());
	std::vector<UnicodeString> usedTraits;
	bool found = false;
	do {
		found = false;
		std::vector<UnicodeString> nextTraitsToLookup;
		for (std::vector<UnicodeString>::iterator it = classesToLookup.begin(); it != classesToLookup.end(); ++it) {
			UnicodeString parentClass;
			if (IsTagFinderInit) {
				std::vector<UnicodeString> traits = TagFinder.GetResourceTraits(*it, methodName, sourceDirs);
				if (!traits.empty()) {
					found = true;
					nextTraitsToLookup.insert(nextTraitsToLookup.end(), traits.begin(), traits.end());
					usedTraits.insert(usedTraits.end(), traits.begin(), traits.end());
				}
			}
			if (IsNativeTagFinderInit) {
				std::vector<UnicodeString> traits = NativeTagFinder.GetResourceTraits(*it, methodName, sourceDirs);
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


UnicodeString t4p::TagFinderListClass::ResolveResourceType(UnicodeString resourceToLookup, const std::vector<wxFileName>& sourceDirs) {
	UnicodeString type;
	t4p::TagSearchClass tagSearch(resourceToLookup);
	tagSearch.SetParentClasses(ClassParents(tagSearch.GetClassName(), tagSearch.GetMethodName()));
	tagSearch.SetSourceDirs(sourceDirs);
	tagSearch.SetTraits(ClassUsedTraits(tagSearch.GetClassName(), tagSearch.GetParentClasses(), tagSearch.GetMethodName(), sourceDirs));

	if (IsDetectedTagFinderInit && !tagSearch.GetClassName().isEmpty()) {
		t4p::DetectedTagExactMemberResultClass detectedResult;
		std::vector<UnicodeString> classNames = tagSearch.GetClassHierarchy();
		detectedResult.Set(classNames, tagSearch.GetMethodName(), sourceDirs);
		if (DetectedTagFinder.Exec(&detectedResult)) {
			detectedResult.Next();
			type = detectedResult.Tag.ReturnType;
		}
	}
	if (type.isEmpty() && IsTagFinderInit) {
		t4p::TagResultClass* tagResults = tagSearch.CreateExactResults();
		if (TagFinder.Exec(tagResults)) {
			// since we are doing fully qualified matches, all matches are from the inheritance chain; ie. all methods
			// should have the same signature (return type)
			tagResults->Next();

			UnicodeString fullyQualifiedClass;
			if (tagResults->Tag.NamespaceName == UNICODE_STRING_SIMPLE("\\")) {
				fullyQualifiedClass = tagResults->Tag.NamespaceName + tagResults->Tag.ClassName;
			} else if (!tagResults->Tag.NamespaceName.isEmpty()) {
				fullyQualifiedClass = tagResults->Tag.NamespaceName + UNICODE_STRING_SIMPLE("\\") + tagResults->Tag.ClassName;
			} else {
				fullyQualifiedClass = tagResults->Tag.ClassName;
			}

			// if the given string was a class name, return the class name
			// if the given string was a method, return the method's return type
			if (t4p::PhpTagClass::CLASS == tagResults->Tag.Type) {
				type = fullyQualifiedClass;
			} else {
				// the parser will always return fully qualified class name for return type that is
				// based on the namespace aliases
				type = tagResults->Tag.ReturnType;
			}
		}
		delete tagResults;
	}
	if (type.isEmpty() && IsNativeTagFinderInit) {
		// tags in the native db file do not have a source_id
		// when we query do not use source_id
		std::vector<wxFileName> emptySourceDirs;
		tagSearch.SetSourceDirs(emptySourceDirs);
		t4p::TagResultClass* tagResults = tagSearch.CreateExactResults();
		if (NativeTagFinder.Exec(tagResults)) {
			tagResults->Next();
			type =  t4p::PhpTagClass::CLASS == tagResults->Tag.Type ? tagResults->Tag.ClassName : tagResults->Tag.ReturnType;
		}
		delete tagResults;
	}
	return type;
}

UnicodeString t4p::TagFinderListClass::ParentClassName(UnicodeString className, int fileTagId) {
	UnicodeString parent;
	if (IsTagFinderInit) {
		parent = TagFinder.ParentClassName(className, 0);
	}
	if (parent.isEmpty() && IsNativeTagFinderInit) {
		parent = NativeTagFinder.ParentClassName(className, 0);
	}
	return parent;
}

void t4p::TagFinderListClass::ExactMatchesFromAll(t4p::TagSearchClass& tagSearch, std::vector<t4p::PhpTagClass>& matches,
		const std::vector<wxFileName>& sourceDirs) {
	tagSearch.SetSourceDirs(sourceDirs);
	t4p::TagResultClass* result = tagSearch.CreateExactResults();
	if (IsTagFinderInit && TagFinder.Exec(result)) {
		while (result->More()) {
			result->Next();
			matches.push_back(result->Tag);
		}
	}
	delete result;

	// tags in the native db file do not have a source_id
	// when we query do not use source_id
	std::vector<wxFileName> emptyVector;
	tagSearch.SetSourceDirs(emptyVector);
	result = tagSearch.CreateExactResults();
	if (IsNativeTagFinderInit && NativeTagFinder.Exec(result)) {
		while (result->More()) {
			result->Next();
			matches.push_back(result->Tag);
		}
	}
	delete result;

	tagSearch.SetSourceDirs(sourceDirs);
	if (IsDetectedTagFinderInit && !tagSearch.GetClassName().isEmpty()) {
		t4p::DetectedTagExactMemberResultClass detectedResult;
		detectedResult.Set(tagSearch.GetClassHierarchy(), tagSearch.GetMethodName(), tagSearch.GetSourceDirs());
		if (DetectedTagFinder.Exec(&detectedResult)) {
			while (detectedResult.More()) {
				detectedResult.Next();
				matches.push_back(detectedResult.Tag);
			}
		}
	}
}

void t4p::TagFinderListClass::NearMatchesFromAll(t4p::TagSearchClass& tagSearch, std::vector<t4p::PhpTagClass>& matches,
		const std::vector<wxFileName>& sourceDirs) {
	if (tagSearch.GetClassName().isEmpty() && tagSearch.GetMethodName().isEmpty() && tagSearch.GetNamespaceName().length() <= 1) {
		// empty query, do not attempt as we dont want to query for all tagsd
		return;
	}
	tagSearch.SetSourceDirs(sourceDirs);
	t4p::TagResultClass* result = tagSearch.CreateNearMatchResults();
	if (IsTagFinderInit && TagFinder.Exec(result)) {
		while (result->More()) {
			result->Next();
			matches.push_back(result->Tag);
		}
	}
	delete result;

	// tags in the native db file do not have a source_id
	// when we query do not use source_id
	std::vector<wxFileName> emptyVector;
	tagSearch.SetSourceDirs(emptyVector);
	result = tagSearch.CreateNearMatchResults();
	if (IsNativeTagFinderInit && NativeTagFinder.Exec(result)) {
		while (result->More()) {
			result->Next();
			matches.push_back(result->Tag);
		}
	}
	delete result;

	tagSearch.SetSourceDirs(sourceDirs);
	if (IsDetectedTagFinderInit && !tagSearch.GetClassName().isEmpty()) {
		t4p::DetectedTagNearMatchMemberResultClass detectedResult;
		detectedResult.Set(tagSearch.GetClassHierarchy(), tagSearch.GetMethodName(), tagSearch.GetSourceDirs());
		if (DetectedTagFinder.Exec(&detectedResult)) {
			while (detectedResult.More()) {
				detectedResult.Next();
				matches.push_back(detectedResult.Tag);
			}
		}
	}
}

void t4p::TagFinderListClass::ExactTraitAliasesFromAll(t4p::TagSearchClass& tagSearch, std::vector<t4p::PhpTagClass>& matches) {
	if (tagSearch.GetClassName().isEmpty()) {
		// no class = impossible to have traits
		return;
	}
	t4p::TraitTagResultClass traitResult;
	traitResult.Set(tagSearch.GetClassHierarchy(), tagSearch.GetMethodName(), true, tagSearch.GetSourceDirs());
	if (IsTagFinderInit && TagFinder.Exec(&traitResult)) {
		std::vector<t4p::PhpTagClass> traitAliases = traitResult.MatchesAsTags();
		for (size_t i = 0; i < traitAliases.size(); ++i) {
			matches.push_back(traitAliases[i]);
		}
	}
	if (IsNativeTagFinderInit && NativeTagFinder.Exec(&traitResult)) {
		std::vector<t4p::PhpTagClass> traitAliases = traitResult.MatchesAsTags();
		for (size_t i = 0; i < traitAliases.size(); ++i) {
			matches.push_back(traitAliases[i]);
		}
	}
}

void t4p::TagFinderListClass::NearMatchTraitAliasesFromAll(t4p::TagSearchClass& tagSearch, std::vector<t4p::PhpTagClass>& matches) {
	if (tagSearch.GetClassName().isEmpty()) {
		// no class = impossible to have traits
		return;
	}
	t4p::TraitTagResultClass traitResult;
	traitResult.Set(tagSearch.GetClassHierarchy(), tagSearch.GetMethodName(), false, tagSearch.GetSourceDirs());
	if (IsTagFinderInit && TagFinder.Exec(&traitResult)) {
		std::vector<t4p::PhpTagClass> traitAliases = traitResult.MatchesAsTags();
		for (size_t i = 0; i < traitAliases.size(); ++i) {
			matches.push_back(traitAliases[i]);
		}
	}
	if (IsNativeTagFinderInit && NativeTagFinder.Exec(&traitResult)) {
		std::vector<t4p::PhpTagClass> traitAliases = traitResult.MatchesAsTags();
		for (size_t i = 0; i < traitAliases.size(); ++i) {
			matches.push_back(traitAliases[i]);
		}
	}
}
