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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <features/DetectorFeatureClass.h>
#include <actions/UrlTagDetectorActionClass.h>
#include <actions/TagDetectorActionClass.h>
#include <actions/CallStackActionClass.h>
#include <actions/TemplateFileTagsDetectorActionClass.h>
#include <actions/DatabaseTagDetectorActionClass.h>
#include <actions/ConfigTagDetectorActionClass.h>
#include <globals/Assets.h>
#include <globals/Errors.h>
#include <Triumph.h>
#include <wx/artprov.h>
#include <wx/file.h>

t4p::DetectorClass::DetectorClass() {

}

t4p::DetectorClass::~DetectorClass() {

}

t4p::UrlTagDetectorClass::UrlTagDetectorClass()  {

}

bool t4p::UrlTagDetectorClass::CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project) {
	t4p::SourceClass source = project.Sources[0];
	wxString rootUrl = globals.Environment.Apache.GetUrl(source.RootDirectory.GetPath());
	if (rootUrl.IsEmpty()) {
		wxMessageBox(_("Cannot determine the root URL of the selected project. Please add a virtual host mapping for ") + source.RootDirectory.GetPath() +
			_(" under Edit -> Preferences -> Apache"));
		return false;
	}
	return true;
}

wxString t4p::UrlTagDetectorClass::TestCommandLine(const t4p::GlobalsClass& globals,
													  const t4p::ProjectClass& project,
													  const wxString& detectorScriptFullPath) {
	t4p::SourceClass source = project.Sources[0];
	wxString rootUrl = globals.Environment.Apache.GetUrl(source.RootDirectory.GetPath());

	t4p::UrlTagDetectorParamsClass params;
	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = t4p::PhpDetectorsBaseAsset();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	params.RootUrl = rootUrl;
	params.ResourceDbFileName = globals.TagCacheDbFileName;
	return params.BuildCmdLine();
}

wxFileName t4p::UrlTagDetectorClass::LocalRootDir() {
	return t4p::UrlTagDetectorsLocalAsset();
}

wxFileName t4p::UrlTagDetectorClass::GlobalRootDir() {
	return t4p::UrlTagDetectorsGlobalAsset();
}

wxFileName t4p::UrlTagDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = t4p::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("UrlDetector.skeleton.php"));
	return skeletonFile;
}

wxString t4p::UrlTagDetectorClass::Label() {
	return _("URL Detectors");
}

t4p::TemplateFileTagsDetectorClass::TemplateFileTagsDetectorClass()
	: DetectorClass() {

}

bool  t4p::TemplateFileTagsDetectorClass::CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project) {
	return true;
}

wxString t4p::TemplateFileTagsDetectorClass::TestCommandLine(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project,
																const wxString& detectorScriptFullPath) {
	t4p::TemplateFileTagsDetectorParamsClass params;
	t4p::SourceClass source = project.Sources[0];

	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = t4p::PhpDetectorsBaseAsset().GetFullPath();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	params.DetectorDbFileName = globals.DetectorCacheDbFileName;
	params.OutputDbFileName = wxT("");
	return params.BuildCmdLine();
}

wxFileName t4p::TemplateFileTagsDetectorClass::LocalRootDir() {
	return t4p::TemplateFileTagsDetectorsLocalAsset();
}

wxFileName t4p::TemplateFileTagsDetectorClass::GlobalRootDir() {
	return t4p::TemplateFilesDetectorsGlobalAsset();
}

wxFileName t4p::TemplateFileTagsDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = t4p::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("TemplateFilesDetector.skeleton.php"));
	return skeletonFile;
}

wxString t4p::TemplateFileTagsDetectorClass::Label() {
	return _("Template File Detectors");
}

t4p::TagDetectorClass::TagDetectorClass()  {

}

bool t4p::TagDetectorClass::CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project) {
	return true;
}

wxString t4p::TagDetectorClass::TestCommandLine(const t4p::GlobalsClass& globals,
													  const t4p::ProjectClass& project,
													  const wxString& detectorScriptFullPath) {
	t4p::SourceClass source = project.Sources[0];
	wxString rootUrl = globals.Environment.Apache.GetUrl(source.RootDirectory.GetPath());

	t4p::TagDetectorParamsClass params;
	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = t4p::PhpDetectorsBaseAsset();
	params.ScriptName = detectorScriptFullPath;
	params.ResourceDbFileName = globals.TagCacheDbFileName;
	params.SourceDir = source.RootDirectory;
	return params.BuildCmdLine();
}

wxFileName t4p::TagDetectorClass::LocalRootDir() {
	return t4p::TagDetectorsLocalAsset();
}

wxFileName t4p::TagDetectorClass::GlobalRootDir() {
	return t4p::TagDetectorsGlobalAsset();
}

wxFileName t4p::TagDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = t4p::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("TagDetector.skeleton.php"));
	return skeletonFile;
}

wxString t4p::TagDetectorClass::Label() {
	return _("Tag Detectors");
}

t4p::DatabaseTagDetectorClass::DatabaseTagDetectorClass()  {

}

bool t4p::DatabaseTagDetectorClass::CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project) {
	return true;
}

wxString t4p::DatabaseTagDetectorClass::TestCommandLine(const t4p::GlobalsClass& globals,
													  const t4p::ProjectClass& project,
													  const wxString& detectorScriptFullPath) {
	t4p::SourceClass source = project.Sources[0];

	t4p::DatabaseTagDetectorParamsClass params;
	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = t4p::PhpDetectorsBaseAsset();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	return params.BuildCmdLine();
}

wxFileName t4p::DatabaseTagDetectorClass::LocalRootDir() {
	return t4p::DatabaseTagDetectorsLocalAsset();
}

wxFileName t4p::DatabaseTagDetectorClass::GlobalRootDir() {
	return t4p::DatabaseTagDetectorsGlobalAsset();
}

wxFileName t4p::DatabaseTagDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = t4p::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("DatabaseDetector.skeleton.php"));
	return skeletonFile;
}

wxString t4p::DatabaseTagDetectorClass::Label() {
	return _("Database Detectors");
}

t4p::ConfigTagDetectorClass::ConfigTagDetectorClass()  {

}

bool t4p::ConfigTagDetectorClass::CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project) {
	return true;
}

wxString t4p::ConfigTagDetectorClass::TestCommandLine(const t4p::GlobalsClass& globals,
													  const t4p::ProjectClass& project,
													  const wxString& detectorScriptFullPath) {

	t4p::SourceClass source = project.Sources[0];

	t4p::ConfigTagDetectorParamsClass params;
	params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = t4p::PhpDetectorsBaseAsset();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	return params.BuildCmdLine();
}

wxFileName t4p::ConfigTagDetectorClass::LocalRootDir() {
	return t4p::ConfigTagDetectorsLocalAsset();
}

wxFileName t4p::ConfigTagDetectorClass::GlobalRootDir() {
	return t4p::ConfigTagDetectorsGlobalAsset();
}

wxFileName t4p::ConfigTagDetectorClass::SkeletonFile() {
	wxFileName skeletonFile = t4p::SkeletonsBaseAsset();
	skeletonFile.Assign(skeletonFile.GetPath(), wxT("ConfigDetector.skeleton.php"));
	return skeletonFile;
}

wxString t4p::ConfigTagDetectorClass::Label() {
	return _("Config Detectors");
}

t4p::DetectorFeatureClass::DetectorFeatureClass(t4p::AppClass &app)
	: FeatureClass(app) {

}

void t4p::DetectorFeatureClass::RunUrlDetectors() {
	if (App.Sequences.Running()) {
		wxMessageBox(_("Please wait for the current background task to finish"));
		return;
	}
	std::vector<t4p::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	actions.push_back(
		new t4p::UrlTagDetectorActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_URL_TAG_DETECTOR)
	);
	App.Sequences.Build(actions);
}

void t4p::DetectorFeatureClass::RunTemplateFileDetectors() {
	if (App.Sequences.Running()) {
		wxMessageBox(_("Please wait for the current background task to finish"));
		return;
	}
	std::vector<t4p::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	t4p::CallStackActionClass* callStackAction =  new t4p::CallStackActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_CALL_STACK);
	t4p::UrlTagClass urlTag = App.Globals.CurrentUrl;

	if (!urlTag.Url.GetServer().IsEmpty() && urlTag.FileName.IsOk()
		&& !urlTag.ClassName.IsEmpty() && !urlTag.MethodName.IsEmpty()) {
		callStackAction->SetCallStackStart(urlTag.FileName,
			t4p::WxToIcu(urlTag.ClassName),
			t4p::WxToIcu(urlTag.MethodName),
			App.Globals.DetectorCacheDbFileName);
		actions.push_back(callStackAction);
		actions.push_back(
			new t4p::TemplateFileTagsDetectorActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_TEMPLATE_FILE_TAG_DETECTOR)
		);
		App.Sequences.Build(actions);
	}
	else {
		t4p::EditorLogWarningFix("Template Error",
			_("Need to choose a URL to detect templates for. Template files feature depends on the URL detectors feature."));
	}
}

void t4p::DetectorFeatureClass::RunTagDetectors() {
	std::vector<t4p::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	actions.push_back(
		new t4p::TagDetectorActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_TAG_DETECTOR)
	);
	App.Sequences.Build(actions);
}

void t4p::DetectorFeatureClass::RunDatabaseDetectors() {
	std::vector<t4p::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	actions.push_back(
		new t4p::DatabaseTagDetectorActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_DATABASE_TAG_DETECTOR)
	);
	App.Sequences.Build(actions);
}

void t4p::DetectorFeatureClass::RunConfigDetectors() {
	std::vector<t4p::GlobalActionClass*> actions;

	// the sequence class will own this pointer
	actions.push_back(
		new t4p::ConfigTagDetectorActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_CONFIG_TAG_DETECTOR)
	);
	App.Sequences.Build(actions);
}

