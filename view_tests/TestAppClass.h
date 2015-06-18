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
#ifndef __T4P_TESTAPPCLASS_H__
#define __T4P_TESTAPPCLASS_H__

#include <wx/aui/aui.h>
#include <wx/frame.h>
#include <widgets/StatusBarWithGaugeClass.h>
#include <widgets/NotebookClass.h>
#include <widgets/AuiManager.h>
#include <globals/GlobalsClass.h>
#include <features/FeatureClass.h>
#include <views/FeatureViewClass.h>
#include <Triumph.h>

/**
 * The TestApp class takes care of initializing a
 * proper environment for views to run in insolation.
 * The proper environment means:
 * 1. proper initialization of the wxWidgets library
 * 2. proper creation of the Application-wide windows, menus, and frame.
 * 3. proper creation of the Application-wide data structures
 * 4. proper wiring up of views and features to the application-wide
 *    event sink, just like in the "real" app
 * 
 * Note that even though we create windows, we never run the 
 * main event loop so the tests always retain control of the 
 * program.
 */
class TestAppClass : public wxApp {

public:

	// the App contains the application data structures
	wxTimer* Timer;
	t4p::AppClass* App;

	// the various widgets that the application
	// creates
	wxFrame* Frame;
	t4p::StatusBarWithGaugeClass* StatusBarWithGauge;
	wxAuiNotebook* ToolsNotebook;
	wxAuiNotebook* OutlineNotebook; 	
	wxMenuBar* MenuBar;
	wxAuiToolBar* ToolBar;
	wxAuiManager* AuiManager;
	
	TestAppClass();
	
	~TestAppClass();
	
	/**
	 * initializes the wxWidgets library but does
	 * NOT start the run loop. 
	 * Also, creates the application windows and
	 * data structures,.
	 */
	void InitEnvironment();
	
	/**
	 * Delete all data structures, windows, and 
	 * un-initialize wxWidgets.
	 */
	void CleanupEnvironment();
	
	/**
	 * Initializes the given feature just like the "real app" does. After
	 * a call to this method, the feature will process all events from the 
	 * event sink, and can access windows, menus, and toolbars.
	 *
	 * @param view the view to bootstrap. This class will NOT own the pointer.
	 */
	void TriumphBootstrapView(t4p::FeatureViewClass* view) ;
	
	/**
	 * Initializes the given feature just like the "real app" does. After
	 * a call to this method, the feature will process all events from the 
	 * event sink.
	 * 
	 * @param feature the feature to bootstrap. This class will NOT own
	 *        the pointer.
	 */
	void TriumphBootstrapFeature(t4p::FeatureClass* feature) ;

	/**
	 * Create the application data structures and windows
	 * Should only be called after wxWidgets has been
	 * initialized.
	 */
	virtual bool OnInit();
	
private:
	
	/**
	 * initialized the wxWidgets library but does
	 * NOT start the run loop. 
	 */
	void InitWxWidgets();
	
	/**
	 * Deletes the app, windows, and wxWidgets 
	 */
	void CleanupWxWidgets();
	
	void InitApp();

	void CleanupApp();
};

/**
 * The ViewTest app helps with automatic init / de-init
 * of the application environment.
 */
class ViewTestClass {
	
public:

	TestAppClass* TestApp;

	/**
	 * Initializes the wxWidgets and the app environment.
	 */
	ViewTestClass();
	
	/**
	 * Uninitialized the wxWidgets and the app environment.
	 */
	~ViewTestClass();
	
private:

};

#endif