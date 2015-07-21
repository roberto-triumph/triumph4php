/**
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
 * The MIT License
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

#include "actions/TotalTagSearchActionClass.h"
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <vector>
#include "globals/Assets.h"
#include "globals/FileCabinetItemClass.h"
#include "globals/FileName.h"
#include "globals/TagList.h"
#include "language_php/TagFinderList.h"

t4p::TotalTagSearchCompleteEventClass::TotalTagSearchCompleteEventClass(int eventId,
	const UnicodeString& searchString, int lineNumber,
	const std::vector<t4p::TotalTagResultClass>& tags)
: wxEvent(eventId, t4p::EVENT_TOTAL_TAG_SEARCH_COMPLETE)
, SearchString(searchString)
, LineNumber(lineNumber)
, Tags(tags) {
}

wxEvent* t4p::TotalTagSearchCompleteEventClass::Clone() const {
		return new t4p::TotalTagSearchCompleteEventClass(GetId(), SearchString, LineNumber, Tags);
}

t4p::TotalTagResultClass::TotalTagResultClass()
: FileTag()
, PhpTag()
, TableTag()
, Type(t4p::TotalTagResultClass::FILE_TAG) {
}

t4p::TotalTagResultClass::TotalTagResultClass(const t4p::FileTagClass& fileTag)
: FileTag(fileTag)
, PhpTag()
, TableTag()
, Type(t4p::TotalTagResultClass::FILE_TAG) {
}

t4p::TotalTagResultClass::TotalTagResultClass(const t4p::TotalTagResultClass& src)
: FileTag()
, PhpTag()
, TableTag()
, Type(t4p::TotalTagResultClass::FILE_TAG) {
	Copy(src);
}

t4p::TotalTagResultClass::TotalTagResultClass(const t4p::PhpTagClass& phpTag)
: FileTag()
, PhpTag(phpTag)
, TableTag()
, Type(t4p::TotalTagResultClass::CLASS_TAG) {
	if (t4p::PhpTagClass::CLASS == PhpTag.Type) {
		Type = t4p::TotalTagResultClass::CLASS_TAG;
	} else if (t4p::PhpTagClass::FUNCTION == PhpTag.Type) {
		Type = t4p::TotalTagResultClass::FUNCTION_TAG;
	} else if (t4p::PhpTagClass::MEMBER == PhpTag.Type) {
		Type = t4p::TotalTagResultClass::METHOD_TAG;
	} else if (t4p::PhpTagClass::METHOD == PhpTag.Type) {
		Type = t4p::TotalTagResultClass::METHOD_TAG;
	} else if (t4p::PhpTagClass::CLASS_CONSTANT == PhpTag.Type) {
		Type = t4p::TotalTagResultClass::METHOD_TAG;
	} else if (t4p::PhpTagClass::DEFINE == PhpTag.Type) {
		Type = t4p::TotalTagResultClass::FUNCTION_TAG;
	}
}

t4p::TotalTagResultClass::TotalTagResultClass(const t4p::DatabaseTableTagClass& tableTag)
: FileTag()
, PhpTag()
, TableTag(tableTag)
, Type(t4p::TotalTagResultClass::TABLE_DATA_TAG) {
}

void t4p::TotalTagResultClass::Copy(const t4p::TotalTagResultClass& src) {
	FileTag = src.FileTag;
	PhpTag = src.PhpTag;
	TableTag = src.TableTag;
	Type = src.Type;
}

t4p::TotalTagSearchActionClass::TotalTagSearchActionClass(t4p::RunningThreadsClass& runningThreads,
	int eventId)
: ActionClass(runningThreads, eventId)
, TagCache()
, Session()
, SqlTagCache(Session)
, SearchString()
, SearchDirs() {
}

void t4p::TotalTagSearchActionClass::SetSearch(t4p::GlobalsClass& globals, const wxString& search, const std::vector<wxFileName>& dirs) {
	// deep copy the string, wxString not thread safe
	SearchString = t4p::WxToIcu(search);
	SearchDirs = t4p::DeepCopyFileNames(dirs);

	t4p::TagFinderListClass* cache = new t4p::TagFinderListClass;

	// only need to initialize the global tag cache, will not show native tags
	// because there is no file that needs to be opened
	cache->InitGlobalTag(globals.TagCacheDbFileName, globals.FileTypes.GetPhpFileExtensions(), globals.FileTypes.GetMiscFileExtensions(),
		globals.Environment.Php.Version);
	TagCache.RegisterGlobal(cache);

	Session.open(
		*soci::factory_sqlite3(),
		t4p::WxToChar(globals.TagCacheDbFileName.GetFullPath())
	);
}

void t4p::TotalTagSearchActionClass::BackgroundWork() {
	bool exactOnly = SearchString.length() <= 2;
	if (IsCancelled()) {
		return;
	}
	std::vector<t4p::TotalTagResultClass> matches;

	// do exact match first, if that succeeds then don't bother doing near matches
	bool exactMatches = SearchExact(matches);
	if (!exactMatches && !exactOnly) {
		SearchNearMatch(matches);
	}

	if (!IsCancelled()) {
		t4p::TagSearchClass tagSearch(SearchString);

		// PostEvent will set the correct event ID
		t4p::TotalTagSearchCompleteEventClass evt(wxID_ANY, SearchString, tagSearch.GetLineNumber(), matches);
		PostEvent(evt);
	}
}

wxString t4p::TotalTagSearchActionClass::GetLabel() const {
	return wxT("Total Tag Search");
}

bool t4p::TotalTagSearchActionClass::SearchExact(std::vector<t4p::TotalTagResultClass>& matches) {
	bool found = false;

	t4p::TagResultClass* results = TagCache.ExactTags(SearchString, SearchDirs);
	while (results->More()) {
		results->Next();

		t4p::TotalTagResultClass result(results->Tag);
		matches.push_back(result);
		found = true;
	}
	delete results;

	if (matches.empty()) {
		t4p::FileTagResultClass* fileTagResults = TagCache.ExactFileTags(SearchString, SearchDirs);
		while (fileTagResults->More()) {
			fileTagResults->Next();

			t4p::TotalTagResultClass result(fileTagResults->FileTag);
			matches.push_back(result);
			found = true;
		}
		delete fileTagResults;
	}
	if (matches.empty()) {
		t4p::FileCabinetExactSearchResultClass cabinetExactResults;
		cabinetExactResults.SetName(t4p::IcuToChar(SearchString));
		if (cabinetExactResults.Exec(Session, true)) {
			do {
				cabinetExactResults.Next();
				if (cabinetExactResults.Item.IsFile()) {
					t4p::FileTagClass fileTag;
					fileTag.FullPath = cabinetExactResults.Item.FileName.GetFullPath();

					t4p::TotalTagResultClass result(fileTag);
					matches.push_back(result);
					found = true;
				}
			} while (cabinetExactResults.More());
		}
	}

	// now look for any sql resources (tables)
	// we want to show the tables even if there's a
	// class with the same name
	t4p::ExactSqlResourceTableResultClass exactTableResults;
	exactTableResults.SetLookup(t4p::IcuToWx(SearchString), "");
	SqlTagCache.Exec(&exactTableResults);
	while (exactTableResults.More()) {
		exactTableResults.Next();

		t4p::DatabaseTableTagClass tableTag;
		tableTag.TableName = t4p::CharToWx(exactTableResults.TableName.c_str());
		tableTag.ConnectionHash = t4p::CharToWx(exactTableResults.Connection.c_str());

		t4p::TotalTagResultClass result(tableTag);
		matches.push_back(result);

		t4p::TotalTagResultClass resultDefinition(tableTag);
		result.Type = t4p::TotalTagResultClass::TABLE_DEFINITION_TAG;
		matches.push_back(result);

		found = true;
	}
	return found;
}

bool t4p::TotalTagSearchActionClass::SearchNearMatch(std::vector<t4p::TotalTagResultClass>& matches) {
	bool found = false;
	t4p::TagResultClass* results = TagCache.NearMatchTags(SearchString, SearchDirs);
	while (results->More()) {
		results->Next();

		t4p::TotalTagResultClass result(results->Tag);
		matches.push_back(result);
		found = true;
	}

	delete results;
	if (matches.empty()) {
		t4p::FileTagResultClass* fileTagResults = TagCache.NearMatchFileTags(SearchString, SearchDirs);
		while (fileTagResults->More()) {
			fileTagResults->Next();

			t4p::TotalTagResultClass result(fileTagResults->FileTag);
			matches.push_back(result);
			found = true;
		}
		delete fileTagResults;
	}
	if (matches.empty()) {
		t4p::FileCabinetNearMatchResultClass cabinetNearMatchResults;
		cabinetNearMatchResults.SetName(t4p::IcuToChar(SearchString));
		if (cabinetNearMatchResults.Exec(Session, true)) {
			do {
				cabinetNearMatchResults.Next();
				if (cabinetNearMatchResults.Item.IsFile()) {
					t4p::FileTagClass fileTag;
					fileTag.FullPath = cabinetNearMatchResults.Item.FileName.GetFullPath();

					t4p::TotalTagResultClass result(fileTag);
					matches.push_back(result);

					found = true;
				}
			} while (cabinetNearMatchResults.More());
		}
	}

	// now look for any sql resources (tables)
	// we want to

	// if we did not find an exact match on the table names
	// then do a near match table names even if we
	// find classes
	t4p::SqlResourceTableResultClass tableResults;
	tableResults.SetLookup(t4p::IcuToWx(SearchString), "");
	SqlTagCache.Exec(&tableResults);
	while (tableResults.More()) {
		tableResults.Next();

		t4p::DatabaseTableTagClass tableTag;
		tableTag.TableName = t4p::CharToWx(tableResults.TableName.c_str());
		tableTag.ConnectionHash = t4p::CharToWx(tableResults.Connection.c_str());

		t4p::TotalTagResultClass result(tableTag);
		matches.push_back(result);

		t4p::TotalTagResultClass resultDefinition(tableTag);
		result.Type = t4p::TotalTagResultClass::TABLE_DEFINITION_TAG;
		matches.push_back(result);

		found = true;
	}

	return found;
}

const wxEventType t4p::EVENT_TOTAL_TAG_SEARCH_COMPLETE = wxNewEventType();

