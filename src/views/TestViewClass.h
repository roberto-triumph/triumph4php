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
#ifndef SRC_VIEWS_TESTVIEWCLASS_H_
#define SRC_VIEWS_TESTVIEWCLASS_H_

#include <views/FeatureViewClass.h>
#include <features/TestFeatureClass.h>

namespace t4p {
/**
 * This is a feature that is designed to test the feature mechanism.
 * This is also useful for prototyping of a feature or debugging.
 */
class TestViewClass : public FeatureViewClass {
	public:
	TestViewClass(t4p::TestFeatureClass& feature);

	/**
	 * This feature will have no Edit menu items
	 */
	void AddEditMenuItems(wxMenu* editMenu);

	/**
	 * This feature will have one toolbar button, when clicked the Go() method will get called
	 */
	void AddToolBarItems(wxAuiToolBar* toolBar);

	/**
	 * This feature will have no extra windows.
	 */
	void AddWindows();

	/**
	 * This feature will have no context menu items
	 */
	void AddCodeControlClassContextMenuItems(wxMenu* menu);

	private:
	/**
	 * the actual code that will get executed when the "GO" button is clicked
	 */
	void Go(wxCommandEvent& event);

	/**
	 * most logic should be contained in the feature
	 */
	t4p::TestFeatureClass& Feature;

	DECLARE_EVENT_TABLE()
};
}

#endif  // SRC_VIEWS_TESTVIEWCLASS_H_
