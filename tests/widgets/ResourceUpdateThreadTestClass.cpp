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


SUITE(ResourceUpdateThreadTestClass) {

TEST(RegisterShouldSucceed) {	
	wxString fileName = wxT("MyFile.php");
	mvceditor::ResourceUpdateClass resourceUpdates;
	CHECK(resourceUpdates.Register(fileName));
}

TEST(RegisterShouldFail) {	
	wxString fileName = wxT("MyFile.php");
	mvceditor::ResourceUpdateClass resourceUpdates;
	//CHECK(resourceUpdates.Register(fileName));
	//CHECK_EQUAL(false, resourceUpdates.Register(fileName));
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

TEST(ExpressionCompletionMatchesWithGlobalFinder) {
	
	// in this test we will create a class in file2; file1 will use that class
	// the ResourceUpdate object should be able to detect the variable type of 
	// the variable in file1
	mvceditor::ResourceUpdateClass resourceUpdates;
	wxString file1 = wxT("file1.php");
	wxString file2 = wxT("file2.php");
	UnicodeString code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");
	UnicodeString code2 = UNICODE_STRING_SIMPLE("<?php class ActionYou  { function w() {} }");
	mvceditor::ResourceFinderClass globalFinder;
	globalFinder.BuildResourceCacheForFile(file2, code2, true);
	
	CHECK(resourceUpdates.Register(file1));
	CHECK(resourceUpdates.Update(file1, code1, true));
	
	int posToCheck = code1.indexOf(UNICODE_STRING_SIMPLE("->")) + 2; // position of "->w()" in code1
	
	std::vector<UnicodeString> matches;
	mvceditor::SymbolClass parsedExpression;
	parsedExpression.Lexeme = UNICODE_STRING_SIMPLE("$action");
	parsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("$action"));
	parsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("->w"));
	resourceUpdates.ExpressionCompletionMatches(file1, parsedExpression, posToCheck, &globalFinder, matches);
	CHECK_EQUAL((size_t)1, matches.size());
	if (!matches.empty()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("w"), matches[0]);
	}
}

TEST(ExpressionCompletionMatchesWithRegisteredFinder) {
	
	// in this test we will create a class in file3; file1 will use that class
	// the ResourceUpdate object should be able to detect the variable type of 
	// the variable in file1
	// the difference here is that the class is now defined in one of the registered files
	mvceditor::ResourceUpdateClass resourceUpdates;
	wxString file1 = wxT("file1.php");
	wxString file2 = wxT("file2.php");
	wxString file3 = wxT("file3.php");
	UnicodeString code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");
	UnicodeString code2 = UNICODE_STRING_SIMPLE("<?php class ActionMe  { function yy() { $this;  } }");
	UnicodeString code3 = UNICODE_STRING_SIMPLE("<?php class ActionYou  { function w() {} }");
	mvceditor::ResourceFinderClass globalFinder;
	globalFinder.BuildResourceCacheForFile(file2, code2, true);
	
	CHECK(resourceUpdates.Register(file1));
	CHECK(resourceUpdates.Update(file1, code1, true));
	CHECK(resourceUpdates.Register(file3));
	CHECK(resourceUpdates.Update(file3, code3, true));
	
	int posToCheck = code1.indexOf(UNICODE_STRING_SIMPLE("->w")) + 3; // position of "->w()" in code1
	
	std::vector<UnicodeString> matches;
	mvceditor::SymbolClass parsedExpression;
	parsedExpression.Lexeme = UNICODE_STRING_SIMPLE("$action");
	parsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("$action"));
	parsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("->w"));
	resourceUpdates.ExpressionCompletionMatches(file1, parsedExpression, posToCheck, &globalFinder, matches);

	CHECK_EQUAL((size_t)1, matches.size());
	if (!matches.empty()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("w"), matches[0]);
	}
}

TEST(ResourceMatchesWithGlobalFinder) {
	
	// in this test we will create a class in file2; file1 will use that class
	// the ResourceUpdate object should be able to detect the variable type of 
	// the variable in file1
	mvceditor::ResourceUpdateClass resourceUpdates;
	wxString file1 = wxT("file1.php");
	wxString file2 = wxT("file2.php");
	UnicodeString code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");
	UnicodeString code2 = UNICODE_STRING_SIMPLE("<?php class ActionYou  { function w() {} }");
	mvceditor::ResourceFinderClass globalFinder;
	globalFinder.BuildResourceCacheForFile(file2, code2, true);
	
	CHECK(resourceUpdates.Register(file1));
	CHECK(resourceUpdates.Update(file1, code1, true));
	
	int posToCheck = code1.indexOf(UNICODE_STRING_SIMPLE("->")) + 2; // position of "->w()" in code1
	
	std::vector<mvceditor::ResourceClass> matches;
	mvceditor::SymbolClass parsedExpression;
	parsedExpression.Lexeme = UNICODE_STRING_SIMPLE("$action");
	parsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("$action"));
	parsedExpression.ChainList.push_back(UNICODE_STRING_SIMPLE("->w"));
	resourceUpdates.ResourceMatches(file1, parsedExpression, posToCheck, &globalFinder, matches);
	CHECK_EQUAL((size_t)1, matches.size());
	if (!matches.empty()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("ActionYou::w"), matches[0].Resource);
	}
}

}