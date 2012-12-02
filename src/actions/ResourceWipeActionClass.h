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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __MVCEDITORRESOUCEWIPEACTIONCLASS_H__
#define __MVCEDITORRESOUCEWIPEACTIONCLASS_H__

#include <actions/ActionClass.h>
#include <wx/filename.h>

namespace mvceditor {

/**
 * Class to 'wipe' resource databases (empty all of their contents)
 */
class ResourceWipeActionClass : public mvceditor::ActionClass {
	
public:

	ResourceWipeActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId,
		const std::vector<mvceditor::ProjectClass>& projects);
	
	bool Init(mvceditor::GlobalsClass& globals);

	wxString GetLabel() const;
	
protected:
	
	void BackgroundWork();
	
private:
		
	/**
	 * The db files that need to be wiped.
	 */
	std::vector<wxFileName> ResourceDbFileNames;
	
};
}

#endif