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
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <main_frame/EditColorsPanelClass.h>
#include <code_control/CodeControlStyles.h>
#include <code_control/CodeControlClass.h>
#include <wx/stc/stc.h>

mvceditor::EditColorsPanelClass::EditColorsPanelClass(wxWindow* parent, mvceditor::CodeControlOptionsClass& options)
: EditColorsPanelGeneratedClass(parent)
, CodeControlOptions(options)
, EditedCodeControlOptions(options)
, CodeCtrl(NULL) 
, Globals() 
, EventSink() {
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		wxString name = wxString::FromAscii(options.PhpStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &options.PhpStyles[i]);
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		wxString name = wxString::FromAscii(options.JsStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &options.JsStyles[i]);
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		wxString name = wxString::FromAscii(options.SqlStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &options.SqlStyles[i]);
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		wxString name = wxString::FromAscii(options.CssStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &options.CssStyles[i]);
	}
	Styles->Select(0);
	mvceditor::StylePreferenceClass firstPref = EditedCodeControlOptions.PhpStyles[0];
	Font->SetSelectedFont(firstPref.Font);
	ForegroundColor->SetColour(firstPref.Color);
	BackgroundColor->SetColour(firstPref.BackgroundColor);
	Bold->SetValue(firstPref.IsBold);
	Italic->SetValue(firstPref.IsItalic);

	Theme->Clear();
	wxArrayString themes = mvceditor::CodeControlStylesGetThemes();
	themes.Sort();
	Theme->Append(themes);

	CodeCtrl = new mvceditor::CodeControlClass(this,
		EditedCodeControlOptions,
		&Globals, EventSink, wxID_ANY);
	StyleEditSizer->Add(CodeCtrl, 1, wxEXPAND|wxLEFT, 5);
	this->Layout();
	CodeCtrl->SetDocumentMode(mvceditor::CodeControlClass::PHP);

	wxString txt = mvceditor::CharToWx(
		"<?php\n"
		"/**\n"
		" * this is a class\n"
		" */\n"
		"class MyClass {\n"
		"\n"
		"	/**\n"
		"	 * @var string\n"
		"	 */\n"
		"	private $name;\n"
		"\n"
		"	public function __construct($name) {\n"
		"		$this->name = $name;\n"
		"		if (empty($this->name)) {\n"
		"			throw new Exception(\"Name Cannot be empty\");\n"
		"		}\n"
		"	}\n"
		"}\n"
		"\n"
	);
	CodeCtrl->SetText(txt);
}

bool mvceditor::EditColorsPanelClass::TransferDataFromWindow() {
	CodeControlOptions = EditedCodeControlOptions;
	return true;
}

void mvceditor::EditColorsPanelClass::OnListBox(wxCommandEvent& event) {
	int selected = event.GetSelection();
	if (selected < 0) {
		return;
	}
	mvceditor::StylePreferenceClass* pref = (mvceditor::StylePreferenceClass*)Styles->GetClientData(selected);
	if (pref) {
		Font->SetSelectedFont(pref->Font);
		ForegroundColor->SetColour(pref->Color);
		BackgroundColor->SetColour(pref->BackgroundColor);
		Bold->SetValue(pref->IsBold);
		Italic->SetValue(pref->IsItalic);
		int style = pref->StcStyle;
		switch (style) {
			case mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET:
			case mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE:
			case mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION:
			case mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING:
			case mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN:
			case wxSTC_STYLE_INDENTGUIDE:
				Font->Enable(false);
				Bold->Enable(false);
				Italic->Enable(false);
				BackgroundColor->Enable(true);
				break;
			case mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT:
				Font->Enable(false);
				Bold->Enable(false);
				Italic->Enable(false);
				BackgroundColor->Enable(false);
				break;
			default:
				Font->Enable(true);
				Bold->Enable(true);
				Italic->Enable(true);
				BackgroundColor->Enable(true);
				break;
		}
	}
}


void mvceditor::EditColorsPanelClass::OnCheck(wxCommandEvent& event) {
	int selected = Styles->GetSelection();
	mvceditor::StylePreferenceClass* pref = (mvceditor::StylePreferenceClass*)Styles->GetClientData(selected);
	if (pref) {
		switch (event.GetId()) {
			case ID_BOLD:
				pref->IsBold = event.IsChecked();
				break;
			case ID_ITALIC:
				pref->IsItalic = event.IsChecked();
				break;
		}
	}
}

void mvceditor::EditColorsPanelClass::OnColorChanged(wxColourPickerEvent& event) {
	int selected = Styles->GetSelection();
	mvceditor::StylePreferenceClass* pref = (mvceditor::StylePreferenceClass*)Styles->GetClientData(selected);
	if (pref) {
		switch (event.GetId()) {
			case ID_FOREGROUND_COLOR:
				pref->Color = event.GetColour();
				CodeCtrl->ApplyPreferences();
				break;
			case ID_BACKGROUND_COLOR:
				pref->BackgroundColor = event.GetColour();
				CodeCtrl->ApplyPreferences();
				break;
		}
	}
}

void mvceditor::EditColorsPanelClass::OnFontChanged(wxFontPickerEvent& event) {
	int selected = Styles->GetSelection();
	mvceditor::StylePreferenceClass* pref = (mvceditor::StylePreferenceClass*)Styles->GetClientData(selected);
	if (pref) {
		wxFont font = event.GetFont();
		pref->Font = font;
		CodeCtrl->ApplyPreferences();
	}
}

void mvceditor::EditColorsPanelClass::OnThemeChoice(wxCommandEvent& event) {
	wxCommandEvent listBoxEvent(wxEVT_COMMAND_LISTBOX_SELECTED, wxID_ANY);
	int sel = Styles->GetSelection();
	if (sel < 0) {
		sel = 0;
	}
	listBoxEvent.SetInt(sel);

	mvceditor::CodeControlStylesSetTheme(EditedCodeControlOptions, Theme->GetStringSelection());
	wxPostEvent(this, listBoxEvent);

	CodeCtrl->ApplyPreferences();
}
