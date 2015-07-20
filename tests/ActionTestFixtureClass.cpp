/**
 * @copyright  2012 Roberto Perpulybn
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

#include <ActionTestFixtureClass.h>

ActionTestFixtureClass::ActionTestFixtureClass()
	: wxEvtHandler()
	, RunningThreads(false)
	, Globals() {
	RunningThreads.AddEventHandler(this);
}

ActionTestFixtureClass::~ActionTestFixtureClass() {
	RunningThreads.RemoveEventHandler(this);
}

void ActionTestFixtureClass::InitTagCache(const wxString& cacheDir) {
	Globals.TagCacheDbFileName.Assign(cacheDir, wxT("tag_cache.db"));
	Globals.DetectorCacheDbFileName.Assign(cacheDir, wxT("detector_cache.db"));
}

void ActionTestFixtureClass::CreateProject(const wxFileName& sourceDir) {
	t4p::ProjectClass project;

	int projectNum = Globals.Projects.size() + 1;
	project.Label = wxString::Format(wxT("project %d"), projectNum);
	project.IsEnabled = true;


	t4p::SourceClass srcProject;
	srcProject.RootDirectory.Assign(sourceDir);
	project.AddSource(srcProject);

	Globals.Projects.push_back(project);
}
