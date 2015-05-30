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
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef T4P_FeatureFactoryClass_H__
#define T4P_FeatureFactoryClass_H__

#include <vector>

namespace t4p {

/**
 * The feature factory class will take care of instantiating
 * and deleting features along with corresponding views.
 * 
 * The main reason for this class existing is to allow
 * for forward declarations of features so that we prevent
 * compilation as much as possible; because feature views
 * need features, we need to keep each feature in a separate
 * variable; if we included the feature then every time 
 * a new feature is created we would need to edit the triumph
 * header, which is required by many features; this would result
 * in all features being recompiled.
 */
 
class FeatureClass;
class FeatureViewClass;
class AppClass;

class EnvironmentFeatureClass;
class FindInFilesFeatureClass;
class FinderFeatureClass;
class ProjectFeatureClass;
class OutlineFeatureClass;
class TagFeatureClass;
class RunConsoleFeatureClass;
class RunBrowserFeatureClass;
class LintFeatureClass;
class SqlBrowserFeatureClass;
class EditorMessagesFeatureClass;
class RecentFilesFeatureClass;
class DetectorFeatureClass;
class TemplateFilesFeatureClass;
class ConfigFilesFeatureClass;
class FileModifiedCheckFeatureClass;
class FileWatcherFeatureClass;
class ExplorerFeatureClass;
class NewUserFeatureClass;
class VersionUpdateFeatureClass;
class TotalSearchFeatureClass;
class DocCommentFeatureClass;
class SyntaxHighlightFeatureClass;
class EditorBehaviorFeatureClass;
class ChangelogFeatureClass;
class BookmarkFeatureClass;
class DebuggerFeatureClass;
class FileCabinetFeatureClass;
class PhpCodeCompletionFeatureClass;
class TestFeatureClass;
class FileOperationsFeatureClass;


class FeatureFactoryClass {

public:

	/**
	 * This class owns these pointers; do not delete them
	 */
	std::vector<t4p::FeatureClass*> Features;
	std::vector<t4p::FeatureViewClass*> FeatureViews;

	FeatureFactoryClass(t4p::AppClass& app);
	
	~FeatureFactoryClass();
	
	bool CreateFeatures();
	void DeleteFeatures();
	
	/**
	 * Feature views are view specific functionality; a view
	 * is the class that makes updates to the GUI of the editor
	 * (adds menu items, buttons, panels, etc).
	 * Each feature view is created once at app start, and will
	 * be deleted when the app main frame is deleted. Note that
	 * on Mac OS X, an application may run without a main frame,
	 * so during a run feature views may be created more than once
	 * (but there will be at most 1 instance to each view).
	 * 
	 * @return bool TRUE if views were created; false f views
	 *         were not created. View will not be created if
	 *         at least 1 view is instantiated. This is an all-or
	 *         nothing operation; either all views are created
	 *         or none are created.
	 */
	bool CreateViews();
	
	void DeleteViews();
	
	private:
	
	t4p::AppClass& App;

	t4p::FileOperationsFeatureClass*       FileOperations;
	t4p::EditorBehaviorFeatureClass*       EditorBehavior;
	t4p::EnvironmentFeatureClass*          Environment;
	t4p::FindInFilesFeatureClass*          FindInFiles;
	t4p::FinderFeatureClass*               Finder;
	t4p::ProjectFeatureClass*              Project;
	t4p::OutlineFeatureClass*              Outline;
	t4p::TagFeatureClass*                  Tag;
	t4p::RunConsoleFeatureClass*           RunConsole;
	t4p::RunBrowserFeatureClass*           RunBrowser;
	t4p::LintFeatureClass*                 Lint;
	t4p::SqlBrowserFeatureClass*           SqlBrowser;
	t4p::EditorMessagesFeatureClass*       EditorMessages;
	t4p::RecentFilesFeatureClass*          RecentFiles;
	t4p::DetectorFeatureClass*             Detector;
	t4p::TemplateFilesFeatureClass*        TemplateFiles;
	t4p::ConfigFilesFeatureClass*          ConfigFiles;
	t4p::FileModifiedCheckFeatureClass*    FileModifiedCheck;
	t4p::FileWatcherFeatureClass*          FileWatcher;
	t4p::ExplorerFeatureClass*             Explorer;
	t4p::NewUserFeatureClass*              NewUser;
	t4p::VersionUpdateFeatureClass*        VersionUpdate;
	t4p::TotalSearchFeatureClass*          TotalSearch;
	t4p::DocCommentFeatureClass*           DocComment;
	t4p::SyntaxHighlightFeatureClass*      SyntaxHighlight;
	t4p::ChangelogFeatureClass*            Changelog;
	t4p::BookmarkFeatureClass*             Bookmark;
	t4p::DebuggerFeatureClass*             Debugger;
	t4p::FileCabinetFeatureClass*          FileCabinet;
	t4p::PhpCodeCompletionFeatureClass*    PhpCodeCompletion;
	t4p::TestFeatureClass*                 Test;


};

}

#endif
