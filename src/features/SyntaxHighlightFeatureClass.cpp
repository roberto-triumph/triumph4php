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
#include <features/SyntaxHighlightFeatureClass.h>
#include <code_control/DocumentClass.h>
#include <Triumph.h>
#include <globals/CodeControlOptionsClass.h>
#include <code_control/CodeControlStyles.h>
#include <globals/Assets.h>

t4p::SyntaxHighlightFeatureClass::SyntaxHighlightFeatureClass(t4p::AppClass& app)
	: FeatureClass(app) {
}

void t4p::SyntaxHighlightFeatureClass::OnFileNew(t4p::CodeControlEventClass& event) {
	ApplyPreferences(event.GetCodeControl());
}

void t4p::SyntaxHighlightFeatureClass::OnFileOpen(t4p::CodeControlEventClass& event) {
	ApplyPreferences(event.GetCodeControl());
}

void t4p::SyntaxHighlightFeatureClass::SetPhpOptions(wxStyledTextCtrl* ctrl) {

	// set the lexer before setting the keywords
	ctrl->SetLexer(wxSTC_LEX_HTML);

	// 7 = as per scintilla docs, HTML lexer uses 7 bits for styles
	ctrl->SetStyleBits(7);

	// Some languages, such as HTML may contain embedded languages, VBScript
	// and JavaScript are common for HTML. For HTML, key word set 0 is for HTML,
	// 1 is for JavaScript and 2 is for VBScript, 3 is for Python, 4 is for PHP
	// and 5 is for SGML and DTD keywords
	t4p::PhpDocumentClass doc(&App.Globals);
	ctrl->SetKeyWords(0, doc.GetHtmlKeywords());
	ctrl->SetKeyWords(1, doc.GetJavascriptKeywords());
	ctrl->SetKeyWords(4, doc.GetPhpKeywords());

	ctrl->AutoCompStops(wxT("!@#$%^&*()_+-=[]{}|;'\",./<?"));
	ctrl->AutoCompSetSeparator('\n');
	ctrl->AutoCompSetChooseSingle(true);
	ctrl->AutoCompSetFillUps(wxT("(["));
	ctrl->AutoCompSetIgnoreCase(true);

	// need to add the namespace operator here, it was the only way i could get the
	// ctrl->AutoCompletion to workwith namespaces.
	ctrl->SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$\\"));

	ctrl->SetMarginType(CODE_CONTROL_LINT_RESULT_MARGIN, wxSTC_MARGIN_SYMBOL);
	ctrl->SetMarginWidth(CODE_CONTROL_LINT_RESULT_MARGIN, 16);
	ctrl->SetMarginSensitive(CODE_CONTROL_LINT_RESULT_MARGIN, false);
	ctrl->SetMarginMask(CODE_CONTROL_LINT_RESULT_MARGIN, ~wxSTC_MASK_FOLDERS);
	ctrl->MarkerDefine(CODE_CONTROL_LINT_RESULT_MARGIN, wxSTC_MARK_ARROW, *wxRED, *wxRED);

	// the annotation styles
	ctrl->StyleSetForeground(CODE_CONTROL_STYLE_PHP_LINT_ANNOTATION, wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
	ctrl->StyleSetBackground(CODE_CONTROL_STYLE_PHP_LINT_ANNOTATION, wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
	ctrl->StyleSetBold(CODE_CONTROL_STYLE_PHP_LINT_ANNOTATION, true);
	ctrl->StyleSetItalic(CODE_CONTROL_STYLE_PHP_LINT_ANNOTATION, true);

	// the lint error ctrl->Marker styles
	ctrl->IndicatorSetStyle(CODE_CONTROL_INDICATOR_PHP_LINT, wxSTC_INDIC_SQUIGGLE);
	ctrl->IndicatorSetForeground(CODE_CONTROL_INDICATOR_PHP_LINT, *wxRED);
}

void t4p::SyntaxHighlightFeatureClass::SetSqlOptions(wxStyledTextCtrl* ctrl) {
	ctrl->SetLexer(wxSTC_LEX_SQL);

	// 5 = default as per scintilla docs. set it because it may have been set by SetPhpOptions()
	ctrl->SetStyleBits(5);

	t4p::DatabaseTagClass emptyTag;
	t4p::SqlDocumentClass doc(&App.Globals, emptyTag);
	ctrl->SetKeyWords(0, doc.GetMySqlKeywords());
	ctrl->SetKeyWords(1, wxT(""));
	ctrl->SetKeyWords(2, wxT(""));
	ctrl->SetKeyWords(3, wxT(""));
	ctrl->SetKeyWords(4, wxT(""));

	ctrl->AutoCompStops(wxT("!@#$%^&*()_+-=[]\\{}|;'\",/?`"));
	ctrl->AutoCompSetSeparator(' ');
	ctrl->AutoCompSetChooseSingle(true);
	ctrl->AutoCompSetIgnoreCase(true);
	ctrl->AutoCompSetFillUps(wxT("(["));
	ctrl->SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"));
}

void t4p::SyntaxHighlightFeatureClass::SetCssOptions(wxStyledTextCtrl* ctrl) {
	ctrl->SetLexer(wxSTC_LEX_CSS);

	// 5 = default as per scintilla docs. set it because it may have been set by SetPhpOptions()
	ctrl->SetStyleBits(5);

	// got this by looking at LexCSS.cxx (bottom of the file)
	// keywords 0 => CSS 1 keywords
	// keywords 1 => Pseudo classes
	// keywords 2 => CSS 2 keywords but we will pass all keywords in 0
	t4p::CssDocumentClass doc;
	ctrl->SetKeyWords(0,  doc.GetCssKeywords());
	ctrl->SetKeyWords(1,  doc.GetCssPseudoClasses());
	ctrl->SetKeyWords(2, wxT(""));

	ctrl->AutoCompStops(wxT("!@#$%^&*()_+-=[]\\{}|;'\",/?`"));
	ctrl->AutoCompSetSeparator(' ');
	ctrl->AutoCompSetIgnoreCase(true);
	ctrl->AutoCompSetChooseSingle(true);
	ctrl->AutoCompSetFillUps(wxT("([:"));
	ctrl->SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"));
}

void t4p::SyntaxHighlightFeatureClass::SetJsOptions(wxStyledTextCtrl* ctrl) {

	// the CPP lexer is used to handle javascript
	ctrl->SetLexer(wxSTC_LEX_CPP);

	// 5 = default as per scintilla docs. set it because it may have been set by SetPhpOptions()
	ctrl->SetStyleBits(5);

	// got this by looking at LexCPP.cxx
	// keywords 0 => Primary keywords and identifiers
	// keywords 1 => Secondary keywords and identifiers
	// keywords 2 => Documentation comment keywords
	// keywords 3 => Global classes and typedefs
	// keywords 4 => Preprocessor definitions

	t4p::JsDocumentClass doc;
	ctrl->SetKeyWords(0, doc.GetJsKeywords());
	ctrl->SetKeyWords(1, wxT(""));
	ctrl->SetKeyWords(2, wxT(""));

	ctrl->AutoCompStops(wxT("!@#$%^&*()_+-=[]\\{}|;'\",/?`"));
	ctrl->AutoCompSetSeparator(' ');
	ctrl->AutoCompSetIgnoreCase(true);
	ctrl->AutoCompSetChooseSingle(true);
	ctrl->AutoCompSetFillUps(wxT("([:"));
	ctrl->SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"));
}

void t4p::SyntaxHighlightFeatureClass::SetPlainTextOptions(wxStyledTextCtrl* ctrl) {

	ctrl->SetLexer(wxSTC_LEX_NULL);

	// 5 = default as per scintilla docs. set it because it may have been set by SetPhpOptions()
	ctrl->SetStyleBits(5);
	ctrl->SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"));

	ctrl->SetMarginType(CODE_CONTROL_LINT_RESULT_MARGIN, wxSTC_MARGIN_SYMBOL);
	ctrl->SetMarginWidth(CODE_CONTROL_LINT_RESULT_MARGIN, 16);
	ctrl->SetMarginSensitive(CODE_CONTROL_LINT_RESULT_MARGIN, false);
	ctrl->SetMarginMask(CODE_CONTROL_LINT_RESULT_MARGIN, ~wxSTC_MASK_FOLDERS);
	ctrl->MarkerDefine(CODE_CONTROL_LINT_RESULT_MARGIN, wxSTC_MARK_ARROW, *wxRED, *wxRED);
}

void t4p::SyntaxHighlightFeatureClass::SetCodeControlOptions(wxStyledTextCtrl* ctrl,
        std::vector<t4p::StylePreferenceClass>& styles) {
	if (App.Preferences.CodeControlOptions.IndentUsingTabs) {
		ctrl->SetUseTabs(true);
		ctrl->SetTabWidth(App.Preferences.CodeControlOptions.TabWidth);
		ctrl->SetIndent(0);
		ctrl->SetTabIndents(true);
		ctrl->SetBackSpaceUnIndents(true);
	}
	else {
		ctrl->SetUseTabs(false);
		ctrl->SetTabWidth(App.Preferences.CodeControlOptions.SpacesPerIndent);
		ctrl->SetIndent(App.Preferences.CodeControlOptions.SpacesPerIndent);
		ctrl->SetTabIndents(false);
		ctrl->SetBackSpaceUnIndents(false);
	}
	if (App.Preferences.CodeControlOptions.RightMargin > 0) {
		ctrl->SetEdgeMode(wxSTC_EDGE_LINE);
		ctrl->SetEdgeColumn(App.Preferences.CodeControlOptions.RightMargin);
	}
	else {
		ctrl->SetEdgeMode(wxSTC_EDGE_NONE);
	}
	ctrl->SetIndentationGuides(App.Preferences.CodeControlOptions.EnableIndentationGuides);
	ctrl->SetEOLMode(App.Preferences.CodeControlOptions.LineEndingMode);
	ctrl->SetViewEOL(App.Preferences.CodeControlOptions.EnableLineEndings);

	// caret, line, selection, margin colors
	for (size_t i = 0; i < styles.size(); ++i) {
		t4p::StylePreferenceClass pref = styles[i];
		int style = pref.StcStyle;
		switch (style) {
			case CodeControlOptionsClass::T4P_STYLE_CARET:
				ctrl->SetCaretForeground(pref.Color);
				break;
			case CodeControlOptionsClass::T4P_STYLE_CARET_LINE:
				ctrl->SetCaretLineVisible(true);
				ctrl->SetCaretLineBackground(pref.BackgroundColor);
				break;
			case CodeControlOptionsClass::T4P_STYLE_SELECTION:
				ctrl->SetSelForeground(true, pref.Color);
				ctrl->SetSelBackground(true, pref.BackgroundColor);
				break;
			case CodeControlOptionsClass::T4P_STYLE_CODE_FOLDING:
				if (App.Preferences.CodeControlOptions.EnableCodeFolding) {
					ctrl->SetFoldMarginColour(true, pref.BackgroundColor);
					ctrl->SetFoldMarginHiColour(true, pref.BackgroundColor);
					ctrl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS, pref.BackgroundColor, pref.Color);
					ctrl->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS, pref.BackgroundColor, pref.Color);
					ctrl->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE, pref.BackgroundColor, pref.Color);
					ctrl->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER, pref.BackgroundColor, pref.Color);
					ctrl->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUSCONNECTED, pref.BackgroundColor, pref.Color);
					ctrl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, pref.BackgroundColor, pref.Color);
					ctrl->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER, pref.BackgroundColor, pref.Color);
				}
			case CodeControlOptionsClass::T4P_STYLE_RIGHT_MARGIN:
				if (App.Preferences.CodeControlOptions.RightMargin > 0) {
					ctrl->SetEdgeColour(pref.Color);
				}
				break;
			case t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT:
				// since we need to share one indicator with the matching word highlight
				// and the parse errors indicators; we will set this setting when the
				// user initiates the matching word feature
				break;
		}
	}
	
	
	// set the search hit margin; we want this marker to be available to
	// all file types
	ctrl->MarkerDefineBitmap(CODE_CONTROL_SEARCH_HIT_GOOD_MARKER, SearchHitGoodBitmap); 
	ctrl->MarkerDefineBitmap(CODE_CONTROL_SEARCH_HIT_BAD_MARKER, SearchHitBadBitmap);
	SetLexerStyles(ctrl, styles);
}

void t4p::SyntaxHighlightFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	t4p::NotebookClass* notebook = GetNotebook();
	for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
		t4p::CodeControlClass* ctrl = notebook->GetCodeControl(i);
		ApplyPreferences(ctrl);
	}
}

void t4p::SyntaxHighlightFeatureClass::ApplyPreferences(t4p::CodeControlClass* ctrl) {
	if (App.Preferences.CodeControlOptions.EnableWordWrap) {
		ctrl->SetWrapMode(wxSTC_WRAP_WORD);
		ctrl->SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_START);
	}
	else {
		ctrl->SetWrapMode(wxSTC_WRAP_NONE);
	}

	if (t4p::CodeControlClass::PHP == ctrl->GetDocumentMode()) {
		SetCodeControlOptions(ctrl, App.Preferences.CodeControlOptions.PhpStyles);
		SetPhpOptions(ctrl);
	}
	else if (t4p::CodeControlClass::CSS == ctrl->GetDocumentMode()) {
		SetCodeControlOptions(ctrl, App.Preferences.CodeControlOptions.CssStyles);
		SetCssOptions(ctrl);
	}
	else if (t4p::CodeControlClass::SQL == ctrl->GetDocumentMode()) {
		SetCodeControlOptions(ctrl, App.Preferences.CodeControlOptions.SqlStyles);
		SetSqlOptions(ctrl);
	}
	else if (t4p::CodeControlClass::JS == ctrl->GetDocumentMode()) {
		SetCodeControlOptions(ctrl, App.Preferences.CodeControlOptions.JsStyles);
		SetJsOptions(ctrl);
	}
	else if (t4p::CodeControlClass::CONFIG == ctrl->GetDocumentMode()) {
		SetCodeControlOptions(ctrl, App.Preferences.CodeControlOptions.ConfigStyles);
		SetPlainTextOptions(ctrl);
		ctrl->SetLexer(wxSTC_LEX_CONF);
	}
	else if (t4p::CodeControlClass::CRONTAB == ctrl->GetDocumentMode()) {
		SetCodeControlOptions(ctrl, App.Preferences.CodeControlOptions.CrontabStyles);
		SetPlainTextOptions(ctrl);
		ctrl->SetLexer(wxSTC_LEX_NNCRONTAB);
	}
	else if (t4p::CodeControlClass::YAML == ctrl->GetDocumentMode()) {
		SetCodeControlOptions(ctrl, App.Preferences.CodeControlOptions.YamlStyles);
		SetPlainTextOptions(ctrl);

		// yaml override; never use tabs for yaml editing since yaml requires spaces
		ctrl->SetUseTabs(false);
		ctrl->SetLexer(wxSTC_LEX_YAML);
	}
	else if (t4p::CodeControlClass::XML == ctrl->GetDocumentMode()) {
		SetCodeControlOptions(ctrl, App.Preferences.CodeControlOptions.PhpStyles);
		SetPlainTextOptions(ctrl);
		ctrl->SetLexer(wxSTC_LEX_HTML);
	}
	else if (t4p::CodeControlClass::RUBY == ctrl->GetDocumentMode()) {
		SetCodeControlOptions(ctrl, App.Preferences.CodeControlOptions.RubyStyles);
		SetPlainTextOptions(ctrl);
		ctrl->SetLexer(wxSTC_LEX_RUBY);
	}
	else if (t4p::CodeControlClass::LUA == ctrl->GetDocumentMode()) {
		SetCodeControlOptions(ctrl, App.Preferences.CodeControlOptions.LuaStyles);
		SetPlainTextOptions(ctrl);
		ctrl->SetLexer(wxSTC_LEX_LUA);
	}
	else if (t4p::CodeControlClass::MARKDOWN == ctrl->GetDocumentMode()) {
		SetCodeControlOptions(ctrl, App.Preferences.CodeControlOptions.MarkdownStyles);
		SetPlainTextOptions(ctrl);
		ctrl->SetLexer(wxSTC_LEX_MARKDOWN);
	}
	else if (t4p::CodeControlClass::BASH == ctrl->GetDocumentMode()) {
		SetCodeControlOptions(ctrl, App.Preferences.CodeControlOptions.BashStyles);
		SetPlainTextOptions(ctrl);
		ctrl->SetLexer(wxSTC_LEX_BASH);
	}
	else if (t4p::CodeControlClass::DIFF == ctrl->GetDocumentMode()) {
		SetCodeControlOptions(ctrl, App.Preferences.CodeControlOptions.DiffStyles);
		SetPlainTextOptions(ctrl);
		ctrl->SetLexer(wxSTC_LEX_DIFF);
	}
	else {
		SetPlainTextOptions(ctrl);

		// plain text files don't have a lexer, but we still want to
		// set a default background and foreground color
		SetCodeControlOptions(ctrl, App.Preferences.CodeControlOptions.PhpStyles);
	}

	// in wxWidgets 2.9.5, need to set margin after setting the lexer
	// otherwise code folding does not work
	SetMargin(ctrl);
	ctrl->Colourise(0, -1);
}


void t4p::SyntaxHighlightFeatureClass::SetMargin(wxStyledTextCtrl* ctrl) {
	if (App.Preferences.CodeControlOptions.EnableLineNumbers) {
		ctrl->SetMarginType(CodeControlOptionsClass::MARGIN_LINE_NUMBER, wxSTC_MARGIN_NUMBER);
		ctrl->SetMarginWidth(CodeControlOptionsClass::MARGIN_LINE_NUMBER, ctrl->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("_99999")));
	}
	else {
		ctrl->SetMarginWidth(CodeControlOptionsClass::MARGIN_LINE_NUMBER, 0);
	}
	if (App.Preferences.CodeControlOptions.EnableCodeFolding) {
		ctrl->SetProperty(wxT("fold"), wxT("1"));
		ctrl->SetProperty(wxT("fold.comment"), wxT("1"));
		ctrl->SetProperty(wxT("fold.html"), wxT("1"));
		ctrl->SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
		ctrl->SetMarginType(CodeControlOptionsClass::MARGIN_CODE_FOLDING, wxSTC_MARGIN_SYMBOL);
		ctrl->SetMarginWidth(CodeControlOptionsClass::MARGIN_CODE_FOLDING, 16);
		ctrl->SetMarginSensitive(CodeControlOptionsClass::MARGIN_CODE_FOLDING, true);
		ctrl->SetMarginMask(CodeControlOptionsClass::MARGIN_CODE_FOLDING, wxSTC_MASK_FOLDERS);
	}
	else {
		ctrl->SetProperty(wxT("fold"), wxT("0"));
		ctrl->SetProperty(wxT("fold.comment"), wxT("0"));
		ctrl->SetProperty(wxT("fold.html"), wxT("0"));
		ctrl->SetFoldFlags(0);
		ctrl->SetMarginType(CodeControlOptionsClass::MARGIN_CODE_FOLDING, wxSTC_MARGIN_SYMBOL);
		ctrl->SetMarginWidth(CodeControlOptionsClass::MARGIN_CODE_FOLDING, 0);
		ctrl->SetMarginSensitive(CodeControlOptionsClass::MARGIN_CODE_FOLDING, false);
	}
}

void t4p::SyntaxHighlightFeatureClass::SetLexerStyles(wxStyledTextCtrl* ctrl,
        std::vector<t4p::StylePreferenceClass>& styles) {

	t4p::StylePreferenceClass pref = App.Preferences.CodeControlOptions.FindByStcStyle(
	        App.Preferences.CodeControlOptions.PhpStyles,
	        wxSTC_HPHP_DEFAULT
	                                       );

	// use the PHP default settings as the catch-all for settings not yet exposed
	// so the user sees a uniform style.
	ctrl->StyleSetFont(wxSTC_STYLE_DEFAULT, pref.Font);
	ctrl->StyleSetForeground(wxSTC_STYLE_DEFAULT, pref.Color);
	ctrl->StyleSetBackground(wxSTC_STYLE_DEFAULT, pref.BackgroundColor);
	ctrl->StyleSetBold(wxSTC_STYLE_DEFAULT, pref.IsBold);
	ctrl->StyleSetItalic(wxSTC_STYLE_DEFAULT, pref.IsItalic);

	for (size_t i = 0; i < styles.size(); ++i) {
		t4p::StylePreferenceClass pref = styles[i];
		int style = pref.StcStyle;
		ctrl->StyleSetFont(style, pref.Font);
		ctrl->StyleSetForeground(style, pref.Color);
		ctrl->StyleSetBackground(style, pref.BackgroundColor);
		ctrl->StyleSetBold(style, pref.IsBold);
		ctrl->StyleSetItalic(style, pref.IsItalic);
	}

	// the found match indicator style
	pref = App.Preferences.CodeControlOptions.FindByStcStyle(
	           styles,
	           t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT
	       );
	ctrl->IndicatorSetStyle(CODE_CONTROL_INDICATOR_FIND,  wxSTC_INDIC_ROUNDBOX);
	ctrl->IndicatorSetForeground(CODE_CONTROL_INDICATOR_FIND, pref.Color);
}

void t4p::SyntaxHighlightFeatureClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	t4p::EditColorsPanelClass* panel = new t4p::EditColorsPanelClass(parent, *this);
	parent->AddPage(panel, _("Styles && Colors"));
}

void t4p::SyntaxHighlightFeatureClass::OnAppReady(wxCommandEvent& event) {
	
	// load the images once at startup
	SearchHitGoodBitmap = t4p::AutoCompleteImageAsset(wxT("magnifier"));
	SearchHitBadBitmap = t4p::AutoCompleteImageAsset(wxT("magnifier-exclamation"));
}

t4p::EditColorsPanelClass::EditColorsPanelClass(wxWindow* parent, t4p::SyntaxHighlightFeatureClass& feature)
: SyntaxHighlightPanelGeneratedClass(parent)
, CodeControlOptions(feature.App.Preferences.CodeControlOptions)
, EditedCodeControlOptions(feature.App.Preferences.CodeControlOptions)
, CodeCtrl(NULL) 
, Globals() 
, EventSink() 
, Feature(feature) {
	for (size_t i = 0; i < CodeControlOptions.PhpStyles.size(); ++i) {
		wxString name = wxString::FromAscii(CodeControlOptions.PhpStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &CodeControlOptions.PhpStyles[i]);
	}
	for (size_t i = 0; i < CodeControlOptions.SqlStyles.size(); ++i) {
		wxString name = wxString::FromAscii(CodeControlOptions.SqlStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &CodeControlOptions.SqlStyles[i]);
	}
	for (size_t i = 0; i < CodeControlOptions.CssStyles.size(); ++i) {
		wxString name = wxString::FromAscii(CodeControlOptions.CssStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &CodeControlOptions.CssStyles[i]);
	}
	for (size_t i = 0; i < CodeControlOptions.JsStyles.size(); ++i) {
		wxString name = wxString::FromAscii(CodeControlOptions.JsStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &CodeControlOptions.JsStyles[i]);
	}
	for (size_t i = 0; i < CodeControlOptions.ConfigStyles.size(); ++i) {
		wxString name = wxString::FromAscii(CodeControlOptions.ConfigStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &CodeControlOptions.ConfigStyles[i]);
	}
	for (size_t i = 0; i < CodeControlOptions.CrontabStyles.size(); ++i) {
		wxString name = wxString::FromAscii(CodeControlOptions.CrontabStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &CodeControlOptions.CrontabStyles[i]);
	}
	for (size_t i = 0; i < CodeControlOptions.YamlStyles.size(); ++i) {
		wxString name = wxString::FromAscii(CodeControlOptions.YamlStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &CodeControlOptions.YamlStyles[i]);
	}
	for (size_t i = 0; i < CodeControlOptions.RubyStyles.size(); ++i) {
		wxString name = wxString::FromAscii(CodeControlOptions.RubyStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &CodeControlOptions.RubyStyles[i]);
	}
	for (size_t i = 0; i < CodeControlOptions.LuaStyles.size(); ++i) {
		wxString name = wxString::FromAscii(CodeControlOptions.LuaStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &CodeControlOptions.LuaStyles[i]);
	}
	for (size_t i = 0; i < CodeControlOptions.MarkdownStyles.size(); ++i) {
		wxString name = wxString::FromAscii(CodeControlOptions.MarkdownStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &CodeControlOptions.MarkdownStyles[i]);
	}
	for (size_t i = 0; i < CodeControlOptions.BashStyles.size(); ++i) {
		wxString name = wxString::FromAscii(CodeControlOptions.BashStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &CodeControlOptions.BashStyles[i]);
	}
	for (size_t i = 0; i < CodeControlOptions.DiffStyles.size(); ++i) {
		wxString name = wxString::FromAscii(CodeControlOptions.DiffStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &CodeControlOptions.DiffStyles[i]);
	}
	Styles->Select(0);
	t4p::StylePreferenceClass firstPref = EditedCodeControlOptions.PhpStyles[0];
	Font->SetSelectedFont(firstPref.Font);
	ForegroundColor->SetColour(firstPref.Color);
	BackgroundColor->SetColour(firstPref.BackgroundColor);
	Bold->SetValue(firstPref.IsBold);
	Italic->SetValue(firstPref.IsItalic);

	Theme->Clear();
	wxArrayString themes = t4p::CodeControlStylesGetThemes();
	themes.Sort();
	Theme->Append(themes);
	PreviewNotebook->SetWindowStyle(wxAUI_NB_BOTTOM);
	AddPreviews();
}

void t4p::EditColorsPanelClass::AddPreviews() {
	CodeCtrl = new t4p::CodeControlClass(this,
		EditedCodeControlOptions,
		&Globals, EventSink, wxID_ANY);
	CodeCtrl->SetDocumentMode(t4p::CodeControlClass::PHP);
	PreviewNotebook->AddPage(CodeCtrl, _("PHP"));
	wxString txt = t4p::CharToWx(
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
	Feature.ApplyPreferences(CodeCtrl);
	
	t4p::CodeControlClass* sql = new t4p::CodeControlClass(this,
		EditedCodeControlOptions,
		&Globals, EventSink, wxID_ANY);
	sql->SetDocumentMode(t4p::CodeControlClass::SQL);
	txt =  t4p::CharToWx(
		" -- table to store users\n"
		"CREATE TABLE my_users(\n"
		"	userId INT(11) NOT NULL PRIMARY KEY AUTO_INCREMENT,\n"
		"	name VARCHAR(255) NOT NULL DEFAULT 'guest',\n"
		"	createdAt DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP\n"
		");"
	);
	sql->SetText(txt);
	PreviewNotebook->AddPage(sql, _("SQL"));
	Feature.ApplyPreferences(sql);
	
	t4p::CodeControlClass* css = new t4p::CodeControlClass(this,
		EditedCodeControlOptions,
		&Globals, EventSink, wxID_ANY);
	css->SetDocumentMode(t4p::CodeControlClass::CSS);
	txt =  t4p::CharToWx(
		" /* render users nicely */\n"
		".user {\n"
		"	font-weight: bold;\n"
		"	font-family: 'Arial';\n"
		"}"
	);
	css->SetText(txt);
	PreviewNotebook->AddPage(css, _("CSS"));
	Feature.ApplyPreferences(css);
	
	t4p::CodeControlClass* js = new t4p::CodeControlClass(this,
		EditedCodeControlOptions,
		&Globals, EventSink, wxID_ANY);
	js->SetDocumentMode(t4p::CodeControlClass::JS);
	txt =  t4p::CharToWx(
		" /* represents a logged-in user */\n"
		"function myFunction() {\n"
		"	var x = \"\"; \n"
		"	var time = new Date().getHours();\n"
		"	if (time < 20) {\n"
		"		x = \"Good day\";\n"
		"	}\n"
		"	document.getElementById(\"demo\").innerHTML = x;\n"
		"}\n"
	);
	js->SetText(txt);
	PreviewNotebook->AddPage(js, _("Javascript"));
	Feature.ApplyPreferences(js);
}

bool t4p::EditColorsPanelClass::TransferDataFromWindow() {
	CodeControlOptions = EditedCodeControlOptions;
	return true;
}

void t4p::EditColorsPanelClass::OnListBox(wxCommandEvent& event) {
	int selected = event.GetSelection();
	if (selected < 0) {
		return;
	}
	t4p::StylePreferenceClass* pref = (t4p::StylePreferenceClass*)Styles->GetClientData(selected);
	if (pref) {
		Font->SetSelectedFont(pref->Font);
		ForegroundColor->SetColour(pref->Color);
		BackgroundColor->SetColour(pref->BackgroundColor);
		Bold->SetValue(pref->IsBold);
		Italic->SetValue(pref->IsItalic);
		int style = pref->StcStyle;
		switch (style) {
			case t4p::CodeControlOptionsClass::T4P_STYLE_CARET:
			case t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE:
			case t4p::CodeControlOptionsClass::T4P_STYLE_SELECTION:
			case t4p::CodeControlOptionsClass::T4P_STYLE_CODE_FOLDING:
			case t4p::CodeControlOptionsClass::T4P_STYLE_RIGHT_MARGIN:
			case wxSTC_STYLE_INDENTGUIDE:
				Font->Enable(false);
				Bold->Enable(false);
				Italic->Enable(false);
				BackgroundColor->Enable(true);
				break;
			case t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT:
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


void t4p::EditColorsPanelClass::OnCheck(wxCommandEvent& event) {
	int selected = Styles->GetSelection();
	t4p::StylePreferenceClass* pref = (t4p::StylePreferenceClass*)Styles->GetClientData(selected);
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

void t4p::EditColorsPanelClass::OnColorChanged(wxColourPickerEvent& event) {
	int selected = Styles->GetSelection();
	t4p::StylePreferenceClass* pref = (t4p::StylePreferenceClass*)Styles->GetClientData(selected);
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

void t4p::EditColorsPanelClass::OnFontChanged(wxFontPickerEvent& event) {
	int selected = Styles->GetSelection();
	t4p::StylePreferenceClass* pref = (t4p::StylePreferenceClass*)Styles->GetClientData(selected);
	if (pref) {
		wxFont font = event.GetFont();
		pref->Font = font;
		CodeCtrl->ApplyPreferences();
	}
}

void t4p::EditColorsPanelClass::OnThemeChoice(wxCommandEvent& event) {
	wxCommandEvent listBoxEvent(wxEVT_COMMAND_LISTBOX_SELECTED, wxID_ANY);
	int sel = Styles->GetSelection();
	if (sel < 0) {
		sel = 0;
	}
	listBoxEvent.SetInt(sel);

	t4p::CodeControlStylesSetTheme(EditedCodeControlOptions, Theme->GetStringSelection());
	wxPostEvent(this, listBoxEvent);

	CodeCtrl->ApplyPreferences();
}

BEGIN_EVENT_TABLE(t4p::SyntaxHighlightFeatureClass, t4p::FeatureClass)
	EVT_APP_FILE_NEW(t4p::SyntaxHighlightFeatureClass::OnFileNew)
	EVT_APP_FILE_OPEN(t4p::SyntaxHighlightFeatureClass::OnFileOpen)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::SyntaxHighlightFeatureClass::OnAppReady)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::SyntaxHighlightFeatureClass::OnPreferencesSaved)
END_EVENT_TABLE()

