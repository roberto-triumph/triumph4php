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

#ifndef __MVCEDITOR_SYNTAXHIGHLIGHTFEATURECLASS_H__
#define __MVCEDITOR_SYNTAXHIGHLIGHTFEATURECLASS_H__

#include <globals/Events.h>
#include <features/FeatureClass.h>
#include <features/wxformbuilder/SyntaxHighlightFeatureForms.h>
#include <globals/CodeControlOptionsClass.h>
#include <globals/GlobalsClass.h>

namespace mvceditor {

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
	                     mvceditor::SyntaxHighlightFeatureClass& feature);

	bool TransferDataFromWindow();
private:

	mvceditor::CodeControlOptionsClass& CodeControlOptions;

	mvceditor::CodeControlOptionsClass EditedCodeControlOptions;

	mvceditor::CodeControlClass* CodeCtrl;

	mvceditor::GlobalsClass Globals;

	mvceditor::EventSinkClass EventSink;

	mvceditor::SyntaxHighlightFeatureClass& Feature;
	
	void AddPreviews();
};


/**
 * This feature sets the appropriate colors
 * to code control.  Syntax highlighting is already
 * part of the Scintilla code control, this feature
 * sets the appropriate colors based on file type. It will
 * also set colors for the code control margins
 */
class SyntaxHighlightFeatureClass : public mvceditor::FeatureClass {

public:
	SyntaxHighlightFeatureClass(mvceditor::AppClass& app);
	
	/**
	 * Determine the correct styles to use based on the
	 * code control's document mode
	 */
	void ApplyPreferences(mvceditor::CodeControlClass* ctrl);

private:

	/*
	 * when a file is opened or created, we will
	 * set the colors on the Scintilla code control
	 */
	void OnFileNew(mvceditor::CodeControlEventClass& event);
	void OnFileOpen(mvceditor::CodeControlEventClass& event);

	void AddPreferenceWindow(wxBookCtrlBase* parent);

	/**
	 * after the user saves the preferences redraw the code
	 * control because the styles might have changed.
	 */
	void OnPreferencesSaved(wxCommandEvent& event);

//------------------------------------------------------------------------
// setting the various wxStyledTextCtrl options
// wxStyledTextCtrl is super-configurable.  These methods will turn on
// some sensible defaults for plain-text, PHP, HTML, and SQL editing.
//------------------------------------------------------------------------

	/**
	 * set the margin look of the source control
	 */
	void SetMargin(wxStyledTextCtrl* ctrl);

	/**
	 * set the colors for all lexer styles
	 */
	void SetLexerStyles(wxStyledTextCtrl* ctrl, std::vector<mvceditor::StylePreferenceClass>& styles);

	/**
	 * Set the font, EOL, tab options of the source control
	 * Set generic defaults for plain text editing.
	 */
	void SetCodeControlOptions(wxStyledTextCtrl* ctrl, std::vector<mvceditor::StylePreferenceClass>& styles);

	/**
	 * Set the PHP syntax highlight options. Note that since PHP is embedded the PHP options will be suitable for
	 * HTML and Javascript editing as well.
	 */
	void SetPhpOptions(wxStyledTextCtrl* ctrl);

	/**
	 * Set the SQL highlight options of the source control
	 */
	void SetSqlOptions(wxStyledTextCtrl* ctrl);

	/**
	 * Set the CSS highlight options of the source control
	 */
	void SetCssOptions(wxStyledTextCtrl* ctrl);

	/**
	 * Set the JS highlight options of the source control
	 */
	void SetJsOptions(wxStyledTextCtrl* ctrl);

	/**
	 * Set the font settings for plain text documents.
	 */
	void SetPlainTextOptions(wxStyledTextCtrl* ctrl);

	DECLARE_EVENT_TABLE()
};

}

#endif // __MVCEDITOR_SYNTAXHIGHLIGHTFEATURECLASS_H__
