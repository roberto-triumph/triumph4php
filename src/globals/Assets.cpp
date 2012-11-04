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
#include <globals/Assets.h>
#include <wx/stdpaths.h>
#include <wx/image.h>

wxFileName mvceditor::NativeFunctionsAsset() {
	wxStandardPaths paths;
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString nativeFileName = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
	                          wxT("..") + wxFileName::GetPathSeparator() +
	                          wxT("resources") + wxFileName::GetPathSeparator() +
	                          wxT("php.db");
	wxFileName fileName(nativeFileName);
	fileName.Normalize();
	return fileName;
}

wxFileName mvceditor::ResourceSqlSchemaAsset() {
	wxStandardPaths paths;
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString nativeFileName = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
	                          wxT("..") + wxFileName::GetPathSeparator() +
	                          wxT("resources") + wxFileName::GetPathSeparator() +
	                          wxT("sql") + wxFileName::GetPathSeparator() +
							  wxT("resources.sql");
	wxFileName fileName(nativeFileName);
	fileName.Normalize();
	return fileName;
}

wxFileName mvceditor::AutoCompleteImageAsset(wxString imageName) {
	if (!wxImage::FindHandler(wxBITMAP_TYPE_XPM)) {
		wxImage::AddHandler(new wxXPMHandler);	
	}
	wxStandardPaths paths;
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString nativeFileName = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
	                          wxT("..") + wxFileName::GetPathSeparator() +
	                          wxT("resources") + wxFileName::GetPathSeparator() +
							  imageName +
	                          wxT(".xpm");
	wxFileName fileName(nativeFileName);
	fileName.Normalize();
	wxASSERT(fileName.IsOk());
	return fileName;
}

wxFileName mvceditor::PhpDetectorsAsset() {
	wxStandardPaths paths;
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString scriptFullPath = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
		wxT("..") + wxFileName::GetPathSeparator() +
		wxT("php_detectors") + wxFileName::GetPathSeparator() +
		wxT("src") + wxFileName::GetPathSeparator() +
		wxT("MvcEditorFrameworkApp.php");
	wxFileName scriptFileName(scriptFullPath);
	scriptFileName.Normalize();
	return scriptFileName;
}

wxFileName mvceditor::TempDirAsset() {
	wxStandardPaths paths;
	wxFileName tempDir;
	tempDir.AssignDir(paths.GetTempDir());
	tempDir.AppendDir(wxT("mvc-editor"));
	if (!tempDir.DirExists()) {
		wxMkdir(tempDir.GetPath(), 0777);
	}
	return tempDir;
}

wxFileName mvceditor::ConfigDirAsset() {
	wxStandardPaths paths;
	wxFileName tempDir;
	tempDir.AssignDir(paths.GetUserConfigDir());
	tempDir.AppendDir(wxT(".mvc-editor"));
	if (!tempDir.DirExists()) {
		wxMkdir(tempDir.GetPath(), 0777);
	}
	return tempDir;
}