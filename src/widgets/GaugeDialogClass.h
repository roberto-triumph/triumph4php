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
#ifndef SRC_WIDGETS_GAUGEDIALOGCLASS_H_
#define SRC_WIDGETS_GAUGEDIALOGCLASS_H_

#include <wx/timer.h>
#include "widgets/wxformbuilder/GaugeDialogWidget.h"

namespace t4p {
/**
 * A small dialog that contains a Gauge (progress bar) and a label. This dialog
 * will make the gauge pulse without any outside code.
 *
 * Sometimes it is best to create a full-fledged dialog because the gauge that is
 * located in the lower status bar might not be enough feedback for the user.
 */
class GaugeDialogClass : public GaugeDialogGeneratedClass {
 public:
    GaugeDialogClass(wxWindow* parent, const wxString& title, const wxString& label);

    /**
     * initialize this dialog's gauge (in pulse mode)
     */
    void Start();

 protected:
    void OnHideButton(wxCommandEvent& event);

    void OnTimer(wxTimerEvent& event);

 private:
    wxTimer Timer;

    DECLARE_EVENT_TABLE()
};
}  // namespace t4p

#endif  // SRC_WIDGETS_GAUGEDIALOGCLASS_H_
