/**
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
 *
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */

#include <actions/TotalTagSearchActionClass.h>
#include <globals/FileName.h>
#include <globals/Assets.h>
#include <language/TagFinderList.h>
#include <globals/TagList.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

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

t4p::TotalTagResultClass::TotalTagResultClass(const t4p::TagClass& phpTag) 
: FileTag()
, PhpTag(phpTag)
, TableTag() 
, Type(t4p::TotalTagResultClass::CLASS_TAG) {
	if (t4p::TagClass::CLASS == PhpTag.Type) {
		Type = t4p::TotalTagResultClass::CLASS_TAG;
	}
	else if (t4p::TagClass::FUNCTION == PhpTag.Type) {
		Type = t4p::TotalTagResultClass::FUNCTION_TAG;
	}
	else if (t4p::TagClass::MEMBER == PhpTag.Type) {
		Type = t4p::TotalTagResultClass::METHOD_TAG;
	}
	else if (t4p::TagClass::METHOD == PhpTag.Type) {
		Type = t4p::TotalTagResultClass::METHOD_TAG;
	}
	else if (t4p::TagClass::CLASS_CONSTANT == PhpTag.Type) {
		Type = t4p::TotalTagResultClass::METHOD_TAG;
	}
	else if (t4p::TagClass::DEFINE == PhpTag.Type) {
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
, SqlTagCache() 
, SearchString()
, SearchDirs() 
, Session() {
	
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
	SqlTagCache.InitSession(&Session);
}

void t4p::TotalTagSearchActionClass::BackgroundWork() {
	bool exactOnly = SearchString.length() <= 2;
	if (IsCancelled()) {
		return;
	}
	std::vector<t4p::TotalTagResultClass> matches;

	// do exact match first, if that succeeds then don't bother doing near matches
	t4p::TagResultClass* results = TagCache.ExactTags(SearchString, SearchDirs);
	std::vector<t4p::TagClass> tags = results->Matches();
	for (size_t i = 0; i < tags.size(); ++i) {
		t4p::TotalTagResultClass result(tags[i]);
		matches.push_back(result);
	}
	if (matches.empty() && !exactOnly) {
		delete results;
		results = TagCache.NearMatchTags(SearchString, SearchDirs);
		tags = results->Matches();
		for (size_t i = 0; i < tags.size(); ++i) {
			t4p::TotalTagResultClass result(tags[i]);
			matches.push_back(result);
		}
		if (matches.empty()) {
			t4p::FileTagResultClass* fileTagResults = TagCache.ExactFileTags(SearchString, SearchDirs);
			std::vector<t4p::FileTagClass> files = fileTagResults->Matches();
			for (size_t i = 0; i < files.size(); ++i) {
				t4p::TotalTagResultClass result(files[i]);
				matches.push_back(result);
			}
			if (matches.empty()) {
				delete fileTagResults;
				fileTagResults = TagCache.NearMatchFileTags(SearchString, SearchDirs);
				files = fileTagResults->Matches();
				for (size_t i = 0; i < files.size(); ++i) {
					t4p::TotalTagResultClass result(files[i]);
					matches.push_back(result);
				}
			}
			delete fileTagResults;
		}
	}
	delete results;
	
	// now look for any sql resources (tables)
	// do an exact match first
	bool foundExactMatchTable = false;
	t4p::ExactSqlResourceTableResultClass exactTableResults;
	exactTableResults.SetLookup(t4p::IcuToWx(SearchString), "");
	SqlTagCache.Exec(&exactTableResults);
	while (exactTableResults.More()) {
		t4p::DatabaseTableTagClass tableTag;
		tableTag.TableName = t4p::CharToWx(exactTableResults.TableName.c_str());
		tableTag.ConnectionHash = t4p::CharToWx(exactTableResults.Connection.c_str());
		
		t4p::TotalTagResultClass result(tableTag);
		matches.push_back(result);
		
		t4p::TotalTagResultClass resultDefinition(tableTag);
		result.Type = t4p::TotalTagResultClass::TABLE_DEFINITION_TAG;
		matches.push_back(result);
		exactTableResults.Next();
		foundExactMatchTable = true;
	}
	if (!foundExactMatchTable) {

		// if we did not find an exact match on the table names
		// then do a near match
		t4p::SqlResourceTableResultClass tableResults;
		tableResults.SetLookup(t4p::IcuToWx(SearchString), "");
		SqlTagCache.Exec(&tableResults);
		while (tableResults.More()) {
			t4p::DatabaseTableTagClass tableTag;
			tableTag.TableName = t4p::CharToWx(tableResults.TableName.c_str());
			tableTag.ConnectionHash = t4p::CharToWx(tableResults.Connection.c_str());
			
			t4p::TotalTagResultClass result(tableTag);
			matches.push_back(result);
			
			t4p::TotalTagResultClass resultDefinition(tableTag);
			result.Type = t4p::TotalTagResultClass::TABLE_DEFINITION_TAG;
			matches.push_back(result);
			tableResults.Next();
		}
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

const wxEventType t4p::EVENT_TOTAL_TAG_SEARCH_COMPLETE = wxNewEventType();

