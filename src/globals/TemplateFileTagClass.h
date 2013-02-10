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
#ifndef __MVCEDITOR_TemplateFileTagClass_H__
#define __MVCEDITOR_TemplateFileTagClass_H__

#include <wx/string.h>
#include <wx/filename.h>
#include <soci/soci.h>
#include <vector>

namespace mvceditor {

/**
 * This class represents a single "view" (template) file  that was detected by the 
 * template files detectors. It holds the full path of the template file as well
 * as the names of the variables that the controller assigned to this template file.
 */
class TemplateFileTagClass {

public:

	/**
	 * the full path of the template file 
	 */
	wxString FullPath;

	/**
	 * the names of the variables that the controller assigned to this template file.
	 */
	std::vector<wxString> Variables;

	TemplateFileTagClass();

	TemplateFileTagClass(const mvceditor::TemplateFileTagClass& src);

	void Copy(const mvceditor::TemplateFileTagClass& src);

	mvceditor::TemplateFileTagClass& operator=(const mvceditor::TemplateFileTagClass& src);

	void Init(const wxString& fullPath, const std::vector<wxString>& variables);

};

/**
 * This class will load template file rows from the detector DB into memeory
 */
class TemplateFileTagFinderClass {

public:

	TemplateFileTagFinderClass();

	/**
	 * Connect to the given db; will disconnect from the previous db
	 *
	 * @param detectorDbFileName full path to the detector sqlite db
	 */
	void Init(const wxFileName& detectorDbFileName);

	/**
	 * @return all of the templates that are in the detectors db file that was last
	 *  initialized.
	 */
	std::vector<mvceditor::TemplateFileTagClass> All();

private:

	soci::session Session;

	bool IsInitialized;
};

}

#endif