/**
 * @copyright  2012 Roberto Perpuly
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
#include <unicode/uclean.h>
#include <wx/wx.h>
#include <vector>
#include "actions/CacheDbVersionActionClass.h"
#include "actions/ProjectTagActionClass.h"
#include "actions/SequenceClass.h"
#include "actions/UrlTagDetectorActionClass.h"
#include <soci/soci.h>  // NOLINT(build/include_order) prevent 'va_list' has not been declared
#include <soci/sqlite3/soci-sqlite3.h>  // NOLINT(build/include_order) prevent 'va_list' has not been declared
#include <soci/mysql/soci-mysql.h>  // NOLINT(build/include_order) prevent 'va_list' has not been declared

/**
 * This is a program that tests a single sequence or action in isolation.  This was
 * done instead of unit tests because wxWidgets event mechanism does not work in
 * console apps (as of wxWidgets 2.8.12).
 *
 * To test an action or sequence, modify the BuildSequence method to add, remove
 * to modify the actions that will be run.  You may also need to initialize the
 * Globals instance so that it has the required data structures.
 */

class MyApp : public wxApp {
 public:
    MyApp();
    bool OnInit();
    int OnExit();

    /**
     * build the sequence to be tested.
     */
    void BuildSequence();

    /**
     * initialize the global data structures.  Note that these are not
     * retrieved from the Triumph .ini file; they are hardcoded into
     * this program.
     */
    void BuildGlobals();

    /**
     * build an instance of a project from a directory
     */
    void CreateProject(wxString label, wxString rootDir);

    void Stop();

    /**
     * used to keep track of running threads
     */
    t4p::RunningThreadsClass RunningThreads;

    /**
     * used to keep track of running threads
     */
    t4p::GlobalsClass Globals;

    /**
     * the sequence being tested
     */
    t4p::SequenceClass Sequence;
};

class MyFrame: public wxFrame {
 public:
    MyFrame(MyApp& app);

    void Log(wxString msg);

 private:
    wxTextCtrl* TextCtrl;
    MyApp& App;
    int Lines;

    void OnActionComplete(wxCommandEvent& event);
    void OnActionInProgress(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnSequenceComplete(wxCommandEvent& event);
    void OnTagFinderListComplete(t4p::TagFinderListCompleteEventClass& event);

    DECLARE_EVENT_TABLE()
};

IMPLEMENT_APP(MyApp)

MyApp::MyApp()
    : wxApp()
    , RunningThreads()
    , Globals()
    , Sequence(Globals, RunningThreads) {
}

bool MyApp::OnInit() {
    MyFrame* frame = new MyFrame(*this);
    SetTopWindow(frame);
    frame->Show(true);

    BuildGlobals();
    BuildSequence();
    frame->Log(_("Starting sequence"));
    return true;
}

void MyApp::Stop() {
    RunningThreads.StopAll();
    Sequence.Stop();
}

void MyApp::BuildSequence() {
    std::vector<t4p::GlobalActionClass*> actions;
    actions.push_back(
        new t4p::TagCacheDbVersionActionClass(RunningThreads, t4p::ID_EVENT_ACTION_TAG_CACHE_VERSION_CHECK));
    actions.push_back(
        new t4p::DetectorCacheDbVersionActionClass(RunningThreads, t4p::ID_EVENT_ACTION_DETECTOR_CACHE_VERSION_CHECK));
    actions.push_back(
        new t4p::ProjectTagActionClass(RunningThreads, t4p::ID_EVENT_ACTION_TAG_FINDER_LIST));
    actions.push_back(
        new t4p::UrlTagDetectorActionClass(RunningThreads, t4p::ID_EVENT_ACTION_URL_TAG_DETECTOR));
    Sequence.Build(actions);
}

void MyApp::BuildGlobals() {
    Globals.Environment.Init();
    Globals.Projects.clear();
    Globals.TagCache.Clear();

    wxFileName tagDbFileName(wxT("C:\\Users\\roberto\\Desktop\\caches\\tags.sqlite"));
    wxFileName detectorTagDbFileName(wxT("C:\\Users\\roberto\\Desktop\\caches\\detectors.sqlite"));

    if (tagDbFileName.FileExists()) {
        wxRemoveFile(tagDbFileName.GetFullPath());
    }
    if (detectorTagDbFileName.FileExists()) {
        wxRemoveFile(detectorTagDbFileName.GetFullPath());
    }

    // needed so that we can know what files need to be parsed
    Globals.FileTypes.PhpFileExtensionsString = wxT("*.php");
    Globals.FileTypes.CssFileExtensionsString = wxT("*.css");
    Globals.FileTypes.SqlFileExtensionsString = wxT("*.php");
    Globals.FileTypes.MiscFileExtensionsString = wxT("*.txt;*.xml;*.yml");

    Globals.Environment.Apache.ManualConfiguration = true;
    Globals.Environment.Apache.SetVirtualHostMapping(wxT("C:\\Users\\roberto\\software\\wamp\\www\\ember"), wxT("http://localhost/"));

    Globals.TagCacheDbFileName = tagDbFileName;
    Globals.DetectorCacheDbFileName = detectorTagDbFileName;

    // create a project
    CreateProject(wxT("ember"), wxT("C:\\Users\\roberto\\software\\wamp\\www\\ember"));
    CreateProject(wxT("symfony"), wxT("C:\\Users\\roberto\\Documents\\php_projects\\symfony"));
}

void MyApp::CreateProject(wxString projectName, wxString rootDir) {
    t4p::ProjectClass project1;
    project1.Label = projectName;
    project1.IsEnabled = true;
    t4p::SourceClass src1;
    src1.RootDirectory.AssignDir(rootDir);
    src1.SetIncludeWildcards(wxT("*.php"));
    project1.AddSource(src1);
    Globals.Projects.push_back(project1);
}

int MyApp::OnExit() {
    // external libs use memory pools; close these
    // down so that we can run this program thtough memory checkers
    // and the memcheckers will not complain.
    u_cleanup();
    mysql_library_end();
    sqlite_api::sqlite3_shutdown();
    return 0;
}

MyFrame::MyFrame(MyApp& app)
    : wxFrame(NULL, wxID_ANY, _("action profiler"), wxDefaultPosition,
              wxSize(640, 480))
    , App(app) {
    TextCtrl = new  wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(640, 480), wxTE_MULTILINE | wxTE_READONLY);
    Lines = 0;
    App.RunningThreads.AddEventHandler(this);
}

void MyFrame::Log(wxString msg) {
    Lines++;
    if (Lines > 1000) {
        Lines = 0;
        TextCtrl->Clear();
    }
    TextCtrl->AppendText(msg);
    TextCtrl->AppendText(wxT("\n"));
}

void MyFrame::OnActionComplete(wxCommandEvent& event) {
}

void MyFrame::OnActionInProgress(wxCommandEvent& event) {
}

void MyFrame::OnClose(wxCloseEvent& event) {
    App.RunningThreads.RemoveEventHandler(this);
    App.Stop();
    event.Skip();
}

void MyFrame::OnTagFinderListComplete(t4p::TagFinderListCompleteEventClass& event) {
    Log(_("global cache completed."));
}

void MyFrame::OnSequenceComplete(wxCommandEvent& event) {
    Log(_("Sequence completed."));

    App.BuildGlobals();
    Log(_("Restarting Sequence."));
    App.BuildSequence();
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_CLOSE(MyFrame::OnClose)
    EVT_COMMAND(wxID_ANY, t4p::EVENT_SEQUENCE_COMPLETE, MyFrame::OnSequenceComplete)
    EVT_TAG_FINDER_LIST_COMPLETE(t4p::ID_EVENT_ACTION_TAG_FINDER_LIST, MyFrame::OnTagFinderListComplete)
END_EVENT_TABLE()
