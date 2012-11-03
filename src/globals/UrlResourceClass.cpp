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
#include <globals/UrlResourceClass.h>
#include <algorithm>

/**
 * Class that will be used with the STL algorithms to find item in the URLResourceFinder list
 * Items will be looked up by URL; only unique URLs will be allowed in the list.
 */
class UrlPredicateClass {

public:

	/**
	 * @param url the URL to match on (according to wxURI::operator==)
	 */
	UrlPredicateClass(const wxURI& url) 
		: Url(url) {

	}

	bool operator()(const mvceditor::UrlResourceClass& item) const {
		return  item.Url == Url;
	}

private:

	wxURI Url;

};

mvceditor::UrlResourceClass::UrlResourceClass()
 : Url()
 , FileName()
 , ClassName()
 , MethodName() {

 }

mvceditor::UrlResourceClass::UrlResourceClass(wxString uri) 
	: Url(uri)
	, FileName()
	, ClassName()
	, MethodName() {
}

mvceditor::UrlResourceClass::UrlResourceClass(const mvceditor::UrlResourceClass& src)
	: Url()	
	, FileName()
	, ClassName()
	, MethodName() {
	Copy(src);
 }

void mvceditor::UrlResourceClass::Reset() {
	Url.Create(wxT(""));
	FileName.Clear();
	ClassName.Clear();
	MethodName.Clear();
}

mvceditor::UrlResourceClass& mvceditor::UrlResourceClass::operator=(const mvceditor::UrlResourceClass& src) {
	Copy(src);
	return *this;
}

void mvceditor::UrlResourceClass::Copy(const mvceditor::UrlResourceClass& src) {
	Url = src.Url;
	FileName = src.FileName;
	ClassName = src.ClassName;
	MethodName = src.MethodName;
}

mvceditor::UrlResourceFinderClass::UrlResourceFinderClass()
	: Browsers()
	, Urls() 
	, ChosenBrowser() 
	, ChosenUrl()  {
		
}

mvceditor::UrlResourceFinderClass::UrlResourceFinderClass(const mvceditor::UrlResourceFinderClass& src)
	: Browsers()
	, Urls()
	, ChosenBrowser()
	, ChosenUrl() {
	ReplaceAll(src);
}


bool mvceditor::UrlResourceFinderClass::FindByUrl(const wxURI& url, mvceditor::UrlResourceClass& urlResource) {
	bool ret = false;
	UrlPredicateClass pred(url);
	std::vector<mvceditor::UrlResourceClass>::const_iterator it = std::find_if(Urls.begin(), Urls.end(), pred);
	if (it != Urls.end()) {
		ret = true;
		urlResource.Copy(*it);
	}
	return ret;
}

void mvceditor::UrlResourceFinderClass::DeleteUrl(const wxURI& url) {
	UrlPredicateClass pred(url);
	std::vector<mvceditor::UrlResourceClass>::iterator it = std::find_if(Urls.begin(), Urls.end(), pred);
	if (it != Urls.end()) {
		Urls.erase(it);
	}
}

void mvceditor::UrlResourceFinderClass::FilterUrls(const wxString& filter, std::vector<UrlResourceClass>& matchedUrls) {
	for (size_t i = 0; i < Urls.size(); ++i) {
		if (Urls[i].Url.BuildURI().Contains(filter)) {
			matchedUrls.push_back(Urls[i]);
		}
	}
}

bool mvceditor::UrlResourceFinderClass::AddUniqueUrl(const wxURI& url) {
	bool added = false;
	UrlPredicateClass pred(url);
	std::vector<mvceditor::UrlResourceClass>::iterator it = std::find_if(Urls.begin(), Urls.end(), pred);
	if (it == Urls.end()) {
		mvceditor::UrlResourceClass newUrl;
		newUrl.Url = url;
		Urls.push_back(newUrl);
		added = true;
	}
	return added;
}

void mvceditor::UrlResourceFinderClass::ReplaceAll(const mvceditor::UrlResourceFinderClass& src) {
	Browsers = src.Browsers;
	Urls = src.Urls;
	ChosenBrowser = src.ChosenBrowser;
	ChosenUrl = src.ChosenUrl;
}

void mvceditor::UrlResourceFinderClass::Clear() {
	Urls.clear();
}