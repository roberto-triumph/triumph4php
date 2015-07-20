/**
 * @copyright  2013 Roberto Perpuly
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
#ifndef SRC_ACTIONS_CACHEDBVERSIONACTIONCLASS_H_
#define SRC_ACTIONS_CACHEDBVERSIONACTIONCLASS_H_

#include <actions/GlobalActionClass.h>
#include <soci/soci.h>

namespace t4p {
/**
 * This action will check the tag cache DB for all projects to make sure
 * that they are on the same 'version' as the source code that is running.
 * The end goal of this class is to prevent code from acessing columns that do not exist in
 * the db files. It will do so by deleting all tables and then executing the
 * schema file.
 * The action will only check the db files for enabled projects.
 */
class TagCacheDbVersionActionClass : public t4p::GlobalActionClass {
	public:
	TagCacheDbVersionActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	bool Init(t4p::GlobalsClass& globals);

	void BackgroundWork();

	wxString GetLabel() const;

	private:
	/**
	 * files to check.
	 */
	std::vector<wxFileName> TagDbs;

	/**
	 * The opened connection to each tag db
	 */
	soci::session Session;
};

/**
 * This action will check the detector cache DB for all projects to make sure
 * that they are on the same 'version' as the source code that is running.
 * The end goal of this class is to prevent code from acessing columns that do not exist in
 * the db files. It will do so by deleting all tables and then executing the
 * schema file.
 * The action will only check the db files for enabled projects.
 */
class DetectorCacheDbVersionActionClass : public t4p::GlobalActionClass {
	public:
	DetectorCacheDbVersionActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	bool Init(t4p::GlobalsClass& globals);

	void BackgroundWork();

	wxString GetLabel() const;

	private:
	/**
	 * files to check.
	 */
	std::vector<wxFileName> DetectorDbs;

	/**
	 * The opened connection to each detector db
	 */
	soci::session Session;
};
}  // namespace t4p

#endif  // SRC_ACTIONS_CACHEDBVERSIONACTIONCLASS_H_
