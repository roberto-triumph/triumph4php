/*
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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <UnitTest++.h>
#include <FileTestFixtureClass.h>
#include <MvcEditorChecks.h>
#include <globals/ProjectClass.h>

class ProjectFixtureClass :  public FileTestFixtureClass {

public:

	mvceditor::ProjectClass Project;

	ProjectFixtureClass() 
	: FileTestFixtureClass(wxT("project_test"))
	, Project() {
		Project.IsEnabled = true;
		Project.CssFileExtensions.push_back(wxT("*.css"));
		Project.MiscFileExtensions.push_back(wxT("*.txt"));
		Project.PhpFileExtensions.push_back(wxT("*.php"));
		Project.SqlFileExtensions.push_back(wxT("*.sql"));
		Project.Label = wxT("unit test project");
		
	}

	void AddSrc(const wxString& srcDir, wxString includeWildcards = wxT("*.*"), wxString excludeWildcards = wxEmptyString) {
		mvceditor::SourceClass src;
		src.RootDirectory.AssignDir(TestProjectDir + srcDir);
		src.SetIncludeWildcards(includeWildcards);
		src.SetExcludeWildcards(excludeWildcards);
		Project.AddSource(src);
	}

};

SUITE(ProjectTestClass) {

TEST_FIXTURE(ProjectFixtureClass, AllSourcesShouldReturnMultiple) {
	AddSrc(wxT("controllers"));
	AddSrc(wxT("models"));

	std::vector<mvceditor::SourceClass> sources = Project.Sources;
	CHECK_VECTOR_SIZE(2, sources);

	CHECK_EQUAL(TestProjectDir + wxT("controllers") + wxFileName::GetPathSeparator(), sources[0].RootDirectory.GetFullPath());
	CHECK_EQUAL(TestProjectDir + wxT("models") + wxFileName::GetPathSeparator(), sources[1].RootDirectory.GetFullPath());
}

TEST_FIXTURE(ProjectFixtureClass, AllPhpSourcesShouldReturnExcludedWildcards) {
	AddSrc(wxT("controllers"), wxT("*.php"), wxT("cache.php"));
	std::vector<mvceditor::SourceClass> sources = Project.AllPhpSources();
	CHECK_VECTOR_SIZE(1, sources);

	mvceditor::SourceClass actual = sources[0];
	CHECK_EQUAL(TestProjectDir + wxT("controllers") + wxFileName::GetPathSeparator(), actual.RootDirectory.GetFullPath());
	CHECK_EQUAL(wxT("cache.php"), actual.ExcludeWildcardsString());
}


TEST_FIXTURE(ProjectFixtureClass, AllSourcesShouldReturnExcludedWildcards) {
	AddSrc(wxT("controllers"), wxT("*.php"), wxT("*\\cache\\*"));

	std::vector<mvceditor::SourceClass> sources = Project.AllSources();
	CHECK_VECTOR_SIZE(1, sources);

	mvceditor::SourceClass actual = sources[0];
	CHECK_EQUAL(TestProjectDir + wxT("controllers") + wxFileName::GetPathSeparator(), actual.RootDirectory.GetFullPath());
	CHECK_EQUAL(wxT("*.css;*.txt;*.php;*.sql"), actual.IncludeWildcardsString());
	CHECK_EQUAL(wxT("*\\cache\\*"), actual.ExcludeWildcardsString());
}

TEST_FIXTURE(ProjectFixtureClass, AllNonPhpExtesions) {
	std::vector<wxString> exts = Project.AllNonPhpExtensions();
	CHECK_VECTOR_SIZE(3, exts);
	CHECK_EQUAL(wxT("*.css"), exts[0]);
	CHECK_EQUAL(wxT("*.txt"), exts[1]);
	CHECK_EQUAL(wxT("*.sql"), exts[2]);
}

}