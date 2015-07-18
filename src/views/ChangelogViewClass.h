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
#ifndef T4P_CHANGELOGVIEWCLASS_H
#define T4P_CHANGELOGVIEWCLASS_H

#include <views/FeatureViewClass.h>

namespace t4p {

// forward declaration, defined in another file
class ChangelogFeatureClass;

/**
 * Adds a menu item that enables the user to see
 * triumph's release change log
 */
class ChangelogViewClass : public t4p::FeatureViewClass {

public:

	ChangelogViewClass(t4p::ChangelogFeatureClass& feature);

	void AddHelpMenuItems(wxMenu* helpMenu);

private:

	void OnHelpChangelog(wxCommandEvent& event);

	t4p::ChangelogFeatureClass& Feature;

	DECLARE_EVENT_TABLE()
};

}

#endif

