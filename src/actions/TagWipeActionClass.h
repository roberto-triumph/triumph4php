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

#include <actions/GlobalActionClass.h>
#include <wx/filename.h>

namespace mvceditor {

/**
 * Class to 'wipe' tag databases (empty all of their contents)
 */
class TagWipeActionClass : public mvceditor::GlobalActionClass {
	
public:

	TagWipeActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);
	
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

/**
 * action to remove only tags from certain directories from the tag
 * databases
 */
class TagDeleteActionClass : public mvceditor::GlobalActionClass {
	
public:

	TagDeleteActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId, const std::vector<wxFileName>& dirsToDelete);
	
	bool Init(mvceditor::GlobalsClass& globals);

	wxString GetLabel() const;
	
protected:
	
	void BackgroundWork();
	
private:
		
	/**
	 * The db files that need to be wiped.
	 */
	std::vector<wxFileName> ResourceDbFileNames;

	/**
	 * the directories to be removed from the cache.
	 */
	std::vector<wxFileName> DirsToDelete;
	
};

/**
 * action to remove only tags from a single file from the tag
 * databases. This action works on the working cache only. 
 */
/*
class TagCleanWorkingCacheActionClass : public mvceditor::GlobalActionClass {
	
public:

	TagCleanWorkingCacheActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId, const wxString& fileToDelete);
	
	bool Init(mvceditor::GlobalsClass& globals);

	wxString GetLabel() const;
	
protected:
	
	void BackgroundWork();
	
private:
		
	/**
	 * The db file that needs to be cleaned up.
	 * /
	wxFileName WorkingTagDbFileName;

	/**
	 * the file to be removed from the cache. this is a string and not
	 * a wxFileName because we may need to cleanup tags from a new buffer that
	 * was never saved
	 * /
	wxString FileToDelete;
	
};
*/
}

#endif