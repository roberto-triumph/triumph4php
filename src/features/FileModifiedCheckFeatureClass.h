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
#ifndef T4P_FILEMODIFIEDCHECKFEATURECLASS_H
#define T4P_FILEMODIFIEDCHECKFEATURECLASS_H

#include <features/FeatureClass.h>
#include <actions/FileModifiedCheckActionClass.h>

namespace t4p {

extern const int ID_FILE_MODIFIED_ACTION;

/**
 * A feature that checks to see if any files that are currently
 * opened have been externally modified and / or deleted. If so, we
 * will prompt the reader to save and or reload them.
 *
 * Note: attempted to read the EVENT_APP_FILE_RENAMED so that
 * we can show the user the rename (3-choice) prompt, but could
 * not successfully do it, since the fs watcher triggers the
 * event loop immediately, which calls the reactivate event and
 * then we perform the file check before we get the EVENT_APP_FILE_RENAMED
 * event (because this event is buffered)
 *
 * The file modification times are read in a background thread so
 * that if the user has many files open the user does not
 * feel lag.
 */
class FileModifiedCheckFeatureClass : public t4p::FeatureClass {

	public:

	FileModifiedCheckFeatureClass(t4p::AppClass& app);

	/**
	 * starts a background action that will check the file mod
	 * timestamps of the files vs. the timestamps we last got
	 * for those files. The results will be posted in events
	 * by the FileModifiedCheckActionClass.
	 */
	void StartFilePoll(std::vector<t4p::FileModifiedTimeClass> filesToPoll);

};

}

#endif
