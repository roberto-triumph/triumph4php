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
#ifndef SRC_WIDGETS_CHOOSEURLDIALOGCLASS_H_
#define SRC_WIDGETS_CHOOSEURLDIALOGCLASS_H_

#include <widgets/wxformbuilder/ChooseUrlDialogWidget.h>
#include <language_php/UrlTagClass.h>
#include <globals/ProjectClass.h>
#include <wx/wx.h>

namespace t4p {
/**
 * Small dialog that allows the user to choose a URL to run. Url list
 * is populated from the URL detector cache (UrlTagFinder).
 */
class ChooseUrlDialogClass : public ChooseUrlDialogGeneratedClass {
	public:
	ChooseUrlDialogClass(wxWindow* parent, UrlTagFinderClass& urls,
		const std::vector<t4p::ProjectClass>& projects,
		const t4p::FileTypeClass& fileType,
		UrlTagClass& chosenUrl);

	protected:
	void OnOkButton(wxCommandEvent& event);

	/**
	 * events on the filter textbox, so that the list gets updates
	 * as the user types in text
	 */
	void OnFilterText(wxCommandEvent& event);
	void OnFilterTextEnter(wxCommandEvent& event);
	void OnFilterKeyDown(wxKeyEvent& event);

	/**
	 * updates the URL label so that the user can see the exact URL to
	 * be opened in the browser
	 */
	void OnListItemSelected(wxCommandEvent& event);

	/**
	 * double click == ok button
	 */
	void OnListItemDoubleClicked(wxCommandEvent& event);

	/**
	 * events on the extra textbox, so that the list gets updates
	 * as the user types in text
	 */
	void OnExtraText(wxCommandEvent& event);
	void OnExtraChar(wxKeyEvent& event);

	/**
	 * when a project is selected filter urls to show only urls from
	 * the chosen project.
	 */
	void OnProjectChoice(wxCommandEvent& event);

	private:
	/**
	 * Get the matching urls. a matching url is one that starts with the
	 * filter string (case insensitive).
	 *
	 * @return the urls that match filter.
	 */
	std::vector<t4p::UrlTagClass> GetFilteredUrls(const wxString& filter);

	/**
	 * Get the matching urls. a matching url is one that starts with the
	 * filter string (case insensitive) AND the URL belongs to the given project.
	 * A URL belongs to a project if the URL's controller file is in the project's
	 * sources.
	 *
	 * @return the urls that match filter and the project.
	 */
	std::vector<t4p::UrlTagClass> GetFilteredUrlsByProject(const wxString& filter,
		const t4p::ProjectClass& project);

	/**
	 * fills the url list box with the given urls. any previously existing urls in the
	 * list box are removed.
	 */
	void FillUrlList(const std::vector<t4p::UrlTagClass>& urls);

	/**
	 * @return list of source directories
	 */
	std::vector<wxFileName> ActiveSourceDirectories();

	/**
	 * The list of URLs, it will contain URLs that were detected and URLs that were input
	 * by the user.
	 * This pointer will not be owned by thie class
	 */
	UrlTagFinderClass& UrlTagFinder;

	/**
	 * The URL that the user selected.
	 */
	UrlTagClass& ChosenUrl;

	/**
	 * The defined projects. Will use them to allow the user to filter URLs
	 * by project
	 */
	std::vector<t4p::ProjectClass> Projects;

	/**
	 * The configured file type extensions.
	 */
	const t4p::FileTypeClass& FileType;
};
}  // namespace t4p

#endif  // SRC_WIDGETS_CHOOSEURLDIALOGCLASS_H_
