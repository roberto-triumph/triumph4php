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
#include <widgets/GaugeDialogClass.h>

const static int ID_GAUGE_DIALOG_TIMER = wxNewId();

t4p::GaugeDialogClass::GaugeDialogClass(wxWindow* parent, const wxString& title, const wxString& label) 
	: GaugeDialogGeneratedClass(parent, wxID_ANY, title)
	, Timer(this, ID_GAUGE_DIALOG_TIMER) {
	Label->SetLabel(label);
}

void t4p::GaugeDialogClass::OnHideButton(wxCommandEvent &event) {
	Hide();
	Timer.Stop();
}

void t4p::GaugeDialogClass::Start() {
	Gauge->Pulse();
	Timer.Start(200, wxTIMER_CONTINUOUS);
}

void t4p::GaugeDialogClass::OnTimer(wxTimerEvent& event) {
	Gauge->Pulse();
}

BEGIN_EVENT_TABLE(t4p::GaugeDialogClass, GaugeDialogGeneratedClass)
	EVT_TIMER(ID_GAUGE_DIALOG_TIMER, t4p::GaugeDialogClass::OnTimer) 
END_EVENT_TABLE()
