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
#include <FileTestFixtureClass.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/utils.h>
#include <fstream>


FileTestFixtureClass::FileTestFixtureClass(const wxString& tempTestDirectory) {
	wxFileName tempDir;
	tempDir.AssignDir(wxFileName::GetTempDir());

	// make sure to add the system user name to the directory that way
	// multiple users will not trample on each other's test files
	// this is useful for building under buildbot
	tempDir.AppendDir(wxT("triumph4php-tests-") + wxGetUserId());

	// put everything under a triumph4php-tests test directory that way we can track
	// what files get added
	if (!tempDir.DirExists()) {
		wxMkdir(tempDir.GetPath(), 0777);
	}

	tempDir.AppendDir(tempTestDirectory);
	TestProjectDir = tempDir.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
	if (wxFileName::DirExists(TestProjectDir)) {
		RecursiveRmDir(TestProjectDir);
	}
}

FileTestFixtureClass::~FileTestFixtureClass() {
}

void FileTestFixtureClass::RecursiveRmDir(wxString path) {
	wxDir dir(path);
	wxString filename;
	bool next = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES | wxDIR_DIRS | wxDIR_HIDDEN);

	// this way so that gcc does not think that good is an unused variable
	bool good;
	wxUnusedVar(good);
	while (next) {
		// wxRmDir does not handle symlinks
		if (wxT("sfPropelPlugin") == filename) {
			wxExecute(wxT("rm ") + path + wxT("sfPropelPlugin"));
		}
		else if (wxT("sfProtoculousPlugin") == filename) {
			wxExecute(wxT("rm ") + path + wxT("sfProtoculousPlugin"));
		}
		else if (wxDirExists(path + filename)) {
			RecursiveRmDir(path + filename + wxFileName::GetPathSeparator());
			wxString fullPath = path + filename;
			good = wxRmdir(fullPath);
			wxASSERT_MSG(good, wxT("could not remove directory:") + fullPath);
		}
		else {
			good = wxRemoveFile(path + filename);
			wxASSERT_MSG(good, wxT("could not remove file:") + (path + filename));
		}
		next = dir.GetNext(&filename);
	}
}

void FileTestFixtureClass::CreateFixtureFile(const wxString& fileName, const wxString& contents) {
	TouchTestDir();
	wxString fileToWrite = TestProjectDir + fileName;
	std::ofstream file;
	file.open(fileToWrite.fn_str(), std::ios::out | std::ios::binary | std::ios::trunc);
	wxASSERT_MSG(file.good(), _("Could not open file for writing: ") + fileToWrite);

	// horrible code: must find a way to write many bytes at once
	for (unsigned int i = 0; i < contents.Length(); ++i) {
		file.put(static_cast<int>(contents[i]));
	}
	file.close();
}

wxString FileTestFixtureClass::HideFile(const wxString& fileName) {
	// since tests are not wxApps we cannot use wxExecute
	// must do it the hard way

	// start with the windows way
	wxPlatformInfo info;
	if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT) {
		wxString wxHideCmd = wxT("attrib +H ");
		wxHideCmd += fileName;
		size_t rawLength = 0;

		wxCharBuffer buf = wxConvUTF8.cWC2MB(wxHideCmd.c_str(), wxHideCmd.length() + 1, &rawLength);
		const char *cmd = buf.data();
		system(cmd);
	}
	else if (info.GetOperatingSystemId() == wxOS_UNIX_LINUX || info.GetOperatingSystemId() == wxOS_MAC_OSX_DARWIN) {
		//hide the file when running tests on a linux / unix box by renaming to a dot file
		wxString hiddenName;
		wxFileName wxf(fileName);
		hiddenName = wxf.GetPath(true) + wxT(".") + wxf.GetFullName();
		wxRenameFile(fileName, hiddenName, true);
		return hiddenName;
	}
	return fileName;
}

wxString FileTestFixtureClass::GetFileContents(const wxString& fileName) {
	wxString fileToRead = TestProjectDir + fileName;
	std::ifstream file;
	file.open(fileToRead.fn_str(), std::ios::in | std::ios::binary);
	wxASSERT(file.good());
	file.seekg(0, std::ios_base::end);
	int filesize = file.tellg();
	file.seekg(0, std::ios_base::beg);
	char* buf = new char[filesize];
	file.read(buf, filesize);
	file.close();
	wxString fileContents(buf, wxConvUTF8, filesize);
	delete[] buf;
	return fileContents;
}

void FileTestFixtureClass::CreateSubDirectory(const wxString& subDirectory) {
	TouchTestDir();
	wxString fullPath = TestProjectDir + subDirectory;
	if (!wxDirExists(fullPath)) {
		bool good = wxMkdir(fullPath, 0777);

		// this way so that gcc does not think that good is an unused variable
		// in release mode
		wxUnusedVar(good);
		wxASSERT_MSG(good, _("Could not create directory:") + fullPath);
	}
}

void FileTestFixtureClass::TouchTestDir() {
	if (!wxDirExists(TestProjectDir)) {
		bool good = wxMkdir(TestProjectDir, 0777);

		// this way so that gcc does not think that good is an unused variable
		// in release mode
		wxUnusedVar(good);
		wxASSERT_MSG(good, _("Could not create directory:") + TestProjectDir);
	}
}

wxFileName FileTestFixtureClass::AbsoluteDir(const wxString& dirName) {
	wxFileName fileName;
	fileName.AssignDir(TestProjectDir);
	fileName.AppendDir(dirName);
	return fileName;
}
