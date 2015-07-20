/**
 * @copyright  2009-2011 Roberto Perpuly
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
#include <widgets/StatusBarWithGaugeClass.h>
#include <map>

t4p::StatusBarWithGaugeClass::StatusBarWithGaugeClass(wxWindow *parent, int id)
           : wxStatusBar(parent, id, wxST_SIZEGRIP | wxFULL_REPAINT_ON_RESIZE)
			, Gauges()
			, GaugeTitles() {
}

bool t4p::StatusBarWithGaugeClass::HasGauge(int id) const {
	return Gauges.count(id) > 0;
}

void t4p::StatusBarWithGaugeClass::AddGauge(const wxString& title, int id, int maxValue, int flags) {
	wxString msg = wxString::Format(wxT("There is already another gauge with ID =%d; gauge name=%s"), id, title.c_str());
	wxUnusedVar(msg);
	wxASSERT_MSG(Gauges.count(id) == 0, msg);
	wxGauge* gauge = new wxGauge(this, wxID_ANY, maxValue, wxDefaultPosition, wxDefaultSize, flags);
	if (maxValue != INDETERMINATE_MODE) {
		gauge->SetValue(0);
	}
	GaugeTitles[id] = title;
	Gauges[id] = gauge;
	RedrawGauges();
}

void t4p::StatusBarWithGaugeClass::UpdateGauge(int id, int update) {
	if (Gauges.count(id) > 0) {
		wxGauge* gauge = Gauges[id];
		if (update != INDETERMINATE_MODE) {
			gauge->SetValue(update);
		} else {
			gauge->Pulse();
		}
	}
}

void t4p::StatusBarWithGaugeClass::SwitchMode(int id, int mode, int value, int range) {
	if (Gauges.count(id) > 0) {
		wxGauge* gauge = Gauges[id];
		if (INDETERMINATE_MODE == mode) {
			gauge->Pulse();
		} else {
			// must set range first, otherwise value could be "invalid" (outside the
			// previous range)
			gauge->SetRange(range);
			gauge->SetValue(value);
		}
	}
}

void t4p::StatusBarWithGaugeClass::IncrementGauge(int id, int increment) {
	if (Gauges.count(id) > 0) {
		wxGauge* gauge = Gauges[id];
		if (increment != INDETERMINATE_MODE) {
			gauge->SetValue(gauge->GetValue() + increment);
		} else {
			gauge->Pulse();
		}
	}
}

void t4p::StatusBarWithGaugeClass::IncrementAndRenameGauge(int id, const wxString& title, int increment) {
	if (Gauges.count(id) > 0) {
		GaugeTitles[id] = title;
		RedrawGauges();

		wxGauge* gauge = Gauges[id];
		if (increment != INDETERMINATE_MODE) {
			gauge->SetValue(gauge->GetValue() + increment);
		} else {
			gauge->Pulse();
		}
	}
}

void t4p::StatusBarWithGaugeClass::RenameGauge(int id, const wxString& title) {
	if (Gauges.count(id) > 0) {
		GaugeTitles[id] = title;
		RedrawGauges();
	}
}

void t4p::StatusBarWithGaugeClass::StopGauge(int id) {
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

void t4p::StatusBarWithGaugeClass::OnSize(wxSizeEvent& event) {
	RedrawGauges();
    event.Skip();
}

void t4p::StatusBarWithGaugeClass::RedrawGauges() {
	// Each gauge takes it 2 columns (one for the gauge title and one for the gauge itself),
	// plus the leftmost default status bar columns (for the menu help and other messages)
	const int DEFAULT_COLUMNS = 2;
	int newColumnCount = (Gauges.size() * 2) + DEFAULT_COLUMNS;
	int* widths = new int[newColumnCount];
	for (int i = 0; i < DEFAULT_COLUMNS; ++i) {
		widths[i] = -1;
	}
	for (int i = DEFAULT_COLUMNS; i < newColumnCount; i += 2) {
		widths[i] = 150;
		widths[i + 1] = 300;
	}
    SetFieldsCount(newColumnCount);
    SetStatusWidths(newColumnCount, widths);

	// ATTN: in MSW this line results in a crash why??
	wxPlatformInfo platform;
	if (!(wxOS_WINDOWS | platform.GetOperatingSystemId())) {
		SetMinHeight(BITMAP_SIZE_Y);
	}
	int col = DEFAULT_COLUMNS;
	for (std::map<int, wxGauge*>::iterator it = Gauges.begin(); it != Gauges.end(); ++it) {
		SetStatusText(GaugeTitles[it->first], col++);
		wxRect rect;
		GetFieldRect(col++, rect);
		it->second->SetSize(rect.x + 2, rect.y + 2, rect.width - 4, rect.height - 4);
	}
	delete[] widths;
}

void t4p::StatusBarWithGaugeClass::SetColumn0Text(const wxString &text) {
	// don't redraw unless necessary
	wxString oldText = GetStatusText(0);
	if (oldText == text) {
		return;
	}
	if (!oldText.IsEmpty() || !text.IsEmpty()) {
		SetStatusText(text, 0);
	}
}

void t4p::StatusBarWithGaugeClass::SetColumn1Text(const wxString &text) {
	// don't redraw unless necessary
	wxString oldText = GetStatusText(1);
	if (oldText == text) {
		return;
	}
	if (!oldText.IsEmpty() || !text.IsEmpty()) {
		SetStatusText(text, 1);
	}
}

BEGIN_EVENT_TABLE(t4p::StatusBarWithGaugeClass, wxStatusBar)
    EVT_SIZE(t4p::StatusBarWithGaugeClass::OnSize)
END_EVENT_TABLE()
