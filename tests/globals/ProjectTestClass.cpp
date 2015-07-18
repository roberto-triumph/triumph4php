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
#include <TriumphChecks.h>
#include <globals/ProjectClass.h>

class ProjectFixtureClass :  public FileTestFixtureClass {

	public:

	t4p::ProjectClass Project;
	t4p::FileTypeClass FileType;

	ProjectFixtureClass()
	: FileTestFixtureClass(wxT("project_test"))
	, Project()
	, FileType() {
		Project.Label = wxT("unit test project");
		Project.IsEnabled = true;
		FileType.CssFileExtensionsString = wxT("*.css");
		FileType.MiscFileExtensionsString = wxT("*.txt");
		FileType.PhpFileExtensionsString = wxT("*.php");
		FileType.SqlFileExtensionsString = wxT("*.sql");
		FileType.ConfigFileExtensionsString = wxT("*.ini");
	}

	void AddSrc(const wxString& srcDir, wxString includeWildcards = wxT("*.*"), wxString excludeWildcards = wxEmptyString) {
		t4p::SourceClass src;
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

	std::vector<t4p::SourceClass> sources = Project.Sources;
	CHECK_VECTOR_SIZE(2, sources);

	CHECK_EQUAL(TestProjectDir + wxT("controllers") + wxFileName::GetPathSeparator(), sources[0].RootDirectory.GetFullPath());
	CHECK_EQUAL(TestProjectDir + wxT("models") + wxFileName::GetPathSeparator(), sources[1].RootDirectory.GetFullPath());
}

TEST_FIXTURE(ProjectFixtureClass, AllPhpSourcesShouldReturnExcludedWildcards) {
	AddSrc(wxT("controllers"), wxT("*.php"), wxT("cache.php"));
	std::vector<t4p::SourceClass> sources = Project.AllPhpSources(FileType);
	CHECK_VECTOR_SIZE(1, sources);

	t4p::SourceClass actual = sources[0];
	CHECK_EQUAL(TestProjectDir + wxT("controllers") + wxFileName::GetPathSeparator(), actual.RootDirectory.GetFullPath());
	CHECK_EQUAL(wxT("cache.php"), actual.ExcludeWildcardsString());
}


TEST_FIXTURE(ProjectFixtureClass, AllSourcesShouldReturnExcludedWildcards) {
	AddSrc(wxT("controllers"), wxT("*.php"), wxT("*\\cache\\*"));

	std::vector<t4p::SourceClass> sources = Project.AllSources(FileType);
	CHECK_VECTOR_SIZE(1, sources);

	t4p::SourceClass actual = sources[0];
	CHECK_EQUAL(TestProjectDir + wxT("controllers") + wxFileName::GetPathSeparator(), actual.RootDirectory.GetFullPath());
	CHECK_EQUAL(FileType.GetAllSourceFileExtensionsString(), actual.IncludeWildcardsString());
	CHECK_EQUAL(wxT("*\\cache\\*"), actual.ExcludeWildcardsString());
}

TEST_FIXTURE(ProjectFixtureClass, AllSourcesShouldReturnIniWildcards) {
	AddSrc(wxT("controllers"), wxT("*.php"), wxT("*\\cache\\*"));

	std::vector<t4p::SourceClass> sources = Project.AllSources(FileType);
	CHECK_VECTOR_SIZE(1, sources);

	t4p::SourceClass actual = sources[0];
	CHECK_EQUAL(TestProjectDir + wxT("controllers") + wxFileName::GetPathSeparator(), actual.RootDirectory.GetFullPath());
	CHECK_EQUAL(FileType.GetAllSourceFileExtensionsString(), actual.IncludeWildcardsString());
	CHECK_EQUAL(wxT("*\\cache\\*"), actual.ExcludeWildcardsString());
}

TEST_FIXTURE(ProjectFixtureClass, IsASourceFileShouldReturnFalseWhenAnUnknownWildcard) {
	AddSrc(wxT("controllers"), wxT("*"), wxT("*\\cache\\*"));
	wxString fileToCheck = TestProjectDir + wxT("controllers") + wxFileName::GetPathSeparator() + wxT("hello.tmp");
	CHECK_EQUAL(false, Project.IsASourceFile(fileToCheck, FileType));
}

TEST_FIXTURE(ProjectFixtureClass, IsASourceFileShouldReturnTrueWhenKnownWildcard) {
	AddSrc(wxT("controllers"), wxT("*"), wxT("*\\cache\\*"));
	wxString fileToCheck = TestProjectDir + wxT("controllers") + wxFileName::GetPathSeparator() + wxT("hello.css");
	CHECK(Project.IsASourceFile(fileToCheck, FileType));
}

TEST_FIXTURE(ProjectFixtureClass, GetNonPhpExtesions) {
	std::vector<wxString> exts = Project.GetNonPhpExtensions(FileType);
	std::vector<wxString> expected = FileType.GetNonPhpFileExtensions();

	CHECK_VECTOR_SIZE(expected.size(), exts);
}

}
