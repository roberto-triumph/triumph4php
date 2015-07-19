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
#include <UnitTest++.h>
#include <TestAppClass.h>

#include <features/FileOperationsFeatureClass.h>
#include <views/FileOperationsViewClass.h>

class FileOperationsViewTestClass : public ViewTestClass {
	public:
	t4p::FileOperationsFeatureClass* Feature;
	t4p::FileOperationsViewClass* View;

	FileOperationsViewTestClass()
	: ViewTestClass() {
		DoSetup();
	}

	~FileOperationsViewTestClass() {
		DoCleanup();
	}

	void DoSetup() {
		Feature = new t4p::FileOperationsFeatureClass(*TestApp->App);
		View = new t4p::FileOperationsViewClass(*Feature);
		TestApp->TriumphBootstrapFeature(Feature);
		TestApp->TriumphBootstrapView(View);
	}

	void DoCleanup() {
		delete View;
		delete Feature;
	}
};

TEST_FIXTURE(FileOperationsViewTestClass, SaveMenuShouldBeDisabledByDefault) {
	wxMenu* fileMenu = new wxMenu;
	View->AddFileMenuItems(fileMenu);
	TestApp->MenuBar->Append(fileMenu, _("File"));

	CHECK(fileMenu->FindChildItem(wxID_SAVE) != NULL);
	CHECK(!fileMenu->IsEnabled(wxID_SAVE));
}

TEST_FIXTURE(FileOperationsViewTestClass, SaveMenuShouldBeEnabledWhenSavepoint) {
	wxMenu* fileMenu = new wxMenu;
	View->AddFileMenuItems(fileMenu);
	TestApp->MenuBar->Append(fileMenu, _("File"));

	//open a new code control
	t4p::NotebookClass* newNotebook = new t4p::NotebookClass(TestApp->Frame);
	newNotebook->InitApp(

		&TestApp->App->Preferences.CodeControlOptions,
		&TestApp->App->Preferences,
		&TestApp->App->Globals,
		&TestApp->App->EventSink,
		TestApp->AuiManager
	);
	t4p::AuiAddCodeNotebook(*TestApp->AuiManager, newNotebook, 1);
	newNotebook->AddTriumphPage(t4p::FILE_TYPE_PHP);
	wxFocusEvent focus;
	newNotebook->GetCodeControl(0)->ProcessWindowEvent(focus);
	newNotebook->GetCodeControl(0)->AppendText("// file.php\n");

	wxStyledTextEvent evt(wxEVT_STC_SAVEPOINTLEFT);
	View->ProcessEvent(evt);

	CHECK(fileMenu->FindChildItem(wxID_SAVE) != NULL);
	CHECK(fileMenu->IsEnabled(wxID_SAVE));
}
