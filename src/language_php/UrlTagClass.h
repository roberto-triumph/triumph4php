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
#ifndef T4P_URLRESOURCECLASS_H
#define T4P_URLRESOURCECLASS_H

#include <globals/EnvironmentClass.h>
#include <wx/url.h>
#include <globals/Sqlite.h>
#include <globals/SqliteFinderClass.h>

namespace t4p {
/**
 * A small class to hold an application URL. We tie together a URL and its PHP source
 * code entry point so that we can easily jump to it.
 */
class UrlTagClass {
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

	UrlTagClass();

	UrlTagClass(wxString uri);

	/**
	 * @param src this item will be a deep copy of src
	 */
	UrlTagClass(const t4p::UrlTagClass& src);

	/**
	 * @param src item to copy from. after a call to this method, this item will have the
	 * same properties as src. This is a deep copy
	 */
	void Copy(const t4p::UrlTagClass& src);

	/**
	 * @param src item to copy from. after a call to this method, this item will have the
	 * same properties as src. This is a deep copy
	 */
	t4p::UrlTagClass& operator=(const t4p::UrlTagClass& src);

	void Reset();
};

/**
 * Class that holds all of the URLs that Triumph has encountered.
 * We remember them because they are a relatively expensive to determine (have to
 * use the UrlTagDetectorActionClass).
 * The URLs here are a combination of detected URLs and URLs manually entered in by the user.
 * For a big application, there may be hundreds or thousands of these.
 */
class UrlTagFinderClass : public t4p::SqliteFinderClass {
	public:
	UrlTagFinderClass(soci::session& session);

	/**
	 * check to see if the given URL exists in the Urls list; if the URL
	 * exists then urlTag is filled with the contents of the URL.
	 * This way, the caller can "resolve" a URL into a file name
	 * Comparison is done in a case-insensitive manner
	 *
	 * @return TRUE if there is a URL resource that has the given URL member
	 */
	bool FindByUrl(const wxURI& url, const std::vector<wxFileName>& sourceDirs, UrlTagClass& urlTag);

	/**
	 * check to see if the given class / method is a URL entry point. If the class/method combination
	 * exists then urlTag is filled with the contents of the URL.
	 * This way, the caller can "resolve" a class/method name into a URL
	 * Comparison is done in a case-insensitive manner
	 *
	 * @return TRUE if there is a URL resource that has the given controller class name AND method name
	 */
	bool FindByClassMethod(const wxString& className, const wxString& methodName, const std::vector<wxFileName>& sourceDirs, UrlTagClass& urlTag);

	/**
	 * Get all Url tags that have their FullPath equal to fullPath. if the URL
	 * exists then urlTag it is appended to urlTags;
	 * This way, the caller can "resolve" a file name into all of the URLs for that file.
	 * Comparison is done in a case-insensitive manner
	 *
	 * @return TRUE if there is a URL resource that has the given full path
	 */
	bool FilterByFullPath(const wxString& fullPath, const std::vector<wxFileName>& sourceDirs, std::vector<UrlTagClass>& urlTags);

	/**
	 * Searches all URLs for the URLs that match the given filter; and will copy matching
	 * URLs to matchedUrls.
	 * @param filter string if filter is a substring of URL then its considered a match.
	 *        No wildcards are accepted.
	 * @param matchedUrls matching URLs will be pushed into the given vector.
	 */
	void FilterUrls(const wxString& filter, const std::vector<wxFileName>& sourceDirs, std::vector<UrlTagClass>& matchedUrls);

	/**
	 * @param url the URL to delete from this list.
	 */
	void DeleteUrl(const wxURI& url, const std::vector<wxFileName>& sourceDirs);

	/**
	 * Removes all URLs from the backing databases. This is a destructive operation;
	 * if you just want to close the opened connections use the Close method
	 */
	void Wipe(const std::vector<wxFileName>& sourceDirs);

	/**
	 * @return int number of urls
	 */
	int Count(const std::vector<wxFileName>& sourceDirs);

	/**
	 * returns all of the controller names that were detected
	 */
	std::vector<wxString> AllControllerNames(const std::vector<wxFileName>& sourceDirs);

	/**
	 * returns all of the method names that were detected. These are just the
	 * methods from the detected URLs.
	 */
	std::vector<wxString> AllMethodNames(const wxString& controllerClassName, const std::vector<wxFileName>& sourceDirs);
};
}
#endif
