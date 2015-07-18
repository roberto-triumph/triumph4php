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
#ifndef T4P_NEWUSERVIEWCLASS_H_
#define T4P_NEWUSERVIEWCLASS_H_

#include <views/FeatureViewClass.h>
#include <features/NewUserFeatureClass.h>
#include <views/wxformbuilder/NewUserFeatureForms.h>
#include <wx/wizard.h>

namespace t4p {

/**
 * The New User feature is a feature that will show the
 * user the welcome wizard the very first time that
 * the user runs the program.
 */
class NewUserViewClass : public t4p::FeatureViewClass {

public:

	NewUserViewClass(t4p::NewUserFeatureClass& feature);

private:

	void OnAppReady(wxCommandEvent& event);

	void OnTimer(wxTimerEvent& event);

	void OnWizardCancel(wxWizardEvent& event);

	wxTimer Timer;

	t4p::NewUserFeatureClass& Feature;

	DECLARE_EVENT_TABLE()
};

/**
 * The new user dialog will prompt the user to fill in settings needed
 * by Triumph to run properly.  This dialog will only be shown once
 * to the user - the first time the program is run on a machine.
- */
class NewUserSettingsPanelClass : public NewUserSettingsPanelGeneratedClass {

public:

	NewUserSettingsPanelClass(wxWindow* parent, t4p::GlobalsClass& globals,
		t4p::PreferencesClass& preferences, wxFileName& configFileDir);

	bool TransferDataFromWindow();

private:

	void OnUpdateUi(wxUpdateUIEvent& event);

	t4p::PreferencesClass& Preferences;

	wxFileName& ConfigFileDir;

	DECLARE_EVENT_TABLE()
};

/**
 * The dialog that shows the user the file extensions to associate to
 * a type (language)
 */
class NewUserAssociationsPanelClass : public NewUserAssociationsPanelGeneratedClass {

public:

	NewUserAssociationsPanelClass(wxWindow* parent, t4p::GlobalsClass& globals);

private:

	t4p::GlobalsClass& Globals;

};

/**
 * The dialog that shows the user the php settings
 */
class NewUserPhpSettingsPanelClass : public NewUserPhpSettingsPanelGeneratedClass {

public:

	NewUserPhpSettingsPanelClass(wxWindow* parent, t4p::GlobalsClass& globals);

	bool TransferDataFromWindow();

private:

	void OnUpdateUi(wxUpdateUIEvent& event);

	t4p::GlobalsClass& Globals;

	DECLARE_EVENT_TABLE()
};

}

#endif
