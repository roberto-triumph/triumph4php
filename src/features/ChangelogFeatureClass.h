/**
 * @copyright  2009-2011 Roberto Perpuly
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
#ifndef SRC_FEATURES_CHANGELOGFEATURECLASS_H_
#define SRC_FEATURES_CHANGELOGFEATURECLASS_H_

#include "features/FeatureClass.h"

namespace t4p {
/**
 * This is a small feature that shows the user the
 * change log file; the file that describes the various
 * updates that triump4hp has released
 */
class ChangelogFeatureClass : public t4p::FeatureClass {
	public:
	ChangelogFeatureClass(t4p::AppClass& app);

	void LoadPreferences(wxConfigBase* config);

	void ShowChangeLog();

	private:
	void OnAppReady(wxCommandEvent& event);

	void OnSavePreferences(wxCommandEvent& event);

	/**
	 * the last version of the app that was opened.
	 * This is value is stored in the config. When we read
	 * it in and it is different than the running version,
	 * it means that the user upgraded (or downgraded)
	 * versions.
	 */
	wxString LastVersion;

	DECLARE_EVENT_TABLE()
};
}  // namespace t4p

#endif  // SRC_FEATURES_CHANGELOGFEATURECLASS_H_
