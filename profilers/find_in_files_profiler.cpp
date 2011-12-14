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
#include <search/FindInFilesClass.h>
#include <search/DirectorySearchClass.h>
#include <search/ResourceFinderClass.h>
#include <wx/filefn.h>
#include <wx/timer.h>
#include <unicode/uclean.h>

/**
 * Find in files over an entire directory, using an exact search
 */
void ProfileFindInFilesExactMode();

/**
 * Find in files over an entire directory, using an code search
 */
void ProfileFindInFilesCodeMode();

/**
 * Full path to a big directory of php files that will be used to profile the execution time of the resource finder.
 */
wxString DirName;

int main() {
	int major,
		minor;
	wxOperatingSystemId os = wxGetOsVersion(&major, &minor);
	if (os == wxOS_WINDOWS_NT) {
		DirName = wxT("C:\\Users\\Roberto\\sample_php_project\\");
	}
	else {
		DirName = wxT("/home/roberto/workspace/sample_php_project/");
	}
	ProfileFindInFilesExactMode();
	ProfileFindInFilesCodeMode();
	
	// calling cleanup here so that we can run this binary through a memory leak detector 
	// ICU will cache many things and that will cause the detector to output "possible leaks"
	u_cleanup();
	return 0;
}

void ProfileFindInFilesExactMode() {
	printf("*******\n");
	wxLongLong time = wxGetLocalTimeMillis();
	mvceditor::DirectorySearchClass directorySearch;
	if (directorySearch.Init(DirName)) {
		mvceditor::FindInFilesClass findInFiles;
		findInFiles.Expression = UNICODE_STRING_SIMPLE("class Db");
		findInFiles.FilesFilter = wxT("*.php");
		findInFiles.Mode = mvceditor::FinderClass::EXACT;
		if(findInFiles.Prepare()) {		
			while (directorySearch.More()) {
				directorySearch.Walk(findInFiles);
			}
			std::vector<wxString> matchedFiles = directorySearch.GetMatchedFiles();
			for (size_t i = 0; i < matchedFiles.size(); ++i) {
				printf("Found at least one match in file %s.\n", (const char*)matchedFiles[i].ToUTF8());
			}
			time = wxGetLocalTimeMillis() - time;
			printf("time for findInFiles Exact Mode:%ld ms \n", time.ToLong());
		}
		else {
			puts("Invalid expression: class Db\n");
		}
	}
	else {
		printf("Could not open Directory: %s\n", (const char*)DirName.ToAscii());
	}
}

void ProfileFindInFilesCodeMode() {
	printf("*******\n");
	wxLongLong time = wxGetLocalTimeMillis();
	mvceditor::DirectorySearchClass directorySearch;
	if (directorySearch.Init(DirName)) {
		mvceditor::FindInFilesClass findInFiles;
		findInFiles.Expression = UNICODE_STRING_SIMPLE("class Db");
		findInFiles.FilesFilter = wxT("*.php");
		findInFiles.Mode = mvceditor::FinderClass::EXACT;
		if(findInFiles.Prepare()) {		
			while (directorySearch.More()) {
				directorySearch.Walk(findInFiles);
			}
			std::vector<wxString> matchedFiles = directorySearch.GetMatchedFiles();
			for (size_t i = 0; i < matchedFiles.size(); ++i) {
				printf("Found at least one match in file %s.\n", (const char*)matchedFiles[i].ToUTF8());
			}
			time = wxGetLocalTimeMillis() - time;
			printf("time for findInFiles Code Mode:%ld ms\n", time.ToLong());
		}
		else {
			puts("Invalid expression: class Db\n");
		}
	}
	else {
		printf("Could not open Directory: %s\n", (const char*)DirName.ToAscii());
	}
}