/**
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
 *
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef T4P_TOTALSEARCHFEATURECLASS_H
#define T4P_TOTALSEARCHFEATURECLASS_H

#include <features/FeatureClass.h>
#include <actions/TotalTagSearchActionClass.h>
#include <actions/ActionClass.h>
#include <wx/timer.h>

namespace t4p {

/**
 * This feature allows the user to search for any PHP class,
 * function, file, or database table.
 * The class will search most of the tables in the tag cache
 * schema.
 */
class TotalSearchFeatureClass : public t4p::FeatureClass {

public:

	TotalSearchFeatureClass(t4p::AppClass& app);

	void OpenFileTag(const t4p::FileTagClass& fileTag, int lineNumber);

	void OpenPhpTag(const t4p::PhpTagClass& tag);

	void OpenDbTable(const t4p::DatabaseTableTagClass& tableTag);

	void OpenDbData(const t4p::DatabaseTableTagClass& tableTag);

};

}

#endif
