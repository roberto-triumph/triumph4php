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
#ifndef __MVCEDITOR_FILETAGS_H__
#define __MVCEDITOR_FILETAGS_H__
 
#include <wx/filename.h>
#include <soci/soci.h>
#include <vector>

namespace mvceditor {

	/**
	 * Get the file_item_id for all files that are in any of the given directories. Files
	 * that are inside of sub-directories of any of the directories are also returned.
	 *
	 * @param session the db connection to the tags cache db
	 * @param dirs the directories to look up.
	 * @param error if there was a db error, this will be set to TRUE
	 * @param errorMsg if there was a db error, this string will be set with the error message
	 * @return list of file_item_id the file IDs for all files that are in the given directories
	 */
	std::vector<int> FileTagIdsForDirs(soci::session& session, const std::vector<wxFileName>& dirs, bool& error, wxString& errorMsg);

}
#endif