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
#ifndef __RESOURCECACHEBUILDERTHREADCLASS_H__
#define __RESOURCECACHEBUILDERTHREADCLASS_H__

#include <actions/ActionClass.h>
#include <language_php/TagCacheClass.h>

namespace t4p {

// forward declaration
class GlobalsClass;

/**
 * The working cache builder class will take care of parsing memory buffers to code; the
 * source code buffers that the user is actively editing.  The working cache bulder produces
 * 2 things:
 * - tags: the builder will parse the code and parse out all tags. this means that it may find
 *         new tags, ie new functions / classes that the user has added to the buffer but before
 *         the buffer is saved. The new tags are written to the global tag db.
 * - a symbol table, it is used to determine variable type information. the symbol table is 
 *   only found in memory, it will be passed in the generated event EVENT_WORKING_CACHE_COMPLETE
 *
 */
class WorkingCacheBuilderClass : public t4p::ActionClass {
	
public:

	/**
	 * @param runningThreads the object will get notified to EVENT_WORK* events with the given ID
	 *        and the EVENT_WORKING_CACHE_COMPLETE.
	 */
	WorkingCacheBuilderClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * Will parse the resources of the given text in a backgound thread and will
	 * post an EVENT_WORKING_CACHE_COMPLETE when the parsing is complete.
	 * 
	 * @param globals to get the projects' directories to be scanned (recursively scan all sources in all projects)
	 * @param fileName full path to the file. this can be empty string is contents are new.
	 * @param fileIdentifier unique identifier for a file
	 * @param code the file's most up-to-date source code (from the user-edited buffer)
	 * @param bool if TRUE then tileName is a new file that is not yet written to disk
	 * @param version The version of PHP to check against
	 * @param bool if TRUE then the code will parsed for PHP tags.  This should be true most of time,
	 *       but it should be false if code is the same as the contents of the file (like for example when
	 *       the file is opened).
	 */
	void Update(t4p::GlobalsClass& globals, const wxString& fileName, const wxString& fileIdentifier, 
		const UnicodeString& code, bool isNew, pelet::Versions version, bool doParseTags);

	wxString GetLabel() const;

protected:
	
	/**
	 * In the background thread we will parse the code for resources
	 */
	void BackgroundWork();
	
private:
	
	/**
	 * the location of the tag cache; the sqlite file where the tags are stored
	 */
	wxFileName TagCacheDbFileName;

	/**
	 * the code that is being worked on by the background thread.
	 */
	UnicodeString Code;
	
	/**
	 * full path to the file that is being worked on by the background thread.
	 * this can be empty string if file is a new file.
	 */
	wxString FileName;

	/**
	 * the source directory of the file; the project that the file is in
	 */
	wxString SourceDir;

	/**
	 * string that uniquely identifies the file that is being worked on by the background thread.
	 */
	wxString FileIdentifier;
	
	/**
	 * We keep a copy of the previously-generated symbol table; so that
	 * if the current version of the file syntax errors we can use
	 * the previous symbol table's variables as a starting point. Ultimately,
	 * we want code completion to be possible even when the current file 
	 * has syntax errors.
	 */
	t4p::SymbolTableClass PreviousSymbolTable;

	/**
	 * The version of PHP to check against
	 */
	pelet::Versions Version;

	/**
	 * if TRUE then tileName is a new file that does not yet exist on disk
	 */
	bool FileIsNew;

	/**
	 * if TRUE then tags will be parsed out of the code
	 */
	bool DoParseTags;

};

}
#endif