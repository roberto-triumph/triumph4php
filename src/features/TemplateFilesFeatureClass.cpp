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
#include <features/TemplateFilesFeatureClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <language_php/TemplateFileTagClass.h>
#include <actions/CallStackActionClass.h>
#include <actions/TemplateFileTagsDetectorActionClass.h>
#include <actions/SequenceClass.h>
#include <Triumph.h>

t4p::TemplateFilesFeatureClass::TemplateFilesFeatureClass(t4p::AppClass& app) 
	: FeatureClass(app) {
}

void t4p::TemplateFilesFeatureClass::StartDetection() {	
	
	// start the chain reaction
	if (App.Sequences.Running()) {
		wxMessageBox(_("Please wait for the current background task to finish"));
		return;
	}
	std::vector<t4p::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	t4p::CallStackActionClass* callStackAction =  new t4p::CallStackActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_CALL_STACK);
	t4p::UrlTagClass urlTag = App.Globals.CurrentUrl;
	callStackAction->SetCallStackStart(urlTag.FileName,
		t4p::WxToIcu(urlTag.ClassName),
		t4p::WxToIcu(urlTag.MethodName),
		App.Globals.DetectorCacheDbFileName);
	actions.push_back(callStackAction);
	actions.push_back(
		new t4p::TemplateFileTagsDetectorActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_TEMPLATE_FILE_TAG_DETECTOR)
	);
	App.Sequences.Build(actions);
}

t4p::UrlTagFinderClass& t4p::TemplateFilesFeatureClass::Urls() {
	return App.Globals.UrlTagFinder;
}

void t4p::TemplateFilesFeatureClass::SetCurrentUrl(t4p::UrlTagClass url) {
	App.Globals.CurrentUrl = url;
}

