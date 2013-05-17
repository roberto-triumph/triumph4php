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
#include <code_control/CodeControlClass.h>
#include <globals/String.h>
#include <code_control/DocumentClass.h>
#include <widgets/StatusBarWithGaugeClass.h>
#include <globals/Errors.h>
#include <search/FindInFilesClass.h>
#include <wx/filename.h>
#include <wx/stc/stc.h>
#include <wx/utils.h>
#include <wx/tokenzr.h>
#include <unicode/ustring.h>

// IMPLEMENTATION NOTE:
// Take care when using positions given by Scintilla.  Scintilla gives positions in bytes while wxString and UnicodeString
// use character positions. Take caution when using methods like GetCurrentPos(), WordStartPosition(), WordEndPosition()
// This causes problems when Scintilla is handling UTF-8 documents.
// There is a method called GetSafeSubString() that will help you in this regard.


// margin 0 is taken up by line numbers, margin 1 is taken up by code folding. use
// margin 2 for lint error markers
static const int LINT_RESULT_MARKER = 2;
static const int LINT_RESULT_MARGIN = 2;

// we'll use up the first available marker & indicator for showing parse results
static const int INDICATOR = 0;

// 128 => 7th bit on since first 7 bits of style bits are for the HTML lexer
static const int INDICATOR_PHP_STYLE = 128;

// 32 => 5th bit on since first 5 bits of style bits are for all other lexers
static const int INDICATOR_TEXT_STYLE = 32;

/**
 * Turns a tag PHPDoc comment into a nicer format that is more suitable
 * to display. Any beginning '*'s are removed.
 *
 * Also, any HTML entities are handled (ignored), and any comment that
 * is too big is truncated.
 */
static wxString NiceDocText(const UnicodeString& comment) {
	wxString wxComment = mvceditor::IcuToWx(comment);
	wxComment = wxComment.Trim();

	// remove the beginning and ending '/*' and '*/'
	wxComment = wxComment.Mid(2, wxComment.Len() - 4);

	wxStringTokenizer tok(wxComment, wxT("\n\r"));
	wxString prettyComment;
	int lineCount = 0;
	while (tok.HasMoreTokens() && lineCount <= 20) {
		wxString line = tok.NextToken();

		// remove the beginning whitespace and '*'s
		size_t pos = 0;
		for (; pos < line.Len(); ++pos) {
			if (wxT(' ') != line[pos] && wxT('*') != line[pos] && wxT('\t') != line[pos]) {
				break;
			}
		}
		if (pos < line.Len()) {
			line = line.Mid(pos);
		}
		else {

			// an empty comment line
			line = wxT("");
		}

		// tag 'conversions'
		// taken from http://manual.phpdoc.org/HTMLSmartyConverter/HandS/phpDocumentor/tutorial_phpDocumentor.howto.pkg.html
		// the DocBlock Description details section
		// <b> -- emphasize/bold text
		// <code> -- Use this to surround php code, some converters will highlight it
		// <br> -- hard line break, may be ignored by some converters
		// <i> -- italicize/mark as important
		// <kbd> -- denote keyboard input/screen display
		// <li> -- list item
		// <ol> -- ordered list
		// <p> -- If used to enclose all paragraphs, otherwise it will be considered text
		// <pre> -- Preserve line breaks and spacing, and assume all tags are text (like XML's CDATA)
		// <samp> -- denote sample or examples (non-php)
		// <ul> -- unordered list
		// <var> -- denote a variable name
		//
		// will ignore all of the tags except '<br>', '<code>', <p>', '<pre>' 
		// since we cannot format the Scintilla call tip window.
		// For the tags we don handle; just translate them to newlies for now.
		wxString remove[] = { 
			wxT("<b>"), wxT("</b>"), wxT("<i>"), wxT("</i>"), 
			wxT("<kbd>"), wxT("</kbd>"), wxT("<samp>"), wxT("</samp>"), 
			wxT("<var>"), wxT("</var>"), 
			wxT("") 
		};
		for (int i = 0; !remove[i].IsEmpty(); ++i) {
			line.Replace(remove[i], wxT(""));	
		}

		wxString toNewline[] =  { 
			wxT("<code>"), wxT("</code>"),	wxT("<br>"), wxT("<br />"), 
			wxT("<li>"), wxT("</li>"), wxT("<ol>"), wxT("</ol>"), 
			wxT("<p>"), wxT("</p>"), wxT("<ul>"), wxT("</ul>"), 
			wxT("") 
		};
		for (int i = 0; !toNewline[i].IsEmpty(); ++i) {
			line.Replace(toNewline[i], wxT("\n"));
		}


		line.Replace(wxT("{@*}"), wxT("*/"));

		// replace tabs with spaces
		// do it here instead of in scintilla; we may want to change this later
		line.Replace(wxT("\t"), wxT("    "));

		prettyComment += line;
		prettyComment += wxT("\n");
		lineCount++;
	}
	if (lineCount > 20) {
		prettyComment += wxT("\n...\n");
	}
	return prettyComment;
}

mvceditor::CodeControlClass::CodeControlClass(wxWindow* parent, CodeControlOptionsClass& options,
			mvceditor::GlobalsClass* globals,
			wxWindowID id, const wxPoint& position, const wxSize& size, long style,
			const wxString& name)
		: wxStyledTextCtrl(parent, id, position, size, style, name)
		, CodeControlOptions(options)
		, CurrentFilename()		
		, WordHighlightFinder()
		, WordHighlightWord()
		, CurrentDbTag()
		, Globals(globals)
		, WordHighlightPreviousIndex(-1)
		, WordHighlightNextIndex(-1)
		, WordHighlightStyle(0)
		, ModifiedDialogOpen(false)
		, WordHighlightIsWordHighlighted(false)
		, DocumentMode(TEXT) 
		, IsHidden(false) {
	Document = NULL;
	
	// we will handle right-click menu ourselves
	UsePopUp(false);
	SetYCaretPolicy(wxSTC_CARET_EVEN, 0);
	ApplyPreferences();
}

mvceditor::CodeControlClass::~CodeControlClass() {
	Document->DetachFromControl(this);
	delete Document;	
}
void mvceditor::CodeControlClass::TrackFile(const wxString& filename, UnicodeString& contents) {
	SetUnicodeText(contents);
	EmptyUndoBuffer();
	SetSavePoint();
	CurrentFilename = filename;
	wxFileName file(filename);
	if (file.IsOk()) {
		FileOpenedDateTime = file.GetModificationTime();
	}
	AutoDetectDocumentMode();

	// order is important; calling FileOpened after document type is detected so that
	// the proper callback is used
	Document->FileOpened(filename);
}

void mvceditor::CodeControlClass::SetUnicodeText(UnicodeString& contents) {

	// lets avoid the IcuToWx to prevent going from 
	// UnicodeString -> UTF8 -> wxString  -> UTF8 -> Scintilla
	// cost of translation (computation and memoory) could be big for big sized files
	// because of the double encoding due to all three libraries using
	// different internal encodings for their strings
	// code below just goes
	// UnicodeString -> UTF8 -> Scintilla
	UErrorCode status = U_ZERO_ERROR;
	int32_t rawLength;
	int32_t length = contents.length();
	const UChar* src = contents.getBuffer();
	u_strToUTF8(NULL, 0, &rawLength, src, length, &status);
	status = U_ZERO_ERROR;
	char* dest = new char[rawLength + 1];
	int32_t written;
	u_strToUTF8(dest, rawLength + 1, &written, src, length, &status);
	if(U_SUCCESS(status)) {
	
		// SetText message
		SendMsg(2181, 0, (long)(const char*)dest);
	}
	delete[] dest;
}

void mvceditor::CodeControlClass::Revert() {
	if (!IsNew()) {
		LoadAndTrackFile(CurrentFilename);
	}
}

void mvceditor::CodeControlClass::LoadAndTrackFile(const wxString& fileName) {
	UnicodeString contents;	

	// not using wxStyledTextCtrl::LoadFile() because it does not correctly handle files with high ascii characters
	mvceditor::FindInFilesClass::OpenErrors error = FindInFilesClass::FileContents(fileName, contents);
	if (error == mvceditor::FindInFilesClass::NONE) {
		TrackFile(fileName, contents);
	}
	else if (error == mvceditor::FindInFilesClass::FILE_NOT_FOUND) {
		mvceditor::EditorLogError(mvceditor::ERR_INVALID_FILE, fileName);
	}
	else if (mvceditor::FindInFilesClass::CHARSET_DETECTION == error) {
		mvceditor::EditorLogError(mvceditor::ERR_CHARSET_DETECTION, fileName);
	}
}

bool mvceditor::CodeControlClass::IsNew() const {
	return CurrentFilename.empty();
}

bool mvceditor::CodeControlClass::SaveAndTrackFile(wxString newFilename) {
	bool saved = false;
	if (CodeControlOptions.TrimTrailingSpaceBeforeSave) {
		TrimTrailingSpaces();
	}
	if (CodeControlOptions.RemoveTrailingBlankLinesBeforeSave) {
		RemoveTrailingBlankLines();
	}
	if (!CurrentFilename.empty() || CurrentFilename == newFilename) {
		saved = SaveFile(CurrentFilename);

		// if file is no changing name then its not changing extension
		// no need to auto detect the document mode
	}
	else if (SaveFile(newFilename)) {
		CurrentFilename = newFilename;
		saved = true;

		// if the file extension changed let's update the code control appropriate
		// for example if a .txt file was saved as a .sql file
		AutoDetectDocumentMode();

		// need to notify the document of the new name
		Document->FileOpened(newFilename);
	}
	if (saved) {

		// when saving, update the internal timestamp so that the OnIdle logic works correctly
		wxFileName file(CurrentFilename);
		FileOpenedDateTime = file.GetModificationTime();
	}
	return saved;
}

wxString mvceditor::CodeControlClass::GetFileName() const {
	return CurrentFilename;
}

void mvceditor::CodeControlClass::SetSelectionAndEnsureVisible(int start, int end) {
	
	// make sure that selection ends up in the middle of the screen, hence the new caret policy
	SetYCaretPolicy(wxSTC_CARET_JUMPS | wxSTC_CARET_EVEN, 0);
	SetSelectionByCharacterPosition(start, end);
	EnsureCaretVisible();
	SetYCaretPolicy(wxSTC_CARET_EVEN, 0);
}

void mvceditor::CodeControlClass::SetSelectionByCharacterPosition(int start, int end) {
	int documentLength = GetTextLength();
	char* buf = new char[documentLength];
	
	// GET_TEXT  message
	SendMsg(2182, documentLength, (long)buf);
	
	SetSelection(mvceditor::CharToUtf8Pos(buf, documentLength, start), 
		mvceditor::CharToUtf8Pos(buf, documentLength, end));
	delete[] buf;
}

void mvceditor::CodeControlClass::OnCharAdded(wxStyledTextEvent &event) {

	// clear the auto complete message
	wxWindow* window = GetGrandParent();
	wxFrame* frame = wxDynamicCast(window, wxFrame);
	if (frame) {
		mvceditor::StatusBarWithGaugeClass* gauge = (mvceditor::StatusBarWithGaugeClass*)frame->GetStatusBar();
		gauge->SetColumn0Text(wxT(""));		
	}

	char ch = (char)event.GetKey();
	if (CodeControlOptions.EnableAutomaticLineIndentation) {
		HandleAutomaticIndentation(ch);
	}
	if (CodeControlOptions.EnableAutoCompletion) {

		// attempt to show code completion on method operator / scope operator
		// since this event happens after currentPos is advanced; so
		// current char is now at currentPos - 1, so previous char is at currentPos - 2
		char prevChar = GetCharAt(GetCurrentPos() - 2);
		if (('-' == prevChar && '>' == ch) || (':' == prevChar && ':' == ch)) {
			HandleAutoCompletion();	
		}	
		HandleCallTip(ch);
	}

	// expand the line if currently folded. adding +1 to the current line so that even if enter is pressed,
	// the folded line is expanded
	int currentLine = GetCurrentLine();
	EnsureVisible(currentLine + 1);
	event.Skip();
}

void mvceditor::CodeControlClass::HandleAutomaticIndentation(char chr) {
	int currentLine = GetCurrentLine();
	
	// ATTN: Change this if support for mac files with \r is needed
	if ('\n' == chr) {
		int lineIndentation = 0;
		if (currentLine > 0) {
			lineIndentation = GetLineIndentation(currentLine - 1);
		}
		if (lineIndentation > 0) {

			// must check if the code uses tabs or spaces
			int indentSize = GetIndent() ? GetIndent() : GetTabWidth();
			int tabs = lineIndentation / indentSize;
			for (int i = 0; i < tabs; ++i) {
				CmdKeyExecute(wxSTC_CMD_TAB);
			}
		}
	}
}

std::vector<mvceditor::TagClass> mvceditor::CodeControlClass::GetCurrentSymbolResource() {
	return Document->GetCurrentSymbolResource();
}

void mvceditor::CodeControlClass::HandleAutoCompletion() {
	wxString completeStatus;
	Document->HandleAutoCompletion(completeStatus);
	if (!completeStatus.IsEmpty()) {
		wxWindow* window = GetGrandParent();

		// show the auto complete message
		wxFrame* frame = wxDynamicCast(window, wxFrame);
		if (frame) {
			mvceditor::StatusBarWithGaugeClass* gauge = (mvceditor::StatusBarWithGaugeClass*)frame->GetStatusBar();
			gauge->SetColumn0Text(completeStatus);		
		}
	}
}

void mvceditor::CodeControlClass::HandleCallTip(wxChar ch, bool force) {
	Document->HandleCallTip(ch, force);
}

void mvceditor::CodeControlClass::OnUpdateUi(wxStyledTextEvent &event) {
	HandleCallTip(0, false);
	event.Skip();
}

void mvceditor::CodeControlClass::OnMarginClick(wxStyledTextEvent& event) {
	if (event.GetMargin() == CodeControlOptionsClass::MARGIN_CODE_FOLDING) {
		int line = LineFromPosition(event.GetPosition());
		ToggleFold(line);
	}
}

void mvceditor::CodeControlClass::SetMargin() {
	if (CodeControlOptions.EnableLineNumbers) {
		SetMarginType(CodeControlOptionsClass::MARGIN_LINE_NUMBER, wxSTC_MARGIN_NUMBER);
		SetMarginWidth(CodeControlOptionsClass::MARGIN_LINE_NUMBER, TextWidth(wxSTC_STYLE_LINENUMBER, wxT("_99999")));
	}
	else {
		SetMarginWidth(CodeControlOptionsClass::MARGIN_LINE_NUMBER, 0);
	}
	if (CodeControlOptions.EnableCodeFolding) {
		SetProperty(wxT("fold"), wxT("1"));
		SetProperty(wxT("fold.comment"), wxT("1"));
		SetProperty(wxT("fold.html"), wxT("1"));
		SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
		SetMarginType(CodeControlOptionsClass::MARGIN_CODE_FOLDING, wxSTC_MARGIN_SYMBOL);
		SetMarginWidth(CodeControlOptionsClass::MARGIN_CODE_FOLDING, 16);
		SetMarginSensitive(CodeControlOptionsClass::MARGIN_CODE_FOLDING, true);
		SetMarginMask(CodeControlOptionsClass::MARGIN_CODE_FOLDING, wxSTC_MASK_FOLDERS);
	}
	else {
		SetProperty(wxT("fold"), wxT("0"));
		SetProperty(wxT("fold.comment"), wxT("0"));
		SetProperty(wxT("fold.html"), wxT("0"));
		SetFoldFlags(0);
		SetMarginType(CodeControlOptionsClass::MARGIN_CODE_FOLDING, wxSTC_MARGIN_SYMBOL);
		SetMarginWidth(CodeControlOptionsClass::MARGIN_CODE_FOLDING, 0);
		SetMarginSensitive(CodeControlOptionsClass::MARGIN_CODE_FOLDING, false);
	}
}

void mvceditor::CodeControlClass::AutoDetectDocumentMode() {
	wxString file = GetFileName();
	
	bool found = false;
	std::vector<wxString> wildcards = Globals->GetPhpFileExtensions();
	for (size_t i = 0; i < wildcards.size(); ++i) {
		if (wxMatchWild(wildcards[i], file)) {
			found = true;
			SetDocumentMode(mvceditor::CodeControlClass::PHP);
			break;
		}
	}
	if (!found) {
		wildcards = Globals->GetCssFileExtensions();
		for (size_t i = 0; i < wildcards.size(); ++i) {
			if (wxMatchWild(wildcards[i], file)) {
				found = true;
				SetDocumentMode(mvceditor::CodeControlClass::CSS);
				break;
			}
		}	
	}
	if (!found) {
		wildcards = Globals->GetSqlFileExtensions();
		for (size_t i = 0; i < wildcards.size(); ++i) {
			if (wxMatchWild(wildcards[i], file)) {
				found = true;
				SetDocumentMode(mvceditor::CodeControlClass::SQL);
				break;
			}
		}
	}
	if (!found) {
		SetDocumentMode(mvceditor::CodeControlClass::TEXT);
	}
}

void mvceditor::CodeControlClass::ApplyPreferences() {
	SetMargin();
	if (CodeControlOptions.EnableWordWrap) {
		SetWrapMode(wxSTC_WRAP_WORD);
		SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_START);
	}
	else {
		SetWrapMode(wxSTC_WRAP_NONE);
	}
	if (CodeControlOptions.EnableCallTipsOnMouseHover) {
		SetMouseDwellTime(1000);
	}
	else {
		SetMouseDwellTime(wxSTC_TIME_FOREVER);
	}
	if (Document) {
		
		// need this here so that any events are not propagated while the object is
		// being destructed (valgrind found this error)
		Document->DetachFromControl(this);
		delete Document;
		Document = NULL;
	}
	if (mvceditor::CodeControlClass::SQL == DocumentMode) {
		Document = new mvceditor::SqlDocumentClass(Globals, CurrentDbTag);
		Document->SetControl(this);
		SetCodeControlOptions(CodeControlOptions.SqlStyles);
		SetSqlOptions();
	}
	else if (mvceditor::CodeControlClass::PHP == DocumentMode) {
		Document = new mvceditor::PhpDocumentClass(Globals);
		Document->SetControl(this);
		SetCodeControlOptions(CodeControlOptions.PhpStyles);
		SetPhpOptions();
	}
	else if (mvceditor::CodeControlClass::CSS == DocumentMode) {
		Document = new mvceditor::CssDocumentClass();
		Document->SetControl(this);
		SetCodeControlOptions(CodeControlOptions.CssStyles);
		SetCssOptions();
	}
	else {
		
		// use the PHP styles; needed to get the caret and margin styling right
		SetCodeControlOptions(CodeControlOptions.PhpStyles);
		Document = new mvceditor::TextDocumentClass();
		Document->SetControl(this);
		SetPlainTextOptions();
	}
	Colourise(0, -1);
}

void mvceditor::CodeControlClass::SetPhpOptions() {
	// Some languages, such as HTML may contain embedded languages, VBScript
	// and JavaScript are common for HTML. For HTML, key word set 0 is for HTML,
	// 1 is for JavaScript and 2 is for VBScript, 3 is for Python, 4 is for PHP
	// and 5 is for SGML and DTD keywords
	SetKeyWords(0, ((mvceditor::PhpDocumentClass*)Document)->GetHtmlKeywords());
	SetKeyWords(1, ((mvceditor::PhpDocumentClass*)Document)->GetJavascriptKeywords());
	SetKeyWords(4, ((mvceditor::PhpDocumentClass*)Document)->GetPhpKeywords());
	
	SetLexer(wxSTC_LEX_HTML);
	
	// 7 = as per scintilla docs, HTML lexer uses 7 bits for styles
	SetStyleBits(7);
	AutoCompStops(wxT("!@#$%^&*()_+-=[]{}|;'\",./<?"));
	AutoCompSetSeparator('\n');
	AutoCompSetChooseSingle(true);
	AutoCompSetFillUps(wxT("(["));
	AutoCompSetIgnoreCase(true);
	
	// need to add the namespace operator here, it was the only way i could get the
	// autocompletion to workwith namespaces.
	SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$\\"));
	
	SetMarginType(LINT_RESULT_MARGIN, wxSTC_MARGIN_SYMBOL);
	SetMarginWidth(LINT_RESULT_MARGIN, 16);
	SetMarginSensitive(LINT_RESULT_MARGIN, false);
	SetMarginMask(LINT_RESULT_MARGIN, ~wxSTC_MASK_FOLDERS);
	MarkerDefine(LINT_RESULT_MARKER, wxSTC_MARK_ARROW, *wxRED, *wxRED);
	WordHighlightStyle = INDICATOR_PHP_STYLE;
	
	// syntax coloring
	for (size_t i = 0; i < CodeControlOptions.PhpStyles.size(); ++i) {
		mvceditor::StylePreferenceClass pref = CodeControlOptions.PhpStyles[i];
		int style = pref.StcStyle;
		if (wxSTC_HPHP_DEFAULT == style) {
			
			// use the PHP default settings as the catch-all for settings not yet exposed
			// (Javascript) so the user sees a uniform style.
			int styles[] = {
				wxSTC_STYLE_DEFAULT, wxSTC_HJ_START, wxSTC_HJ_DEFAULT, 
				wxSTC_HJ_COMMENT, wxSTC_HJ_COMMENTLINE, wxSTC_HJ_COMMENTDOC, 
				wxSTC_HJ_NUMBER, wxSTC_HJ_WORD, wxSTC_HJ_KEYWORD, 
				wxSTC_HJ_DOUBLESTRING, wxSTC_HJ_SINGLESTRING, wxSTC_HJ_SYMBOLS, 

				// sgml styles take care of the <!DOCTYPE declarations
				wxSTC_HJ_STRINGEOL, wxSTC_HJ_REGEX, wxSTC_H_SGML_1ST_PARAM, 
				wxSTC_H_SGML_1ST_PARAM_COMMENT, wxSTC_H_SGML_BLOCK_DEFAULT, wxSTC_H_SGML_COMMAND, 
				wxSTC_H_SGML_COMMENT, wxSTC_H_SGML_DEFAULT, wxSTC_H_SGML_DOUBLESTRING, 
				wxSTC_H_SGML_ENTITY, wxSTC_H_SGML_ERROR, wxSTC_H_SGML_SIMPLESTRING, 
				wxSTC_H_SGML_SPECIAL
			};
			for (int j = 0; j < 25; ++j) {
				StyleSetFont(styles[j], pref.Font);
				StyleSetForeground(styles[j], pref.Color);
				StyleSetBackground(styles[j], pref.BackgroundColor);
				StyleSetBold(styles[j], pref.IsBold);
				StyleSetItalic(styles[j], pref.IsItalic);
			}
		}
		StyleSetFont(style, pref.Font);
		StyleSetForeground(style, pref.Color);
		StyleSetBackground(style, pref.BackgroundColor);
		StyleSetBold(style, pref.IsBold);
		StyleSetItalic(style, pref.IsItalic);
	}
}

void mvceditor::CodeControlClass::SetSqlOptions() {	
	SetKeyWords(0, ((mvceditor::SqlDocumentClass*)Document)->GetMySqlKeywords());
	SetKeyWords(1, wxT(""));
	SetKeyWords(2, wxT(""));
	SetKeyWords(3, wxT(""));
	SetKeyWords(4, wxT(""));
	
	SetLexer(wxSTC_LEX_SQL);
	
	// 5 = default as per scintilla docs. set it because it may have been set by SetPhpOptions()
	SetStyleBits(5);
	AutoCompStops(wxT("!@#$%^&*()_+-=[]\\{}|;'\",/?`"));
	AutoCompSetSeparator(' ');
	AutoCompSetChooseSingle(true);
	AutoCompSetIgnoreCase(true);
	AutoCompSetFillUps(wxT("(["));
	SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"));
	WordHighlightStyle = INDICATOR_TEXT_STYLE;
	
	for (size_t i = 0; i < CodeControlOptions.SqlStyles.size(); ++i) {
		mvceditor::StylePreferenceClass pref = CodeControlOptions.SqlStyles[i];
		int style = pref.StcStyle;
		if (wxSTC_SQL_DEFAULT == style) {
			
			// wxSTC_STYLE_DEFAULT controls the background of the portions where text does not reach
			StyleSetFont(wxSTC_STYLE_DEFAULT, pref.Font);
			StyleSetForeground(wxSTC_STYLE_DEFAULT, pref.Color);
			StyleSetBackground(wxSTC_STYLE_DEFAULT, pref.BackgroundColor);
			StyleSetBold(wxSTC_STYLE_DEFAULT, pref.IsBold);
			StyleSetItalic(wxSTC_STYLE_DEFAULT, pref.IsItalic);
		}
		StyleSetFont(style, pref.Font);
		StyleSetForeground(style, pref.Color);
		StyleSetBackground(style, pref.BackgroundColor);
		StyleSetBold(style, pref.IsBold);
		StyleSetItalic(style, pref.IsItalic);	
	}
}

void mvceditor::CodeControlClass::SetCssOptions() {
	SetLexer(wxSTC_LEX_CSS);
	
	// got this by looking at LexCSS.cxx (bottom of the file)
	// keywords 0 => CSS 1 keywords
	// keywords 1 => Pseudo classes
	// keywords 2 => CSS 2 keywords but we will pass all keywords in 0
	SetKeyWords(0,  ((mvceditor::CssDocumentClass*)Document)->GetCssKeywords());
	SetKeyWords(1,  ((mvceditor::CssDocumentClass*)Document)->GetCssPseudoClasses());
	SetKeyWords(2, wxT(""));
	
	// 5 = default as per scintilla docs. set it because it may have been set by SetPhpOptions()
	SetStyleBits(5);
	AutoCompStops(wxT("!@#$%^&*()_+-=[]\\{}|;'\",/?`"));
	AutoCompSetSeparator(' ');
	AutoCompSetIgnoreCase(true);
	AutoCompSetChooseSingle(true);
	AutoCompSetFillUps(wxT("([:"));
	SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"));
	WordHighlightStyle = INDICATOR_TEXT_STYLE;
	
	for (size_t i = 0; i < CodeControlOptions.CssStyles.size(); ++i) {
		mvceditor::StylePreferenceClass pref = CodeControlOptions.CssStyles[i];
		int style = pref.StcStyle;
		if (wxSTC_CSS_DEFAULT == style) {
			
			// wxSTC_STYLE_DEFAULT controls the background of the portions where text does not reach
			StyleSetFont(wxSTC_STYLE_DEFAULT, pref.Font);
			StyleSetForeground(wxSTC_STYLE_DEFAULT, pref.Color);
			StyleSetBackground(wxSTC_STYLE_DEFAULT, pref.BackgroundColor);
			StyleSetBold(wxSTC_STYLE_DEFAULT, pref.IsBold);
			StyleSetItalic(wxSTC_STYLE_DEFAULT, pref.IsItalic);
		}
		StyleSetFont(style, pref.Font);
		StyleSetForeground(style, pref.Color);
		StyleSetBackground(style, pref.BackgroundColor);
		StyleSetBold(style, pref.IsBold);
		StyleSetItalic(style, pref.IsItalic);	
	}
}

void mvceditor::CodeControlClass::SetPlainTextOptions() {
	
	SetLexer(wxSTC_LEX_NULL);

	// 5 = default as per scintilla docs. set it because it may have been set by SetPhpOptions()
	SetStyleBits(5);
	SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"));
	
	SetMarginType(LINT_RESULT_MARGIN, wxSTC_MARGIN_SYMBOL);
	SetMarginWidth(LINT_RESULT_MARGIN, 16);
	SetMarginSensitive(LINT_RESULT_MARGIN, false);
	SetMarginMask(LINT_RESULT_MARGIN, ~wxSTC_MASK_FOLDERS);
	MarkerDefine(LINT_RESULT_MARKER, wxSTC_MARK_ARROW, *wxRED, *wxRED);
	WordHighlightStyle = INDICATOR_TEXT_STYLE;
	
	// syntax coloring; use the same font as PHP code for now
	for (size_t i = 0; i < CodeControlOptions.CssStyles.size(); ++i) {
		mvceditor::StylePreferenceClass pref = CodeControlOptions.PhpStyles[i];
		int style = pref.StcStyle;
		if (wxSTC_HPHP_DEFAULT == style) {
			
			// wxSTC_STYLE_DEFAULT controls the background of the portions where text does not reach
			StyleSetFont(wxSTC_STYLE_DEFAULT, pref.Font);
			StyleSetForeground(wxSTC_STYLE_DEFAULT, pref.Color);
			StyleSetBackground(wxSTC_STYLE_DEFAULT, pref.BackgroundColor);
			StyleSetBold(wxSTC_STYLE_DEFAULT, pref.IsBold);
			StyleSetItalic(wxSTC_STYLE_DEFAULT, pref.IsItalic);

			// 0 = only style since plain text files dont use a scintilla lexer
			StyleSetFont(0, pref.Font);
			StyleSetForeground(0, pref.Color);
			StyleSetBackground(0, pref.BackgroundColor);
			StyleSetBold(0, pref.IsBold);
			StyleSetItalic(0, pref.IsItalic);
		}
		StyleSetFont(style, pref.Font);
		StyleSetForeground(style, pref.Color);
		StyleSetBackground(style, pref.BackgroundColor);
		StyleSetBold(style, pref.IsBold);
		StyleSetItalic(style, pref.IsItalic);	
	}
}

void mvceditor::CodeControlClass::SetCodeControlOptions(const std::vector<mvceditor::StylePreferenceClass>& styles) {
	if (CodeControlOptions.IndentUsingTabs) {
		SetUseTabs(true);
		SetTabWidth(CodeControlOptions.TabWidth);
		SetIndent(0);
		SetTabIndents(true);
		SetBackSpaceUnIndents(true);
	}
	else {
		SetUseTabs(false);
		SetTabWidth(CodeControlOptions.SpacesPerIndent);
		SetIndent(CodeControlOptions.SpacesPerIndent);
		SetTabIndents(false);
		SetBackSpaceUnIndents(false);
	}
	if (CodeControlOptions.RightMargin > 0) {
		SetEdgeMode(wxSTC_EDGE_LINE);
		SetEdgeColumn(CodeControlOptions.RightMargin);
	}
	else {
		SetEdgeMode(wxSTC_EDGE_NONE);
	}
	SetIndentationGuides(CodeControlOptions.EnableIndentationGuides);
	SetEOLMode(CodeControlOptions.LineEndingMode);
	SetViewEOL(CodeControlOptions.EnableLineEndings);
	
	// caret, line, selection, margin colors
	for (size_t i = 0; i < styles.size(); ++i) {
		mvceditor::StylePreferenceClass pref = styles[i];
		int style = pref.StcStyle;
		switch (style) {
			case CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET:
				SetCaretForeground(pref.Color);
				break;
			case CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE:
				SetCaretLineVisible(true);
				SetCaretLineBackground(pref.BackgroundColor);
				break;
			case CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION:
				SetSelForeground(true, pref.Color);
				SetSelBackground(true, pref.BackgroundColor);
				break;
			case CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING:
				if (CodeControlOptions.EnableCodeFolding) {
					SetFoldMarginColour(true, pref.BackgroundColor);
					SetFoldMarginHiColour(true, pref.BackgroundColor);
					MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS, pref.BackgroundColor, pref.Color);
					MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS, pref.BackgroundColor, pref.Color);
					MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE, pref.BackgroundColor, pref.Color);
					MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER, pref.BackgroundColor, pref.Color);
					MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUSCONNECTED, pref.BackgroundColor, pref.Color);
					MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, pref.BackgroundColor, pref.Color);
					MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER, pref.BackgroundColor, pref.Color);
				}
			case CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN:
				if (CodeControlOptions.RightMargin > 0) {
					SetEdgeColour(pref.Color);
				}
				break;
			case mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT:
				// since we need to share one indicator with the matching word highlight
				// and the parse errors indicators; we will set this setting when the 
				// user initiates the matching word feature
				break;
		}
	}
}

void  mvceditor::CodeControlClass::OnDoubleClick(wxStyledTextEvent& event) {
	int pos = WordStartPosition(GetCurrentPos(), true);
	int endPos = WordEndPosition(GetCurrentPos(), true);

	// we must share the indicator between highlight words functionality and
	// parse errors functionality.  This is because the HTML lexer uses 7 of the
	// eight style bits, leaving only one bit for indicators
	mvceditor::StylePreferenceClass pref = CodeControlOptions.FindByStcStyle(CodeControlOptions.PhpStyles, 
		mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT);
	IndicatorSetStyle(INDICATOR,  wxSTC_INDIC_ROUNDBOX);
	IndicatorSetForeground(INDICATOR, pref.Color);

	// remove any parse error indicators. if we don't do this the 
	// parse error will get highlighted like a match.
	StartStyling(0, WordHighlightStyle);
	SetStyling(GetTextLength(), 0);
	
	int charStartIndex = 0;
	
	// pos, endPos are byte offsets into the UTF-8 string, need to convert to char numbers
	int documentLength = GetTextLength();
	char* buf = new char[documentLength];
		
	// GET_TEXT  message
	SendMsg(2182, documentLength, (long)buf);
	charStartIndex = mvceditor::Utf8PosToChar(buf, documentLength, pos);
	mvceditor::Utf8PosToChar(buf, documentLength, endPos);
	
	UnicodeString word = Document->GetSafeSubstring(pos, endPos);
	if (!word.isEmpty()) {
		WordHighlightFinder.Expression = word;
		WordHighlightFinder.Mode = mvceditor::FinderClass::EXACT;
		if (WordHighlightFinder.Prepare()) {
			WordHighlightPreviousIndex = charStartIndex;
			WordHighlightNextIndex = charStartIndex;
		}
	}
	delete[] buf;
	event.Skip();
}

void mvceditor::CodeControlClass::OnContextMenu(wxContextMenuEvent& event) {
	wxWindow* frame = GetGrandParent();

	// Let the frame handle it because we want features to have menu items
	if (frame) {
		frame->ProcessEvent(event);
	}
	else {
		event.Skip();
	}
}

UnicodeString mvceditor::CodeControlClass::GetSafeText() {
	return Document->GetSafeText();
}

void mvceditor::CodeControlClass::OnIdle(wxIdleEvent& event) {
	if (!IsNew() && !ModifiedDialogOpen) {
		
		// check to see if file has been removed or saved externally.
		// careful, GetModificationTime requires the file to exist
		wxFileName file(CurrentFilename);
		wxDateTime modifiedDateTime;
		bool exists = file.FileExists();
		if (exists) {
			modifiedDateTime = file.GetModificationTime();
		}
		if (!exists || modifiedDateTime.IsLaterThan(FileOpenedDateTime)) {
			
			ModifiedDialogOpen = true;
			wxString message = CurrentFilename;
			int opts = 0;

			// check again, in case file was modified externally at the same time
			// this code is running
			exists = file.FileExists();
			if (exists) {
				message += _("\n\nFile has been modified externally. Reload file and lose any changes?\n");
				message += _("Yes will reload file, No will allow you to override the file.");
				opts = wxYES_NO | wxICON_QUESTION;
			}
			else {
				message += _("\n\nFile has been deleted externally.\n");
				message += _("You will need to save the file to store the contents.");
				opts = wxICON_QUESTION;
			}
			int res = wxMessageBox(message, _("Warning"), 
				opts, this);
			if (wxYES == res && exists) {
				LoadAndTrackFile(CurrentFilename);
			}
			else if (!exists) {

				// so that next idle event user does not get asked the same question again
				// since file does not exist, make it look like a new file
				CurrentFilename = wxT("");
			}
			else {

				// so that next idle event user does not get asked the same question again
				FileOpenedDateTime = modifiedDateTime;
			}
			ModifiedDialogOpen = false;
		}
	}
	
	// moving match braces here from UpdateUi because when I put this code in UpdateUi there
	// is a bad flicker in GTK when the braces are highlighted.
	// Code folding seems to make the flicker appear all the time.
	Document->MatchBraces(GetCurrentPos());
	event.Skip();
}

void mvceditor::CodeControlClass::OnKeyDown(wxKeyEvent& event) {
	UndoHighlight();
	if (event.GetKeyCode() == WXK_ESCAPE) {
		CallTipCancel();
	}
	event.Skip();
}

void mvceditor::CodeControlClass::OnLeftDown(wxMouseEvent& event) {
	UndoHighlight();
	event.Skip();
}

void mvceditor::CodeControlClass::UndoHighlight() {
	if (WordHighlightIsWordHighlighted) {
		// kill any current highlight searches
		WordHighlightNextIndex = -1;
		WordHighlightPreviousIndex = -1;
		
		StartStyling(0, WordHighlightStyle);
		SetStyling(GetTextLength(), 0);
		WordHighlightIsWordHighlighted = false;
	}
}

void mvceditor::CodeControlClass::WordHiglightForwardSearch(wxIdleEvent& event) {
	if (WordHighlightNextIndex > -1) {
		UnicodeString codeText = GetSafeText();
		int documentLength = GetTextLength();
		char* buf = new char[documentLength];
		
		// GET_TEXT  message
		SendMsg(2182, documentLength, (long)buf);
		
		bool found = WordHighlightFinder.FindNext(codeText, WordHighlightNextIndex);
		int32_t matchStart(0);
		int32_t	matchLength(0);
		if (found && WordHighlightFinder.GetLastMatch(matchStart, matchLength)) {
			WordHighlightIsWordHighlighted = true;
			
			// convert match back to UTF-8 ugh
			int utf8Start = mvceditor::CharToUtf8Pos(buf, documentLength, matchStart);
			int utf8End = mvceditor::CharToUtf8Pos(buf, documentLength, matchStart + matchLength);

			StartStyling(utf8Start, WordHighlightStyle);
			SetStyling(utf8End - utf8Start, WordHighlightStyle);
			WordHighlightNextIndex = matchStart + matchLength + 1; // prevent infinite find next's
			event.RequestMore();
		}
		else {
			WordHighlightNextIndex = -1;
		}
		delete[] buf;
	}
	event.Skip();
}

void mvceditor::CodeControlClass::WordHiglightPreviousSearch(wxIdleEvent& event) {
	if (WordHighlightPreviousIndex > -1) {
		UnicodeString codeText = GetSafeText();		
		int documentLength = GetTextLength();
		char* buf = new char[documentLength];
		
		// GET_TEXT  message
		SendMsg(2182, documentLength, (long)buf);
		
		bool found = WordHighlightFinder.FindPrevious(codeText, WordHighlightPreviousIndex);
		int32_t matchStart(0);
		int32_t	matchLength(0);
		if (found && WordHighlightFinder.GetLastMatch(matchStart, matchLength)) {
			WordHighlightIsWordHighlighted = true;
			
			// convert match back to UTF-8 ugh
			int utf8Start = mvceditor::CharToUtf8Pos(buf, documentLength, matchStart);
			int utf8End = mvceditor::CharToUtf8Pos(buf, documentLength, matchStart + matchLength);

			StartStyling(utf8Start, WordHighlightStyle);
			SetStyling(utf8End - utf8Start, WordHighlightStyle);
			WordHighlightPreviousIndex = matchStart - 1; // prevent infinite find previous's
			event.RequestMore();
		}
		else {
			WordHighlightPreviousIndex = -1;
		}
		delete[] buf;
	}
	event.Skip();
}

void mvceditor::CodeControlClass::MarkLintError(const pelet::LintResultsClass& result) {
	
	// positions in scintilla are byte offsets. convert chars to bytes so we can mark
	// the squigglies properly
	int byteNumber = 0;
	if (result.CharacterPosition >= 0) {
		MarkerAdd(result.LineNumber - 1, LINT_RESULT_MARKER);

		// we must share the indicator between highlight words functionality and
		// parse errors functionality.  This is because the HTML lexer uses 7 of the
		// eight style bits, leaving only one bit for indicators
		IndicatorSetStyle(INDICATOR, wxSTC_INDIC_SQUIGGLE);
		IndicatorSetForeground(INDICATOR, *wxRED);

		int charNumber = result.CharacterPosition;
		int errorLength = 5;

		int documentLength = GetTextLength();
		char* buf = new char[documentLength];
		
		// GET_TEXT  message
		SendMsg(2182, documentLength, (long)buf);	
		byteNumber = mvceditor::CharToUtf8Pos(buf, documentLength, charNumber);
		StartStyling(byteNumber, WordHighlightStyle);
		SetStyling(errorLength, WordHighlightStyle);

		GotoPos(byteNumber);

		delete[] buf;
	}
	Colourise(0, -1);	

	wxString error = mvceditor::IcuToWx(result.Error);
	CallTipShow(byteNumber, error);
}

void mvceditor::CodeControlClass::ClearLintErrors() {
	MarkerDeleteAll(LINT_RESULT_MARKER);
	StartStyling(0, WordHighlightStyle);
	SetStyling(GetLength(), 0);
}

void mvceditor::CodeControlClass::SetCurrentDbTag(const mvceditor::DatabaseTagClass& currentDbTag) {
	CurrentDbTag.Copy(currentDbTag);
	
	// if SQL document is active we need to change the currentInfo in that object
	// but since C++ does has poor RTTI we dont know what type Document pointer currently is
	// for now just refresh everything which will update CurrentDbTag
	ApplyPreferences();
}

void mvceditor::CodeControlClass::SetDocumentMode(Mode mode) {
	DocumentMode = mode;
	ApplyPreferences();
}

mvceditor::CodeControlClass::Mode mvceditor::CodeControlClass::GetDocumentMode() {
	return DocumentMode;
}

void mvceditor::CodeControlClass::OnDwellStart(wxStyledTextEvent& event) {
	if (event.GetEventObject() != this || IsHidden) {
		event.Skip();
		return;
	}
	/*
	 * do not use wxTipWindow 
	 * there is a crash bug  with wxTipWindow
	 * 
	 * http://trac.wxwidgets.org/ticket/11125
	 *
	 * use the wxStyledTextCtrl call tip instead 
	 */
	if (DocumentMode == PHP) {
		int pos = event.GetPosition();

		// if the cursor is in the middle of an identifier, find the end of the
		// current identifier; that way we can know the full name of the tag we want
		// to get
		int endPos = WordEndPosition(pos, true);
		std::vector<mvceditor::TagClass> matches = ((PhpDocumentClass *)Document)->GetSymbolAt(endPos);
		if (!matches.empty()) {
			mvceditor::TagClass tag = matches[0];
			wxString msg;
			if (tag.Type == mvceditor::TagClass::FUNCTION) {
				msg = mvceditor::IcuToWx(tag.Identifier);
				msg += wxT("\n\n");
				msg += mvceditor::IcuToWx(tag.Signature);
				msg += wxT(" [ ");
				msg += mvceditor::IcuToWx(tag.ReturnType);
				msg += wxT(" ]");
				
			}
			else if (tag.Type == mvceditor::TagClass::METHOD) {
				msg = mvceditor::IcuToWx(tag.ClassName);
				msg += wxT("::");
				msg += mvceditor::IcuToWx(tag.Identifier);
				msg += wxT("\n\n");
				msg += mvceditor::IcuToWx(tag.Signature);
				if (!tag.ReturnType.isEmpty()) {
					msg += wxT(" [ ");
					msg += mvceditor::IcuToWx(tag.ReturnType);
					msg += wxT(" ]");	
				}
			}
			else if (tag.Type == mvceditor::TagClass::MEMBER || tag.Type == mvceditor::TagClass::CLASS_CONSTANT) {
				msg = mvceditor::IcuToWx(tag.ClassName);
				msg += wxT("::");
				msg += mvceditor::IcuToWx(tag.Identifier);
				msg += wxT("\n\n");
				msg += mvceditor::IcuToWx(tag.Signature);
				if (!tag.ReturnType.isEmpty()) {
					msg += wxT(" [ ");
					msg += mvceditor::IcuToWx(tag.ReturnType);
					msg += wxT(" ]");	
				}
			}
			else {
				msg = mvceditor::IcuToWx(tag.Identifier);
				msg += wxT("\n\n");
				msg += mvceditor::IcuToWx(tag.Signature);
			}
			if (!tag.Comment.isEmpty()) {
				msg += wxT("\n\n");
				msg += NiceDocText(tag.Comment);
			}
			if (!msg.IsEmpty()) {
				if (CallTipActive()) {
					CallTipCancel();
				}
				CallTipShow(event.GetPosition(), msg);
			}
		}
	}
	event.Skip();
}

void mvceditor::CodeControlClass::OnDwellEnd(wxStyledTextEvent& event) {
	CallTipCancel();
}

int mvceditor::CodeControlClass::LineFromCharacter(int charPos) {
	int documentLength = GetTextLength();
	char* buf = new char[documentLength];
	
	// GET_TEXT  message
	SendMsg(2182, documentLength, (long)buf);
	int pos = mvceditor::CharToUtf8Pos(buf, documentLength, charPos);
	delete[] buf;
	return LineFromPosition(pos);
}

void mvceditor::CodeControlClass::SetAsHidden(bool isHidden) {
	IsHidden = isHidden;

	// in case tool tip, auto complete lists that are currently active
	if (IsHidden && CallTipActive()) {
		CallTipCancel();
	}
	if (IsHidden && AutoCompActive()) {
		AutoCompCancel();
	}
}

void mvceditor::CodeControlClass::TrimTrailingSpaces() {
	int maxLines = GetLineCount();
	for (int line = 0; line < maxLines; line++) {
		int lineStart = PositionFromLine(line);
		int lineEnd = GetLineEndPosition(line);
		int i = lineEnd - 1;
		char ch = (char)GetCharAt(i);
		while ((i >= lineStart) && ((ch == ' ') || (ch == '\t'))) {
			i--;
			ch = GetCharAt(i);
		}
		if (i < (lineEnd - 1)) {
			SetTargetStart(i + 1);
			SetTargetEnd(lineEnd);
			ReplaceTarget(wxT(""));
		}
	}
}

void mvceditor::CodeControlClass::RemoveTrailingBlankLines() {
	if (DocumentMode != PHP) {
		return;
	}

	// search backwards from the end of the file
	// if the first non-whitespace characters are "?>" then
	// we remove all space characters
	int maxPos = GetTextLength();
	char c = 0, prev = 0;
	bool done = false;
	bool seenAngleBracket = false;
	bool seenQuestion = false;
	for (int i = maxPos - 1; i >= 0 && !done; i--) {
		if ('>' == prev && '?' == c) {

			// if we get here then the first non-space characters that were found were
			// the PHP end tags
			// 3 = don't remove the ending PHP tag
			SetTargetStart(i + 3);
			SetTargetEnd(maxPos);
			ReplaceTarget(wxT(""));
			done = true;
		}
		prev = c;
		c = GetCharAt(i);

		// once we see an angle bracket, we come to our decision
		// if we dont do this, then a single angle bracket will get removed
		// for example when the file ends like this:
		// "?>
		// \n
		// \n>"
		
		if ('>' == c && seenAngleBracket) {
			done = true;
		}
		else if ('>' == c && !seenAngleBracket) {
			seenAngleBracket = true;
		}
		if ('?' == c && seenQuestion) {
			done = true;
		}
		else if ('?' == c && !seenQuestion) {
			seenQuestion = true;
		}

		// if we hit anything other than a space character or the PHP end
		// tag, then don't modify anything
		if (!isspace(c) && c != '?' && c != '>') {
			done = true;
		}
	}
}

wxString mvceditor::CodeControlClass::GetIdString() const {
	wxString idString = wxString::Format(wxT("File_%d"), GetId());
	return idString;
}

BEGIN_EVENT_TABLE(mvceditor::CodeControlClass, wxStyledTextCtrl)
	EVT_STC_MARGINCLICK(wxID_ANY, mvceditor::CodeControlClass::OnMarginClick)
	EVT_STC_DOUBLECLICK(wxID_ANY, mvceditor::CodeControlClass::OnDoubleClick)
	EVT_CONTEXT_MENU(mvceditor::CodeControlClass::OnContextMenu)
	EVT_IDLE(mvceditor::CodeControlClass::OnIdle)
	EVT_IDLE(mvceditor::CodeControlClass::WordHiglightPreviousSearch)
	EVT_IDLE(mvceditor::CodeControlClass::WordHiglightForwardSearch)
	EVT_STC_CHARADDED(wxID_ANY, mvceditor::CodeControlClass::OnCharAdded)
	EVT_STC_UPDATEUI(wxID_ANY, mvceditor::CodeControlClass::OnUpdateUi) 
	EVT_LEFT_DOWN(mvceditor::CodeControlClass::OnLeftDown)
	EVT_KEY_DOWN(mvceditor::CodeControlClass::OnKeyDown)
	EVT_STC_DWELLSTART(wxID_ANY, mvceditor::CodeControlClass::OnDwellStart)
	EVT_STC_DWELLEND(wxID_ANY, mvceditor::CodeControlClass::OnDwellEnd)
END_EVENT_TABLE()