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
#include <views/ChangelogViewClass.h>
#include <features/ChangelogFeatureClass.h>

t4p::ChangelogViewClass::ChangelogViewClass(t4p::ChangelogFeatureClass& feature)
: FeatureViewClass()
, Feature(feature)
{

}

void t4p::ChangelogViewClass::AddHelpMenuItems(wxMenu* helpMenu) {
	helpMenu->Append(t4p::MENU_CHANGELOG + 0, _("See ChangeLog"),
		_("Open the triumph4php ChangeLog file"), wxITEM_NORMAL);
}

void t4p::ChangelogViewClass::OnHelpChangelog(wxCommandEvent& event) {
	Feature.ShowChangeLog();
}

BEGIN_EVENT_TABLE(t4p::ChangelogViewClass, t4p::FeatureViewClass)
	EVT_MENU(t4p::MENU_CHANGELOG + 0, t4p::ChangelogViewClass::OnHelpChangelog)
END_EVENT_TABLE()
