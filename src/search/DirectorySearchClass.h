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
#ifndef __directorysearchclass__
#define __directorysearchclass__

#include <wx/string.h>
#include <stack>
#include <vector>

namespace mvceditor {
	
/**
 * Classes that need to recurse through directories and search for files need to implement this interface. The walker
 * will be given a file name and the walker will respond with a yay or nay
 */
class DirectoryWalkerClass {

public:

	/**
	 * The walker should determine whether it thinks the given file is special and wants to mark it as so.
	 * 
	 * @param wxString file the full file path
	 * @return bool true if the dir walker wants the file to be saved as a match
	 */
	virtual bool Walk(const wxString& file) = 0;
};

/**
 * This class will iterate through directories, giving each file to a DirectoryWalker. The unique feature of this class is 
 * that only one file will be walked with each invocation of the Walk() method. This makes it possible for  
 * dialogs that need to recurse the file system to be built that will leave the UI responsive (by only walking through one file at
 * a time).
 */
class DirectorySearchClass {

public:
	
	enum Modes {

		/**
		 * In RECURSIVE mode, sub-directories are recursed one at a time.  This method is more memory efficient, at the cost
		 * of not knowing how many files still need to be walked over.
		 */
		RECURSIVE = 0,
		
		/**
		 * In PRECISE mode, all files for all sub-directories are enumerated at once, making the total files count available.
		 * This method of searching is more memory-intensive.
		 */
		PRECISE
	};

	DirectorySearchClass();
	
	~DirectorySearchClass();
	
	/**
	 * Initialize a search.
	 * 
	 * @param const wxString& path the path to recurse
	 * @param one of RECURSIVE or PRECISE.  in PRECISE mode, all files for all sub-directories are enumerated at once, making the 
	 *        total files count available.  In RECURSIVE mode, sub-directories are recursed one at a time.  PRECISE mode
	 *        is useful when the caller needs to know how many total files will be walked over, but it is also more
	 *        memory intensive.  Note that both modes will result in walking of all files.
	 * @return bool true of the given path exists
	 */
	bool Init(const wxString& path, Modes mode = RECURSIVE);
	
	/**
	 * Passes the current file to the given walker and advances to the next file. Note that the files are not guaranteed to
	 * be walked in any particular order. If the walker.Walk() method returns true then the file will saved in the MatchedFiles
	 * list.
	 * 
	 * @param DirectoryWalker& walker the object needing to examine the file.
	 * @return bool true if the walker saved the current file (walker.Walk() method returns true) 
	 */
	bool Walk(DirectoryWalkerClass& walker);
	
	/**
	 * return true if there are more files to be searched.
	 * 
	 * @return bool true if there are more files to be searched.
	 */
	bool More();
	
	/**
	 * Returns the total number of files that will be walked over.  This number will only be available if Init() method was
	 * called with the PRECISE flag.
	 * 
	 * @return int the total number of files in the directory that was given in Init(), including sub-directories. 0 if Init()
	 *         method was called with RECURSIVE flag.
	 */
	int GetTotalFileCount();
	
	/**
	 * Get the matched files
	 * @return std::vector<wxString> the files that the walker returned true for
	 */
	const std::vector<wxString>& GetMatchedFiles();
	
private:

	/**
	 * Populate CurrentFiles with all files in the given path (will recurse through sub-directories as well.)
	 * 
	 * @param wxString path the directory to be enumerated.
	 */
	void EnumerateAllFiles(const wxString& path);
	
	/**
	 * The files that the DirectoryWalker matched on
	 * 
	 * @var std::vector<wxString>
	 */
	std::vector<wxString> MatchedFiles;
	
	/**
	 * The files we still need to search
	 * 
	 * @var std::vector<wxString>
	 */
	std::stack<wxString> CurrentFiles;
	
	/**
	 * The directories we have left to recurse through
	 * 
	 * @var std::vector<wxString>
	 */
	std::stack<wxString*> Directories;
	
	/**
	 * the total number of files that will be walked over.  This number will only be available if Init() method was
	 * called with the PRECISE flag.
	 * 
	 * @var int
	 */
	int TotalFileCount;
	
};

}
#endif // __directorysearchclass__
