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
#include <wx/regex.h>
#include <wx/filename.h>
#include <wx/dir.h>
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
	 * This method will be called right before the first file is iterated through. Walkers will
	 * perform any initialization here.
	 * Note that BeginSearch() will NOT be called if the DirectorySearch was given a non-existent or empty directory.
	 * 
	 * @param fullPath the full path to the directory being iterated that was given in the DirectorySearch::Init() method
	 */
	virtual void BeginSearch(const wxString& fullPath) { };

	/**
	 * The walker should determine whether it thinks the given file is special and wants to mark it as so.
	 * 
	 * @param wxString file the full file path
	 * @return bool true if the dir walker wants the file to be saved as a match
	 */
	virtual bool Walk(const wxString& file) = 0;

	/**
	 * This method will be called after all files have be iterated. Walkers will
	 * perform any cleanup here.
	 * Note that EndSearch() will NOT be called if the DirectorySearch was given a non-existent or empty directory.
	 */
	virtual void EndSearch() { };
};

/**
 * A source class represents a group of files that Triumph will 
 * look at when indexing a project. A user will define the locations 
 * and wildcards. Source directories contain any files that the 
 * user wants to open in Triumph. Additionally, it defines wildcards
 * to ignore certain files in a source directory, for example for
 * ignoring cached files, or log files. Source directories are always
 * recursive; once a directory is labeled as a source all sub-directories
 * automatically become a source.
 *
 * An include/exclude wilcard can multiple wildcard expression; where each
 * expression has only 3 wildcard symbols:
 *
 *  * = Matches Any number of characters
 *  ? = Matches zero or 1 character
 *  ; = OR Separator;  
 *
 * Examaple valid include wildcards:
 * "*.phtml;class.*.php;*.php3?"
 *
 */
class SourceClass {
	
public:

	/**
	 * The location where to start looking files from.
	 */
	wxFileName RootDirectory;

	SourceClass();
	~SourceClass();

	/**
	 * Needed overloads to make SourceClass usable in STL data structures
	 */
	SourceClass(const mvceditor::SourceClass& src);
	SourceClass& operator=(const mvceditor::SourceClass& src);

	/**
	 * @param src object to copy from. after a call, this
	 * object will have the same root directory, include and
	 * exclude wildcards as src.
	 */
	void Copy(const mvceditor::SourceClass& src);

	/**
	 * Remove all include wildcards. Removing all include wildcards
	 * will make the Contains() method always return false, so you would
	 * want to make sure that you append at least one wildcard.
	 */
	void ClearIncludeWildcards();

	/**
	 * Remove all exclude wildcards.
	 */
	void ClearExcludeWildcards();

	/**
	 * @param includeWildcards wildcards in the given string and adds them to the
	 *        include wildcards
	 * Each wildcard is assumed to be separated by a semicolon ';'
	 * Existing wildcards are ovewritten
     */
	void SetIncludeWildcards(const wxString& includeWildcards);

	/**
	 * @param excludeWildcards wildcards in the given string and adds them to the
	 *        exclude wildcards
	 * Each wildcard is assumed to be separated by a semicolon ';'
	 * Existing wildcards are overwritten
     */
	void SetExcludeWildcards(const wxString& excludeWildcards);

	/**
	 * Returns the include wildcards for this source
	 * @return wxString file extensions. This string will be suitable to
	 * serialize the wildcard list.
	 */
	wxString IncludeWildcardsString() const;

	/**
	 * Returns the include wildcards for this source
	 * @return wxString file extensions. This string will be suitable to
	 * serialize the wildcard list.
	 */
	wxString ExcludeWildcardsString() const;

	/**
	 * @return bool TRUE if the given full path is contained in this source.
	 * This means that fullPath is in the RootDirectory (or one of RootDirectory's
	 * sub-directories) and fullPath's file extension matches the
	 * include files AND it does NOT match the exclude files.
	 */
	bool Contains(const wxString& fullPath);

	/**
	 * Check to see if the given full path is in the RootDirectory.  This check
	 * can be done to quickly eliminate a full path without needing to check the
	 * wildcards.  
	 *
	 * @return bool TRUE if the given full path is in the RootDirectory (or one of RootDirectory's
	 * sub-directories)
	 */
	bool IsInRootDirectory(const wxString& fullPath) const;

	/**
	 * @return TRUE if this source directory actually exists in the file system
	 */
	bool Exists() const;

	private:

	/**
	 * Create the regular expression from the a wildcard string. returned regular expression may or may not be valid,
	 * use the IsValid() method of the wxRegEx object to test. 
	 * A wildcard string has only 3 wildcard symbols:
	 *
	 *  * = Matches Any number of characters
	 *  ? = Matches zero or 1 character
	 *  ; = OR Separator; 
	 * 
	 * @return wxString string ready to be compiled into a wxRegEx object.
	 */
	wxString WildcardRegEx(const wxString& expr);

	/**
	 * The regular expression made from IncludeWildcards
	 * This is a pointer so that instances of SourceClass can be pushed
	 * into vectors.
	 * This object will own the pointer
	 */
	wxRegEx* IncludeRegEx;

	/**
	 * The given include wildcards
	 */
	wxString IncludeWildcards;

	/**
	 * The regular expression made from ExcludeWildcards
	 * This is a pointer so that instances of SourceClass can be pushed
	 * into vectors.
	 * This object will own the pointer
	 */
	wxRegEx* ExcludeRegEx;

	/**
	 * The given eclude wildcards
	 */
	wxString ExcludeWildcards;

};

/**
 * @return bool TRUE if has the same sources as b. source lists are the same if and only if
 *  a and b have the same number of items
 *  each source a and b has the same root directory, include, and exclude wildcards
 */
bool CompareSourceLists(const std::vector<mvceditor::SourceClass>& a, const std::vector<mvceditor::SourceClass>& b);

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
	
	/**
	 * Initialize a search.
	 * 
	 * @param const wxString& path the path to recurse
	 * @param one of RECURSIVE or PRECISE.  in PRECISE mode, all files for all sub-directories are enumerated at once, making the 
	 *        total files count available.  In RECURSIVE mode, sub-directories are recursed one at a time.  PRECISE mode
	 *        is useful when the caller needs to know how many total files will be walked over, but it is also more
	 *        memory intensive.  Note that both modes will result in walking of all files.
	 * @return bool doHidden if TRUE then hidden files will be walked as well.
	 * @return bool true of the given path exists
	 */
	bool Init(const wxString& path, Modes mode = RECURSIVE, bool doHidden = false);

	/**
	 * Initialize a search that looks in multiple directories.
	 * 
	 * @param sources the list of directories to recurse
	 * @param one of RECURSIVE or PRECISE.  in PRECISE mode, all files for all sub-directories are enumerated at once, making the 
	 *        total files count available.  In RECURSIVE mode, sub-directories are recursed one at a time.  PRECISE mode
	 *        is useful when the caller needs to know how many total files will be walked over, but it is also more
	 *        memory intensive.  Note that both modes will result in walking of all files.
	 * @return bool doHidden if TRUE then hidden files will be walked as well.
	 * @return bool true if ALL of the given path exists
	 */
	bool Init(const std::vector<mvceditor::SourceClass>& sources, Modes mode = RECURSIVE, bool doHidden = false);
	
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
	 * @param fullPath full path to the file to be checked.
	 * @return bool TRUE if given full path matches the include/exclude wildcards
	 */
	bool MatchesWildcards(const wxString& fullPath);

	/**
	 * Adds all of the given paths' subdirectories into the directory stack.
	 *
	 * @param dir the directory to look in
	 */
	void AddSubDirectories(wxDir& dir);

	/**
	 * Adds all of the given paths' files into the file stack.
	 *
	 * @param dir the directory to look in
	 */
	void AddFiles(wxDir& dir);

	/**
	 * pops the next directory off the stack, reads the files in the dir, and 
	 * populates the files and directories stack.
	 * @param walker to notify that a new source is being searched
	 */
	void EnumerateNextDir(mvceditor::DirectoryWalkerClass& walker);
	
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
	std::stack<wxString> Directories;

	/**
	 * The source directories to iterate through. InitSources are the directories
	 * where the recursion started.
	 */
	std::vector<wxString> InitSources;

	/**
	 * Stores the wildcards so that we ignore files 
	 */
	std::vector<mvceditor::SourceClass> Sources;
	
	/**
	 * the total number of files that will be walked over.  This number will only be available if Init() method was
	 * called with the PRECISE flag.
	 * 
	 * @var int
	 */
	int TotalFileCount;
	
	/**
	 * Flag to search hidden files.
	 * @var bool if true then hidden files are searched.
	 */
	bool DoHiddenFiles;

	/**
	 * Keeps track of whether the walker's Begin() method has been called.
	 */
	bool HasCalledBegin;

	/**
	 * Keeps track of whether the walker's End() method has been called.
	 */
	bool HasCalledEnd;
};

}
#endif // __directorysearchclass__
