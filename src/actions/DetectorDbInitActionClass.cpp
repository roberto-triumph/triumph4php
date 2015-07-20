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

#include <actions/DetectorDbInitActionClass.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <string>
#include <vector>

t4p::DetectorDbInitActionClass::DetectorDbInitActionClass(t4p::RunningThreadsClass& runningThreads, int eventId)
	: InitializerGlobalActionClass(runningThreads, eventId) {
}

void t4p::DetectorDbInitActionClass::Work(t4p::GlobalsClass &globals) {
	SetStatus(_("Detector Db Init"));

	// open the tag db
	// it may not exist if the user screwed up and pointed their settings
	// dir to a non-existing location.
	if (!globals.DetectorCacheDbFileName.Exists()) {
		return;
	}
	std::string sqliteFile = t4p::WxToChar(globals.DetectorCacheDbFileName.GetFullPath());
	globals.DetectorCacheSession.open(*soci::factory_sqlite3(), sqliteFile);

	// reload the detected database tags

	// first remove all detected connections that were previously detected
	std::vector<t4p::DatabaseTagClass>::iterator info;
	info = globals.DatabaseTags.begin();
	while(info != globals.DatabaseTags.end()) {
		if (info->IsDetected) {
			info = globals.DatabaseTags.erase(info);
		} else {
			info++;
		}
	}

	std::vector<wxFileName> sourceDirectories = globals.AllEnabledSourceDirectories();

	// initialize the detected tag cache only the enabled projects
	t4p::DatabaseTagFinderClass finder(globals.DetectorCacheSession);

	std::vector<t4p::DatabaseTagClass> detected = finder.All(sourceDirectories);
	std::vector<t4p::DatabaseTagClass>::const_iterator tag;
	for (tag = detected.begin(); tag != detected.end(); ++tag) {
		if (!tag->Host.isEmpty() && !tag->Schema.isEmpty()) {
			globals.DatabaseTags.push_back(*tag);
		}
	}
}

wxString t4p::DetectorDbInitActionClass::GetLabel() const {
	return _("Database tags detector initialization");
}
