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
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <views/FileWatcherViewClass.h>
#include <wx/valgen.h>

t4p::FileWatcherViewClass::FileWatcherViewClass(t4p::FileWatcherFeatureClass& feature)
: FeatureViewClass() 
, Feature(feature) {

}

void t4p::FileWatcherViewClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	t4p::FileWatcherPreferencesPanelClass* prefs = new t4p::FileWatcherPreferencesPanelClass(parent, Feature);
	parent->AddPage(prefs, _("File Watcher"));
}

void t4p::FileWatcherViewClass::OnFileClosed(t4p::CodeControlEventClass& event) {
	t4p::CodeControlClass* ctrl = event.GetCodeControl();
	if (ctrl) {
		Feature.UntrackOpenedFile(ctrl->GetFileName());
	}
}

void t4p::FileWatcherViewClass::OnFileOpened(t4p::CodeControlEventClass& event) {
	t4p::CodeControlClass* ctrl = event.GetCodeControl();
	if (ctrl) {
		Feature.TrackOpenedFile(ctrl->GetFileName());
	}
}

t4p::FileWatcherPreferencesPanelClass::FileWatcherPreferencesPanelClass(wxWindow* parent, t4p::FileWatcherFeatureClass& feature)
: FileWatcherPreferencesPanelGeneratedClass(parent, wxID_ANY)
, Feature(feature) {
	wxGenericValidator validator(&Feature.Enabled);
	Enabled->SetValidator(validator);
}

BEGIN_EVENT_TABLE(t4p::FileWatcherViewClass, t4p::FeatureViewClass)
	EVT_APP_FILE_OPEN(t4p::FileWatcherViewClass::OnFileOpened)
	EVT_APP_FILE_CLOSED(t4p::FileWatcherViewClass::OnFileClosed)
END_EVENT_TABLE()
