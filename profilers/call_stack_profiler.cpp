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
#include <wx/app.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <string>
#include <vector>
#include "actions/CallStackActionClass.h"
#include "actions/TagDetectorActionClass.h"
#include "globals/Assets.h"
#include "language_php/CallStackClass.h"
#include "language_php/TagCacheClass.h"
#include "language_php/TagFinderList.h"
#include <soci/sqlite3/soci-sqlite3.h>  // NOLINT(build/include_order) prevent 'va_list' has not been declared

/**
 * Caches the given directory, putting all resources in the resource cache.
 * A full resource cache is required so that we can know the location
 * of the function definitions.
 */
void CacheLargeProject(t4p::TagCacheClass& tagCache, wxString dirName);

/**
 * Creates the cache db by removing the file if it exists, then creating a new file
 * and running the sql in sqlScriptFullPath
 */
void CreateCacheDb(const wxString& cacheDbFullPath, const wxFileName& sqlScript);

/**
 * load all of the tag files into the given cache. This is needed so that the
 * cache class queries the cache Db files we create in this program.
 */
void TagCacheInit(t4p::TagCacheClass& tagCache);


t4p::TagCacheClass TagCache;
t4p::RunningThreadsClass RunningThreads;
wxString SourceDir;
wxString StartingFile;
wxString PhpExecutableFullPath;
wxString PhpIncludePathFullPath;
wxString PhpTagDectectorFullPath;
wxString TagCacheDbFullPath;
wxString DetectorDbFullPath;

/**
 * This program will generate the call stack output for the test directory.
 * This is a standalone program that can be used to test the generated
 * call stack on its own
 */
int main() {
    wxInitializer init;
    if (!init) {
        printf("Could not initialize wxWidgets library!\n");
        return -1;
    }

    int major, minor;
    wxOperatingSystemId os = wxGetOsVersion(&major, &minor);
    if (os == wxOS_WINDOWS_NT) {
        SourceDir = wxT("C:\\Users\\roberto\\Software\\wamp\\www\\ember");
        StartingFile = SourceDir + wxT("\\application\\controllers\\news.php");
        PhpExecutableFullPath = wxT("php.exe");
        PhpIncludePathFullPath = wxT("C:\\Users\\roberto\\Documents\\triumph4php\\php_detectors");
        PhpTagDectectorFullPath = wxT("C:\\Users\\roberto\\Documents\\triumph4php\\php_detectors\\tag_detectors\\CodeIgniterTagDetector.php");
        TagCacheDbFullPath = wxT("C:\\Users\\roberto\\Desktop\\tags.db");
        DetectorDbFullPath = wxT("C:\\Users\\roberto\\Desktop\\detectors.db");
    } else {
        SourceDir = wxT("/home/roberto/public_html/ember");
        StartingFile = SourceDir + wxT("/application/controllers/news.php");
        PhpExecutableFullPath = wxT("php");
        PhpIncludePathFullPath = wxT("/home/roberto/workspace/triumph4php/php_detectors");
        PhpTagDectectorFullPath = wxT("/home/roberto/workspace/triumph4php/php_detectors/tag_detectors/CodeIgniterTagDetector.php");
        TagCacheDbFullPath = wxT("/home/roberto/workspace/tags.db");
        DetectorDbFullPath = wxT("/home/roberto/workspace/detectors.db");
    }
    CreateCacheDb(TagCacheDbFullPath, t4p::ResourceSqlSchemaAsset());
    CreateCacheDb(DetectorDbFullPath, t4p::DetectorSqlSchemaAsset());

    t4p::CallStackClass callStack(TagCache);
    CacheLargeProject(TagCache, SourceDir);
    TagCacheInit(TagCache);

    wxFileName fileName(StartingFile);
    UnicodeString className = UNICODE_STRING_SIMPLE("News");
    UnicodeString methodName = UNICODE_STRING_SIMPLE("index");
    t4p::CallStackClass::Errors error = t4p::CallStackClass::NONE;
    callStack.Build(fileName, className, methodName, pelet::PHP_53, error);

    UFILE* ufout = u_finit(stdout, NULL, NULL);
    if (t4p::CallStackClass::NONE != error) {
        u_fprintf(ufout, "Call stack error:%d Match Error:%d Error Lexeme:%S Error Class:%S\n",
                  static_cast<int>(error), static_cast<int>(callStack.MatchError.Type),
                  callStack.MatchError.ErrorLexeme.getTerminatedBuffer(),
                  callStack.MatchError.ErrorClass.getTerminatedBuffer());
    }
    u_fclose(ufout);
    printf("The call stack is %ld items long\n", callStack.Variables.size());
    soci::session session(*soci::factory_sqlite3(), t4p::WxToChar(DetectorDbFullPath));
    callStack.Persist(session);
    printf("Call stack written to:%s\n", (const char*)DetectorDbFullPath.ToAscii());
    return 0;
}

void CacheLargeProject(t4p::TagCacheClass& tagCache, wxString sourceDir) {
    std::vector<wxString> phpFileExtensions,
        miscFileExtensions;
    phpFileExtensions.push_back(wxT("*.php"));

    // parse tags of the code igniter project
    t4p::DirectorySearchClass directorySearch;
    bool found = directorySearch.Init(sourceDir);
    if (!found) {
        printf("Directory does not exist: %s\n", (const char*)sourceDir.ToAscii());
    }

    t4p::TagFinderListClass projectCache;
    projectCache.InitGlobalTag(wxFileName(TagCacheDbFullPath), phpFileExtensions, miscFileExtensions, pelet::PHP_53);
    while (directorySearch.More()) {
        projectCache.Walk(directorySearch);
    }
    printf("Caching complete\n");

    // run the tag detector script for code igniter
    t4p::TagDetectorParamsClass params;
    params.PhpExecutablePath = PhpExecutableFullPath;
    params.PhpIncludePath.AssignDir(PhpIncludePathFullPath);
    params.ScriptName.Assign(PhpTagDectectorFullPath);
    params.SourceDir.AssignDir(sourceDir);
    params.OutputDbFileName = DetectorDbFullPath;

    wxArrayString output, error;
    wxExecute(params.BuildCmdLine(), output, error, 0);
    for (size_t i = 0; i < output.GetCount(); i++) {
        printf("%s\n", t4p::WxToChar(output[i]).c_str());
    }
    for (size_t i = 0; i < error.GetCount(); i++) {
        printf("%s\n", t4p::WxToChar(error[i]).c_str());
    }
    printf("Command is %s\n", t4p::WxToChar(params.BuildCmdLine()).c_str());
}

void CreateCacheDb(const wxString& cacheDbFullPath, const wxFileName& sqlScript) {
    // if file exists delete it; it may have an old schema
    if (wxFileName::FileExists(cacheDbFullPath)) {
        wxRemoveFile(cacheDbFullPath);
    }
    wxSleep(1);

    std::string stdFilename = t4p::WxToChar(cacheDbFullPath);
    soci::session session(*soci::factory_sqlite3(), stdFilename);
    wxString error;
    bool good = t4p::SqliteSqlScript(sqlScript, session, error);
    if (!good) {
        puts(error.ToAscii());
    }
    session.close();
}

void TagCacheInit(t4p::TagCacheClass& tagCache) {
    std::vector<wxString> phpFileExtensions,
        miscFileExtensions;
    phpFileExtensions.push_back(wxT("*.php"));
    t4p::TagFinderListClass* tagFinderlist = new t4p::TagFinderListClass;

    // load the project tags that were just parsed
    tagFinderlist->InitGlobalTag(wxFileName(TagCacheDbFullPath), phpFileExtensions, miscFileExtensions, pelet::PHP_53);

    // load the php native functions into the cache
    tagFinderlist->InitNativeTag(t4p::NativeFunctionsAsset());

    // load the detected tags cache
    tagFinderlist->InitDetectorTag(wxFileName(DetectorDbFullPath));

    tagCache.RegisterGlobal(tagFinderlist);
}
