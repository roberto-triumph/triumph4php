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
#include <language/TagCacheClass.h>
#include <language/TagFinderList.h>
#include <language/DetectedTagFinderResultClass.h>
#include <globals/Assets.h>
#include <globals/Sqlite.h>
#include <globals/GlobalsClass.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

#include <algorithm>

/**
 * Fills completeStatus with a human-friendly version of the symbol table error
 */
static void HandleAutoCompletionPhpStatus(const t4p::SymbolTableMatchErrorClass& error, 
	const UnicodeString& lastExpression, const pelet::VariableClass& parsedVariable,
	const pelet::ScopeClass& variableScope, 
	const std::vector<t4p::TagClass>& autoCompletionResourceMatches,
	wxString& completeStatus) {
	if (lastExpression.isEmpty()) {
		completeStatus = _("Nothing to complete");
	}
	else if (lastExpression.startsWith(UNICODE_STRING_SIMPLE("$")) && parsedVariable.ChainList.size() <= 1) {
		completeStatus = _("No matching variables for: ");
		completeStatus += t4p::IcuToWx(lastExpression);
		completeStatus +=  _(" in scope: ");
		completeStatus += t4p::IcuToWx(variableScope.ClassName);
		completeStatus += _("::");
		completeStatus += t4p::IcuToWx(variableScope.MethodName);
	}
	else if (parsedVariable.ChainList.size() == 1) {
		completeStatus = _("No matching class, function, define, or keyword for: \"");
		completeStatus += t4p::IcuToWx(lastExpression);
		completeStatus += wxT("\"");
	}
	else if (autoCompletionResourceMatches.empty()) {
		if (t4p::SymbolTableMatchErrorClass::PARENT_ERROR == error.Type) {
			completeStatus = _("parent not valid for scope: ");
			completeStatus += t4p::IcuToWx(variableScope.ClassName);
			completeStatus += _("::");
			completeStatus += t4p::IcuToWx(variableScope.MethodName);
		}
		else if (t4p::SymbolTableMatchErrorClass::STATIC_ERROR == error.Type) {
			completeStatus = _("Cannot access protected or private static member \"");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
			completeStatus += _("\" in class: ");
			completeStatus += t4p::IcuToWx(error.ErrorClass);
		}
		else if (t4p::SymbolTableMatchErrorClass::TYPE_RESOLUTION_ERROR == error.Type) {
			completeStatus = _("Could not resolve type for \"");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
			completeStatus += wxT("\"");
		}
		else if (t4p::SymbolTableMatchErrorClass::UNKNOWN_RESOURCE == error.Type) {
			if (!parsedVariable.ChainList.empty() &&
				parsedVariable.ChainList[0].Name == UNICODE_STRING_SIMPLE("$this")) {
				completeStatus = _("No public, protected, or private member matches for \"");
			}
			else {
				completeStatus = _("No public member matches for \"");
			}
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
			completeStatus += _("\" in class: ");
			completeStatus += t4p::IcuToWx(error.ErrorClass);
		}
		else if (t4p::SymbolTableMatchErrorClass::UNKNOWN_STATIC_RESOURCE == error.Type) {
			completeStatus = _("No static member matches for \"");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
			completeStatus += _("\" in class: ");
			completeStatus += t4p::IcuToWx(error.ErrorClass);
		}
		else if (t4p::SymbolTableMatchErrorClass::VISIBILITY_ERROR == error.Type) {
			completeStatus = _("Cannot access protected or private member \"");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
			completeStatus += _("\" in class: ");
			completeStatus += t4p::IcuToWx(error.ErrorClass);
		}
		else if (t4p::SymbolTableMatchErrorClass::ARRAY_ERROR == error.Type && !error.ErrorClass.isEmpty()) {
			completeStatus = _("Cannot use object operator for array returned by \"");
			completeStatus += t4p::IcuToWx(error.ErrorClass);
			completeStatus += _("::");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
		}
		else if (t4p::SymbolTableMatchErrorClass::ARRAY_ERROR == error.Type) {
			completeStatus = _("Cannot use object operator for array variable ");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
		}
		else if (t4p::SymbolTableMatchErrorClass::PRIMITIVE_ERROR == error.Type && !error.ErrorClass.isEmpty()) {
			completeStatus = _("Cannot use object operator for primitive returned by \"");
			completeStatus += t4p::IcuToWx(error.ErrorClass);
			completeStatus += _("::");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
		}
		else if (t4p::SymbolTableMatchErrorClass::PRIMITIVE_ERROR == error.Type) {
			completeStatus = _("Cannot use object operator for primitive variable \"");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
		}
	}
}

t4p::WorkingCacheCompleteEventClass::WorkingCacheCompleteEventClass(int eventId, 
																		  const wxString& fileName,
																		  const wxString& fileIdentifier, t4p::WorkingCacheClass* cache)
	: wxEvent(eventId, t4p::EVENT_WORKING_CACHE_COMPLETE)
	, WorkingCache(cache) 
	, FileName(fileName.c_str())
	, FileIdentifier(fileIdentifier.c_str()) {

}

wxEvent* t4p::WorkingCacheCompleteEventClass::Clone() const {
	t4p::WorkingCacheCompleteEventClass* evt = new t4p::WorkingCacheCompleteEventClass(
			GetId(), FileName, FileIdentifier, WorkingCache);
	return evt;
}

wxString t4p::WorkingCacheCompleteEventClass::GetFileIdentifier() const {
	return FileIdentifier;
}

wxString t4p::WorkingCacheCompleteEventClass::GetFileName() const {
	return FileName;
}

t4p::TagFinderListCompleteEventClass::TagFinderListCompleteEventClass(int eventId)
	: wxEvent(eventId, t4p::EVENT_TAG_FINDER_LIST_COMPLETE) {

}

wxEvent* t4p::TagFinderListCompleteEventClass::Clone() const {
	t4p::TagFinderListCompleteEventClass* evt = new t4p::TagFinderListCompleteEventClass(GetId());
	return evt;
}

const wxEventType t4p::EVENT_WORKING_CACHE_COMPLETE = wxNewEventType();
const wxEventType t4p::EVENT_TAG_FINDER_LIST_COMPLETE = wxNewEventType();

t4p::WorkingCacheClass::WorkingCacheClass()
	: SymbolTable()
	, FileName() 
	, IsNew(true)
	, Parser() {

}

bool t4p::WorkingCacheClass::Update(const UnicodeString& code) {
	pelet::LintResultsClass results;

	// check for syntax so that only 'good' code modifies the cache
	// allow empty code to be valid; that way code completion works
	// on newly created files
	bool ret = false;
	if (code.isEmpty() || Parser.LintString(code, results)) {
		SymbolTable.CreateSymbols(code);
		ret = true;
	}
	return ret;
}

void t4p::WorkingCacheClass::Init(const wxString& fileName, 
										const wxString& fileIdentifier, bool isNew, pelet::Versions version, bool createSymbols) {
	FileName = fileName;
	FileIdentifier = fileIdentifier;
	IsNew = isNew;
	Parser.SetVersion(version);
	SymbolTable.SetVersion(version);
	if (createSymbols) {
		SymbolTable.CreateSymbolsFromFile(fileName);
	}
}

t4p::TagCacheClass::TagCacheClass()
	: TagFinderList(NULL)
	, WorkingCaches() {
	
}

t4p::TagCacheClass::~TagCacheClass() {
	Clear();
 }

bool t4p::TagCacheClass::RegisterWorking(const wxString& fileName, t4p::WorkingCacheClass* cache) {
	bool ret = false;

	// careful to not overwrite the symbol table, tag finder pointers
	std::map<wxString, t4p::WorkingCacheClass*>::iterator it = WorkingCaches.find(fileName);
	if (it == WorkingCaches.end()) {
		WorkingCaches[fileName] = cache;
		ret = true;
	}
	return ret;
}

bool t4p::TagCacheClass::ReplaceWorking(const wxString& fileName, t4p::WorkingCacheClass* cache) {
	RemoveWorking(fileName);
	return RegisterWorking(fileName, cache);
}

void t4p::TagCacheClass::RemoveWorking(const wxString& fileName) {
	std::map<wxString, t4p::WorkingCacheClass*>::iterator it = WorkingCaches.find(fileName);
	if (it != WorkingCaches.end()) {
		delete it->second;
		WorkingCaches.erase(it);
	}
}

void t4p::TagCacheClass::RegisterGlobal(t4p::TagFinderListClass* cache) {
	TagFinderList = cache;
}

void t4p::TagCacheClass::RegisterDefault(t4p::GlobalsClass& globals) {
	t4p::TagFinderListClass* cache = new t4p::TagFinderListClass;
	cache->InitGlobalTag(globals.TagCacheDbFileName, globals.FileTypes.GetPhpFileExtensions(), globals.FileTypes.GetMiscFileExtensions(),
		globals.Environment.Php.Version);
	cache->InitNativeTag(t4p::NativeFunctionsAsset());
	cache->InitDetectorTag(globals.DetectorCacheDbFileName);
	RegisterGlobal(cache);
}

t4p::TagResultClass* t4p::TagCacheClass::ExactTags(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs) {
	t4p::TagSearchClass tagSearch(search);
	tagSearch.SetSourceDirs(sourceDirs);
	t4p::TagResultClass* result = tagSearch.CreateExactResults();
	TagFinderList->TagFinder.Exec(result);
	return result;
}

t4p::TagResultClass* t4p::TagCacheClass::ExactNativeTags(const UnicodeString& search) {
	t4p::TagSearchClass tagSearch(search);
	t4p::TagResultClass* result = tagSearch.CreateExactResults();
	TagFinderList->NativeTagFinder.Exec(result);
	return result;
}


t4p::DetectedTagExactMemberResultClass* t4p::TagCacheClass::ExactDetectedTags(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs) {
	t4p::TagSearchClass tagSearch(search);
	t4p::DetectedTagExactMemberResultClass* result = new t4p::DetectedTagExactMemberResultClass();
	std::vector<UnicodeString> classNames;
	classNames.push_back(tagSearch.GetClassName());
	result->Set(classNames, tagSearch.GetMethodName(), sourceDirs);
	TagFinderList->DetectedTagFinder.Exec(result);
	return result;
}

t4p::TagResultClass* t4p::TagCacheClass::NearMatchTags(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs) {
	t4p::TagSearchClass tagSearch(search);
	tagSearch.SetSourceDirs(sourceDirs);

	t4p::TagResultClass* result = tagSearch.CreateNearMatchResults();
	TagFinderList->TagFinder.Exec(result);
	return result;
}

t4p::DetectedTagNearMatchMemberResultClass* t4p::TagCacheClass::NearMatchDetectedTags(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs) {
	t4p::TagSearchClass tagSearch(search);
	tagSearch.SetSourceDirs(sourceDirs);

	t4p::DetectedTagNearMatchMemberResultClass* result = new t4p::DetectedTagNearMatchMemberResultClass();
	std::vector<UnicodeString> classNames;
	classNames.push_back(tagSearch.GetClassName());
	result->Set(classNames, tagSearch.GetMethodName(), sourceDirs);
	TagFinderList->DetectedTagFinder.Exec(result);
	return result;
}

t4p::TagResultClass* t4p::TagCacheClass::NearMatchNativeTags(const UnicodeString& search) {
	t4p::TagSearchClass tagSearch(search);

	t4p::TagResultClass* result = tagSearch.CreateNearMatchResults();
	TagFinderList->NativeTagFinder.Exec(result);
	return result;
}

t4p::FileTagResultClass* t4p::TagCacheClass::ExactFileTags(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs) {
	t4p::TagSearchClass tagSearch(search);
	tagSearch.SetSourceDirs(sourceDirs);

	t4p::FileTagResultClass* result = tagSearch.CreateExactFileResults();
	TagFinderList->TagFinder.Exec(result);
	return result;
}

t4p::FileTagResultClass* t4p::TagCacheClass::NearMatchFileTags(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs) {
	t4p::TagSearchClass tagSearch(search);
	tagSearch.SetSourceDirs(sourceDirs);

	t4p::FileTagResultClass* result = tagSearch.CreateNearMatchFileResults();
	TagFinderList->TagFinder.Exec(result);
	return result;
}

std::vector<t4p::TagClass> t4p::TagCacheClass::ExactClassOrFile(const UnicodeString& search) {
	std::vector<t4p::TagClass> matches;
	t4p::TagSearchClass tagSearch(search);

	std::vector<t4p::ParsedTagFinderClass*> finders = AllFinders();
	for (size_t i = 0; i < finders.size(); ++i) {
		t4p::ParsedTagFinderClass* tagFinder = finders[i];
		std::vector<t4p::TagClass> finderMatches = tagFinder->ExactClassOrFile(tagSearch);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			t4p::TagClass tag = finderMatches[j];
			matches.push_back(tag);
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<t4p::TagClass> t4p::TagCacheClass::ExactClass(const UnicodeString& search) {
	std::vector<t4p::TagClass> matches;
	t4p::TagSearchClass tagSearch(search);

	std::vector<t4p::ParsedTagFinderClass*> finders = AllFinders();
	for (size_t i = 0; i < finders.size(); ++i) {
		t4p::ParsedTagFinderClass* tagFinder = finders[i];
		std::vector<t4p::TagClass> finderMatches = tagFinder->ExactClass(tagSearch);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			t4p::TagClass tag = finderMatches[j];
			matches.push_back(tag);
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<t4p::TagClass> t4p::TagCacheClass::ExactFunction(const UnicodeString& search) {
	std::vector<t4p::TagClass> matches;
	t4p::TagSearchClass tagSearch(search);

	std::vector<t4p::ParsedTagFinderClass*> finders = AllFinders();
	for (size_t i = 0; i < finders.size(); ++i) {
		t4p::ParsedTagFinderClass* tagFinder = finders[i];
		std::vector<t4p::TagClass> finderMatches = tagFinder->ExactFunction(tagSearch);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			t4p::TagClass tag = finderMatches[j];
			matches.push_back(tag);
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<t4p::TagClass> t4p::TagCacheClass::ExactMethod(const UnicodeString& search, bool onlyStatic) {
	std::vector<t4p::TagClass> matches;

	// method search is activated by not giving a class name
	t4p::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("::") + search);

	std::vector<t4p::ParsedTagFinderClass*> finders = AllFinders();
	for (size_t i = 0; i < finders.size(); ++i) {
		t4p::ParsedTagFinderClass* tagFinder = finders[i];
		std::vector<t4p::TagClass> finderMatches = tagFinder->ExactMethod(tagSearch, onlyStatic);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			t4p::TagClass tag = finderMatches[j];
			matches.push_back(tag);
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<t4p::TagClass> t4p::TagCacheClass::ExactProperty(const UnicodeString& search, bool onlyStatic) {
	std::vector<t4p::TagClass> matches;

	// method search is activated by not giving a class name
	t4p::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("::") + search);

	std::vector<t4p::ParsedTagFinderClass*> finders = AllFinders();
	for (size_t i = 0; i < finders.size(); ++i) {
		t4p::ParsedTagFinderClass* tagFinder = finders[i];
		std::vector<t4p::TagClass> finderMatches = tagFinder->ExactProperty(tagSearch, onlyStatic);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			t4p::TagClass tag = finderMatches[j];
			matches.push_back(tag);
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<t4p::TagClass> t4p::TagCacheClass::NearMatchClassesOrFiles(const UnicodeString& search) {
	std::vector<t4p::TagClass> matches;
	t4p::TagSearchClass tagSearch(search);

	// return all of the matches from all finders that were found by the Collect* call.
	// This is a bit tricky because we want to prioritize matches in opened files 
	// instead of the global finder, since the global finder will be outdated.
	std::vector<t4p::ParsedTagFinderClass*> finders = AllFinders();
	for (size_t i = 0; i < finders.size(); ++i) {
		t4p::ParsedTagFinderClass* tagFinder = finders[i];
		std::vector<t4p::TagClass> finderMatches = tagFinder->NearMatchClassesOrFiles(tagSearch);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			t4p::TagClass tag = finderMatches[j];
			matches.push_back(tag);
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<t4p::ParsedTagFinderClass*> t4p::TagCacheClass::AllFinders() {
	std::vector<t4p::ParsedTagFinderClass*> allTagFinders;
	if (TagFinderList) {
		if (TagFinderList->IsNativeTagFinderInit) {
			allTagFinders.push_back(&TagFinderList->NativeTagFinder);
		}
		if (TagFinderList->IsTagFinderInit) {
			allTagFinders.push_back(&TagFinderList->TagFinder);
		}
	}
	return allTagFinders;
}

void t4p::TagCacheClass::ExpressionCompletionMatches(const wxString& fileName, 
													const pelet::VariableClass& parsedVariable, 
													const pelet::ScopeClass& variableScope, 
													const std::vector<wxFileName>& sourceDirs,
													 std::vector<UnicodeString>& autoCompleteList,
													 std::vector<t4p::TagClass>& resourceMatches,
													 bool doDuckTyping,
													 t4p::SymbolTableMatchErrorClass& error) {
	std::map<wxString, t4p::WorkingCacheClass*>::const_iterator itWorkingCache = WorkingCaches.find(fileName);
	bool foundSymbolTable = false;
	if (itWorkingCache != WorkingCaches.end()) {
		foundSymbolTable = true;
		t4p::WorkingCacheClass* cache = itWorkingCache->second;
		cache->SymbolTable.ExpressionCompletionMatches(parsedVariable, variableScope, sourceDirs, *TagFinderList, 
			autoCompleteList, resourceMatches, doDuckTyping, error);
	
	}
	if (!foundSymbolTable) {
		error.Type = t4p::SymbolTableMatchErrorClass::UNREGISTERED_FILE;
	}
}

void t4p::TagCacheClass::ResourceMatches(const wxString& fileName, 
												const pelet::VariableClass& parsedVariable,
												const pelet::ScopeClass& variableScope, 
												const std::vector<wxFileName>& sourceDirs,
												std::vector<t4p::TagClass>& matches,
												bool doDuckTyping, bool doFullyQualifiedMatchOnly,
												t4p::SymbolTableMatchErrorClass& error) {
	std::map<wxString, t4p::WorkingCacheClass*>::const_iterator itWorkingCache = WorkingCaches.find(fileName);
	bool foundSymbolTable = false;
	if (itWorkingCache != WorkingCaches.end()) {
		foundSymbolTable = true;
		t4p::WorkingCacheClass* cache = itWorkingCache->second;
		cache->SymbolTable.ResourceMatches(parsedVariable, variableScope, sourceDirs, *TagFinderList, 
			matches, doDuckTyping, doFullyQualifiedMatchOnly, error);	
	}
	if (!foundSymbolTable) {
		error.Type = t4p::SymbolTableMatchErrorClass::UNREGISTERED_FILE;
	}
}

void t4p::TagCacheClass::Print() {
	UFILE* ufout = u_finit(stdout, NULL, NULL);
	u_fprintf(ufout, "Number of working caches: %d\n", WorkingCaches.size());
	u_fclose(ufout);
	std::map<wxString, t4p::WorkingCacheClass*>::const_iterator it = WorkingCaches.begin();
	for (; it != WorkingCaches.end(); ++it) {
		it->second->SymbolTable.Print();
	}
}

bool t4p::TagCacheClass::IsFileCacheEmpty() {

	// if at least one tag finder is not empty, return false
	// no need to check native tag cache, as that is always not empty
	if (TagFinderList && TagFinderList->IsTagFinderInit && !TagFinderList->TagFinder.IsFileCacheEmpty()) {
		return false;
	}
	return true;
}

bool t4p::TagCacheClass::IsResourceCacheEmpty() {

	// if at least one tag finder is not empty, return false
	if (TagFinderList && TagFinderList->IsNativeTagFinderInit && !TagFinderList->NativeTagFinder.IsResourceCacheEmpty()) {
		return false;
	}
	if (TagFinderList && TagFinderList->IsTagFinderInit && !TagFinderList->TagFinder.IsResourceCacheEmpty()) {
		return false;
	}
	if (TagFinderList && TagFinderList->IsDetectedTagFinderInit) {
		t4p::DetectedTagTotalCountResultClass result;
		TagFinderList->DetectedTagFinder.Exec(&result);
		if (result.GetTotalCount() <= 0) {
			return false;
		}
	}
	return true;
}

void t4p::TagCacheClass::Clear() {

	// ATTN: do NOT wipe finders, Clear() is meant for memory
	// cleanup only
	if (TagFinderList) {
		delete TagFinderList;
		TagFinderList	= NULL;
	}

	std::map<wxString, t4p::WorkingCacheClass*>::iterator itWorking;
	for (itWorking = WorkingCaches.begin(); itWorking != WorkingCaches.end(); ++itWorking) {
		delete itWorking->second;
	}
	WorkingCaches.clear();
}

std::vector<t4p::TagClass> t4p::TagCacheClass::AllMemberTags(const UnicodeString& fullyQualifiedClassName, int fileTagId, std::vector<wxFileName>& sourceDirs) {
	std::vector<t4p::TagClass> allMatches;

	// add the double colon so that we search for all members
	// first search for all members of the given class that is also in the given file
	t4p::TagSearchClass tagSearch(fullyQualifiedClassName + UNICODE_STRING_SIMPLE("::"));
	tagSearch.SetFileItemId(fileTagId);
	tagSearch.SetTraits(TagFinderList->ClassUsedTraits(fullyQualifiedClassName, tagSearch.GetParentClasses(), 
			tagSearch.GetMethodName(), sourceDirs));
	
	TagFinderList->NearMatchesFromAll(tagSearch, allMatches, sourceDirs);
	
	// now get all parent class  (look in all files) also look for inherited members and traits
	UnicodeString parentClassName  = TagFinderList->ParentClassName(fullyQualifiedClassName, fileTagId);
	if (!parentClassName.isEmpty()) {
		t4p::TagSearchClass hierarchySearch(parentClassName + UNICODE_STRING_SIMPLE("::"));
		
		hierarchySearch.SetParentClasses(TagFinderList->ClassParents(parentClassName, hierarchySearch.GetMethodName()));
		hierarchySearch.SetTraits(TagFinderList->ClassUsedTraits(parentClassName, hierarchySearch.GetParentClasses(), 
			hierarchySearch.GetMethodName(), sourceDirs));
		
		// search classes from the enabled source directories only
		hierarchySearch.SetSourceDirs(sourceDirs);
		
		TagFinderList->NearMatchesFromAll(hierarchySearch, allMatches, sourceDirs);
		TagFinderList->NearMatchTraitAliasesFromAll(hierarchySearch, allMatches);
	}
	return allMatches;
}

std::vector<t4p::TagClass> t4p::TagCacheClass::AllClassesFunctionsDefines(const wxString& fullPath) {
	std::vector<t4p::TagClass> allMatches;

	std::vector<t4p::ParsedTagFinderClass*> allTagFinders = AllFinders();
	for (size_t j = 0; j < allTagFinders.size(); ++j) {
		t4p::ParsedTagFinderClass* finder = allTagFinders[j];
		allMatches = finder->ClassesFunctionsDefines(fullPath);
		if (!allMatches.empty()) {

			// even if the file tag is is multiple finders, they should both be the 
			// same. stop when we find anything
			break;	
		}
	}
	return allMatches;
}

std::vector<UnicodeString> t4p::TagCacheClass::ParentClassesAndTraits(const UnicodeString& className, const std::vector<wxFileName>& sourceDirs) {
	std::vector<UnicodeString> classParents = TagFinderList->ClassParents(className, UNICODE_STRING_SIMPLE(""));
	std::vector<UnicodeString> classTraits = TagFinderList->ClassUsedTraits(className, classParents, UNICODE_STRING_SIMPLE(""), sourceDirs);
	
	std::vector<UnicodeString> all;
	all.insert(all.end(), classParents.begin(), classParents.end());
	all.insert(all.end(), classTraits.begin(), classTraits.end());

	std::sort(all.begin(), all.end());
	std::vector<UnicodeString>::iterator it = std::unique(all.begin(), all.end());
	all.erase(it, all.end());
	return all;
}

bool t4p::TagCacheClass::FindById(int id, t4p::TagClass& tag) {
	bool found = false;
	if (TagFinderList->IsTagFinderInit) {
		found = TagFinderList->TagFinder.FindById(id, tag);
	}
	return found;
}

void t4p::TagCacheClass::DeleteFromFile(const wxString& fullPath) {
	if (TagFinderList->IsTagFinderInit) {
		TagFinderList->TagParser.DeleteFromFile(fullPath);
	}
}

bool t4p::TagCacheClass::HasFullPath(const wxString& fullPath) {
	bool found  = false;
	if (TagFinderList->IsTagFinderInit) {
		found = TagFinderList->TagFinder.HasFullPath(fullPath);
	}	
	return found;
}

bool t4p::TagCacheClass::HasDir(const wxString& dir) {
	bool found  = false;
	if (TagFinderList->IsTagFinderInit) {
		found = TagFinderList->TagFinder.HasDir(dir);
	}	
	return found;
}

std::vector<t4p::TagClass> t4p::TagCacheClass::GetTagsAtPosition(
		const wxString& fileName, 
		const UnicodeString& code, int posToCheck, 
		const std::vector<wxFileName>& sourceDirs, t4p::GlobalsClass& globals,
		wxString& status) {
	std::vector<t4p::TagClass> matches;
	pelet::LexicalAnalyzerClass lexer;
	pelet::ParserClass parser;
	t4p::ScopeFinderClass scopeFinder;
	pelet::ScopeClass variableScope;
	pelet::VariableClass parsedVariable(variableScope);

	lexer.SetVersion(globals.Environment.Php.Version);
	parser.SetVersion(globals.Environment.Php.Version);
	scopeFinder.SetVersion(globals.Environment.Php.Version);
	
	UnicodeString codeUntilPos(code, 0, posToCheck);
	
	UnicodeString lastExpression = lexer.LastExpression(codeUntilPos);
	UnicodeString resourceName;
	bool doDuckTyping = true;
	if (!lastExpression.isEmpty()) {
		scopeFinder.GetScopeString(codeUntilPos, posToCheck, variableScope);
		if (lastExpression.indexOf(UNICODE_STRING_SIMPLE("\\")) > 0 && 
			variableScope.ClassName.isEmpty() &&
			variableScope.MethodName.isEmpty()) {

			// the expression is a namespace name outside a class or method.  this is 
			// most likely a namespace in the "use" statement
			// namespace in a use statement is always fully qualified, even if it does
			// not begin with a backslash
			lastExpression = UNICODE_STRING_SIMPLE("\\") + lastExpression;
		}
		parser.ParseExpression(lastExpression, parsedVariable);
		t4p::SymbolTableMatchErrorClass error;
		ResourceMatches(fileName, parsedVariable, variableScope, sourceDirs, matches, 
			doDuckTyping, true, error);
		if (matches.empty()) {
			HandleAutoCompletionPhpStatus(error, lastExpression, parsedVariable, 
				variableScope, matches, status);
		}
	}
	return matches;
}