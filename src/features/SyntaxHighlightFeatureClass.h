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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */

#ifndef __T4P_SYNTAXHIGHLIGHTFEATURECLASS_H__
#define __T4P_SYNTAXHIGHLIGHTFEATURECLASS_H__

#include <globals/Events.h>
#include <features/FeatureClass.h>
#include <features/wxformbuilder/SyntaxHighlightFeatureForms.h>
#include <globals/CodeControlOptionsClass.h>
#include <globals/GlobalsClass.h>

namespace t4p {

// forward declaration - defined below
class SyntaxHighlightFeatureClass;

// forward declaration - to prevent unncessary re-compilation
class CodeControlClass;

/** Implementing EditColorsPanelGeneratedClass */
class EditColorsPanelClass : public SyntaxHighlightPanelGeneratedClass {
protected:
	// Handlers for EditColorsPanelGeneratedClass events.
	void OnListBox(wxCommandEvent& event);
	void OnCheck(wxCommandEvent& event);
	void OnColorChanged(wxColourPickerEvent& event);
	void OnFontChanged(wxFontPickerEvent& event);
	void OnThemeChoice(wxCommandEvent& event);

public:
	/** Constructor */
	EditColorsPanelClass(wxWindow* parent,
	                     t4p::SyntaxHighlightFeatureClass& feature);

	bool TransferDataFromWindow();
private:

	t4p::CodeControlOptionsClass& CodeControlOptions;

	t4p::CodeControlOptionsClass EditedCodeControlOptions;

	// the different code previews
	t4p::CodeControlClass* PhpCodeCtrl;
	t4p::CodeControlClass* SqlCodeCtrl;
	t4p::CodeControlClass* CssCodeCtrl;
	t4p::CodeControlClass* JsCodeCtrl;

	t4p::GlobalsClass Globals;

	t4p::EventSinkClass EventSink;

	t4p::SyntaxHighlightFeatureClass& Feature;
	
	void AddPreviews();
};


/**
 * This feature sets the appropriate colors
 * to code control.  Syntax highlighting is already
 * part of the Scintilla code control, this feature
 * sets the appropriate colors based on file type. It will
 * also set colors for the code control margins
 */
class SyntaxHighlightFeatureClass : public t4p::FeatureClass {

public:
	SyntaxHighlightFeatureClass(t4p::AppClass& app);
	
	/**
	 * Change the styles to use based on the
	 * code control's document mode and the given options
	 */
	void ApplyPreferences(t4p::CodeControlClass* ctrl, t4p::CodeControlOptionsClass& options);

private:

	/*
	 * when a file is opened or created, we will
	 * set the colors on the Scintilla code control
	 */
	void OnFileNew(t4p::CodeControlEventClass& event);
	void OnFileOpen(t4p::CodeControlEventClass& event);

	void AddPreferenceWindow(wxBookCtrlBase* parent);

	/**
	 * after the user saves the preferences redraw the code
	 * control because the styles might have changed.
	 */
	void OnPreferencesSaved(wxCommandEvent& event);
	
	/**
	 * on app start we load the bitmaps we use
	 * on the margin markers
	 */
	void OnAppReady(wxCommandEvent& event);
	
	// bitmaps to show in the margin for search hits
	// and bookmarks
	// we load from disk once per feature
	wxBitmap SearchHitGoodBitmap;
	wxBitmap SearchHitBadBitmap;
	wxBitmap BookmarkBitmap;

	DECLARE_EVENT_TABLE()
};

}

#endif // __T4P_SYNTAXHIGHLIGHTFEATURECLASS_H__
