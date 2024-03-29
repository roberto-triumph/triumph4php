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
#ifndef SRC_FEATURES_ENVIRONMENTFEATURECLASS_H_
#define SRC_FEATURES_ENVIRONMENTFEATURECLASS_H_

#include "features/BackgroundFileReaderClass.h"
#include "features/FeatureClass.h"
#include "globals/EnvironmentClass.h"

namespace t4p {
/**
 * Event that will get generated by the ApacheFileReaderClass once an
 * apache config file has been found.
 */
extern const wxEventType EVENT_APACHE_FILE_READ_COMPLETE;

/**
 * Event that will get generated by the ApacheFileReaderClass once an
 * apache config file has been found.
 * This event contains the location of the Apache config file.
 */
class ApacheFileReadCompleteEventClass : public wxEvent {
 public:
    t4p::ApacheClass Apache;

    ApacheFileReadCompleteEventClass(int eventId, const t4p::ApacheClass& apache);

    wxEvent* Clone() const;
};

typedef void (wxEvtHandler::*ApacheFileReadCompleteEventClassFunction)(ApacheFileReadCompleteEventClass&);

#define EVT_APACHE_FILE_READ_COMPLETE(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_APACHE_FILE_READ_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent(ApacheFileReadCompleteEventClassFunction, &fn), (wxObject *) NULL),


/**
 * A small class that will search for the Apache config files
 * in a background thread. The results of the config file parsing
 * will be wxPost'ed
 */
class ApacheFileReaderClass : public BackgroundFileReaderClass {
 public:
    /**
     * @param handler the event handler for the EVENT_APACHE_FILE_READ_COMPLETE and EVENT_WORK_* events
     */
    ApacheFileReaderClass(t4p::RunningThreadsClass& runningThreads, int eventId);

    /**
     * setup the background thread (does not start the thread)
     * @param startDirectory the directory to start looking in
     */
    bool Init(const wxString& startDirectory);

    wxString GetLabel() const;

 protected:
    bool BackgroundFileMatch(const wxString& file);

    bool BackgroundFileRead(DirectorySearchClass& search);

 private:
    ApacheClass ApacheResults;
};


/**
 * This feature will handle the application stack configuration options.
 */
class EnvironmentFeatureClass : public FeatureClass {
 public:
    EnvironmentFeatureClass(t4p::AppClass& app);

    /**
     * Handle the menu item
     */
    void OnMenuEnvironment(wxCommandEvent& event);

 private:
    void OnPreferencesSaved(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};
}  // namespace t4p

#endif  // SRC_FEATURES_ENVIRONMENTFEATURECLASS_H_
