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

#ifndef __MVCEDITOR_STRUCTSCLASS_H__
#define __MVCEDITOR_STRUCTSCLASS_H__

#include <environment/EnvironmentClass.h>
#include <environment/UrlResourceClass.h>
#include <widgets/ResourceCacheClass.h>
#include <php_frameworks/FrameworkDetectorClass.h>
#include <vector>

namespace mvceditor {

/**
 * Class that groups together all of the Analysis source code; ie
 * the source code's resources, URLs, and templates.
 */
class StructsClass {

public:

	/**
	 * The environment stack.
	 * 
	 * @var EnvironmentClass
	 */
	EnvironmentClass Environment;
	
	/**
	 * This object will be used to parse the resources of files that are currently open.
	 */
	ResourceCacheClass ResourceCache;

	/**
	 * The URLs (entry points to the current project) that have been detected so far. Also holds the 
	 * "current" URL.
	 */
	UrlResourceFinderClass UrlResourceFinder;

	/**
	 * All of the views of the currently selected controller/action
	 * pair.
	 */
	std::vector<mvceditor::ViewInfoClass> CurrentViewInfos;

	/**
	 * the URL that the user chose to view files from
	 */
	UrlResourceClass CurrentUrl;

	StructsClass();

};

}
#endif
