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
#ifndef SRC_GLOBALS_FILENAME_H_
#define SRC_GLOBALS_FILENAME_H_

#include <wx/filename.h>
#include <vector>

namespace t4p {
/**
 * Deep Copy a a wxFileName. This is not trivial since wxFileName's assignment performs shallow copies
 * of internal wxStrings.
 * This function should be used when wxFileNames are passed between threads via posted events.
 *
 * @param fileName file to deep copy
 * @param wxFileName new filename
 */
wxFileName FileNameCopy(const wxFileName& fileName);

/**
 * Deep Copy a vector of wxFileNames.  This is not trivial since wxFileName's assignment performs shallow copies
 * of internal wxStrings.
 * This will be used when vectors of wxFileNames are passed between threads via posted events.
 *
 * @param dest vector to insert/delete wxFileNames from
 * @return the cloned filenames
 */
std::vector<wxFileName> DeepCopyFileNames(const std::vector<wxFileName>& fileNames);
}  // namespace t4p

#endif  // SRC_GLOBALS_FILENAME_H_
