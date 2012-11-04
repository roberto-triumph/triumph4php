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
#include <globals/UrlResourceClass.h>
#include <MvcEditorChecks.h>

class UrlResourceFixtureClass {

public:

	mvceditor::UrlResourceFinderClass Finder;
	
	UrlResourceFixtureClass()
		: Finder() {
		Add(wxT("http://localhost/index.php"));
		Add(wxT("http://localhost/frontend.php"));
	}

	void Add(wxString urlString) {
		mvceditor::UrlResourceClass urlResource(urlString);
		Finder.Urls.push_back(urlResource);
	}

};

SUITE(UrlResourceTestClass) {

TEST_FIXTURE(UrlResourceFixtureClass, FindByUrlMatch) {
	wxURI toFind(wxT("http://localhost/frontend.php"));
	mvceditor::UrlResourceClass urlResource;
	CHECK(Finder.FindByUrl(toFind, urlResource));
	CHECK(toFind == urlResource.Url);
	CHECK_EQUAL(wxT("http://localhost/frontend.php"), urlResource.Url.BuildURI());

	toFind.Create(wxT("http://localhost/index.php"));
	CHECK(Finder.FindByUrl(toFind, urlResource));
	CHECK(toFind == urlResource.Url);
	CHECK_EQUAL(wxT("http://localhost/index.php"), urlResource.Url.BuildURI());
}

TEST_FIXTURE(UrlResourceFixtureClass, FindByUrlNoMatch) {
	wxURI toFind(wxT("http://localhost/backend.php"));
	mvceditor::UrlResourceClass urlResource;
	CHECK_EQUAL(false, Finder.FindByUrl(toFind, urlResource));
	CHECK(urlResource.Url.BuildURI().IsEmpty());
}

TEST_FIXTURE(UrlResourceFixtureClass, FilterUrl) {
	std::vector<mvceditor::UrlResourceClass> urls;
	Finder.FilterUrls(wxT("front"), urls);
	CHECK_VECTOR_SIZE(1, urls);
	CHECK_EQUAL(wxT("http://localhost/frontend.php"), urls[0].Url.BuildURI());
}

TEST_FIXTURE(UrlResourceFixtureClass, FilterUrlNoMatches) {
	std::vector<mvceditor::UrlResourceClass> urls;
	Finder.FilterUrls(wxT("back"), urls);
	CHECK_VECTOR_SIZE(0, urls);
}

TEST_FIXTURE(UrlResourceFixtureClass, DeleteUrlMatch) {
	wxURI toDelete(wxT("http://localhost/frontend.php"));
	Finder.DeleteUrl(toDelete);
	CHECK_VECTOR_SIZE(1, Finder.Urls);
	CHECK_EQUAL(wxT("http://localhost/index.php"), Finder.Urls[0].Url.BuildURI());
}

TEST_FIXTURE(UrlResourceFixtureClass, DeleteUrlNoMatch) {
	wxURI toDelete(wxT("http://localhost/backend.php"));
	Finder.DeleteUrl(toDelete);
	CHECK_VECTOR_SIZE(2, Finder.Urls);
}

TEST_FIXTURE(UrlResourceFixtureClass, AddUniqueUrl) {
	wxURI newUri(wxT("http://localhost/backend.php"));
	CHECK(Finder.AddUniqueUrl(newUri));
	CHECK_VECTOR_SIZE(3, Finder.Urls);

	// dont test the index for now, just make sure that find works
	mvceditor::UrlResourceClass urlResource;
	CHECK(Finder.FindByUrl(newUri, urlResource));
}

TEST_FIXTURE(UrlResourceFixtureClass, AddUniqueUrlNonUnique) {
	wxURI newUri(wxT("http://localhost/frontend.php"));
	CHECK_EQUAL(false, Finder.AddUniqueUrl(newUri));
	CHECK_VECTOR_SIZE(2, Finder.Urls);
}

TEST_FIXTURE(UrlResourceFixtureClass, ReplaceAll) {
	mvceditor::UrlResourceFinderClass newFinder;
	wxURI newUri(wxT("http://localhost/backend.php"));
	newFinder.AddUniqueUrl(newUri);

	newFinder.ReplaceAll(Finder);
	CHECK_VECTOR_SIZE(2, newFinder.Urls);

	// dont test the index for now, just make sure that find works
	wxURI firstUri(wxT("http://localhost/index.php"));
	wxURI secondUri(wxT("http://localhost/frontend.php"));
	mvceditor::UrlResourceClass urlResource;
	CHECK(newFinder.FindByUrl(firstUri, urlResource));
	CHECK(newFinder.FindByUrl(secondUri, urlResource));

	// existing item should be gone
	CHECK_EQUAL(false, newFinder.FindByUrl(newUri, urlResource));
}

}