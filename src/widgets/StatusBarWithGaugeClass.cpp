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
 * @author     $Author: robertop2004@gmail.com $
 * @date       $Date: 2011-06-10 17:19:31 -0700 (Fri, 10 Jun 2011) $
 * @version    $Rev: 555 $ 
 */
#include <widgets/StatusBarWithGaugeClass.h>

mvceditor::StatusBarWithGaugeClass::StatusBarWithGaugeClass(wxWindow *parent, int id)
           : wxStatusBar(parent, id, wxST_SIZEGRIP | wxFULL_REPAINT_ON_RESIZE)
			, Gauges()
			, GaugeTitles() {
}

void mvceditor::StatusBarWithGaugeClass::AddGauge(const wxString& title, int id, int maxValue, int flags) {
	wxGauge* gauge = new wxGauge(this, wxID_ANY, maxValue, wxDefaultPosition, wxDefaultSize, flags);
	if (maxValue != INDETERMINATE_MODE) {
		gauge->SetValue(0);
	}
	GaugeTitles[id] = title;
	Gauges[id] = gauge;
	RedrawGauges();
}

void mvceditor::StatusBarWithGaugeClass::UpdateGauge(int id, int update) {
	if (Gauges.count(id) > 0) {
		wxGauge* gauge = Gauges[id];
		if (update != INDETERMINATE_MODE) {
			gauge->SetValue(update);
		}
		else {
			gauge->Pulse();
		}
	}
}

void mvceditor::StatusBarWithGaugeClass::IncrementGauge(int id, int increment) {
	if (Gauges.count(id) > 0) {
		wxGauge* gauge = Gauges[id];
		if (increment != INDETERMINATE_MODE) {
			gauge->SetValue(gauge->GetValue() + increment);
		}
		else {
			gauge->Pulse();
		}
	}
}

void mvceditor::StatusBarWithGaugeClass::StopGauge(int id) {
	if (Gauges.count(id) > 0) {
		wxGauge* gauge = Gauges[id];
		RemoveChild(gauge);
		gauge->Destroy();
		Gauges.erase(id);
		GaugeTitles.erase(id);		
		SetStatusText(wxT(""), FIELD_GAUGE_TITLE);
		RedrawGauges();
	}
}

void mvceditor::StatusBarWithGaugeClass::OnSize(wxSizeEvent& event) {
	RedrawGauges();
    event.Skip();
}

void mvceditor::StatusBarWithGaugeClass::RedrawGauges() {
	
	// Each gauge takes it 2 columns (one for the gauge title and one for the gauge itself), plus the leftmost status bar column
	int numberOfColumns = (Gauges.size() * 2) + 1;
	int* widths = new int[numberOfColumns];
	widths[0] = -1;
	for (int i = 1; i < numberOfColumns; i += 2) {		
		widths[i] = 150;
		widths[i + 1] = 300;
	}
    SetFieldsCount(numberOfColumns);
    SetStatusWidths(numberOfColumns, widths);
	//ATTN: in MSW this line results in a crash why??
	wxPlatformInfo platform;
	if (!(wxOS_WINDOWS | platform.GetOperatingSystemId())) {
		SetMinHeight(BITMAP_SIZE_Y);
	}
	int col = 1;
	for (std::map<int, wxGauge*>::iterator it = Gauges.begin(); it != Gauges.end(); ++it) {
		SetStatusText(GaugeTitles[it->first], col++);
		wxRect rect;
		GetFieldRect(col++, rect);
		it->second->SetSize(rect.x + 2, rect.y + 2, rect.width - 4, rect.height - 4);
	}
	delete[] widths;
}

BEGIN_EVENT_TABLE(mvceditor::StatusBarWithGaugeClass, wxStatusBar)
    EVT_SIZE(mvceditor::StatusBarWithGaugeClass::OnSize)
END_EVENT_TABLE()