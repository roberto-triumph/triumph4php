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
#ifndef __statusbarwithgaugeclass__
#define __statusbarwithgaugeclass__

#include <wx/wx.h>
#include <map>

namespace mvceditor {

/**
 * This class allows the status bar to be populated with a gauge (progress bar).
 */
class StatusBarWithGaugeClass : public wxStatusBar {
public:
    StatusBarWithGaugeClass(wxWindow *parent, int id = wxID_ANY);
	
	/**
	 * When window is resized redraw accordingly
	 */
    void OnSize(wxSizeEvent& event);
	
	/**
	 * Adds a progress bar to this status bar.
	 * 
	 * @param wxString title will be displayed next to the progress bar
	 * @param int id the gauge ID
	 * @param int maxValue the maximum value of the gauge. INDETERMINATE_MODE can be used to make the gauge just pulse
	 * @param int flags wxWindow style flags
	 */
	void AddGauge(const wxString& title, int id, int maxValue, int flags);
	
	/**
	 * Increment / decrement the progress.
	 * 
	 * @param int id the gauge ID 
	 * @param update new progress bar value can be INDETERMINATE_MODE to make the gauge just pulse
	 */
	void UpdateGauge(int id, int update);
	
	/**
	 * Increments the progress.
	 * 
	 * @param int id the gauge ID
	 * @param update number to increment by can be INDETERMINATE_MODE to make the gauge just pulse
	 */
	void IncrementGauge(int id, int increment = 1);
	
	/**
	 * Removes the progress bar (user will no longer see it)
	 */
	void StopGauge(int id);

	/**
	 * Set the text of the column index 0 (the first column)
	 */
	void SetColumn0Text(const wxString& text);

	/**
	 * Set the text of the column index 1 (the second column)
	 */
	void SetColumn1Text(const wxString& text);
	
	enum {
		INDETERMINATE_MODE = -1
	};

private:
    enum {
        FIELD_TEXT,
        FIELD_GAUGE_TITLE,
		FIELD_GAUGE,
		FIELD_MAX
    };
	
	std::map<int, wxGauge*> Gauges;
	std::map<int, wxString> GaugeTitles;
	static const int BITMAP_SIZE_Y = 15;
	
	/**
	 * This method should be called when a new gauge is added or removed.  
	 */
	void RedrawGauges();
	
	DECLARE_EVENT_TABLE()
};

}

#endif // __statusbarwithgaugeclass__
