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
#include <wx/string.h>
#include <vector>

namespace mvceditor {

/**
 * A small class to hold an application URL. We tie together a URL and its PHP source
 * code entry point so that we can easily jump to it.
 */
class UrlResourceClass {
	
public:

	/**
	 * These are relative URLs; they are relative to the server root and may contain a query string.
	 * For example, if a user would type in "http://localhost.codeigniter/index.php/news/index" the
	 * this URL should contain "index.php/news/index"
	 */
	wxString Url;
	
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
};
 
/**
 * Class that holds all of the URLs that MVC Editor has encountered.
 * We remember them because they are a relatively expensive to determine (have to
 * use the UrlDetectorActionClass).
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
	
	/**
	 * check to see if the given URL exists in the Urls list; if the URL
	 * exists then urlResource is filled with the contents of the URL.
	 * This way, the caller can "resolve" a URL into a file name
	 * Comparison is done in a case-insensitive manner
	 *
	 * @return TRUE if there is a URL resource that has the given URL member
	 */
	bool FindByUrl(const wxString& url, UrlResourceClass& urlResource);
};
	
}
#endif