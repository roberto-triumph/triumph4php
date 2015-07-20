/**
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#ifndef SRC_ACTIONS_TAGWIPEACTIONCLASS_H_
#define SRC_ACTIONS_TAGWIPEACTIONCLASS_H_

#include <actions/GlobalActionClass.h>
#include <wx/filename.h>

namespace t4p {
/**
 * Class to 'wipe' tag databases (empty all of their contents)
 */
class TagWipeActionClass : public t4p::GlobalActionClass {
	public:
	TagWipeActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	bool Init(t4p::GlobalsClass& globals);

	wxString GetLabel() const;

	protected:
	void BackgroundWork();

	private:
	/**
	 * The db files that need to be wiped.
	 */
	wxFileName ResourceDbFileName;
	wxFileName DetectorDbFileName;
};

/**
 * action to remove only tags from entire source directories from the tag
 * databases
 */
class TagDeleteSourceActionClass : public t4p::GlobalActionClass {
	public:
	TagDeleteSourceActionClass(t4p::RunningThreadsClass& runningThreads, int eventId, const std::vector<wxFileName>& sourceDirsToDelete);

	bool Init(t4p::GlobalsClass& globals);

	wxString GetLabel() const;

	protected:
	void BackgroundWork();

	private:
	/**
	 * The db files that need to be wiped.
	 */
	wxFileName ResourceDbFileName;
	wxFileName DetectorDbFileName;

	/**
	 * the directories to be removed from the cache.
	 */
	std::vector<wxFileName> SourceDirsToDelete;
};

/**
 * action to remove only tags from certain directories from the tag
 * databases
 */
class TagDeleteDirectoryActionClass : public t4p::GlobalActionClass {
	public:
	TagDeleteDirectoryActionClass(t4p::RunningThreadsClass& runningThreads, int eventId, const std::vector<wxFileName>& dirsToDelete);

	bool Init(t4p::GlobalsClass& globals);

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
 * action to remove only tags from certain files from the tag
 * databases
 */
class TagDeleteFileActionClass : public t4p::GlobalActionClass {
	public:
	TagDeleteFileActionClass(t4p::RunningThreadsClass& runningThreads, int eventId, const std::vector<wxFileName>& filesToDelete);

	bool Init(t4p::GlobalsClass& globals);

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
	std::vector<wxFileName> FilesToDelete;
};
}  // namespace t4p

#endif  // SRC_ACTIONS_TAGWIPEACTIONCLASS_H_
