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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <actions/TagWipeActionClass.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

mvceditor::TagWipeActionClass::TagWipeActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId,
															const std::vector<mvceditor::ProjectClass>& projects)
	: ActionClass(runningThreads, eventId) 
	, ResourceDbFileNames() {
	std::vector<mvceditor::ProjectClass>::const_iterator it;
	for (it = projects.begin(); it != projects.end(); ++it) {
		if (it->IsEnabled && it->ResourceDbFileName.IsOk()) {
			ResourceDbFileNames.push_back(it->ResourceDbFileName);
		}
	}
}

bool mvceditor::TagWipeActionClass::Init(mvceditor::GlobalsClass& globals) {

	// will only wipe the projects given in the constructor, not all of the projects
	return !ResourceDbFileNames.empty();
}

void mvceditor::TagWipeActionClass::BackgroundWork() {
	std::vector<wxFileName>::iterator it;
	for (it = ResourceDbFileNames.begin(); it != ResourceDbFileNames.end() && !TestDestroy(); ++it) {
		
		// initialize the sqlite db
		soci::session session;
		try {
			session.open(*soci::factory_sqlite3(), mvceditor::WxToChar(it->GetFullPath()));
			mvceditor::TagParserClass tagParser;
			tagParser.Init(&session);
			tagParser.Wipe();
		} catch(std::exception const& e) {
			session.close();
			wxString msg = mvceditor::CharToWx(e.what());
			wxASSERT_MSG(false, msg);
		}
	}
}

wxString mvceditor::TagWipeActionClass::GetLabel() const {
	return wxT("Tag Cache Wipe");
}