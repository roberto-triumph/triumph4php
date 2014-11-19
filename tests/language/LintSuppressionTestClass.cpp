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
#include <TriumphChecks.h>
#include <language/LintSuppressionClass.h>
#include <unicode/unistr.h>

class LintSuppressionFixtureClass : public FileTestFixtureClass {
	
public:

	/**
	 * the object under test
	 */
	t4p::LintSuppressionClass Suppressions;

	/**
	 * location of the suppression file (where the rules 
	 * are stored)
	 */
	wxFileName SuppressionFile;
	
	/**
	 * the name of a file being suppressed
	 */
	wxFileName CodeFile;

	/**
	 * the name of a directory being suppressed
	 */	
	wxFileName CodeDir;
	
	/**
	 * errors when suppression file contains invalid rules
	 */
	std::vector<UnicodeString> Errors;

	LintSuppressionFixtureClass()
	: FileTestFixtureClass("lint_suppression")
	, Suppressions() 
	, SuppressionFile(TestProjectDir, wxT("suppressions.csv"))
	, CodeFile(TestProjectDir, wxT("code.php"))
	, CodeDir() 
	, Errors() {
		CreateSubDirectory(wxT("code_dir"));
		CodeDir.AssignDir(TestProjectDir);
		CodeDir.AppendDir(wxT("code_dir"));
	}
	
	/**
	 * creates one of each kind of suppression
	 * suppress all for an entire directory
	 * suppress unknown classes for code.php
	 * suppress unknown methods for code.php
	 * suppress unknown functions for code.php
	 * suppress uninitialized variables for code.php
	 */
	void DefaultSuppressions() {
		
		// create a file of suppressions
		// 1 of each kind of suppression
		wxString contents = wxString::FromAscii(
			"# this is a comment, one, two, it should be skipped\n"
			"skip_unknown_class, Couchbase, %locationFile%\n"
			"skip_all,,%locationDir%\n"
			"skip_unknown_method, serializeToCouchbase, %locationFile%\n"
			"skip_unknown_function, couchbase_init, %locationFile%\n"
			"skip_uninitialized_var, $connection, %locationFile%\n"
		);
		
		// create 1 code file that will be used as the file
		// to be suppressed. file can be empty (no contents)
		// because its not needed for suppressions.
		CreateFixtureFile(CodeFile.GetFullName(), wxT(""));
		
		// replace %location% tokens with an actual paths
		contents.Replace(wxT("%locationFile%"), CodeFile.GetFullPath());
		contents.Replace(wxT("%locationDir%"), CodeDir.GetPathWithSep());
		
		CreateFixtureFile(SuppressionFile.GetFullName(), contents);
	}
};

SUITE(LintSuppressionTestClass) {

TEST_FIXTURE(LintSuppressionFixtureClass, LoadFromFile) {
	
	// test that the suppressions can be loaded from
	// a CSV file
	DefaultSuppressions();
	bool loaded = Suppressions.Init(SuppressionFile.GetFullPath(), Errors);
	CHECK(loaded);
}

TEST_FIXTURE(LintSuppressionFixtureClass, LoadFromFileWithErrors) {
	
	// test that when a suppression file has some invalid rules
	// the good rules are still loaded from the CSV file
	
	wxString contents = wxString::FromAscii(
		"# this is a comment, one, two, it should be skipped\n"
		"skip_unknown_class, Couchbase, %locationFile%\n"
		"skip_, Couchbase, %locationDir%\n"
		"skip_all, Couchbase\n"
		"skip_all, Couchbase, %locationDir%\n"
		"skip_unknown_method, serializeToCouchbase, %locationFile%\n"
		"skip_unknown_function, couchbase_init, %locationFile%\n"
		"skip_uninitialized_var, $connection, %locationFile%\n"
	);
	
	// create 1 code file that will be used as the file
	// to be suppressed. file can be empty (no contents)
	// because its not needed for suppressions.
	CreateFixtureFile(CodeFile.GetFullName(), wxT(""));
	
	// replace %location% tokens with an actual paths
	contents.Replace(wxT("%locationFile%"), CodeFile.GetFullPath());
	contents.Replace(wxT("%locationDir%"), CodeDir.GetPathWithSep());
	
	CreateFixtureFile(SuppressionFile.GetFullName(), contents);
	
	bool loaded = Suppressions.Init(SuppressionFile.GetFullPath(), Errors);
	CHECK(loaded);
	CHECK_VECTOR_SIZE(2, Errors);
}

TEST_FIXTURE(LintSuppressionFixtureClass, TypeAllDirectory) {
	
	// test that the "all" suppressions properly ignores
	// all types when using directory suppressions
	DefaultSuppressions();
	
	// create a file inside of the suppressed dir
	wxFileName passFile;
	passFile.AssignDir(CodeDir.GetPath());
	passFile.AppendDir(wxT("code_dir"));
	passFile.SetFullName(wxT("pass.php"));
	
	CreateFixtureFile(CodeDir.GetDirs().Last() + wxFileName::GetPathSeparators() + wxT("pass.php"), wxT(""));
	
	bool loaded = Suppressions.Init(SuppressionFile.GetFullPath(), Errors);
	CHECK(loaded);
	
	UnicodeString target = UNICODE_STRING_SIMPLE("Couchbase");
	t4p::SuppressionRuleClass::Types type;
	
	type = t4p::SuppressionRuleClass::SKIP_UNKNOWN_CLASS;
	bool ignore = Suppressions.ShouldIgnore(passFile, target, type);
	CHECK(ignore);
	
	target = UNICODE_STRING_SIMPLE("connect");
	type = t4p::SuppressionRuleClass::SKIP_UNKNOWN_METHOD;
	ignore = Suppressions.ShouldIgnore(passFile, target, type);
	CHECK(ignore);
	
	target = UNICODE_STRING_SIMPLE("couchbase_close");
	type = t4p::SuppressionRuleClass::SKIP_UNKNOWN_FUNCTION;	
	ignore = Suppressions.ShouldIgnore(passFile, target, type);
	CHECK(ignore);
	
	target = UNICODE_STRING_SIMPLE("$connect");
	type = t4p::SuppressionRuleClass::SKIP_UNINITIALIZED_VAR;
	ignore = Suppressions.ShouldIgnore(passFile, target, type);
	CHECK(ignore);
	
	// make sure that we don't suppress items outside of the
	// suppressed dir
	type = t4p::SuppressionRuleClass::SKIP_UNKNOWN_CLASS;
	wxFileName outsideFile(TestProjectDir, wxT("good.php"));
	ignore = Suppressions.ShouldIgnore(outsideFile.GetFullPath(), target, type);
	CHECK_EQUAL(false, ignore);
}

TEST_FIXTURE(LintSuppressionFixtureClass, TypeUnknownClass) {
	
	// test that the class suppressions are properly ignored
	DefaultSuppressions();
	
	bool loaded = Suppressions.Init(SuppressionFile.GetFullPath(), Errors);
	CHECK(loaded);
	
	UnicodeString target = UNICODE_STRING_SIMPLE("Couchbase");
	t4p::SuppressionRuleClass::Types type = t4p::SuppressionRuleClass::SKIP_UNKNOWN_CLASS;
	
	bool ignore = Suppressions.ShouldIgnore(CodeFile, target, type);
	CHECK(ignore);
	
	// class not in suppression file
	target = UNICODE_STRING_SIMPLE("CouchbaseConn");
	ignore = Suppressions.ShouldIgnore(CodeFile, target, type);
	CHECK_EQUAL(false, ignore);
}

TEST_FIXTURE(LintSuppressionFixtureClass, TypeUnknownMethod) {
	
	// test that the method suppressions are properly ignored
	DefaultSuppressions();
	
	bool loaded = Suppressions.Init(SuppressionFile.GetFullPath(), Errors);
	CHECK(loaded);
	
	UnicodeString target = UNICODE_STRING_SIMPLE("serializeToCouchbase");
	t4p::SuppressionRuleClass::Types type = t4p::SuppressionRuleClass::SKIP_UNKNOWN_METHOD;
	
	bool ignore = Suppressions.ShouldIgnore(CodeFile, target, type);
	CHECK(ignore);
	
	// method not in suppression file
	target = UNICODE_STRING_SIMPLE("unserializeToCouchbase");
	ignore = Suppressions.ShouldIgnore(CodeFile, target, type);
	CHECK_EQUAL(false, ignore);
}

TEST_FIXTURE(LintSuppressionFixtureClass, TypeUnknownFunction) {
	
	// test that the function suppressions are properly ignored
	DefaultSuppressions();
	
	bool loaded = Suppressions.Init(SuppressionFile.GetFullPath(), Errors);
	CHECK(loaded);
	
	UnicodeString target = UNICODE_STRING_SIMPLE("couchbase_init");
	t4p::SuppressionRuleClass::Types type = t4p::SuppressionRuleClass::SKIP_UNKNOWN_FUNCTION;
	
	bool ignore = Suppressions.ShouldIgnore(CodeFile, target, type);
	CHECK(ignore);
	
	
	// function not in suppression file
	target = UNICODE_STRING_SIMPLE("couchbase_open");
	ignore = Suppressions.ShouldIgnore(CodeFile, target, type);
	CHECK_EQUAL(false, ignore);
}

TEST_FIXTURE(LintSuppressionFixtureClass, TypeUninitializedVariable) {
	
	// test that the variable suppressions are properly ignored
	DefaultSuppressions();
	
	bool loaded = Suppressions.Init(SuppressionFile.GetFullPath(), Errors);
	CHECK(loaded);
	
	UnicodeString target = UNICODE_STRING_SIMPLE("$connection");
	t4p::SuppressionRuleClass::Types type = t4p::SuppressionRuleClass::SKIP_UNINITIALIZED_VAR;
	
	bool ignore = Suppressions.ShouldIgnore(CodeFile, target, type);
	CHECK(ignore);
	
	// variable not in suppression file
	target = UNICODE_STRING_SIMPLE("$conn");
	ignore = Suppressions.ShouldIgnore(CodeFile, target, type);
	CHECK_EQUAL(false, ignore);
}

}
