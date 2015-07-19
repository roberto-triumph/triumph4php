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

#ifndef T4P_DATABASETABLETAGCLASS_H
#define T4P_DATABASETABLETAGCLASS_H

#include <wx/string.h>

namespace t4p {
/**
 * This class represents a database table that exists in one of the
 * configured SQL connections
 */
class DatabaseTableTagClass {
	public:
	/**
	 * The name of the table.  Note that this is case sensitive
	 * on some platforms (ie MySQL on MSW)
	 */
	wxString TableName;

	/**
	 * The connection where this table can be found
	 */
	wxString ConnectionHash;

	DatabaseTableTagClass();

	DatabaseTableTagClass(const t4p::DatabaseTableTagClass& src);

	void Copy(const t4p::DatabaseTableTagClass& src);
};
}

#endif // T4P_DATABASETABLETAGCLASS_H
