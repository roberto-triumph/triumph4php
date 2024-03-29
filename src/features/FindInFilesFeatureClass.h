/**
 * @copyright  2009-2011 Roberto Perpuly
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
#ifndef SRC_FEATURES_FINDINFILESFEATURECLASS_H_
#define SRC_FEATURES_FINDINFILESFEATURECLASS_H_

/**
@file
Subclass of FindInFilesPanelGeneratedClass, which is generated by wxFormBuilder.
*/

#include <wx/object.h>
#include <wx/thread.h>
#include <vector>
#include "features/BackgroundFileReaderClass.h"
#include "features/FeatureClass.h"
#include "globals/ProjectClass.h"
#include "search/DirectorySearchClass.h"
#include "search/FindInFilesClass.h"

namespace t4p {
/**
 * A single hit that resulted from a find in files action.
 */
class FindInFilesHitClass : public wxObject {
    DECLARE_DYNAMIC_CLASS(t4p::FindInFilesHitClass)


 public:
    /**
     * the full path of the file searched
     */
    wxString FileName;

    /**
     * preview is the entire line where the hit occurred.
     */
    wxString Preview;

    /**
     * line where the hit was found (1- based)
     */
    int LineNumber;

    /**
     * character position where the hit was found (0- based),
     * relative to the start of the line.
     *
     * 0 <= LineOffset < Preview.Length()
     */
    int LineOffset;

    /**
     * character position where the hit was found (0- based)
     * relative to the beginning of the file.
     */
    int FileOffset;

    /**
     * Character length of the matching hit
     */
    int MatchLength;

    FindInFilesHitClass();

    /**
     * This will fully clone hit. Deep copy makes assignment thread-safe because by default
     * wxStrings are shallow-cloned.
     * @param hit to deep copy
     */
    FindInFilesHitClass(const t4p::FindInFilesHitClass& hit);

    /**
     * All parameters will be deep copied. Deep copy makes assignment thread-safe because by default
     * wxStrings are shallow-cloned.

     * @param fileName the full path to the file
     * @param preview the contents of the line where the hit ocurred
     * @param lineNumber line (1 based) where the hit
     * @param lineOffset character position (0 based) where the hit ocurred. relative to the line
     * @param fileOffset character position (0 based) where the hit ocurred, relative to the beginning
     *        of the file
     * @param matchLength length of the matching text
     */
    FindInFilesHitClass(const wxString& fileName, const wxString& preview, int lineNumber, int lineOffset,
                        int fileOffset, int matchLength);

    /**
     * This will fully clone hit. Deep copy makes assignment thread-safe because by default
     * wxStrings are shallow-cloned.
     * @param hit to deep copy
     */
    FindInFilesHitClass& operator=(const t4p::FindInFilesHitClass& hit);

    /**
     * Equality operator, needed to implment wxVariant stuff
     * @param bool return if hits are the same
     */
    bool operator==(const t4p::FindInFilesHitClass& hit);

    /**
     * This will fully clone hit. Deep copy makes assignment thread-safe because by default
     * wxStrings are shallow-cloned.
     * @param hit to deep copy
     */
    void Copy(const t4p::FindInFilesHitClass& src);
};

DECLARE_VARIANT_OBJECT(t4p::FindInFilesHitClass)

/**
 * One EVENT_FIND_IN_FILES_FILE_HIT event will be generated once an entire file has been searched.
 */
class FindInFilesHitEventClass : public wxEvent {
 public:
    /**
     * @param id of the event, used to differentiate between multiple searches if they are happpening
     *        simultaneously
     * @param hits the hits found.  this vector will be deep copied.
     */
    FindInFilesHitEventClass(int eventId, const std::vector<t4p::FindInFilesHitClass>& hits);

    wxEvent* Clone() const;

    /**
     * @return vector of All of the hits for a single file.
     */
    std::vector<t4p::FindInFilesHitClass> GetHits() const;

 private:
    /**
     * All of the hits for a single file. Hiding this vector because we want to make sure
     * it is deep copied every time since we this event will be handled
     * by multiple threads.
     */
    std::vector<t4p::FindInFilesHitClass> Hits;
};


extern const wxEventType EVENT_FIND_IN_FILES_FILE_HIT;

typedef void (wxEvtHandler::*FindInFilesHitEventClassFunction)(FindInFilesHitEventClass&);

#define EVT_FIND_IN_FILES_HITS(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_FIND_IN_FILES_FILE_HIT, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent(FindInFilesHitEventClassFunction, & fn), (wxObject *) NULL),


/**
 * This class is the background thread where all finding and replacing will be done.
 */
class FindInFilesBackgroundReaderClass: public BackgroundFileReaderClass {
 public:
    /**
     * @param wxEvtHandler This object will receive the EVENT_FIND_IN_FILES_FILE_HIT events.
     *        and the EVENT_WORK_* events
     */
    FindInFilesBackgroundReaderClass(t4p::RunningThreadsClass& runningThreads, int eventId);

    /**
     * Prepare to iterate through all files in the given directory.
     *
     * @param FindInFilesClass findInFiles the expression to search for
     * @param doHiddenFiles if TRUE then hidden files are searched
     * @param skipFiles full paths of files to not search. We want to NOT perform searches
     *        in files that are already opened; those would result in incorrect hits.
     * @return True if directory is valid and the find expression is valid.
     */
    bool InitForFind(FindInFilesClass findInFiles, bool doHiddenFiles, std::vector<wxString> skipFiles);

    /**
     * When replacing, the thread will replace the files in the given hits. In case files are opened, we don't want to
     * replace them in the background since the user may have modified them but not saved them yet.
     *
     * @param FindInFilesClass findInFiles the expression to search and replace with
     * @param allHits the files to perform replacements in.
     * @param skipFiles full paths of files to not replace. We want to NOT perform replacements
     * in files that are already opened.
     * @return true if hits is not empty
     */
    bool InitForReplace(FindInFilesClass findInFiles, const std::vector<wxString>& replaceFiles, std::vector<wxString> skipFiles);

    /**
     * Creates a Hit event for the current FindInFiles match. (the event will NOT be posted).
     * @param lineNumber the line that where the hit occurred
     * @param the line text itself
     * @param fileName the name of the file that was searched.
     */
    static wxCommandEvent MakeHitEvent(int lineNumber, const wxString& lineText, const wxString& fileName);

    wxString GetLabel() const;

 protected:
    /**
     * Finds the expression in all files.
     */
    bool BackgroundFileRead(DirectorySearchClass& search);

    /**
     * Replaces this expression in all files.
     */
    bool BackgroundFileMatch(const wxString& file);

 private:
    /**
     * To find matches in files.
     *
     * @var FindInFilesClass
     */
    FindInFilesClass FindInFiles;

    /**
     * Matched files that will NOT be replaced / searched
     * The feature will make the background thread skip the files that are currently opened; this way the result do not
     * show stale (and possibly wrong) hits
     */
    std::vector<wxString> SkipFiles;
};

class FindInFilesFeatureClass : public FeatureClass {
 public:
    /**
     * We will NOT use this object to actually search, we will just use this to keep track of the last
     * inputs by the user so we can show them
     *
     */
    FindInFilesClass PreviousFindInFiles;

    /**
     * If TRUE, hidden files are searched
     */
    bool DoHiddenFiles;

    /**
     * Constructor
     */
    FindInFilesFeatureClass(t4p::AppClass& app);

 private:
};
}  // namespace t4p

#endif  // SRC_FEATURES_FINDINFILESFEATURECLASS_H_
