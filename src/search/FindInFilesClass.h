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
#ifndef __findinfilesclass__
#define __findinfilesclass__

#include <search/DirectorySearchClass.h>
#include <search/FinderClass.h>
#include <wx/ffile.h>
#include <wx/regex.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <unicode/unistr.h>
#include <unicode/ustdio.h>

namespace mvceditor {

/**
 * This class can be used to search for a subset of text in multiple files of a given directory.  The find logic
 * is identical to that of FinderClass in regards to the different modes (EXACT vs. REGULAR_EXPRESSION). Searching
 * files will be line-based, each line of each file will be compared against the search expression.
 * 
 * File filters string is a GLOB-like string, using asterisk ('*') as wildcard. There may be multiple filters, each 
 * filter should be separated by a semicolon (';').
 * 
 * Examples:
 * 
 * FindInFilesClass can be used to quickly find the number of matches of an expression in a directory of files.
 * 
 * <code>
 *   FindInFilesClass finder(UNICODE_STRING_SIMPLE("UserClass"), FinderClass::EXACT);
 *   DirectorySearchClass search();
 *   if (search.Init(wxT("/path/to/phpfiles/"))) {
 *   	if (finder.Prepare()) {
 *     	while (search.More()) {
 *       	bool matchFound = search.Walk(finder);
 *      		printf("Match Found: %d in file %s.\n", matchFound, finder.GetCurrentFileName());
 *     	}
 *       else puts("Find expression is not valid");
 *   }
 *   else puts("Path not valid!");
 * </code>
 *
 * FindInFilesClass can be used to only search a subset of files.  For example, the code below only searches files
 * with the php or inc extensions.
 * <code>
 * 
 *   FindInFilesClass finder(UNICODE_STRING_SIMPLE("UserClass"), FinderClass::EXACT);
 *   finder.FilesFilter = wxT("*.php;*.inc");
 *   DirectorySearchClass search();
 *   if (search.Init(wxT("/path/to/phpfiles/"))) {
 *   	if (finder.Prepare()) {
 *     	while (search.More()) {
 *       	bool matchFound = search.Walk(finder);
 *      		printf("Match Found: %d in file %s.\n", matchFound, finder.GetCurrentFileName());
 *     	}
 *       else puts("Find expression is not valid");
 *   }
 *   else puts("Path not valid!");
 * 
 * The DirectorySearchClass will save the files that contained at least one match, that way the caller may iterate through 
 * the matched files at some point in the future.
 * 
 *  <code>
 *   FindInFilesClass finder(UNICODE_STRING_SIMPLE("UserClass"), FinderClass::EXACT);
 *   finder.FilesFilter = wxT("*.php;*.inc");
 *   DirectorySearchClass search();
 *   if (search.Init(wxT("/path/to/phpfiles/"))) {
 *   	if (finder.Prepare()) {
 *     	while (search.More()) {
 *       	bool matchFound = search.Walk(finder);
 *      		printf("Match Found: %d in file %s.\n", matchFound, finder.GetCurrentFileName());
 *     	}
 *       else puts("Find expression is not valid");
 *   }
 *   else puts("Path not valid!");
 *   //  some code ...
 *   std::vector<wxString> matchedFiles = search.GetMatchedFiles();
 *   for (int i = 0; i < matchedFiles.size(); ++i) {
 * 		printf("Found at least one match in file %s.\n", (const char*)matchedFiles[i].ToUTF8());
 *   }
 * </code> 
 * 
 * @see FinderClass.h
 */
class FindInFilesClass : public DirectoryWalkerClass {

public:

	/**
	 * Reasons why a file could not be opened.
	 */
	enum OpenErrors {
		NONE,
		FILE_NOT_FOUND,
		CHARSET_DETECTION
	};

	FindInFilesClass(const UnicodeString& expression = UNICODE_STRING_SIMPLE(""), FinderClass::Modes mode = FinderClass::EXACT);
	
	/**
	 * Will copy source into this new instance. However, this variable is not prepared, and
	 * the caller MUST call Prepare on this new  variable.  
	 */
	FindInFilesClass(const FindInFilesClass& source);

	~FindInFilesClass();
	
	/**
	 * The string to look for. If Expression is modified, Prepare() method MUST be called in order to
	 * set the internal state machine.
	 * @var wxString
	 */
	UnicodeString Expression;
	
	/**
	 * The string to replace
	 * @var wxString
	 */
	UnicodeString ReplaceExpression;

	/**
	 * The folder and wildcards to search.
	 */
	SourceClass Source;
	
	/**
	 * The find mode. If Mode is modified, Prepare() method MUST be called in order to
	 * set the internal state machine.
	 * @var one of FinderClass::MODES
	 */
	int Mode;
	
	/**
	 * Prepare  Expression, ReplaceExpression. 
	 * @return bool if Expression, ReplaceExpression are valid
	 */
	bool Prepare();
	
	/**
	 * Searches the given file for matches of this expression. If the given file has at least one match, then 
	 * true is returned.  Otherwise false is returned.  You MUST call Prepare() method you call this method,,
	 * if Prepare method has not been called this method will return false.
	 * 
	 * @return bool true if the given file has at least one match of this expression. If a match is found, then 
	 * 	      GetCurrentLine() will contain the line number of the hit.  Then FindNext() can be used to 
	 * 		 iterate through the rest of the hits of the file.
	 */
	virtual bool Walk(const wxString& fileName);
	
	/**
	 * Finds the next match in the given text. The matched line number is then made available with the GetCurrentLineNumber() method.
	 * 
	 * @return bool true if match was found.
	 */
	bool FindNext();

	/**
	 * Returns the replacement string, with any back references already replaced with
	 * the correct text.
	 * 
	 * @param const UnicodeString& text the original text 
	 * @param UnicodeString& replacementText text that matched this expression
	 * @return bool true if this expression was found in the last call to FindPrevious()
	 * or FindNext() methods
	 */
	bool GetLastReplacementText(const UnicodeString& text, UnicodeString& replacementText) const;
	
	/**
	 * Replace all occurrences of this Expression in text with ReplaceExpression.
	 * 
	 * @var UnicodeString& text the text to modify
	 * @return int the number of replacements made, 0 if ReplaceExpression is
	 * an empty string
	 */
	int ReplaceAllMatches(UnicodeString& text) const;
	
	/**
	 * Replaces this Expression with this ReplaceExpression in the current matched file.
	 * 
	 * @param const wxString& fileName the full path of the file to find & replace
	 * @return int the number of replacement made.
	 */
	int ReplaceAllMatchesInFile(const wxString& fileName) const;
	
	/**
	 * Assignment operator will copy an instance to the new variable. However, the assigned variable is not prepared, and
	 * the called MUST call Prepare on the assigned variable.  
	 */
	FindInFilesClass& operator=(const FindInFilesClass& source);

	/**
	 * Will copy source into this instance. However, this variable is not prepared, and
	 * the caller MUST call Prepare on this new variable.  
	 */
	void Copy(const FindInFilesClass& source);
	
	/**
	 * Copy this expression into the given finder.
	 * 
	 * @var FinderClass finder. After a call to this method, dest.Expression will be equivalent to this expression.
	 */
	void CopyFinder(FinderClass& dest);
	
	/**
	 * Returns the current line. If FindNext() returned true, then this line is the line that the match
	 * was found in.
	 * 
	 * @return UnicodeString
	 */
	UnicodeString GetCurrentLine();
	
	/**
	 * Returns the current line number. If FindNext() returned true, then this line number is the line number that the match
	 * was found in. Liner number starts at 1.
	 * 
	 * @return int
	 */
	int GetCurrentLineNumber();
	
	/**
	 * Returns the start of the current hit. If FindNext() returned true, then this is the position 
	 * where the match was found in. Position number starts at 0
	 * and it's relative to the start of the file.
	 * @return int
	 */
	int GetFileOffset();
	
	/**
	 * Returns the offset where the hit is found in the matching line. 0-based
	 * and it's relative to the beginning of the line
	 * @return int 0 <= GetLineOffset() <= GetCurrentLine.length()
	 */
	int GetLineOffset();
	
	/**
	 * Get the file contents from the file and write them to the string
	 * @param fileName full path of the file to open
	 * @param content the file contents will be written to this variable
	 * @return OpenErrors reason for content not being set
	 */
	static OpenErrors FileContents(const wxString& fileName, UnicodeString& content);

private:

	/**
	 * To find text
	 * @var FinderClass
	 */
	FinderClass Finder;

	/**
	 * use wxWidgets file class as it allows us to properly open
	 * unicode filenames
	 */
	wxFFile FFile;
	
	/**
	 * The input stream; used to open the file
	 * 
	 * @var UFILE*
	 */
	 UFILE* File;
	
	/**
	 * The current line 
	 * 
	 * @var UnicodeString
	 */
	UnicodeString CurrentLine;
	
	/**
	 * The current line number
	 * 
	 * @var int
	 */
	int LineNumber;
	
	/**
	 * The offset where the hit is found in the matching line.. 0-based
	 * 
	 * @var int
	 */
	int LineOffset;
	
	/**
	 * The start of the hit (relative to the start of the file) found by FindNext
	 * 
	 * @var int
	 */
	int FileOffset;
	
	/**
	 * The current position of the start of the current line
	 * 
	 * @var int
	 */
	int LineStartOffset;
	
	/**
	 * Close the associated input streams.
	 */
	void CleanupStreams();
};

}
#endif // __findinfilesclass__
