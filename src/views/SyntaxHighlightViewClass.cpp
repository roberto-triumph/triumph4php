/**
 * @copyright  2014 Roberto Perpuly
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
#include <views/SyntaxHighlightViewClass.h>
#include <features/SyntaxHighlightFeatureClass.h>
#include <code_control/CodeControlClass.h>
#include <Triumph.h>
#include <globals/CodeControlOptionsClass.h>
#include <code_control/CodeControlStyles.h>
#include <globals/Assets.h>
#include <language_php/Keywords.h>
#include <vector>

//------------------------------------------------------------------------
// setting the various wxStyledTextCtrl options
// wxStyledTextCtrl is super-configurable.  These methods will turn on
// some sensible defaults for plain-text, PHP, HTML, and SQL editing.
//------------------------------------------------------------------------

/**
 * set the margin look of the source control
 */
static void SetMargin(wxStyledTextCtrl* ctrl, t4p::CodeControlOptionsClass& options) {
	if (options.EnableLineNumbers) {
		ctrl->SetMarginType(t4p::CodeControlOptionsClass::MARGIN_LINE_NUMBER, wxSTC_MARGIN_NUMBER);
		ctrl->SetMarginWidth(t4p::CodeControlOptionsClass::MARGIN_LINE_NUMBER, ctrl->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("_99999")));
		ctrl->SetMarginMask(t4p::CodeControlOptionsClass::MARGIN_LINE_NUMBER, 0);
	} else {
		ctrl->SetMarginWidth(t4p::CodeControlOptionsClass::MARGIN_LINE_NUMBER, 0);
	}
	if (options.EnableCodeFolding) {
		ctrl->SetProperty(wxT("fold"), wxT("1"));
		ctrl->SetProperty(wxT("fold.comment"), wxT("1"));
		ctrl->SetProperty(wxT("fold.html"), wxT("1"));
		ctrl->SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
		ctrl->SetMarginType(t4p::CodeControlOptionsClass::MARGIN_CODE_FOLDING, wxSTC_MARGIN_SYMBOL);
		ctrl->SetMarginWidth(t4p::CodeControlOptionsClass::MARGIN_CODE_FOLDING, 16);
		ctrl->SetMarginSensitive(t4p::CodeControlOptionsClass::MARGIN_CODE_FOLDING, true);
		ctrl->SetMarginMask(t4p::CodeControlOptionsClass::MARGIN_CODE_FOLDING, wxSTC_MASK_FOLDERS);
	} else {
		ctrl->SetProperty(wxT("fold"), wxT("0"));
		ctrl->SetProperty(wxT("fold.comment"), wxT("0"));
		ctrl->SetProperty(wxT("fold.html"), wxT("0"));
		ctrl->SetFoldFlags(0);
		ctrl->SetMarginType(t4p::CodeControlOptionsClass::MARGIN_CODE_FOLDING, wxSTC_MARGIN_SYMBOL);
		ctrl->SetMarginWidth(t4p::CodeControlOptionsClass::MARGIN_CODE_FOLDING, 0);
		ctrl->SetMarginSensitive(t4p::CodeControlOptionsClass::MARGIN_CODE_FOLDING, false);
	}

	ctrl->SetMarginType(t4p::CodeControlOptionsClass::MARGIN_MARKERS, wxSTC_MARGIN_SYMBOL);
	ctrl->SetMarginWidth(t4p::CodeControlOptionsClass::MARGIN_MARKERS, 16);
	ctrl->SetMarginSensitive(t4p::CodeControlOptionsClass::MARGIN_MARKERS, true);

	// even though we dont use the lint margin on all languages, we do use
	// the search hit marker on all languages. we must set the margin mask
	// for all languages
	ctrl->SetMarginMask(t4p::CodeControlOptionsClass::MARGIN_MARKERS, ~wxSTC_MASK_FOLDERS);
}

/**
 * set the colors for all lexer styles
 */
static void SetLexerStyles(wxStyledTextCtrl* ctrl, std::vector<t4p::StylePreferenceClass>& styles, t4p::CodeControlOptionsClass& options) {
	t4p::StylePreferenceClass pref = options.FindByStcStyle(
        options.PhpStyles,
        wxSTC_HPHP_DEFAULT);

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
	pref = options.FindByStcStyle(
	   styles,
	   t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT);
	ctrl->IndicatorSetStyle(t4p::CODE_CONTROL_INDICATOR_FIND,  wxSTC_INDIC_ROUNDBOX);
	ctrl->IndicatorSetForeground(t4p::CODE_CONTROL_INDICATOR_FIND, pref.Color);
}

/**
 * Set the font, EOL, tab options of the source control
 * Set generic defaults for plain text editing.
 */
static void SetCodeControlOptions(wxStyledTextCtrl* ctrl, std::vector<t4p::StylePreferenceClass>& styles,
						   t4p::CodeControlOptionsClass& options, wxBitmap& searchHitGoodBitmap,
						   wxBitmap& searchHitBadBitmap, wxBitmap& bookmarkBitmap,
						   wxBitmap& executionLineBitmap, wxBitmap& breakpointBitmap) {
	// caret, line, selection, margin colors
	for (size_t i = 0; i < styles.size(); ++i) {
		t4p::StylePreferenceClass pref = styles[i];
		int style = pref.StcStyle;
		switch (style) {
			case t4p::CodeControlOptionsClass::T4P_STYLE_CARET:
				ctrl->SetCaretForeground(pref.Color);
				break;
			case t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE:
				ctrl->SetCaretLineVisible(true);
				ctrl->SetCaretLineBackground(pref.BackgroundColor);
				break;
			case t4p::CodeControlOptionsClass::T4P_STYLE_SELECTION:
				ctrl->SetSelForeground(true, pref.Color);
				ctrl->SetSelBackground(true, pref.BackgroundColor);
				break;
			case t4p::CodeControlOptionsClass::T4P_STYLE_CODE_FOLDING:
				if (options.EnableCodeFolding) {
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
			case t4p::CodeControlOptionsClass::T4P_STYLE_RIGHT_MARGIN:
				if (options.RightMargin > 0) {
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
	ctrl->MarkerDefineBitmap(t4p::CODE_CONTROL_SEARCH_HIT_GOOD_MARKER, searchHitGoodBitmap);
	ctrl->MarkerDefineBitmap(t4p::CODE_CONTROL_SEARCH_HIT_BAD_MARKER, searchHitBadBitmap);
	ctrl->MarkerDefineBitmap(t4p::CODE_CONTROL_BOOKMARK_MARKER, bookmarkBitmap);
	ctrl->MarkerDefine(t4p::CODE_CONTROL_LINT_RESULT_MARKER, wxSTC_MARK_ARROW, *wxRED, *wxRED);
	ctrl->MarkerDefineBitmap(t4p::CODE_CONTROL_EXECUTION_MARKER, executionLineBitmap);
	ctrl->MarkerDefineBitmap(t4p::CODE_CONTROL_BREAKPOINT_MARKER, breakpointBitmap);
	SetLexerStyles(ctrl, styles, options);
}

/**
 * Set the PHP syntax highlight options. Note that since PHP is embedded the PHP options will be suitable for
 * HTML and Javascript editing as well.
 */
static void SetPhpOptions(wxStyledTextCtrl* ctrl, t4p::CodeControlOptionsClass& options, t4p::GlobalsClass& globals) {
	// set the lexer before setting the keywords
	ctrl->SetLexer(wxSTC_LEX_HTML);

	// 7 = as per scintilla docs, HTML lexer uses 7 bits for styles
	ctrl->SetStyleBits(7);

	// Some languages, such as HTML may contain embedded languages, VBScript
	// and JavaScript are common for HTML. For HTML, key word set 0 is for HTML,
	// 1 is for JavaScript and 2 is for VBScript, 3 is for Python, 4 is for PHP
	// and 5 is for SGML and DTD keywords
	ctrl->SetKeyWords(0, t4p::KeywordsHtmlAll());
	ctrl->SetKeyWords(1, t4p::KEYWORDS_JAVASCRIPT);
	ctrl->SetKeyWords(4, t4p::KeywordsPhpAll(globals.Environment.Php.Version));

	ctrl->AutoCompStops(wxT("!@#$%^&*()_+-=[]{}|;'\",./<?"));
	ctrl->AutoCompSetSeparator('\n');
	ctrl->AutoCompSetChooseSingle(true);
	ctrl->AutoCompSetFillUps(wxT("(["));
	ctrl->AutoCompSetIgnoreCase(true);

	// need to add the namespace operator here, it was the only way i could get the
	// ctrl->AutoCompletion to workwith namespaces.
	ctrl->SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$\\"));

	// the annotation styles
	ctrl->StyleSetForeground(t4p::CODE_CONTROL_STYLE_PHP_LINT_ANNOTATION, wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
	ctrl->StyleSetBackground(t4p::CODE_CONTROL_STYLE_PHP_LINT_ANNOTATION, wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
	ctrl->StyleSetBold(t4p::CODE_CONTROL_STYLE_PHP_LINT_ANNOTATION, true);
	ctrl->StyleSetItalic(t4p::CODE_CONTROL_STYLE_PHP_LINT_ANNOTATION, true);

	// the lint error ctrl->Marker styles
	ctrl->IndicatorSetStyle(t4p::CODE_CONTROL_INDICATOR_PHP_LINT, wxSTC_INDIC_SQUIGGLE);
	ctrl->IndicatorSetForeground(t4p::CODE_CONTROL_INDICATOR_PHP_LINT, *wxRED);
}

/**
 * Set the SQL highlight options of the source control
 */
static void SetSqlOptions(wxStyledTextCtrl* ctrl, t4p::CodeControlOptionsClass& options, t4p::GlobalsClass& globals) {
	ctrl->SetLexer(wxSTC_LEX_SQL);

	// 5 = default as per scintilla docs. set it because it may have been set by SetPhpOptions()
	ctrl->SetStyleBits(5);

	ctrl->SetKeyWords(0, t4p::KEYWORDS_MYSQL);
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

/**
 * Set the CSS highlight options of the source control
 */
static void SetCssOptions(wxStyledTextCtrl* ctrl, t4p::CodeControlOptionsClass& options) {
	ctrl->SetLexer(wxSTC_LEX_CSS);

	// 5 = default as per scintilla docs. set it because it may have been set by SetPhpOptions()
	ctrl->SetStyleBits(5);

	// got this by looking at LexCSS.cxx (bottom of the file)
	// keywords 0 => CSS 1 keywords
	// keywords 1 => Pseudo classes
	// keywords 2 => CSS 2 keywords but we will pass all keywords in 0
	ctrl->SetKeyWords(0,  t4p::KEYWORDS_CSS);
	ctrl->SetKeyWords(1,  t4p::KEYWORDS_CSS_PSEUDOCLASSES);
	ctrl->SetKeyWords(2, wxT(""));

	ctrl->AutoCompStops(wxT("!@#$%^&*()_+-=[]\\{}|;'\",/?`"));
	ctrl->AutoCompSetSeparator(' ');
	ctrl->AutoCompSetIgnoreCase(true);
	ctrl->AutoCompSetChooseSingle(true);
	ctrl->AutoCompSetFillUps(wxT("([:"));
	ctrl->SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"));
}

/**
 * Set the JS highlight options of the source control
 */
static void SetJsOptions(wxStyledTextCtrl* ctrl, t4p::CodeControlOptionsClass& options) {
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

	ctrl->SetKeyWords(0, t4p::KEYWORDS_JAVASCRIPT);
	ctrl->SetKeyWords(1, wxT(""));
	ctrl->SetKeyWords(2, wxT(""));

	ctrl->AutoCompStops(wxT("!@#$%^&*()_+-=[]\\{}|;'\",/?`"));
	ctrl->AutoCompSetSeparator(' ');
	ctrl->AutoCompSetIgnoreCase(true);
	ctrl->AutoCompSetChooseSingle(true);
	ctrl->AutoCompSetFillUps(wxT("([:"));
	ctrl->SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"));
}

/**
 * Set the font settings for plain text documents.
 */
static void SetPlainTextOptions(wxStyledTextCtrl* ctrl, t4p::CodeControlOptionsClass& options) {
	ctrl->SetLexer(wxSTC_LEX_NULL);

	// 5 = default as per scintilla docs. set it because it may have been set by SetPhpOptions()
	ctrl->SetStyleBits(5);
	ctrl->SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"));
}

t4p::SyntaxHighlightViewClass::SyntaxHighlightViewClass(t4p::SyntaxHighlightFeatureClass& feature)
	: FeatureViewClass()
	, Feature(feature) {
}

void t4p::SyntaxHighlightViewClass::OnFileNew(t4p::CodeControlEventClass& event) {
	ApplyPreferences(event.GetCodeControl(), Feature.App.Preferences.CodeControlOptions);
}

void t4p::SyntaxHighlightViewClass::OnFileOpen(t4p::CodeControlEventClass& event) {
	LoadMarkerBitmaps();
	ApplyPreferences(event.GetCodeControl(), Feature.App.Preferences.CodeControlOptions);
}

void t4p::SyntaxHighlightViewClass::OnPreferencesSaved(wxCommandEvent& event) {
	std::vector<t4p::CodeControlClass*> ctrls = AllCodeControls();
	for (size_t i = 0; i < ctrls.size(); ++i) {
		ApplyPreferences(ctrls[i], Feature.App.Preferences.CodeControlOptions);
	}
}

void t4p::SyntaxHighlightViewClass::ApplyPreferences(t4p::CodeControlClass* ctrl, t4p::CodeControlOptionsClass& options) {
	if (t4p::FILE_TYPE_PHP == ctrl->GetFileType()) {
		SetCodeControlOptions(ctrl, options.PhpStyles, options, SearchHitGoodBitmap, SearchHitBadBitmap,
			BookmarkBitmap, ExecutionLineBitmap, BreakpointBitmap);
		SetPhpOptions(ctrl, options, Feature.App.Globals);
	} else if (t4p::FILE_TYPE_CSS == ctrl->GetFileType()) {
		SetCodeControlOptions(ctrl, options.CssStyles, options, SearchHitGoodBitmap, SearchHitBadBitmap,
			BookmarkBitmap, ExecutionLineBitmap, BreakpointBitmap);
		SetCssOptions(ctrl, options);
	} else if (t4p::FILE_TYPE_SQL == ctrl->GetFileType()) {
		SetCodeControlOptions(ctrl, options.SqlStyles, options, SearchHitGoodBitmap, SearchHitBadBitmap,
			BookmarkBitmap, ExecutionLineBitmap, BreakpointBitmap);
		SetSqlOptions(ctrl, options, Feature.App.Globals);
	} else if (t4p::FILE_TYPE_JS == ctrl->GetFileType()) {
		SetCodeControlOptions(ctrl, options.JsStyles, options, SearchHitGoodBitmap, SearchHitBadBitmap,
			BookmarkBitmap, ExecutionLineBitmap, BreakpointBitmap);
		SetJsOptions(ctrl, options);
	} else if (t4p::FILE_TYPE_CONFIG == ctrl->GetFileType()) {
		SetCodeControlOptions(ctrl, options.ConfigStyles, options, SearchHitGoodBitmap, SearchHitBadBitmap,
			BookmarkBitmap, ExecutionLineBitmap, BreakpointBitmap);
		SetPlainTextOptions(ctrl, options);
		ctrl->SetLexer(wxSTC_LEX_CONF);
	} else if (t4p::FILE_TYPE_CRONTAB == ctrl->GetFileType()) {
		SetCodeControlOptions(ctrl, options.CrontabStyles, options, SearchHitGoodBitmap, SearchHitBadBitmap,
			BookmarkBitmap, ExecutionLineBitmap, BreakpointBitmap);
		SetPlainTextOptions(ctrl, options);
		ctrl->SetLexer(wxSTC_LEX_NNCRONTAB);
	} else if (t4p::FILE_TYPE_YAML == ctrl->GetFileType()) {
		SetCodeControlOptions(ctrl, options.YamlStyles, options, SearchHitGoodBitmap, SearchHitBadBitmap,
			BookmarkBitmap, ExecutionLineBitmap, BreakpointBitmap);
		SetPlainTextOptions(ctrl, options);

		// yaml override; never use tabs for yaml editing since yaml requires spaces
		ctrl->SetUseTabs(false);
		ctrl->SetLexer(wxSTC_LEX_YAML);
	} else if (t4p::FILE_TYPE_XML == ctrl->GetFileType()) {
		SetCodeControlOptions(ctrl, options.PhpStyles, options, SearchHitGoodBitmap, SearchHitBadBitmap,
			BookmarkBitmap, ExecutionLineBitmap, BreakpointBitmap);
		SetPlainTextOptions(ctrl, options);
		ctrl->SetLexer(wxSTC_LEX_HTML);
	} else if (t4p::FILE_TYPE_RUBY == ctrl->GetFileType()) {
		SetCodeControlOptions(ctrl, options.RubyStyles, options, SearchHitGoodBitmap, SearchHitBadBitmap,
			BookmarkBitmap, ExecutionLineBitmap, BreakpointBitmap);
		SetPlainTextOptions(ctrl, options);
		ctrl->SetLexer(wxSTC_LEX_RUBY);
	} else if (t4p::FILE_TYPE_LUA == ctrl->GetFileType()) {
		SetCodeControlOptions(ctrl, options.LuaStyles, options, SearchHitGoodBitmap, SearchHitBadBitmap,
			BookmarkBitmap, ExecutionLineBitmap, BreakpointBitmap);
		SetPlainTextOptions(ctrl, options);
		ctrl->SetLexer(wxSTC_LEX_LUA);
	} else if (t4p::FILE_TYPE_MARKDOWN == ctrl->GetFileType()) {
		SetCodeControlOptions(ctrl, options.MarkdownStyles, options, SearchHitGoodBitmap, SearchHitBadBitmap,
			BookmarkBitmap, ExecutionLineBitmap, BreakpointBitmap);
		SetPlainTextOptions(ctrl, options);
		ctrl->SetLexer(wxSTC_LEX_MARKDOWN);
	} else if (t4p::FILE_TYPE_BASH == ctrl->GetFileType()) {
		SetCodeControlOptions(ctrl, options.BashStyles, options, SearchHitGoodBitmap, SearchHitBadBitmap,
			BookmarkBitmap, ExecutionLineBitmap, BreakpointBitmap);
		SetPlainTextOptions(ctrl, options);
		ctrl->SetLexer(wxSTC_LEX_BASH);
	} else if (t4p::FILE_TYPE_DIFF == ctrl->GetFileType()) {
		SetCodeControlOptions(ctrl, options.DiffStyles, options, SearchHitGoodBitmap, SearchHitBadBitmap,
			BookmarkBitmap, ExecutionLineBitmap, BreakpointBitmap);
		SetPlainTextOptions(ctrl, options);
		ctrl->SetLexer(wxSTC_LEX_DIFF);
	} else {
		SetPlainTextOptions(ctrl, options);

		// plain text files don't have a lexer, but we still want to
		// set a default background and foreground color
		SetCodeControlOptions(ctrl, options.PhpStyles, options, SearchHitGoodBitmap, SearchHitBadBitmap,
			BookmarkBitmap, ExecutionLineBitmap, BreakpointBitmap);
	}

	// in wxWidgets 2.9.5, need to set margin after setting the lexer
	// otherwise code folding does not work
	SetMargin(ctrl, options);
	ctrl->Colourise(0, -1);
}

void t4p::SyntaxHighlightViewClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	t4p::EditColorsPanelClass* panel = new t4p::EditColorsPanelClass(parent, Feature, *this);
	parent->AddPage(panel, _("Styles && Colors"));
}

void t4p::SyntaxHighlightViewClass::OnAppReady(wxCommandEvent& event) {
	// load the images once at startup
	LoadMarkerBitmaps();
}

void t4p::SyntaxHighlightViewClass::LoadMarkerBitmaps() {
	if (!SearchHitGoodBitmap.IsOk()) {
		SearchHitGoodBitmap = t4p::AutoCompleteImageAsset(wxT("magnifier"));
	}
	if (!SearchHitBadBitmap.IsOk()) {
		SearchHitBadBitmap = t4p::AutoCompleteImageAsset(wxT("magnifier-exclamation"));
	}
	if (!BookmarkBitmap.IsOk()) {
		BookmarkBitmap = t4p::AutoCompleteImageAsset(wxT("bookmark"));
	}
	if (!BreakpointBitmap.IsOk()) {
		BreakpointBitmap = t4p::AutoCompleteImageAsset(wxT("breakpoint"));
	}
	if (!ExecutionLineBitmap.IsOk()) {
		ExecutionLineBitmap = t4p::AutoCompleteImageAsset(wxT("arrow-right"));
	}
}

t4p::EditColorsPanelClass::EditColorsPanelClass(wxWindow* parent,
	t4p::SyntaxHighlightFeatureClass& feature,
	t4p::SyntaxHighlightViewClass& view)
: SyntaxHighlightPanelGeneratedClass(parent)
, CodeControlOptions(feature.App.Preferences.CodeControlOptions)
, EditedCodeControlOptions(feature.App.Preferences.CodeControlOptions)
, PhpCodeCtrl(NULL)
, SqlCodeCtrl(NULL)
, CssCodeCtrl(NULL)
, JsCodeCtrl(NULL)
, Globals()
, EventSink()
, View(view) {
	for (size_t i = 0; i < EditedCodeControlOptions.PhpStyles.size(); ++i) {
		wxString name = wxString::FromAscii(EditedCodeControlOptions.PhpStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &EditedCodeControlOptions.PhpStyles[i]);
	}
	for (size_t i = 0; i < EditedCodeControlOptions.SqlStyles.size(); ++i) {
		wxString name = wxString::FromAscii(EditedCodeControlOptions.SqlStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &EditedCodeControlOptions.SqlStyles[i]);
	}
	for (size_t i = 0; i < EditedCodeControlOptions.CssStyles.size(); ++i) {
		wxString name = wxString::FromAscii(EditedCodeControlOptions.CssStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &EditedCodeControlOptions.CssStyles[i]);
	}
	for (size_t i = 0; i < EditedCodeControlOptions.JsStyles.size(); ++i) {
		wxString name = wxString::FromAscii(EditedCodeControlOptions.JsStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &EditedCodeControlOptions.JsStyles[i]);
	}
	for (size_t i = 0; i < EditedCodeControlOptions.ConfigStyles.size(); ++i) {
		wxString name = wxString::FromAscii(EditedCodeControlOptions.ConfigStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &EditedCodeControlOptions.ConfigStyles[i]);
	}
	for (size_t i = 0; i < EditedCodeControlOptions.CrontabStyles.size(); ++i) {
		wxString name = wxString::FromAscii(EditedCodeControlOptions.CrontabStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &EditedCodeControlOptions.CrontabStyles[i]);
	}
	for (size_t i = 0; i < EditedCodeControlOptions.YamlStyles.size(); ++i) {
		wxString name = wxString::FromAscii(EditedCodeControlOptions.YamlStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &EditedCodeControlOptions.YamlStyles[i]);
	}
	for (size_t i = 0; i < EditedCodeControlOptions.RubyStyles.size(); ++i) {
		wxString name = wxString::FromAscii(EditedCodeControlOptions.RubyStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &EditedCodeControlOptions.RubyStyles[i]);
	}
	for (size_t i = 0; i < EditedCodeControlOptions.LuaStyles.size(); ++i) {
		wxString name = wxString::FromAscii(EditedCodeControlOptions.LuaStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &EditedCodeControlOptions.LuaStyles[i]);
	}
	for (size_t i = 0; i < EditedCodeControlOptions.MarkdownStyles.size(); ++i) {
		wxString name = wxString::FromAscii(EditedCodeControlOptions.MarkdownStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &EditedCodeControlOptions.MarkdownStyles[i]);
	}
	for (size_t i = 0; i < EditedCodeControlOptions.BashStyles.size(); ++i) {
		wxString name = wxString::FromAscii(EditedCodeControlOptions.BashStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &EditedCodeControlOptions.BashStyles[i]);
	}
	for (size_t i = 0; i < EditedCodeControlOptions.DiffStyles.size(); ++i) {
		wxString name = wxString::FromAscii(EditedCodeControlOptions.DiffStyles[i].Name);
		Styles->Append(wxGetTranslation(name), &EditedCodeControlOptions.DiffStyles[i]);
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
	PhpCodeCtrl = new t4p::CodeControlClass(this,
		EditedCodeControlOptions,
		&Globals, EventSink, wxID_ANY);
	PhpCodeCtrl->SetFileType(t4p::FILE_TYPE_PHP);
	PreviewNotebook->AddPage(PhpCodeCtrl, _("PHP"));
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
	PhpCodeCtrl->SetText(txt);
	View.ApplyPreferences(PhpCodeCtrl, EditedCodeControlOptions);

	SqlCodeCtrl = new t4p::CodeControlClass(this,
		EditedCodeControlOptions,
		&Globals, EventSink, wxID_ANY);
	SqlCodeCtrl->SetFileType(t4p::FILE_TYPE_SQL);
	txt =  t4p::CharToWx(
		" -- table to store users\n"
		"CREATE TABLE my_users(\n"
		"	userId INT(11) NOT NULL PRIMARY KEY AUTO_INCREMENT,\n"
		"	name VARCHAR(255) NOT NULL DEFAULT 'guest',\n"
		"	createdAt DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP\n"
		");"
	);
	SqlCodeCtrl->SetText(txt);
	PreviewNotebook->AddPage(SqlCodeCtrl, _("SQL"));
	View.ApplyPreferences(SqlCodeCtrl, EditedCodeControlOptions);

	CssCodeCtrl = new t4p::CodeControlClass(this,
		EditedCodeControlOptions,
		&Globals, EventSink, wxID_ANY);
	CssCodeCtrl->SetFileType(t4p::FILE_TYPE_CSS);
	txt =  t4p::CharToWx(
		" /* render users nicely */\n"
		".user {\n"
		"	font-weight: bold;\n"
		"	font-family: 'Arial';\n"
		"}"
	);
	CssCodeCtrl->SetText(txt);
	PreviewNotebook->AddPage(CssCodeCtrl, _("CSS"));
	View.ApplyPreferences(CssCodeCtrl, EditedCodeControlOptions);

	JsCodeCtrl = new t4p::CodeControlClass(this,
		EditedCodeControlOptions,
		&Globals, EventSink, wxID_ANY);
	JsCodeCtrl->SetFileType(t4p::FILE_TYPE_JS);
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
	JsCodeCtrl->SetText(txt);
	PreviewNotebook->AddPage(JsCodeCtrl, _("Javascript"));
	View.ApplyPreferences(JsCodeCtrl, EditedCodeControlOptions);
}

bool t4p::EditColorsPanelClass::TransferDataFromWindow() {
	// ATTN: only copy the styles
	// do not copy the other flags (editor behavior) since
	// they are being edited in the editor behavtior panel
	// if we just copy the entire EditedCodeControlOption
	// the we would revert the user's changes
	CodeControlOptions.CopyStyles(EditedCodeControlOptions);
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
		bool apply = false;
		switch (event.GetId()) {
			case ID_FOREGROUND_COLOR:
				pref->Color = event.GetColour();
				apply = true;
				break;
			case ID_BACKGROUND_COLOR:
				pref->BackgroundColor = event.GetColour();
				apply = true;
				break;
		}

		if (apply) {
			PhpCodeCtrl->ApplyPreferences();
			SqlCodeCtrl->ApplyPreferences();
			CssCodeCtrl->ApplyPreferences();
			JsCodeCtrl->ApplyPreferences();
			View.ApplyPreferences(PhpCodeCtrl, EditedCodeControlOptions);
			View.ApplyPreferences(SqlCodeCtrl, EditedCodeControlOptions);
			View.ApplyPreferences(CssCodeCtrl, EditedCodeControlOptions);
			View.ApplyPreferences(JsCodeCtrl, EditedCodeControlOptions);
		}
	}
}

void t4p::EditColorsPanelClass::OnFontChanged(wxFontPickerEvent& event) {
	int selected = Styles->GetSelection();
	t4p::StylePreferenceClass* pref = (t4p::StylePreferenceClass*)Styles->GetClientData(selected);
	if (pref) {
		wxFont font = event.GetFont();
		pref->Font = font;

		PhpCodeCtrl->ApplyPreferences();
		SqlCodeCtrl->ApplyPreferences();
		CssCodeCtrl->ApplyPreferences();
		JsCodeCtrl->ApplyPreferences();
		View.ApplyPreferences(PhpCodeCtrl, EditedCodeControlOptions);
		View.ApplyPreferences(SqlCodeCtrl, EditedCodeControlOptions);
		View.ApplyPreferences(CssCodeCtrl, EditedCodeControlOptions);
		View.ApplyPreferences(JsCodeCtrl, EditedCodeControlOptions);
	}
}

void t4p::EditColorsPanelClass::OnThemeChoice(wxCommandEvent& event) {
	wxCommandEvent listBoxEvent(wxEVT_COMMAND_LISTBOX_SELECTED, wxID_ANY);
	int sel = Styles->GetSelection();
	if (sel < 0) {
		sel = 0;
	}
	listBoxEvent.SetInt(sel);

	wxString newTheme = Theme->GetStringSelection();
	t4p::CodeControlStylesSetTheme(EditedCodeControlOptions, newTheme);
	wxPostEvent(this, listBoxEvent);

	PhpCodeCtrl->ApplyPreferences();
	SqlCodeCtrl->ApplyPreferences();
	CssCodeCtrl->ApplyPreferences();
	JsCodeCtrl->ApplyPreferences();
	View.ApplyPreferences(PhpCodeCtrl, EditedCodeControlOptions);
	View.ApplyPreferences(SqlCodeCtrl, EditedCodeControlOptions);
	View.ApplyPreferences(CssCodeCtrl, EditedCodeControlOptions);
	View.ApplyPreferences(JsCodeCtrl, EditedCodeControlOptions);
}

BEGIN_EVENT_TABLE(t4p::SyntaxHighlightViewClass, t4p::FeatureViewClass)
	EVT_APP_FILE_NEW(t4p::SyntaxHighlightViewClass::OnFileNew)
	EVT_APP_FILE_OPEN(t4p::SyntaxHighlightViewClass::OnFileOpen)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::SyntaxHighlightViewClass::OnAppReady)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::SyntaxHighlightViewClass::OnPreferencesSaved)
END_EVENT_TABLE()

