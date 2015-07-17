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
#ifndef T4P_CALLTIPFEATURECLASS_H
#define T4P_CALLTIPFEATURECLASS_H

#include <features/FeatureClass.h>
#include <views/wxformbuilder/DocCommentFeatureForms.h>
#include <wx/hyperlink.h>

namespace t4p {

// forward declaration, defined in another file
class CodeControlClass;

/**
 * this feature will show a small panel with the PHP Doc
 * comment of the item that is currently under the 
 * mouse pointer or at the current cursor position. 
 */
class DocCommentFeatureClass : public t4p::FeatureClass {

public:

	DocCommentFeatureClass(t4p::AppClass& app);
	
	/**
	 * @return TRUE if this feature is enabled
	 */
	bool IsEnabled() const;

};

}

#endif
