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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __T4P_FILECABINETFEATURECLASS_H__
#define __T4P_FILECABINETFEATURECLASS_H__

#include <features/FeatureClass.h>
#include <globals/FileCabinetItemClass.h>
#include <globals/Sqlite.h>

namespace t4p {

/**
 * The file cabinet features allows the user to add save any filename
 * or directory name in the user's system to be recalled later.
 * The file cabinet allows the user to save the location of a file or
 * directory that is opened infrequently, yet it is deep in a sub
 * directory that makes it cumbersome to open.  As an example, the
 * user can add the system hosts file into the file cabinet; the user
 * can recall and open that file very quickly using the file
 * cabinet.
 */
class FileCabinetFeatureClass : public t4p::FeatureClass {

public:
	
	FileCabinetFeatureClass(t4p::AppClass& app);

	/**
	 * add a file to the file cabinet store (sqlite)
	 */
	t4p::FileCabinetItemClass AddFileToCabinet(const wxString& file);

	/**
	 * add a directory to the file cabinet store (sqlite)
	 */
	t4p::FileCabinetItemClass AddDirectoryToCabinet(const wxString& dir);

	/**
	 * removes the given cabinet item from the file cabinet store (sqlite)
	 */
	void DeleteCabinetItem(int id);
		
private:

	/**
	 * to modify the file cabinet
	 */
	t4p::FileCabinetStoreClass Store;
	
};

}

#endif // __T4P_FILECABINETFEATURECLASS_H__
