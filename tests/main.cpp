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
#include <cstdarg>
#include <UnitTest++.h>
#include <TestReporterStdout.h>
#include <TestRunner.h>
#include <unicode/uclean.h>
#include <soci/mysql/soci-mysql.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <string.h>
#include <wx/wx.h>

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// To add a test, simply put the following code in the a .cpp file of your choice:
//
// =================================
// Simple Test
// =================================
//
//  TEST(YourTestName)
//  {
//  }
//
// The TEST macro contains enough machinery to turn this slightly odd-looking syntax into legal C++, and automatically register the test in a global list.
// This test list forms the basis of what is executed by RunAllTests().
//
// If you want to re-use a set of test data for more than one test, or provide setup/teardown for tests,
// you can use the TEST_FIXTURE macro instead. The macro requires that you pass it a class name that it will instantiate, so any setup and teardown code should be in its constructor and destructor.
//
//  struct SomeFixture
//  {
//    SomeFixture() { /* some setup */ }
//    ~SomeFixture() { /* some teardown */ }
//
//    int testData;
//  };
//
//  TEST_FIXTURE(SomeFixture, YourTestName)
//  {
//    int temp = testData;
//  }
//
// =================================
// Test Suites
// =================================
//
// Tests can be grouped into suites, using the SUITE macro. A suite serves as a namespace for test names, so that the same test name can be used in two difference contexts.
//
//  SUITE(YourSuiteName)
//  {
//    TEST(YourTestName)
//    {
//    }  //
//    TEST(YourOtherTestName)
//    {
//    }
//  }
//
// This will place the tests into a C++ namespace called YourSuiteName, and make the suite name available to UnitTest++.
// RunAllTests() can be called for a specific suite name, so you can use this to build named groups of tests to be run together.
// Note how members of the fixture are used as if they are a part of the test, since the macro-generated test class derives from the provided fixture class.
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SingleTestsPredicateClass {
	public:
	std::vector<std::string> TestCases;

	SingleTestsPredicateClass(const std::vector<std::string>& testCases)
		: TestCases(testCases) {
	}

	bool operator()(const UnitTest::Test* test) const {
		// if no specific test cases need to be run, then run all tests in
		// a suite
		if (TestCases.empty()) {
			return true;
		}
		bool ret = false;
		for (std::vector<std::string>::const_iterator it =  TestCases.begin(); it != TestCases.end(); ++it) {
			if (it->compare(test->m_details.testName) == 0) {
				ret = true;
				break;
			}
		}
		return ret;
	}
};

std::vector<std::string> listSuites() {
	UnitTest::TestList& list = UnitTest::Test::GetTestList();
	UnitTest::Test* test = list.GetHead();
	std::vector<std::string> suites;
	while (test) {
		suites.push_back(test->m_details.suiteName);
		test = test->next;
	}
	std::sort(suites.begin(), suites.end());
	std::vector<std::string>::iterator end = std::unique(suites.begin(), suites.end());
	suites.erase(end, suites.end());
	return suites;
}

void printSuites(const std::vector<std::string>& suites) {
	std::cout << "All available test suites\n\n";
	for (size_t i = 0; i < suites.size(); i++) {
		std::cout << suites[i] << std::endl;
	}
	std::cout << std::endl;
}

bool matchesSuite(const std::vector<std::string>& suites, const std::string& search) {
	for (size_t i = 0; i < suites.size(); i++) {
		// lower case suite names so that searches are case insensitive
		std::string suite(suites[i]);
		std::transform(suite.begin(), suite.end(), suite.begin(), ::tolower);
		if (suite.find(search) != std::string::npos) {
			return true;
		}
	}
	return false;
}

int runMatchingSuites(const std::string& search) {
	std::cout << "Running all tests matching \"" << search << "\"\n\n";

	UnitTest::TestReporterStdout reporter;
	UnitTest::TestResults results(&reporter);
	UnitTest::CurrentTest::Results() = &results;
	UnitTest::Timer timer;

	timer.Start();
	UnitTest::TestList& list = UnitTest::Test::GetTestList();
	UnitTest::Test* test = list.GetHead();
	while (test) {
		std::string suiteName(test->m_details.suiteName);

		// lower case suite names so that searches are case insensitive
		std::transform(suiteName.begin(), suiteName.end(), suiteName.begin(), ::tolower);
		if (suiteName.find(search) != std::string::npos) {
			results.OnTestStart(test->m_details);
			test->Run();
			int const testTimeInMs = timer.GetTimeInMs();
			results.OnTestFinish(test->m_details, testTimeInMs / 1000.0f);
		}
		test = test->next;
	}

	float const secondsElapsed = timer.GetTimeInMs() / 1000.0f;
	if (results.GetFailureCount() > 0) {
		std::cout << "FAILURE: " << results.GetFailedTestCount()
			<< " out of " << results.GetTotalTestCount()
			<< " failed (" << results.GetFailureCount() << ")." << std::endl;
	} else {
		std::cout << "Success: " << results.GetTotalTestCount()
			<< " tests passed." << std::endl;
	}
	std::cout << "Test time: " << secondsElapsed << " seconds.\n";
	return results.GetFailureCount();
}

int chooseTests() {
	int ret = 0;
	std::string suiteToRun;
	std::vector<std::string> testCasesToRun;
	std::vector<std::string> suites = listSuites();
	std::string prompt = "Choose:\n";
	prompt += "1. Run all tests\n";
	prompt += "2. Run suites matching a string\n";
	prompt += "3. List all suites\n";
	prompt += "4. Exit the program\n";
	std::string choice = "";

	std::cout << "Triumph4PHP test runner." << std::endl;
	std::cout << prompt;
	std::cin >> choice;
	while (choice != "4") {
		if (choice == "1") {
			std::cout << "Running all tests\n\n";
			ret = UnitTest::RunAllTests();
		} else if (choice == "2") {
			std::cout << "Enter suite string:" << std::endl;
			std::cin >> suiteToRun;

			// lowe case the given name
			std::transform(suiteToRun.begin(), suiteToRun.end(), suiteToRun.begin(), ::tolower);
			if (matchesSuite(suites, suiteToRun)) {
				ret = runMatchingSuites(suiteToRun);
			} else {
				std::cout << "No suites matched" << std::endl;
			}
		} else if (choice == "3") {
			printSuites(suites);
		} else {
			std::cout << "invalid choice." << std::endl;
		}
		std::cout << prompt;
		std::cin >> choice;
	}
	return ret;
}

// run all tests
int main(int argc, char **argv) {
	// our classes use wxWidgets we must initialize the
	// library
	wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");
	wxInitializer initializer;
	if (!initializer) {
		puts("Could not initialize wxWidgets\n");
		return -1;
	}

	int ret = 0;
	if (argc > 1 && strcmp("--all", argv[1]) == 0) {
		ret = UnitTest::RunAllTests();
	} else {
		ret = chooseTests();
	}

	// calling cleanup here so that we can run this binary through a memory leak detector
	// ICU will cache many things and that will cause the detector to output "possible leaks"
	u_cleanup();

	// clean up the MySQL library. Same reason as the ICU cleanup.
	mysql_library_end();
	sqlite_api::sqlite3_shutdown();
	wxUninitialize();
	return ret;
}
