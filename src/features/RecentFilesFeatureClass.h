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
#ifndef T4P_RECENTFILESFEATURECLASS_H
#define T4P_RECENTFILESFEATURECLASS_H

#include <features/FeatureClass.h>
#include <wx/docview.h>

namespace t4p {
extern const int MAX_RECENT_FILES;

/**
 * The recent files feature keeps track of the most
 * recent files that the user has opened in Triumph.
 */
class RecentFilesFeatureClass : public FeatureClass {
	public:
	/**
	 * class that encapsulates the logic
	 */
	wxFileHistory FileHistory;

	RecentFilesFeatureClass(t4p::AppClass& app);

	void LoadPreferences(wxConfigBase* config);

	void SavePreferences();

	private:
};
}

#endif
