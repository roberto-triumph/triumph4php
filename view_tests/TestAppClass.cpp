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

#include "TestAppClass.h"
#include <TestAppClass.h>
#include <UnitTest++.h>

TestAppClass::TestAppClass()
: wxApp() {
}

TestAppClass::~TestAppClass() {
}

void TestAppClass::InitEnvironment() {
	InitWxWidgets();

	// don't call wxTheApp->CallOnInit()
	// as that function will run the event loop
	// on Mac OS X, and we don't want to run an
	// event loop in the tests
	// OnInit creates the app windows
	wxTheApp->OnInit();
}

void TestAppClass::CleanupEnvironment() {
	CleanupApp();
	CleanupWxWidgets();
}

void TestAppClass::TriumphBootstrapView(t4p::FeatureViewClass* view) {
	App->EventSink.PushHandler(view);
	view->InitWindow(
		StatusBarWithGauge,
		ToolsNotebook,
		OutlineNotebook,
		AuiManager,
		MenuBar,
		ToolBar);
}

void TestAppClass::TriumphBootstrapFeature(t4p::FeatureClass* feature) {
	App->EventSink.PushHandler(feature);
}

bool TestAppClass::OnInit() {
	if (!wxApp::OnInit()) {
		return false;
	}
	InitApp();
	return true;
}

void TestAppClass::InitWxWidgets() {
	int argc = 0;
	char** argv = 0;
	wxApp::SetInstance(this);
	bool good = wxEntryStart(argc, argv);
	CHECK(good);
}

void TestAppClass::CleanupWxWidgets() {
	wxEntryCleanup();
}

void TestAppClass::InitApp() {
	Timer = new wxTimer();
	App = new t4p::AppClass(*Timer);

	Frame = new wxFrame(NULL, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE);
	StatusBarWithGauge = new t4p::StatusBarWithGaugeClass(Frame);
	ToolsNotebook = new wxAuiNotebook(Frame);
	OutlineNotebook = new wxAuiNotebook(Frame);
	MenuBar = new wxMenuBar();
	ToolBar = new wxAuiToolBar(Frame, wxID_ANY);

	Frame->SetStatusBar(StatusBarWithGauge);
	Frame->SetMenuBar(MenuBar);
	SetTopWindow(Frame);
	AuiManager = new wxAuiManager();
}

void TestAppClass::CleanupApp() {
	Frame->Close();
	wxTheApp->OnRun();
	wxTheApp->OnExit();

	delete AuiManager;
	delete App;
	delete Timer;
}

ViewTestClass::ViewTestClass() {
	TestApp = new TestAppClass();
	TestApp->InitEnvironment();
}

ViewTestClass::~ViewTestClass() {
	// CleanupEnvironment also deletes
	// the TestApp pointer
	TestApp->CleanupEnvironment();
}

IMPLEMENT_APP_NO_MAIN(TestAppClass)
