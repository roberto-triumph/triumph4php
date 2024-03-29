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
#ifndef SRC_LANGUAGE_PHP_DETECTORDBCLASS_H_
#define SRC_LANGUAGE_PHP_DETECTORDBCLASS_H_

#include <soci/soci.h>
#include <wx/filename.h>
#include "globals/Sqlite.h"

namespace t4p {
/**
 * This class performs write operations on the detector tags
 * cache database.
 */
class DetectorDbClass {
 public:
    DetectorDbClass();

    /**
     * @param session opened db connection. This class will not own the pointer.
     */
    void Init(soci::session* session);

    /**
     * deletes all detected tags (db, config, urls, templates, etc...)
     * that were detected from the given source directory.
     */
    void DeleteSource(const wxFileName& sourceDir);

    /**
     * deletes all rows from all tables.
     */
    void Wipe();

 private:
    /**
     * This class will not own the pointer.
     */
    soci::session* Session;
};
}  // namespace t4p

#endif  // SRC_LANGUAGE_PHP_DETECTORDBCLASS_H_
