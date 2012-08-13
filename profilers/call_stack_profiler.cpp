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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */

#include <language/ResourceCacheClass.h>
#include <php_frameworks/CallStackClass.h>
#include <php_frameworks/FrameworkDetectorClass.h>
#include <MvcEditorAssets.h>
#include <wx/app.h>
#include <wx/stdpaths.h>

/**
 * Caches the given directory, putting all resources in the resource cache.
 * A full resource cache is required so that we can know the location
 * of the function definitions.
 */
void CacheLargeProject(mvceditor::ResourceCacheClass& resourceCache, wxString dirName) ;

class HandlerClass : public wxEvtHandler {

public:
		HandlerClass()
			: wxEvtHandler() {
				
		}


};

HandlerClass Handler;
mvceditor::ResourceCacheClass ResourceCache;
mvceditor::RunningThreadsClass RunningThreads;
mvceditor::CallStackClass CallStack(ResourceCache);
mvceditor::ResourcesDetectorActionClass ResourceDetector(Handler, RunningThreads);
wxString DirName;
wxString StartingFile;

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
		DirName = wxT("C:\\Users\\roberto\\Software\\wamp\\www\\ember");
		StartingFile = wxT("C:\\Users\\roberto\\Software\\wamp\\www\\ember\\application\\controllers\\news.php");
	}
	else {
		DirName = wxT("/home/roberto/public_html/ember");
		StartingFile = wxT("/home/roberto/public_html/ember/application/controllers/news.php");
	}
	CacheLargeProject(ResourceCache, DirName);
	
	wxFileName fileName(StartingFile);
	UnicodeString className = UNICODE_STRING_SIMPLE("News");
	UnicodeString methodName = UNICODE_STRING_SIMPLE("index");
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	CallStack.Build(fileName, className, methodName, pelet::PHP_53, error);
	
	UFILE* ufout = u_finit(stdout, NULL, NULL);
	if (mvceditor::CallStackClass::NONE != error) {
		u_fprintf(ufout, "Call stack error:%d Match Error:%d Error Lexeme:%S\n", 
			error, CallStack.MatchError.Type, CallStack.MatchError.ErrorLexeme.getTerminatedBuffer());
	}
	u_fclose(ufout);
	printf("The call stack is %ld items long\n", CallStack.List.size());
	wxFileName outputFile;
	
	// the temporary file where the output will go
	// make it a unique name
	
	wxStandardPaths paths;
	wxString tmpDir = paths.GetTempDir();
	outputFile.AssignDir(tmpDir);
	wxLongLong time = wxGetLocalTimeMillis();
	wxString tmpName = wxT("call_stack") + wxString::Format(wxT("_%s.ini"), time.ToString().c_str());
	outputFile.SetFullName(tmpName);
	outputFile.Normalize();	
	
	CallStack.Persist(outputFile);
	
	printf("Call stack written to:%s\n", (const char*)outputFile.GetFullPath().ToAscii());

	return 0;
}
	
void CacheLargeProject(mvceditor::ResourceCacheClass& resourceCache, wxString dirName) {
	std::vector<wxString> fileFilters;
	fileFilters.push_back(wxT("*.php"));

	mvceditor::GlobalCacheClass* globalCache = new mvceditor::GlobalCacheClass;
	globalCache->Init(mvceditor::NativeFunctionsAsset(), fileFilters, pelet::PHP_53);
	resourceCache.RegisterGlobal(globalCache);
	mvceditor::DirectorySearchClass directorySearch;
	bool found = directorySearch.Init(dirName);
	if (!found) {
		printf("Directory does not exist: %s\n", (const char*)dirName.ToAscii());
	}
	bool walked = true;
	wxFileName fileName(wxFileName::GetTempDir() + wxFileName::GetPathSeparators() + wxT("call_stack_profiler_resource_cache.db"));
	if (fileName.FileExists()) {
		wxRemoveFile(fileName.GetFullPath());
	}
	mvceditor::GlobalCacheClass* projectCache = new mvceditor::GlobalCacheClass;
	projectCache->Init(fileName, fileFilters, pelet::PHP_53);
	while (directorySearch.More()) {
		projectCache->Walk(directorySearch);
	}

	if (!resourceCache.RegisterGlobal(projectCache)) {
		printf("Could not initialize the project cache.\n");
	}
	if (!walked) {
		printf("Resource Cache could not be initialized!\n");
	}
	else {
		printf("Caching complete\n");
	}
	
	// create these resources so that call stack can recurse into CodeIgniter libs
	wxStandardPaths paths;
	wxFileName outputFile;
	wxString action = wxT("resources");
	wxFileName scriptFileName = mvceditor::PhpDetectorsAsset();
	
	// the temporary file where the output will go
	// make it a unique name
	wxString tmpDir = paths.GetTempDir();
	outputFile.AssignDir(tmpDir);
	wxLongLong time = wxGetLocalTimeMillis();
	wxString tmpName = action + wxString::Format(wxT("_%s.ini"), time.ToString().c_str());
	outputFile.SetFullName(tmpName);
	outputFile.Normalize();	
	
	wxString cmd = wxString::FromAscii("php  ");
	cmd += scriptFileName.GetFullPath();
	cmd += wxString::FromAscii(" "
		"--identifier=code-igniter "
		"--action=\"resources\" ");
	cmd += wxT("--dir=\"") + dirName + wxT("\" ");
	cmd += wxT("--output=\"") + outputFile.GetFullPath() + wxT("\" ");
		
	wxExecute(cmd, wxEXEC_SYNC);
	ResourceDetector.InitFromFile(outputFile.GetFullPath());
	ResourceCache.GlobalAddDynamicResources(ResourceDetector.Resources);
	printf("dynamic resources=%ld\n", ResourceDetector.Resources.size());
	wxRemoveFile(outputFile.GetFullPath());
}