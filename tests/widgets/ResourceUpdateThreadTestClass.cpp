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
#include <UnitTest++.h>
#include <widgets/ResourceUpdateThreadClass.h>
#include "unicode/ustream.h" //get the << overloaded operator, needed by UnitTest++

class ExpressionCompletionMatchesFixtureClass  {

public:

	mvceditor::ResourceUpdateClass ResourceUpdates;
	wxString File1;
	wxString File2;
	wxString File3;
	UnicodeString Code1;
	UnicodeString Code2;
	UnicodeString Code3;
	mvceditor::ResourceFinderClass GlobalFinder;
	mvceditor::SymbolClass ParsedExpression;

	std::vector<UnicodeString> VariableMatches;
	std::vector<mvceditor::ResourceClass> ResourceMatches;
	mvceditor::SymbolTableMatchErrorClass Error;
	
	
	ExpressionCompletionMatchesFixtureClass() 
		: ResourceUpdates()
		, File1(wxT("file1.php"))
		, File2(wxT("file2.php"))
		, File3(wxT("file3.php"))
		, Code1()
		, Code2()
		, Code3()
		, GlobalFinder()
		, ParsedExpression()
		, VariableMatches()
		, ResourceMatches()
		, Error() {
	}
};

SUITE(ResourceUpdateThreadTestClass) {

TEST(RegisterShouldSucceed) {	
	wxString fileName = wxT("MyFile.php");
	mvceditor::ResourceUpdateClass resourceUpdates;
	CHECK(resourceUpdates.Register(fileName));
}

TEST(RegisterShouldFail) {	
	wxString fileName = wxT("MyFile.php");
	mvceditor::ResourceUpdateClass resourceUpdates;
}

TEST(RegisterShouldSucceedAfterSucceedAfterUnregistering) {	
	wxString fileName = wxT("MyFile.php");
	mvceditor::ResourceUpdateClass resourceUpdates;
	CHECK(resourceUpdates.Register(fileName));
	resourceUpdates.Unregister(fileName);
	CHECK(resourceUpdates.Register(fileName));
}

TEST(CollectShouldGetFromAllFinders) {
	
	// going to create 3 'files'
	mvceditor::ResourceUpdateClass resourceUpdates;
	wxString file1 = wxT("file1.php");
	wxString file2 = wxT("file2.php");
	wxString file3 = wxT("file3.php");
	UnicodeString code1 = UNICODE_STRING_SIMPLE("<?php class ActionMy   { function w() {} }");
	UnicodeString code2 = UNICODE_STRING_SIMPLE("<?php class ActionYou  { function w() {} }");
	UnicodeString code3 = UNICODE_STRING_SIMPLE("<?php class ActionThey { function w() {} }");
	
	// parse the 3 files for resources
	CHECK(resourceUpdates.Register(file1));
	CHECK(resourceUpdates.Register(file2));
	CHECK(resourceUpdates.Update(file1, code1, true));
	CHECK(resourceUpdates.Update(file2, code2, true));
	mvceditor::ResourceFinderClass globalFinder;
	globalFinder.BuildResourceCacheForFile(file3, code3, true);
	
	// now perform the search. will search for any resource that starts with 'Action'
	// all 3 caches should hit
	CHECK(resourceUpdates.PrepareAll(&globalFinder, wxT("Action")));
	CHECK(resourceUpdates.CollectNearMatchResourcesFromAll(&globalFinder));
	
	
	std::vector<mvceditor::ResourceClass> matches = resourceUpdates.Matches(&globalFinder);
	CHECK_EQUAL((size_t)3, matches.size());
	if (3 == matches.size()) {
		
		// results should be sorted
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("ActionMy"), matches[0].Resource);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("ActionThey"), matches[1].Resource);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("ActionYou"), matches[2].Resource);
	}
}

TEST(CollectShouldIgnoreStaleMatches) {
	
	// create a class in file1 with methodA
	// update file1, remove methodA from class
	// perform a search
	// methodA should not be a hit since it has been removed
	mvceditor::ResourceUpdateClass resourceUpdates;
	wxString file1 = wxT("file1.php");
	UnicodeString code1 = UNICODE_STRING_SIMPLE("<?php class ActionMy   { function methodA() {} }");
	UnicodeString code2 = UNICODE_STRING_SIMPLE("<?php class ActionMy   { function methodB() {} }");

	mvceditor::ResourceFinderClass globalFinder;
	globalFinder.BuildResourceCacheForFile(file1, code1, true);
	CHECK(resourceUpdates.Register(file1));
	CHECK(resourceUpdates.Update(file1, code2, true));

	CHECK(resourceUpdates.PrepareAll(&globalFinder, wxT("ActionMy::methodA")));
	CHECK(resourceUpdates.CollectNearMatchResourcesFromAll(&globalFinder));
	std::vector<mvceditor::ResourceClass> matches = resourceUpdates.Matches(&globalFinder);
	CHECK_EQUAL((size_t)0, matches.size());

	CHECK(resourceUpdates.PrepareAll(&globalFinder, wxT("ActionMy::methodB")));
	CHECK(resourceUpdates.CollectNearMatchResourcesFromAll(&globalFinder));
	matches = resourceUpdates.Matches(&globalFinder);
	CHECK_EQUAL((size_t)1, matches.size());
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, GlobalFinder) {
	
	// in this test we will create a class in file2; file1 will use that class
	// the ResourceUpdate object should be able to detect the variable type of 
	// the variable in file1
	Code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");
	Code2 = UNICODE_STRING_SIMPLE("<?php class ActionYou  { function w() {} }");
	GlobalFinder.BuildResourceCacheForFile(File2, Code2, true);
	
	CHECK(ResourceUpdates.Register(File1));
	CHECK(ResourceUpdates.Update(File1, Code1, true));
	
	ParsedExpression.Lexeme = UNICODE_STRING_SIMPLE("$action");
	ParsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("$action"));
	ParsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("->w"));
	ResourceUpdates.ExpressionCompletionMatches(File1, ParsedExpression, UNICODE_STRING_SIMPLE("::"), 
		&GlobalFinder, VariableMatches, ResourceMatches, Error);
	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("w"), ResourceMatches[0].Identifier);
	}
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, RegisteredFinder) {
	
	// in this test we will create a class in file3; file1 will use that class
	// the ResourceUpdate object should be able to detect the variable type of 
	// the variable in file1
	// the difference here is that the class is now defined in one of the registered files
	Code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");
	Code2 = UNICODE_STRING_SIMPLE("<?php class ActionMe  { function yy() { $this;  } }");
	Code3 = UNICODE_STRING_SIMPLE("<?php class ActionYou  { function w() {} }");
	GlobalFinder.BuildResourceCacheForFile(File2, Code2, true);
	
	CHECK(ResourceUpdates.Register(File1));
	CHECK(ResourceUpdates.Update(File1, Code1, true));
	CHECK(ResourceUpdates.Register(File3));
	CHECK(ResourceUpdates.Update(File3, Code3, true));
	
	ParsedExpression.Lexeme = UNICODE_STRING_SIMPLE("$action");
	ParsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("$action"));
	ParsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("->w"));
	ResourceUpdates.ExpressionCompletionMatches(File1, ParsedExpression, UNICODE_STRING_SIMPLE("::"), 
		&GlobalFinder, VariableMatches, ResourceMatches, Error);

	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("w"), ResourceMatches[0].Identifier);
	}
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, ResourceMatchesWithGlobalFinder) {
	
	// in this test we will create a class in file2; file1 will use that class
	// the ResourceUpdate object should be able to detect the variable type of 
	// the variable in file1
	Code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");
	Code2 = UNICODE_STRING_SIMPLE("<?php class ActionYou  { function w() {} }");
	GlobalFinder.BuildResourceCacheForFile(File2, Code2, true);
	
	CHECK(ResourceUpdates.Register(File1));
	CHECK(ResourceUpdates.Update(File1, Code1, true));
	
	ParsedExpression.Lexeme = UNICODE_STRING_SIMPLE("$action");
	ParsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("$action"));
	ParsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("->w"));
	ResourceUpdates.ResourceMatches(File1, ParsedExpression, UNICODE_STRING_SIMPLE("::"), 
		&GlobalFinder, ResourceMatches, Error);
	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("ActionYou::w"), ResourceMatches[0].Resource);
	}
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, ResourceMatchesWithRegisteredFile) {

	// create a class in file1 with methodA
	// file2 will use the class from file1; file1 and file2 will be registered
	// perform a search
	// methodA should be a hit
	Code1 = UNICODE_STRING_SIMPLE("<?php class ActionMy   { function methodA() {} }");
	Code2 = UNICODE_STRING_SIMPLE("<?php $action = new ActionMy(); ");

	GlobalFinder.BuildResourceCacheForFile(File1, Code1, true);
	CHECK(ResourceUpdates.Register(File1));
	CHECK(ResourceUpdates.Register(File2));
	CHECK(ResourceUpdates.Update(File1, Code1, true));
	CHECK(ResourceUpdates.Update(File2, Code2, true));

	ParsedExpression.Lexeme = UNICODE_STRING_SIMPLE("$action");
	ParsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("$action"));
	ParsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("->methodA"));

	ResourceUpdates.ResourceMatches(File2, ParsedExpression, UNICODE_STRING_SIMPLE("::"), 
		&GlobalFinder, ResourceMatches, Error);
	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("ActionMy::methodA"), ResourceMatches[0].Resource);
	}
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, ResourceMatchesWithStaleMatches) {

	// create a class in file1 with methodA
	// file2 will use the class from file1; file2 will be registered
	// then file1 will be registered with code3 (invalidating methodA)
	// perform a search
	// methodA should not be a hit since it has been removed
	Code1 = UNICODE_STRING_SIMPLE("<?php class ActionMy   { function methodA() {} }");
	Code2 = UNICODE_STRING_SIMPLE("<?php $action = new ActionMy(); ");
	Code3 = UNICODE_STRING_SIMPLE("<?php class ActionMy   { function methodB() {} }");

	GlobalFinder.BuildResourceCacheForFile(File1, Code1, true);
	CHECK(ResourceUpdates.Register(File1));
	CHECK(ResourceUpdates.Register(File2));
	CHECK(ResourceUpdates.Update(File1, Code1, true));
	CHECK(ResourceUpdates.Update(File2, Code2, true));

	ParsedExpression.Lexeme = UNICODE_STRING_SIMPLE("$action");
	ParsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("$action"));
	ParsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("->methodA"));

	ResourceUpdates.ResourceMatches(File2, ParsedExpression, UNICODE_STRING_SIMPLE("::"), 
		&GlobalFinder, ResourceMatches, Error);
	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("ActionMy::methodA"), ResourceMatches[0].Resource);
	}

	// now update the code
	CHECK(ResourceUpdates.Update(File1, Code3, true));

	ResourceMatches.clear();
	ResourceUpdates.ResourceMatches(File2, ParsedExpression, UNICODE_STRING_SIMPLE("::"), 
		&GlobalFinder, ResourceMatches, Error);
	CHECK_EQUAL((size_t)0, ResourceMatches.size());
}

}