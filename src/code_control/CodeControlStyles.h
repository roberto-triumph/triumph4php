/**
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
 * The MIT License
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
#ifndef SRC_CODE_CONTROL_CODECONTROLSTYLES_H_
#define SRC_CODE_CONTROL_CODECONTROLSTYLES_H_

#include <globals/CodeControlOptionsClass.h>

namespace t4p {
/**
 * Initializes the color schemes for the source code editor.
 * These are the hard-coded program defaults only.
 */
void CodeControlStylesInit(t4p::CodeControlOptionsClass& options);

/**
 * Set the color scheme to the default scheme
 */
void CodeControlStylesSetTheme(t4p::CodeControlOptionsClass& options, const wxString& theme);

/**
 * @return all of the theme names
 */
wxArrayString CodeControlStylesGetThemes();
}

#endif  // SRC_CODE_CONTROL_CODECONTROLSTYLES_H_
