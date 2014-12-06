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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <UnitTest++.h>
#include <FileTestFixtureClass.h>
#include <SqliteTestFixtureClass.h>
#include <language/PhpFunctionCallLintClass.h>

class PhpFunctionCallLintTestFixtureClass : 
	public FileTestFixtureClass, public SqliteTestFixtureClass {
	
public:
	
	t4p::PhpFunctionCallLintClass Lint;
	std::vector<t4p::PhpFunctionCallLintResultClass> Results;
	bool HasError;
	
	PhpFunctionCallLintTestFixtureClass()
	: FileTestFixtureClass(wxT("php-function-call-lint"))
	, SqliteTestFixtureClass() 
	, PhpFunctionCallLint()
	, Results()
	, HasError(false) {
		Lint.SetVersion(pelet::PHP_53);
		PhpFileExtensions.push_back(wxT("*.php"));
		CreateSubDirectory(wxT("src"));
	}

	void Parse(const UnicodeString& code) {
		HasError = Lint.ParseString(code, Results);
	}

	void SetupFile(const wxString& fileName, const wxString& contents) {
		CreateFixtureFile(wxT("src") + wxFileName::GetPathSeparators() + fileName, contents);
	}

	void BuildCache(bool includeNativeFunctions = false) {
		
		// make the cache consume the source code file; to prime it with the resources because the
		// function call linter won't work without the cache
		soci::session* session = new soci::session(*soci::factory_sqlite3(), ":memory:");
		CreateDatabase(*session, t4p::ResourceSqlSchemaAsset()); 

		t4p::TagFinderListClass* tagFinderList = new t4p::TagFinderListClass;
		tagFinderList->AdoptGlobalTag(session, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);
		if (includeNativeFunctions) {
			tagFinderList->InitNativeTag(t4p::NativeFunctionsAsset());
		}
		t4p::DirectorySearchClass search;
		search.Init(TestProjectDir + wxT("src"));
		while (search.More()) {
			tagFinderList->Walk(search);
		}
		TagCache.RegisterGlobal(tagFinderList);
		Lint.Init(TagCache);
	}
};

