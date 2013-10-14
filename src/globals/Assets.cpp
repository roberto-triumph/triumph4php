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
#include <wx/fileconf.h>

wxFileName mvceditor::NativeFunctionsAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
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
	wxStandardPaths paths = wxStandardPaths::Get();
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

wxBitmap mvceditor::AutoCompleteImageAsset(wxString imageName) {
	if (!wxImage::FindHandler(wxBITMAP_TYPE_XPM)) {
		wxImage::AddHandler(new wxXPMHandler);	
	}
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString nativeFileName = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
	                          wxT("..") + wxFileName::GetPathSeparator() +
	                          wxT("resources") + wxFileName::GetPathSeparator() +
							  wxT("auto_complete") + wxFileName::GetPathSeparator() +
							  imageName +
	                          wxT(".xpm");
	wxFileName fileName(nativeFileName);
	fileName.Normalize();
	wxASSERT(fileName.IsOk());
	wxBitmap bitmap;
	bitmap.LoadFile(fileName.GetFullPath(), wxBITMAP_TYPE_XPM);
	return bitmap;
}

wxBitmap mvceditor::IconImageAsset(wxString imageName) {
	if (!wxImage::FindHandler(wxBITMAP_TYPE_PNG)) {
		wxImage::AddHandler(new wxPNGHandler());	
	}
	wxBitmap bitmap;
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString nativeFileName = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
	                          wxT("..") + wxFileName::GetPathSeparator() +
	                          wxT("resources") + wxFileName::GetPathSeparator() +
							  wxT("icons") + wxFileName::GetPathSeparator() +
							  imageName +
	                          wxT(".png");
	wxFileName fileName(nativeFileName);
	fileName.Normalize();
	wxASSERT(fileName.IsOk());
	bitmap.LoadFile(fileName.GetFullPath(), wxBITMAP_TYPE_PNG);
	return bitmap;
}


wxFileName mvceditor::PhpDetectorsBaseAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString scriptsFullPath = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
		wxT("..") + wxFileName::GetPathSeparator() +
		wxT("php_detectors");
	wxFileName scriptsFileName;
	scriptsFileName.AssignDir(scriptsFullPath);
	scriptsFileName.Normalize();
	return scriptsFileName;
}

wxFileName mvceditor::UrlTagDetectorsGlobalAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString scriptsFullPath = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
		wxT("..") + wxFileName::GetPathSeparator() +
		wxT("php_detectors") + wxFileName::GetPathSeparator() +
		wxT("url_detectors");
	wxFileName scriptsFileName;
	scriptsFileName.AssignDir(scriptsFullPath);
	scriptsFileName.Normalize();
	return scriptsFileName;
}

wxFileName mvceditor::UrlTagDetectorsLocalAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName configDir = mvceditor::ConfigDirAsset();
	configDir.AppendDir(wxT("url_detectors"));
	if (!configDir.DirExists()) {
		wxMkdir(configDir.GetPath(), 0777);
	}
	return configDir;
}

wxFileName mvceditor::TemplateFilesDetectorsGlobalAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString scriptsFullPath = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
		wxT("..") + wxFileName::GetPathSeparator() +
		wxT("php_detectors") + wxFileName::GetPathSeparator() +
		wxT("template_files_detectors");
	wxFileName scriptsFileName;
	scriptsFileName.AssignDir(scriptsFullPath);
	scriptsFileName.Normalize();
	return scriptsFileName;
}

wxFileName mvceditor::TemplateFileTagsDetectorsLocalAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName configDir = mvceditor::ConfigDirAsset();
	configDir.AppendDir(wxT("template_files_detectors"));
	if (!configDir.DirExists()) {
		wxMkdir(configDir.GetPath(), 0777);
	}
	return configDir;
}

wxFileName mvceditor::TagDetectorsLocalAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName configDir = mvceditor::ConfigDirAsset();
	configDir.AppendDir(wxT("tag_detectors"));
	if (!configDir.DirExists()) {
		wxMkdir(configDir.GetPath(), 0777);
	}
	return configDir;
}

wxFileName mvceditor::TagDetectorsGlobalAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString scriptsFullPath = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
		wxT("..") + wxFileName::GetPathSeparator() +
		wxT("php_detectors") + wxFileName::GetPathSeparator() +
		wxT("tag_detectors");
	wxFileName scriptsFileName;
	scriptsFileName.AssignDir(scriptsFullPath);
	scriptsFileName.Normalize();
	return scriptsFileName;
}

wxFileName mvceditor::DatabaseTagDetectorsLocalAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName configDir = mvceditor::ConfigDirAsset();
	configDir.AppendDir(wxT("database_detectors"));
	if (!configDir.DirExists()) {
		wxMkdir(configDir.GetPath(), 0777);
	}
	return configDir;
}

wxFileName mvceditor::DatabaseTagDetectorsGlobalAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString scriptsFullPath = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
		wxT("..") + wxFileName::GetPathSeparator() +
		wxT("php_detectors") + wxFileName::GetPathSeparator() +
		wxT("database_detectors");
	wxFileName scriptsFileName;
	scriptsFileName.AssignDir(scriptsFullPath);
	scriptsFileName.Normalize();
	return scriptsFileName;
}

wxFileName mvceditor::ConfigTagDetectorsLocalAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName configDir = mvceditor::ConfigDirAsset();
	configDir.AppendDir(wxT("config_detectors"));
	if (!configDir.DirExists()) {
		wxMkdir(configDir.GetPath(), 0777);
	}
	return configDir;
}

wxFileName mvceditor::ConfigTagDetectorsGlobalAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString scriptsFullPath = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
		wxT("..") + wxFileName::GetPathSeparator() +
		wxT("php_detectors") + wxFileName::GetPathSeparator() +
		wxT("config_detectors");
	wxFileName scriptsFileName;
	scriptsFileName.AssignDir(scriptsFullPath);
	scriptsFileName.Normalize();
	return scriptsFileName;
}

wxFileName mvceditor::TempDirAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName tempDir;
	tempDir.AssignDir(paths.GetTempDir());
	tempDir.AppendDir(wxT("mvc-editor"));
	if (!tempDir.DirExists()) {
		wxMkdir(tempDir.GetPath(), 0777);
	}
	return tempDir;
}

wxFileName mvceditor::ConfigDirAsset() {

	// the config dir is in the bootstrap file
	// the bootstrap file could be located in the same dir as the executable
	// or in the user data directory
	wxFileName bootstrapConfigFile = mvceditor::BootstrapConfigFileAsset();
	wxFileConfig config(wxT("bootstrap"), wxEmptyString, 
		bootstrapConfigFile.GetFullPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
	wxString configDirString;
	if (!config.Read("SettingsDirectory", &configDirString)) {

		// the first time the program runs there is no bootstrap config,
		// use the bootstrap dir as the config dir
		configDirString = bootstrapConfigFile.GetPath();
	}
	wxFileName configDir;
	configDir.AssignDir(configDirString);
	return configDir;
}

wxFileName mvceditor::BootstrapConfigFileAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName bootstrapConfigFile;

	// look at the local bootstrap config file
	bootstrapConfigFile.Assign(paths.GetExecutablePath());
	bootstrapConfigFile.SetFullName(wxT("mvc-editor-bootstrap.ini"));
	if (!bootstrapConfigFile.FileExists()) {
		
		// look at the global config file
		bootstrapConfigFile.Assign(paths.GetUserConfigDir());
		bootstrapConfigFile.SetFullName(wxT("mvc-editor-bootstrap.ini"));
	}
	return bootstrapConfigFile;
}


wxFileName mvceditor::SettingsDirAsset() {
	
	// get the location of the settings dir from the bootstrap file
	wxFileName bootstrapConfigFile = mvceditor::BootstrapConfigFileAsset();
	wxFileConfig bootstrapConfig(wxT("bootstrap"), wxEmptyString, 
		bootstrapConfigFile.GetFullPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
	wxString settingsDirString;
	bootstrapConfig.Read(wxT("SettingsDirectory"), &settingsDirString);
	wxFileName settingsDir;
	settingsDir.AssignDir(settingsDirString);
	return settingsDir;
}

void mvceditor::SetSettingsDirLocation(const wxFileName& settingsDir) {
	wxFileName bootstrapConfigFile;
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName executableDir(paths.GetExecutablePath());

	// the settings dir is in the same directory as the executable. save
	// settings dir in the local bootstrap file
	if (settingsDir.GetPathWithSep().Find(executableDir.GetPathWithSep()) != wxNOT_FOUND) {
		bootstrapConfigFile.Assign(paths.GetExecutablePath());
		bootstrapConfigFile.SetFullName(wxT("mvc-editor-bootstrap.ini"));
	}
	else {

		// save settings dire in the global bootstrap config file
		bootstrapConfigFile.Assign(paths.GetUserConfigDir());
		bootstrapConfigFile.SetFullName(wxT("mvc-editor-bootstrap.ini"));
	}
	wxFileConfig bootstrapConfig(wxT("bootstrap"), wxEmptyString, 
		bootstrapConfigFile.GetFullPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
	wxString s = settingsDir.GetPath();
	bootstrapConfig.Write(wxT("SettingsDirectory"), s);
	bootstrapConfig.Flush();
}

wxFileName mvceditor::DetectorSqlSchemaAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString nativeFileName = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
	                          wxT("..") + wxFileName::GetPathSeparator() +
	                          wxT("resources") + wxFileName::GetPathSeparator() +
	                          wxT("sql") + wxFileName::GetPathSeparator() +
							  wxT("detectors.sql");
	wxFileName fileName(nativeFileName);
	fileName.Normalize();
	return fileName;
}

wxFileName mvceditor::SkeletonsBaseAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString scriptsFullPath = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
		wxT("..") + wxFileName::GetPathSeparator() +
		wxT("resources") + wxFileName::GetPathSeparator() +
		wxT("skeletons");
	wxFileName scriptsFileName;
	scriptsFileName.AssignDir(scriptsFullPath);
	scriptsFileName.Normalize();
	return scriptsFileName;
}

wxFileName mvceditor::TagCacheAsset() {
	wxFileName configDir = mvceditor::ConfigDirAsset();
	wxFileName tagCacheFileName(configDir.GetFullPath(), wxT("tags.db"));
	return tagCacheFileName;
}

wxFileName mvceditor::TagCacheWorkingAsset() {
	wxFileName configDir = mvceditor::ConfigDirAsset();
	wxFileName tagCacheFileName(configDir.GetFullPath(), wxT("working_tags.db"));
	return tagCacheFileName;
}

wxFileName mvceditor::DetectorCacheAsset() {
	wxFileName configDir = mvceditor::ConfigDirAsset();
	wxFileName tagCacheFileName(configDir.GetFullPath(), wxT("detectors.db"));
	return tagCacheFileName;
}

wxFileName mvceditor::VersionFileAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName versionFile(paths.GetExecutablePath());
	versionFile.RemoveLastDir();
	versionFile.SetFullName(wxT("version.txt"));
	return versionFile;
}