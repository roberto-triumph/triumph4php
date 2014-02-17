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
#ifndef __T4P_NEWUSERFEATURECLASS_H_
#define __T4P_NEWUSERFEATURECLASS_H_

#include <features/FeatureClass.h>
#include <features/wxformbuilder/NewUserFeatureForms.h>

namespace t4p {

class NewUserFeatureClass : public t4p::FeatureClass {

public:

	NewUserFeatureClass(t4p::AppClass& app);

private:

	void OnAppReady(wxCommandEvent& event);

	void OnTimer(wxTimerEvent& event);


	wxTimer Timer;

	DECLARE_EVENT_TABLE()
};

/**
 * The new user dialog will prompt the user to fill in settings needed
 * by Triumph to run properly.  This dialog will only be shown once
 * to the user - the first time the program is run on a machine.
 */
class NewUserDialogClass : public NewUserDialogGeneratedClass {

public:

	NewUserDialogClass(wxWindow* parent, t4p::GlobalsClass& globals, wxFileName& configFileDir);

private:

	void OnUpdateUi(wxUpdateUIEvent& event);

	void OnOkButton(wxCommandEvent& event);

	t4p::GlobalsClass& Globals;

	wxFileName& ConfigFileDir;

	DECLARE_EVENT_TABLE()
};

}

#endif
