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
#ifndef PROJECTFEATURECLASS_H
#define PROJECTFEATURECLASS_H

#include <features/FeatureClass.h>

namespace t4p {


class ProjectFeatureClass : public FeatureClass {

	public:

	/**
	 * the location of the "file explorer" binary.  This is dependent per OS.
	 * @var wxString
	 */
	wxString ExplorerExecutable;

	/**
	 * Create a new instance of ProjectFeatureClass.
	 */
	ProjectFeatureClass(t4p::AppClass& app);

	~ProjectFeatureClass();

	/**
	 * Load the preferences from persistent storage
	 */
	void LoadPreferences(wxConfigBase* config);

	/**
	 * Creates a project with only 1 source directory. The
	 * source will include all of the Global file types
	 * The new project will be stored in memory and the
	 * config will be flushed so that it is persisted right away.
	 * Also, the app will be notified that a new project has been
	 * created. Project will be set as enabled; its label
	 * will be the last directory's name.
	 *
	 * @param dir the full path to the root of the project.The
	 *        directory is assumed to exist.
	 * @param doTag if TRUE, then the new project will be tagged (indexed)
	 *        the indexing will be done in a background thread; after this
	 *        method completes the indexing will be taking place and the
	 *        indexing will complete at a later time.
	 */
	void CreateProject(const wxString& dir, bool doTag);

	private:

	/**
	 * Save the preferences to persistent storage
	 */
	void OnPreferencesSaved(wxCommandEvent& event);

	void OnPreferencesExternallyUpdated(wxCommandEvent& event);

	/**
	 * close all projects and all resources that depend on it
	 */
	void CleanupProjects();

	DECLARE_EVENT_TABLE()

};

}

#endif
