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

mvceditor::TotalTagSearchCompleteEventClass::TotalTagSearchCompleteEventClass(int eventId,
	const UnicodeString& searchString, int lineNumber,
	const std::vector<mvceditor::TotalTagResultClass>& tags) 
: wxEvent(eventId, mvceditor::EVENT_TOTAL_TAG_SEARCH_COMPLETE)
, SearchString(searchString) 
, LineNumber(lineNumber)
, Tags(tags) {
	
}

wxEvent* mvceditor::TotalTagSearchCompleteEventClass::Clone() const {
		return new mvceditor::TotalTagSearchCompleteEventClass(GetId(), SearchString, LineNumber, Tags);
}

mvceditor::TotalTagResultClass::TotalTagResultClass() 
: FileTag()
, PhpTag()
, TableTag() 
, Type(mvceditor::TotalTagResultClass::FILE_TAG) {
}

mvceditor::TotalTagResultClass::TotalTagResultClass(const mvceditor::FileTagClass& fileTag) 
: FileTag(fileTag)
, PhpTag()
, TableTag() 
, Type(mvceditor::TotalTagResultClass::FILE_TAG) {
}

mvceditor::TotalTagResultClass::TotalTagResultClass(const mvceditor::TotalTagResultClass& src) 
: FileTag()
, PhpTag()
, TableTag() 
, Type(mvceditor::TotalTagResultClass::FILE_TAG) {
	Copy(src);
}

mvceditor::TotalTagResultClass::TotalTagResultClass(const mvceditor::TagClass& phpTag) 
: FileTag()
, PhpTag(phpTag)
, TableTag() 
, Type(mvceditor::TotalTagResultClass::CLASS_TAG) {
	if (mvceditor::TagClass::CLASS == PhpTag.Type) {
		Type = mvceditor::TotalTagResultClass::CLASS_TAG;
	}
	else if (mvceditor::TagClass::FUNCTION == PhpTag.Type) {
		Type = mvceditor::TotalTagResultClass::FUNCTION_TAG;
	}
	else if (mvceditor::TagClass::MEMBER == PhpTag.Type) {
		Type = mvceditor::TotalTagResultClass::METHOD_TAG;
	}
	else if (mvceditor::TagClass::METHOD == PhpTag.Type) {
		Type = mvceditor::TotalTagResultClass::METHOD_TAG;
	}
	else if (mvceditor::TagClass::CLASS_CONSTANT == PhpTag.Type) {
		Type = mvceditor::TotalTagResultClass::METHOD_TAG;
	}
	else if (mvceditor::TagClass::DEFINE == PhpTag.Type) {
		Type = mvceditor::TotalTagResultClass::FUNCTION_TAG;
	}
}

mvceditor::TotalTagResultClass::TotalTagResultClass(const mvceditor::DatabaseTableTagClass& tableTag) 
: FileTag()
, PhpTag()
, TableTag(tableTag) 
, Type(mvceditor::TotalTagResultClass::TABLE_DATA_TAG) {

}

void mvceditor::TotalTagResultClass::Copy(const mvceditor::TotalTagResultClass& src) {
	FileTag = src.FileTag;
	PhpTag = src.PhpTag;
	TableTag = src.TableTag;
	Type = src.Type;
}

mvceditor::TotalTagSearchActionClass::TotalTagSearchActionClass(mvceditor::RunningThreadsClass& runningThreads, 
	int eventId) 
: ActionClass(runningThreads, eventId) 
, TagCache()
, SqlTagCache() 
, SearchString()
, SearchDirs() 
, Session() {
	
}

void mvceditor::TotalTagSearchActionClass::SetSearch(mvceditor::GlobalsClass& globals, const wxString& search, const std::vector<wxFileName>& dirs) {

	// deep copy the string, wxString not thread safe
	SearchString = mvceditor::WxToIcu(search);
	SearchDirs = mvceditor::DeepCopyFileNames(dirs);

	mvceditor::TagFinderListClass* cache = new mvceditor::TagFinderListClass;

	// only need to initialize the global tag cache, will not show native tags 
	// because there is no file that needs to be opened
	cache->InitGlobalTag(globals.TagCacheDbFileName, globals.GetPhpFileExtensions(), globals.GetMiscFileExtensions(),
		globals.Environment.Php.Version);
	TagCache.RegisterGlobal(cache);
	
	Session.open(
		*soci::factory_sqlite3(),
		mvceditor::WxToChar(globals.TagCacheDbFileName.GetFullPath())
	);
	SqlTagCache.InitSession(&Session);
}

void mvceditor::TotalTagSearchActionClass::BackgroundWork() {
	bool exactOnly = SearchString.length() <= 2;
	if (IsCancelled()) {
		return;
	}
	std::vector<mvceditor::TotalTagResultClass> matches;

	// do exact match first, if that succeeds then don't bother doing near matches
	mvceditor::TagResultClass* results = TagCache.ExactTags(SearchString, SearchDirs);
	std::vector<mvceditor::TagClass> tags = results->Matches();
	for (size_t i = 0; i < tags.size(); ++i) {
		mvceditor::TotalTagResultClass result(tags[i]);
		matches.push_back(result);
	}
	if (matches.empty() && !exactOnly) {
		delete results;
		results = TagCache.NearMatchTags(SearchString, SearchDirs);
		tags = results->Matches();
		for (size_t i = 0; i < tags.size(); ++i) {
			mvceditor::TotalTagResultClass result(tags[i]);
			matches.push_back(result);
		}
		if (matches.empty()) {
			mvceditor::FileTagResultClass* fileTagResults = TagCache.ExactFileTags(SearchString, SearchDirs);
			std::vector<mvceditor::FileTagClass> files = fileTagResults->Matches();
			for (size_t i = 0; i < files.size(); ++i) {
				mvceditor::TotalTagResultClass result(files[i]);
				matches.push_back(result);
			}
			if (matches.empty()) {
				delete fileTagResults;
				fileTagResults = TagCache.NearMatchFileTags(SearchString, SearchDirs);
				files = fileTagResults->Matches();
				for (size_t i = 0; i < files.size(); ++i) {
					mvceditor::TotalTagResultClass result(files[i]);
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
	mvceditor::ExactSqlResourceTableResultClass exactTableResults;
	exactTableResults.SetLookup(mvceditor::IcuToWx(SearchString), "");
	SqlTagCache.Exec(&exactTableResults);
	while (exactTableResults.More()) {
		mvceditor::DatabaseTableTagClass tableTag;
		tableTag.TableName = mvceditor::CharToWx(exactTableResults.TableName.c_str());
		tableTag.ConnectionHash = mvceditor::CharToWx(exactTableResults.Connection.c_str());
		
		mvceditor::TotalTagResultClass result(tableTag);
		matches.push_back(result);
		
		mvceditor::TotalTagResultClass resultDefinition(tableTag);
		result.Type = mvceditor::TotalTagResultClass::TABLE_DEFINITION_TAG;
		matches.push_back(result);
		exactTableResults.Next();
		foundExactMatchTable = true;
	}
	if (!foundExactMatchTable) {

		// if we did not find an exact match on the table names
		// then do a near match
		mvceditor::SqlResourceTableResultClass tableResults;
		tableResults.SetLookup(mvceditor::IcuToWx(SearchString), "");
		SqlTagCache.Exec(&tableResults);
		while (tableResults.More()) {
			mvceditor::DatabaseTableTagClass tableTag;
			tableTag.TableName = mvceditor::CharToWx(tableResults.TableName.c_str());
			tableTag.ConnectionHash = mvceditor::CharToWx(tableResults.Connection.c_str());
			
			mvceditor::TotalTagResultClass result(tableTag);
			matches.push_back(result);
			
			mvceditor::TotalTagResultClass resultDefinition(tableTag);
			result.Type = mvceditor::TotalTagResultClass::TABLE_DEFINITION_TAG;
			matches.push_back(result);
			tableResults.Next();
		}
	}
	if (!IsCancelled()) {
		mvceditor::TagSearchClass tagSearch(SearchString);

		// PostEvent will set the correct event ID
		mvceditor::TotalTagSearchCompleteEventClass evt(wxID_ANY, SearchString, tagSearch.GetLineNumber(), matches);
		PostEvent(evt);
	}
}
wxString mvceditor::TotalTagSearchActionClass::GetLabel() const {
	return wxT("Total Tag Search");
}

const wxEventType mvceditor::EVENT_TOTAL_TAG_SEARCH_COMPLETE = wxNewEventType();

