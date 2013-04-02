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
#ifndef __RESOURCECACHEBUILDERTHREADCLASS_H__
#define __RESOURCECACHEBUILDERTHREADCLASS_H__

#include <actions/ActionClass.h>
#include <language/TagCacheClass.h>

namespace mvceditor {

/**
 * This class will run the tag updates in a background thread.  The caller will use
 * the Register() method when a new file is opened by the user.  Every so often, the 
 * StartBackgroundUpdate() method should be called to trigger re-parsing of the resources
 * on the background task.  The results of the tag parsing will be stored in an 
 * internal cache that's separate from the 'global' cache; that way the entire global cache
 * does not have to be re-sorted every time we want to parse new contents.
 * This class will NEVER update the 'global' tag finder; the caller must take care
 * of updating the global tag finder when the user closes the file.
 */
// TODO above doc is wrong
class WorkingCacheBuilderClass : public mvceditor::ActionClass {
	
public:

	/**
	 * @param runningThreads the object will get notified to EVENT_WORK* events with the given ID
	 *        and the EVENT_WORKING_CACHE_COMPLETE.
	 */
	WorkingCacheBuilderClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * Will parse the resources of the given text in a backgound thread and will
	 * post an EVENT_WORKING_CACHE_COMPLETE when the parsing is complete.
	 * 
	 * @param fileName full path to the file. this can be empty string is contents are new.
	 * @param fileIdentifier unique identifier for a file
	 * @param code the file's most up-to-date source code (from the user-edited buffer)
	 * @param bool if TRUE then tileName is a new file that is not yet written to disk
	 * @param version The version of PHP to check against
	 */
	void Update(const wxString& fileName, const wxString& fileIdentifier, const UnicodeString& code, bool isNew, pelet::Versions version);

	wxString GetLabel() const;

protected:
	
	/**
	 * In the background thread we will parse the code for resources
	 */
	void BackgroundWork();
	
private:

	/**
	 * the code that is being worked on by the background thread.
	 */
	UnicodeString Code;
	
	/**
	 * full path to the file that is being worked on by the background thread.
	 * this can be empty string if file is a new file.
	 */
	wxString FileName;

	/**
	 * string that uniquely identifies the file that is being worked on by the background thread.
	 */
	wxString FileIdentifier;

	/**
	 * if TRUE then tileName is a new file that does not yet exist on disk
	 */
	bool FileIsNew;

	/**
	 * The version of PHP to check against
	 */
	pelet::Versions Version;

};

}
#endif