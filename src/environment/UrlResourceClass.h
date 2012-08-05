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
#ifndef __MVCEDITORURLRESOURCECLASS_H__
#define __MVCEDITORURLRESOURCECLASS_H__

#include <environment/EnvironmentClass.h>
#include <wx/url.h>
#include <vector>

namespace mvceditor {

/**
 * A small class to hold an application URL. We tie together a URL and its PHP source
 * code entry point so that we can easily jump to it.
 */
class UrlResourceClass {
	
public:

	/**
	 * These are absolute URLs; may contain a query string.
	 * For example, "http://localhost.codeigniter/index.php/news/index?val=123"
	 * Using a wxURI instead of wxURL because wxURL needs some sort of socket initialization
	 * and it crashes without the extra code. since we won't be iniating connections we
	 * don't need that extra functionality.
	 */
	wxURI Url;
	
	/**
	 * The file where the source code of the URL is located in.  This is the entry point
	 * to the URL; for a framework $fileName will be the location of the controller.
	 * fileName is full path (os-dependent).
	 */
	wxFileName FileName;
	
	/**
	 * The name of the controller class that handles this URL.  If a project does not
	 * use a framework, this will be empty.
	 */
	wxString ClassName;
	
	/**
	 * The name of the controller method that handles this URL.  If a project does not
	 * use a framework, this will be empty.
	 *
	 */
	wxString MethodName;
	
	UrlResourceClass();

	UrlResourceClass(wxString uri);

	UrlResourceClass(const UrlResourceClass& src);

	/**
	 * @param src item to copy from. after a call to this method, this item will have the
	 * same properties as src
	 */
	void Copy(const UrlResourceClass& src);

	void Reset();
};
 
/**
 * Class that holds all of the URLs that MVC Editor has encountered.
 * We remember them because they are a relatively expensive to determine (have to
 * use the UrlDetectorActionClass).
 * The URLs here are a combination of detected URLs and URLs manually entered in by the user.
 * For a big application, there may be hundreds or thousands of these.
 */
class UrlResourceFinderClass {

public:

	/**
	 * the list of web browsers available to the editor. These are the 'friendly'
	 * names that are used by the Environment class.
	 */
	std::vector<wxString> Browsers;
	
	/**
	 * the list of URLs that have been detected thus far. These are full URLs; "http://codeigniter.localhost/news/index"
	 * These URLs are usually detected by the UrlDetectorClass
	 */
	std::vector<UrlResourceClass> Urls;
	
	/**
	 * The name of the browser that is selected
	 */
	wxString ChosenBrowser;
	
	/**
	 * the URL that is selected
	 */
	UrlResourceClass ChosenUrl;
		
	UrlResourceFinderClass();

	UrlResourceFinderClass(const UrlResourceFinderClass& src);
	
	/**
	 * check to see if the given URL exists in the Urls list; if the URL
	 * exists then urlResource is filled with the contents of the URL.
	 * This way, the caller can "resolve" a URL into a file name
	 * Comparison is done in a case-insensitive manner
	 *
	 * @return TRUE if there is a URL resource that has the given URL member
	 */
	bool FindByUrl(const wxURI& url, UrlResourceClass& urlResource);

	/**
	 * Searches all URLs for the URLs that match the given filter; and will copy matching
	 * URLs to matchedUrls.
	 * @param filter string if filter is a substring of URL then its considered a match.
	 *        No wildcards are accepted.
	 * @param matchedUrls matching URLs will be pushed into the given vector.
	 */
	void FilterUrls(const wxString& filter, std::vector<UrlResourceClass>& matchedUrls);

	/**
	 * @param url the URL to delete from this list.
	 */
	void DeleteUrl(const wxURI& url);

	/**
	 * @param the URL to add
	 * @return FALSE if URL is already there (a duplicate will NOT be inserted)
	 */
	bool AddUniqueUrl(const wxURI& url);

	/**
	 * @param src Replace all items in all public properties with the items in src.
	 * After a call to this method, this and src will have the same (copies)
	 * of items.
	 */
	void ReplaceAll(const UrlResourceFinderClass& src);

	/**
	 * Removes all URLs from this object.
	 */
	void Clear();
};
	
}
#endif