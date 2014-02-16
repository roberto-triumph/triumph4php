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

// these macros will expand a macro into its 
// these are needed to expand the asset root location 
// which is given as a macro by the premake script
#define T4P_STR_EXPAND(s) #s
#define T4P_STR(s) T4P_STR_EXPAND(s)

/**
 * @return wxFileName the root directory of the asset directory
 *         The asset directory is set via #define, this function
 *         will construct a wxFileName from it depending on whether
 *         the asset directory is relative or absolute.
 */
static wxFileName AssetRootDir() {
	
	std::string stdRoot;
	#ifdef T4P_ASSET_DIR
		stdRoot = T4P_STR(T4P_ASSET_DIR);
	#endif
	
    wxFileName assetRoot;
    wxString assetDir = wxString::FromAscii(stdRoot.c_str());
    if (assetDir.StartsWith(wxT("/"))) {
        assetRoot.AssignDir(assetDir);
    }
    else {
        
        // assume that the path is relative to the executable
		wxStandardPaths paths = wxStandardPaths::Get();
		wxFileName pathExecutableFileName(paths.GetExecutablePath());
		wxString assetStr = pathExecutableFileName.GetPathWithSep() + assetDir;
		assetRoot.AssignDir(assetStr);
		assetRoot.Normalize();
    }
    return assetRoot;
}

wxFileName mvceditor::NativeFunctionsAsset() {
	wxFileName asset = AssetRootDir();
	wxFileName dbFile(asset.GetPath(), wxT("php.db"));
	return dbFile;
}

wxFileName mvceditor::ResourceSqlSchemaAsset() {
	wxFileName asset = AssetRootDir();
    asset.AppendDir(wxT("sql"));
	
	wxFileName sqlFile(asset.GetPath(), wxT("resources.sql"));
	return sqlFile;
}

wxBitmap mvceditor::AutoCompleteImageAsset(wxString imageName) {
	if (!wxImage::FindHandler(wxBITMAP_TYPE_XPM)) {
		wxImage::AddHandler(new wxXPMHandler);	
	}
	wxFileName asset = AssetRootDir();
    asset.AppendDir(wxT("auto_complete"));
	wxFileName iconFile(asset.GetPath(), imageName + wxT(".xpm"));
	
	wxASSERT(iconFile.IsOk());
	wxBitmap bitmap;
	bool loaded = bitmap.LoadFile(iconFile.GetFullPath(), wxBITMAP_TYPE_XPM);
	wxASSERT_MSG(loaded, wxT("failed to load: ") + iconFile.GetFullPath());
	return bitmap;
}

wxBitmap mvceditor::IconImageAsset(wxString imageName) {
	if (!wxImage::FindHandler(wxBITMAP_TYPE_PNG)) {
		wxImage::AddHandler(new wxPNGHandler());
	}
	wxFileName asset = AssetRootDir();
    asset.AppendDir(wxT("icons"));
    wxFileName iconFile(asset.GetPath(), imageName + wxT(".png"));
    
	wxASSERT(iconFile.IsOk());
	wxBitmap bitmap;
	bool loaded = bitmap.LoadFile(iconFile.GetFullPath(), wxBITMAP_TYPE_PNG);
	wxASSERT_MSG(loaded, wxT("failed to load: ") + iconFile.GetFullPath());
	return bitmap;
}

wxFileName mvceditor::PhpDetectorsBaseAsset() {
    wxFileName asset = AssetRootDir();
    asset.AppendDir(wxT("php_detectors"));
	
	wxFileName scriptsFileName;
	scriptsFileName.AssignDir(asset.GetPath());
	return scriptsFileName;
}

wxFileName mvceditor::UrlTagDetectorsGlobalAsset() {
	wxFileName asset = AssetRootDir();
	asset.AppendDir(wxT("php_detectors"));
	asset.AppendDir(wxT("url_detectors"));
	
	wxFileName scriptsFileName;
	scriptsFileName.AssignDir(asset.GetPath());
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
	wxFileName asset = AssetRootDir();
	asset.AppendDir(wxT("php_detectors"));
	asset.AppendDir(wxT("template_files_detectors"));
	
	wxFileName scriptsFileName;
	scriptsFileName.AssignDir(asset.GetPath());
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
	wxFileName asset = AssetRootDir();
	asset.AppendDir(wxT("php_detectors"));
	asset.AppendDir(wxT("tag_detectors"));
	
	wxFileName scriptsFileName;
	scriptsFileName.AssignDir(asset.GetPath());
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
	wxFileName asset = AssetRootDir();
	asset.AppendDir(wxT("php_detectors"));
	asset.AppendDir(wxT("database_detectors"));
	
	wxFileName scriptsFileName;
	scriptsFileName.AssignDir(asset.GetPath());
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
	wxFileName asset = AssetRootDir();
	asset.AppendDir(wxT("php_detectors"));
	asset.AppendDir(wxT("config_detectors"));
	
	wxFileName scriptsFileName;
	scriptsFileName.AssignDir(asset.GetPath());
	return scriptsFileName;
}

wxFileName mvceditor::TempDirAsset() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName tempDir;
	tempDir.AssignDir(paths.GetTempDir());
	tempDir.AppendDir(wxT("triumph4php"));
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
	bootstrapConfigFile.SetFullName(wxT(".triumph4php-bootstrap.ini"));
	if (!bootstrapConfigFile.FileExists()) {
		
		// look at the global config file
		bootstrapConfigFile.AssignDir(paths.GetUserConfigDir());
		bootstrapConfigFile.SetFullName(wxT(".triumph4php-bootstrap.ini"));
	}
	return bootstrapConfigFile;
}


wxFileName mvceditor::SettingsDirAsset() {
	
	// get the location of the settings dir from the bootstrap file
	wxFileName bootstrapConfigFile = mvceditor::BootstrapConfigFileAsset();
	wxString bootstrapFullPath = bootstrapConfigFile.GetFullPath();
	wxFileConfig bootstrapConfig(wxT("bootstrap"), wxEmptyString, 
		bootstrapFullPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
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
		bootstrapConfigFile.SetFullName(wxT(".triumph4php-bootstrap.ini"));
	}
	else {

		// save settings dire in the global bootstrap config file
		bootstrapConfigFile.AssignDir(paths.GetUserConfigDir());
		bootstrapConfigFile.SetFullName(wxT(".triumph4php-bootstrap.ini"));
	}
	wxString bootstrapFullPath = bootstrapConfigFile.GetFullPath();
	wxFileConfig bootstrapConfig(wxT("bootstrap"), wxEmptyString, 
		bootstrapFullPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
	wxString s = settingsDir.GetPath();
	bootstrapConfig.Write(wxT("SettingsDirectory"), s);
	bootstrapConfig.Flush();
}

wxFileName mvceditor::DetectorSqlSchemaAsset() {
	wxFileName asset = AssetRootDir();
	asset.AppendDir(wxT("sql"));
	
	wxFileName sqlFile(asset.GetPath(), wxT("detectors.sql"));
	return sqlFile;
}

wxFileName mvceditor::SkeletonsBaseAsset() {
	wxFileName asset = AssetRootDir();
	asset.AppendDir(wxT("skeletons"));
	
	wxFileName scriptsFileName;
	scriptsFileName.AssignDir(asset.GetPath());
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
	wxFileName asset = AssetRootDir();
	wxFileName versionFile(asset.GetPath(), wxT("version.txt"));
	return versionFile;
}
