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
#ifndef SRC_VIEWS_RECENTFILESVIEWCLASS_H_
#define SRC_VIEWS_RECENTFILESVIEWCLASS_H_

#include <views/FeatureViewClass.h>
#include <features/RecentFilesFeatureClass.h>
#include <globals/Events.h>
#include <wx/docview.h>

namespace t4p {
class RecentFilesViewClass : public FeatureViewClass {
	public:
	RecentFilesViewClass(t4p::RecentFilesFeatureClass& feature);

	~RecentFilesViewClass();

	void AddFileMenuItems(wxMenu* fileMenu);

	void SavePreferences();

	private:
	/**
	 * sub-menu to hold the recent files
	 */
	wxMenu* RecentFilesMenu;

	/**
	 * class that encapsulates the logic
	 */
	t4p::RecentFilesFeatureClass& Feature;

	/**
	 * handler for the file menu event
	 */
	void OnRecentFileMenu(wxCommandEvent& event);

	/**
	 * when a file has been opened, add it to the recent
	 * list.
	 */
	void OnAppFileOpened(t4p::CodeControlEventClass& event);

	/**
	 * when a file has been created, add it to the recent
	 * list.
	 */
	void OnAppFileCreated(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};
}  // namespace t4p

#endif  // SRC_VIEWS_RECENTFILESVIEWCLASS_H_
