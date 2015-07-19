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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __TEMPLATEFILES_FEATURECLASS_H
#define __TEMPLATEFILES_FEATURECLASS_H

#include <features/FeatureClass.h>
#include <language_php/TemplateFileTagClass.h>
#include <actions/ActionClass.h>
#include <language_php/UrlTagClass.h>

namespace t4p {
/**
 * This is a class that will show the user any view files that are
 * associated with the current URL (the URL that the user chose in
 * the toolbar).
 *
 * The class will do the following:
 * (1) generate the call stack file in a background thread
 * (2) call the PHP view file detector (in an external process)
 *
 * After all of this, it will populate a window with the template files
 * for the URL that the user has selected.
 */
class TemplateFilesFeatureClass : public t4p::FeatureClass {
	public:
	TemplateFilesFeatureClass(t4p::AppClass& app);

	void SetCurrentUrl(t4p::UrlTagClass url);

	UrlTagFinderClass& Urls();

	/**
	 * starts the view file detection process. This is an asynchronous operation.
	 * When view files are detected, the feature will update the
	 * panel appropriately.
	 */
	void StartDetection();

	private:
};
}

#endif
