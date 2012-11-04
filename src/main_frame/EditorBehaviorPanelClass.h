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
#ifndef __EditorBehaviorPanelClass__
#define __EditorBehaviorPanelClass__

/**
@file
Subclass of EditorBehaviorPanelGeneratedClass, which is generated by wxFormBuilder.
*/

#include <main_frame/wxformbuilder/MainFrameForms.h>
#include <globals/CodeControlOptionsClass.h>

namespace mvceditor {
	
/** Implementing EditorBehaviorPanelClass */
class EditorBehaviorPanelClass : public EditorBehaviorPanelGeneratedClass {
protected:
	// Handlers for EditorBehaviorPanelGeneratedClass events.
	void OnIndentUsingSpaces(wxCommandEvent& event);
	void OnCheckRightMargin(wxCommandEvent& event);
public:
	/** Constructor */
	EditorBehaviorPanelClass(wxWindow* parent, CodeControlOptionsClass& options);
	bool TransferDataToWindow();
	bool TransferDataFromWindow();
};

}
#endif // __EditorBehaviorPanelClass__
