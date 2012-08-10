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

#include <widgets/ThreadWithHeartbeatClass.h>
#include <language/ResourceCacheClass.h>
#include <wx/event.h>

namespace mvceditor {

/**
 * This class will run the resource updates in a background thread.  The caller will use
 * the Register() method when a new file is opened by the user.  Every so ofte, the 
 * StartBackgroundUpdate() method should be called to trigger re-parsing of the resources
 * on the background task.  The results of the resource parsing will be stored in an 
 * internal cache that's separate from the 'global' cache; that way the entire global cache
 * does not have to be re-sorted every time we want to parse new contents.
 * This class will NEVER update the 'global' resource finder; the caller must take care
 * of updating the global resource finder when the user closes the file.
 */
class ResourceCacheBuilderClass : public ThreadWithHeartbeatClass {
	
public:

	/**
	 * @param the handler will get notified to EVENT_WORK* events with the given ID
	 * pointer will NOT be owned by this object
	 */
	ResourceCacheBuilderClass(ResourceCacheClass* resourceCache, wxEvtHandler& handler,
		mvceditor::RunningThreadsClass& runningThreads, int eventId = wxID_ANY);
		
	/**
	 * Will run a background thread to parse the resources of the given 
	 * text.
	 * This method is thread-safe.
	 * 
	 * @param fileName unique identifier for a file
	 * @param code the file's most up-to-date source code (from the user-edited buffer)
	 * @param bool if TRUE then tileName is a new file that is not yet written to disk
	 */
	wxThreadError StartBackgroundUpdate(const wxString& fileName, const UnicodeString& code, bool isNew);

protected:
	
	/**
	 * In the background thread we will parse the code for resources
	 */
	void Entry();
	
private:

	/**
	 * the type of work that will happen in the background thread
	 */
	enum Modes {
		UPDATE
	} Mode;

	/**
	 * This is the object that will hold all of the resource cache. It should not be accessed while
	 * the thread is running. Pointer will NOT be owned by this object.
	 */
	ResourceCacheClass* ResourceCache;

	/**
	 * the code that is being worked on by the background thread.
	 */
	UnicodeString CurrentCode;
	
	/**
	 * the name of the file that is being worked on by the background thread.
	 */
	wxString CurrentFileName;

	/**
	 * if TRUE then tileName is a new file that does not yet exist on disk
	 */
	bool CurrentFileIsNew;

};

}
#endif