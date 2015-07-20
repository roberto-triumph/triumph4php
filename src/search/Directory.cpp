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
#include <search/Directory.h>
#include <wx/dir.h>
#include <wx/utils.h>
#include <wx/filename.h>

bool t4p::RecursiveRmDir(wxString path) {
	wxDir dir(path);
	wxString name;
	bool next = dir.GetFirst(&name, wxEmptyString, wxDIR_FILES | wxDIR_DIRS | wxDIR_HIDDEN);

	// if dir is already empty then we want to return true
	bool good = !next;
	wxFileName dirName;
	dirName.AssignDir(path);
	while (next) {
		wxString nextFile = dirName.GetPathWithSep() + name;
		if (wxDirExists(nextFile)) {
			RecursiveRmDir(nextFile);
		} else {
			good = wxRemoveFile(nextFile);
			wxASSERT_MSG(good, wxT("could not remove file:") + nextFile);
		}
		next = dir.GetNext(&name);
	}

	// remove this directory
	good = wxRmdir(path);
	wxASSERT_MSG(good, wxT("could not remove directory:") + path);
	return good;
}
