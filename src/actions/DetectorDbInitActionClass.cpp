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

#include <actions/DetectorDbInitActionClass.h>
#include <soci/sqlite3/soci-sqlite3.h>

mvceditor::DetectorDbInitActionClass::DetectorDbInitActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: InitializerGlobalActionClass(runningThreads, eventId) {

}

void mvceditor::DetectorDbInitActionClass::Work(mvceditor::GlobalsClass &globals) {
	SetStatus(_("Detector Db Init"));

	// open the tag db
	globals.DetectorCacheSession.open(*soci::factory_sqlite3(), 
		mvceditor::WxToChar(globals.DetectorCacheDbFileName.GetFullPath()));
	globals.UrlTagFinder.InitSession(&globals.DetectorCacheSession);
	
	// reload the detected database tags
	
	// first remove all detected connections that were previously detected
	std::vector<mvceditor::DatabaseTagClass>::iterator info;
	info = globals.DatabaseTags.begin();
	while(info != globals.DatabaseTags.end()) {
		if (info->IsDetected) {
			info = globals.DatabaseTags.erase(info);
		}
		else {
			info++;
		}
	}

	std::vector<wxFileName> sourceDirectories = globals.AllEnabledSourceDirectories();

	// initialize the detected tag cache only the enabled projects
	mvceditor::DatabaseTagFinderClass finder;
	finder.InitSession(&globals.DetectorCacheSession);
	
	std::vector<mvceditor::DatabaseTagClass> detected = finder.All(sourceDirectories);
	std::vector<mvceditor::DatabaseTagClass>::const_iterator tag;
	for (tag = detected.begin(); tag != detected.end(); ++tag) {
		if (!tag->Host.isEmpty() && !tag->Schema.isEmpty()) {
			globals.DatabaseTags.push_back(*tag);
		}
	}
}

wxString mvceditor::DetectorDbInitActionClass::GetLabel() const {
	return _("Database tags detector initialization");
}