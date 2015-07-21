/**
 * @copyright  2015 Roberto Perpuly
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
#include "FeatureFactoryClass.h"
#include "features/BookmarkFeatureClass.h"
#include "features/ChangelogFeatureClass.h"
#include "features/ConfigFilesFeatureClass.h"
#include "features/DebuggerFeatureClass.h"
#include "features/DetectorFeatureClass.h"
#include "features/DocCommentFeatureClass.h"
#include "features/EditorBehaviorFeatureClass.h"
#include "features/EditorMessagesFeatureClass.h"
#include "features/EnvironmentFeatureClass.h"
#include "features/ExplorerFeatureClass.h"
#include "features/FeatureClass.h"
#include "features/FileCabinetFeatureClass.h"
#include "features/FileModifiedCheckFeatureClass.h"
#include "features/FileOperationsFeatureClass.h"
#include "features/FileWatcherFeatureClass.h"
#include "features/FinderFeatureClass.h"
#include "features/FindInFilesFeatureClass.h"
#include "features/LintFeatureClass.h"
#include "features/NewUserFeatureClass.h"
#include "features/NotebookLayoutFeatureClass.h"
#include "features/OutlineFeatureClass.h"
#include "features/PhpCodeCompletionFeatureClass.h"
#include "features/ProjectFeatureClass.h"
#include "features/RecentFilesFeatureClass.h"
#include "features/RunBrowserFeatureClass.h"
#include "features/RunConsoleFeatureClass.h"
#include "features/SqlBrowserFeatureClass.h"
#include "features/SyntaxHighlightFeatureClass.h"
#include "features/TagFeatureClass.h"
#include "features/TemplateFilesFeatureClass.h"
#include "features/TestFeatureClass.h"
#include "features/TotalSearchFeatureClass.h"
#include "features/VersionUpdateFeatureClass.h"
#include "views/BookmarkViewClass.h"
#include "views/ChangelogViewClass.h"
#include "views/ConfigFilesViewClass.h"
#include "views/DebuggerViewClass.h"
#include "views/DetectorViewClass.h"
#include "views/DocCommentViewClass.h"
#include "views/EditorBehaviorViewClass.h"
#include "views/EditorMessagesViewClass.h"
#include "views/EnvironmentViewClass.h"
#include "views/ExplorerViewClass.h"
#include "views/FeatureViewClass.h"
#include "views/FileCabinetViewClass.h"
#include "views/FileModifiedCheckViewClass.h"
#include "views/FileOperationsViewClass.h"
#include "views/FileWatcherViewClass.h"
#include "views/FinderViewClass.h"
#include "views/FindInFilesViewClass.h"
#include "views/HtmlViewClass.h"
#include "views/JavascriptViewClass.h"
#include "views/LintViewClass.h"
#include "views/NavigationViewClass.h"
#include "views/NewUserViewClass.h"
#include "views/NotebookLayoutViewClass.h"
#include "views/OutlineViewClass.h"
#include "views/PhpCodeCompletionViewClass.h"
#include "views/ProjectViewClass.h"
#include "views/RecentFilesViewClass.h"
#include "views/RunBrowserViewClass.h"
#include "views/RunConsoleViewClass.h"
#include "views/SqlBrowserViewClass.h"
#include "views/SyntaxHighlightViewClass.h"
#include "views/TagViewClass.h"
#include "views/TemplateFilesViewClass.h"
#include "views/TestViewClass.h"
#include "views/TotalSearchViewClass.h"
#include "views/VersionUpdateViewClass.h"

t4p::FeatureFactoryClass::FeatureFactoryClass(t4p::AppClass& app)
: Features()
, FeatureViews()
, App(app)
, FileOperations(NULL)
, EditorBehavior(NULL)
, Environment(NULL)
, FindInFiles(NULL)
, Finder(NULL)
, Project(NULL)
, Outline(NULL)
, Tag(NULL)
, RunConsole(NULL)
, RunBrowser(NULL)
, Lint(NULL)
, SqlBrowser(NULL)
, EditorMessages(NULL)
, RecentFiles(NULL)
, Detector(NULL)
, TemplateFiles(NULL)
, ConfigFiles(NULL)
, FileModifiedCheck(NULL)
, FileWatcher(NULL)
, Explorer(NULL)
, NewUser(NULL)
, VersionUpdate(NULL)
, TotalSearch(NULL)
, DocComment(NULL)
, SyntaxHighlight(NULL)
, Changelog(NULL)
, Bookmark(NULL)
, Debugger(NULL)
, FileCabinet(NULL)
, PhpCodeCompletion(NULL)
, NotebookLayout(NULL)
, Test(NULL) {
}

t4p::FeatureFactoryClass::~FeatureFactoryClass() {
	DeleteFeatures();
	DeleteViews();
}

void t4p::FeatureFactoryClass::DeleteFeatures() {
	for (size_t i = 0; i < Features.size(); i++) {
		delete Features[i];
	}
	Features.clear();
	FileOperations = NULL;
	EditorBehavior = NULL;
	Environment = NULL;
	FindInFiles = NULL;
	Finder = NULL;
	Project = NULL;
	Outline = NULL;
	Tag = NULL;
	RunConsole = NULL;
	RunBrowser = NULL;
	Lint = NULL;
	SqlBrowser = NULL;
	EditorMessages = NULL;
	RecentFiles = NULL;
	Detector = NULL;
	TemplateFiles = NULL;
	ConfigFiles = NULL;
	FileModifiedCheck = NULL;
	FileWatcher = NULL;
	Explorer = NULL;
	NewUser = NULL;
	VersionUpdate = NULL;
	TotalSearch = NULL;
	DocComment = NULL;
	SyntaxHighlight = NULL;
	EditorBehavior = NULL;
	Changelog = NULL;
	Bookmark = NULL;
	Debugger = NULL;
	FileCabinet = NULL;
	PhpCodeCompletion = NULL;
	NotebookLayout = NULL;
	Test = NULL;
}

void t4p::FeatureFactoryClass::DeleteViews() {
	for (size_t i = 0; i < FeatureViews.size(); i++) {
		delete FeatureViews[i];
	}
	FeatureViews.clear();
}

bool t4p::FeatureFactoryClass::CreateFeatures() {
	wxASSERT_MSG(Features.empty(), "features can only be created once");
	if (!Features.empty()) {
		return false;
	}
	FileOperations = new t4p::FileOperationsFeatureClass(App);
	Features.push_back(FileOperations);
	EditorBehavior = new t4p::EditorBehaviorFeatureClass(App);
	Features.push_back(EditorBehavior);
	Environment = new t4p::EnvironmentFeatureClass(App);
	Features.push_back(Environment);
	FindInFiles = new t4p::FindInFilesFeatureClass(App);
	Features.push_back(FindInFiles);
	Finder = new t4p::FinderFeatureClass(App);
	Features.push_back(Finder);
	Project = new t4p::ProjectFeatureClass(App);
	Features.push_back(Project);
	Outline = new t4p::OutlineFeatureClass(App);
	Features.push_back(Outline);
	Tag = new t4p::TagFeatureClass(App);
	Features.push_back(Tag);
	RunConsole = new t4p::RunConsoleFeatureClass(App);
	Features.push_back(RunConsole);
	RunBrowser = new t4p::RunBrowserFeatureClass(App);
	Features.push_back(RunBrowser);
	Lint = new t4p::LintFeatureClass(App);
	Features.push_back(Lint);
	SqlBrowser = new t4p::SqlBrowserFeatureClass(App);
	Features.push_back(SqlBrowser);
	EditorMessages = new t4p::EditorMessagesFeatureClass(App);
	Features.push_back(EditorMessages);
	RecentFiles = new t4p::RecentFilesFeatureClass(App);
	Features.push_back(RecentFiles);
	Detector = new t4p::DetectorFeatureClass(App);
	Features.push_back(Detector);
	TemplateFiles = new t4p::TemplateFilesFeatureClass(App);
	Features.push_back(TemplateFiles);
	ConfigFiles = new t4p::ConfigFilesFeatureClass(App);
	Features.push_back(ConfigFiles);
	FileModifiedCheck = new t4p::FileModifiedCheckFeatureClass(App);
	Features.push_back(FileModifiedCheck);
	FileWatcher = new t4p::FileWatcherFeatureClass(App);
	Features.push_back(FileWatcher);
	Explorer = new t4p::ExplorerFeatureClass(App);
	Features.push_back(Explorer);
	NewUser = new t4p::NewUserFeatureClass(App);
	Features.push_back(NewUser);
	VersionUpdate = new t4p::VersionUpdateFeatureClass(App);
	Features.push_back(VersionUpdate);
	TotalSearch = new t4p::TotalSearchFeatureClass(App);
	Features.push_back(TotalSearch);
	DocComment = new t4p::DocCommentFeatureClass(App);
	Features.push_back(DocComment);
	SyntaxHighlight = new t4p::SyntaxHighlightFeatureClass(App);
	Features.push_back(SyntaxHighlight);
	Changelog = new t4p::ChangelogFeatureClass(App);
	Features.push_back(Changelog);
	Bookmark = new t4p::BookmarkFeatureClass(App);
	Features.push_back(Bookmark);
	Debugger = new t4p::DebuggerFeatureClass(App);
	Features.push_back(Debugger);
	FileCabinet = new t4p::FileCabinetFeatureClass(App);
	Features.push_back(FileCabinet);
	PhpCodeCompletion = new t4p::PhpCodeCompletionFeatureClass(App);
	Features.push_back(PhpCodeCompletion);
	NotebookLayout = new t4p::NotebookLayoutFeatureClass(App);
	Features.push_back(NotebookLayout);
#if T4P_USE_TEST_FEATURE
	Test = new t4p::TestFeatureClass(App);
	Features.push_back(Test);
#endif

	return true;
}

bool t4p::FeatureFactoryClass::CreateViews() {
	wxASSERT_MSG(FeatureViews.empty(), "views can only be created once");
	if (!FeatureViews.empty()) {
		return false;
	}

	// ATTN: currently the way that views are added to the vector
	// determines the order of the menu items
	// that is why the FileOperations view is first
	FeatureViews.push_back(new t4p::FileOperationsViewClass(*FileOperations));
	FeatureViews.push_back(new t4p::EditorBehaviorViewClass(*EditorBehavior));

	FeatureViews.push_back(new t4p::EnvironmentViewClass(*Environment));
	FeatureViews.push_back(new t4p::FindInFilesViewClass(*FindInFiles));
	FeatureViews.push_back(new t4p::FinderViewClass(*Finder));
	FeatureViews.push_back(new t4p::ProjectViewClass(*Project));
	FeatureViews.push_back(new t4p::OutlineViewClass(*Outline));
	FeatureViews.push_back(new t4p::TagViewClass(*Tag));
	FeatureViews.push_back(new t4p::RunConsoleViewClass(*RunConsole));
	FeatureViews.push_back(new t4p::RunBrowserViewClass(*RunBrowser));
	FeatureViews.push_back(new t4p::LintViewClass(*Lint));
	FeatureViews.push_back(new t4p::SqlBrowserViewClass(*SqlBrowser));
	FeatureViews.push_back(new t4p::EditorMessagesViewClass());
	FeatureViews.push_back(new t4p::RecentFilesViewClass(*RecentFiles));
	FeatureViews.push_back(new t4p::DetectorViewClass(*Detector));
	FeatureViews.push_back(new t4p::TemplateFilesViewClass(*TemplateFiles));
	FeatureViews.push_back(new t4p::ConfigFilesViewClass(*ConfigFiles));
	FeatureViews.push_back(new t4p::FileModifiedCheckViewClass(*FileModifiedCheck));
	FeatureViews.push_back(new t4p::FileWatcherViewClass(*FileWatcher));
	FeatureViews.push_back(new t4p::ExplorerViewClass(*Explorer));
	FeatureViews.push_back(new t4p::NewUserViewClass(*NewUser));
	FeatureViews.push_back(new t4p::VersionUpdateViewClass(*VersionUpdate));
	FeatureViews.push_back(new t4p::TotalSearchViewClass(*TotalSearch));
	FeatureViews.push_back(new t4p::DocCommentViewClass(*DocComment));
	FeatureViews.push_back(new t4p::SyntaxHighlightViewClass(*SyntaxHighlight));
	FeatureViews.push_back(new t4p::ChangelogViewClass(*Changelog));
	FeatureViews.push_back(new t4p::BookmarkViewClass(*Bookmark));
	FeatureViews.push_back(new t4p::DebuggerViewClass(*Debugger));
	FeatureViews.push_back(new t4p::FileCabinetViewClass(*FileCabinet));
	FeatureViews.push_back(new t4p::PhpCodeCompletionViewClass(*PhpCodeCompletion));
	FeatureViews.push_back(new t4p::NotebookLayoutViewClass(*NotebookLayout));
	FeatureViews.push_back(new t4p::NavigationViewClass());
	FeatureViews.push_back(new t4p::JavascriptViewClass(App));
	FeatureViews.push_back(new t4p::HtmlViewClass());
#if T4P_USE_TEST_FEATURE
	FeatureViews.push_back(new t4p::TestViewClass(*Test));
#endif

	return true;
}
