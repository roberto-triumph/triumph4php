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
#include <widgets/CodeControlClass.h>
#include <windows/StringHelperClass.h>
#include <wx/filename.h>
#include <wx/stc/stc.h>
#include <wx/regex.h>
#include <wx/tipwin.h>
#include <wx/utils.h>
#include <unicode/ustring.h>
#include <algorithm>

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

mvceditor::CodeControlClass::CodeControlClass(wxWindow* parent, CodeControlOptionsClass& options,  ProjectClass* project, 
			mvceditor::ResourceUpdateThreadClass* resourceUpdates,
			int id, const wxPoint& position, const wxSize& size, long style,
			const wxString& name)
		: wxStyledTextCtrl(parent, id, position, size, style, name)
		, CurrentFilename()
		, CurrentSignature()
		, FileIdentifier()
		, Timer()
		, CodeControlOptions(options)
		, WordHighlightFinder()
		, WordHighlightWord()
		, CurrentInfo()
		, ResourceUpdates(resourceUpdates)
		, Project(project)
		, WordHighlightPreviousIndex(-1)
		, WordHighlightNextIndex(-1)
		, WordHighlightStyle(0)
		, ModifiedDialogOpen(false)
		, WordHighlightIsWordHighlighted(false)
		, NeedToUpdateResources(false)
		, DocumentMode(TEXT) {
	Document = NULL;
	
	// we will handle right-click menu ourselves
	UsePopUp(false);
	SetYCaretPolicy(wxSTC_CARET_EVEN, 0);
	ApplyPreferences();
	SetMouseDwellTime(1500);
	Timer.SetOwner(this);
	Timer.Start(500, false);
	
	// need to give this new file unique name so because the
	// resource updates object needs a unique name
	wxLongLong time = wxGetLocalTimeMillis();
	FileIdentifier = wxString::Format(wxT("File_%s"), time.ToString().c_str());
	
}

bool mvceditor::CodeControlClass::LoadAndTrackFile(const wxString& filename) {
	UnicodeString fileContents;
	wxFileName file(filename);
	bool ret = false;
	// not using LoadFile() because it does not correctly handle files with high ascii characters
	if (file.IsOk() && file.IsFileReadable()) {
		FindInFilesClass::FileContents(filename, fileContents);
		
		// lets avoid the IcuToWx to prevent going from 
		// UnicodeString -> UTF8 -> wxString  -> UTF8 -> Sciintilla
		// cost of translation could be big for big sized files
		// because of the double encoding due to all three libraries using
		// different internal encodings for their strings
		UErrorCode status = U_ZERO_ERROR;
		int32_t rawLength;
		int32_t length = fileContents.length();
		const UChar* src = fileContents.getBuffer();
		u_strToUTF8(NULL, 0, &rawLength, src, length, &status);
		status = U_ZERO_ERROR;
		char* dest = new char[rawLength + 1];
		int32_t written;
		u_strToUTF8(dest, rawLength + 1, &written, src, length, &status);
		if(U_SUCCESS(status)) {
			
			// SetText message
			SendMsg(2181, 0, (long)(const char*)dest);
			
			EmptyUndoBuffer();
			SetSavePoint();
			NeedToUpdateResources = false;
			CurrentFilename = filename;
			
			// important to set the fileIdentifier to the name;
			// the ResourceUpdates object will need to know what files are being edited so it can mark them as 'dirty'
			ResourceUpdates->Unregister(FileIdentifier);
			FileIdentifier = filename;
			
			FileOpenedDateTime = file.GetModificationTime();
			AutoDetectDocumentMode();
		}
		delete[] dest;
		ret = true;
	}
	return ret;
}

void mvceditor::CodeControlClass::Revert() {

	// TODO: when a file is reverted the editor crashes
	// seen this on Win32
	if (!IsNew()) {
		if (ResourceUpdates) {
			ResourceUpdates->Unregister(FileIdentifier);
		}
		NeedToUpdateResources = true;
		LoadAndTrackFile(CurrentFilename);
	}
}

bool mvceditor::CodeControlClass::IsNew() const {
	return CurrentFilename.empty();
}

bool mvceditor::CodeControlClass::SaveAndTrackFile(wxString newFilename) {
	bool saved = false;
	
	// when saving, update the internal timestamp so that the OnIdle logic works correctly
	if (!CurrentFilename.empty()) {
		saved = SaveFile(CurrentFilename);
	}
	else if (SaveFile(newFilename)) {
		CurrentFilename = newFilename;
		saved = true;
	}
	if (saved) {
		wxFileName file(CurrentFilename);
		FileOpenedDateTime = file.GetModificationTime();
		
		// when a file is saved update the resource cache
		// TODO: this causes a crash in the UCharBufferedFile class when
		// seems to be that the Save event (from the menu) and the key
		// event from the keyboard are getting executed simultaneously
		// since UCharBufferedFile is no reentrant, especially when here 
		// a File is given and in the key event (auto complete) a string is given
		//mvceditor::ResourceFinderClass* finder = Project->GetResourceFinder();
		//if (finder != NULL) {
		//	finder->Walk(CurrentFilename);
		//}
		
		// important to set the fileIdentifier to the name;
		// the ResourceUpdates object will need to know what files are being edited so it can mark them as 'dirty'
		ResourceUpdates->Unregister(FileIdentifier);
		FileIdentifier = newFilename;
		
		// if the file extension changed let's update the code control appropriate
		// for example if a .txt file was saved as a .sql file
		AutoDetectDocumentMode();
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
	
	SetSelection(StringHelperClass::CharToUtf8Pos(buf, documentLength, start), 
		StringHelperClass::CharToUtf8Pos(buf, documentLength, end));
	delete[] buf;
}

void mvceditor::CodeControlClass::OnCharAdded(wxStyledTextEvent &event) {
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

wxString mvceditor::CodeControlClass::GetCurrentSymbol() {
	return GetSymbolAt(GetCurrentPos());
}

wxString mvceditor::CodeControlClass::GetSymbolAt(int posToCheck) {
	ResourceFinderClass* resourceFinder = Project->GetResourceFinder();
	mvceditor::SymbolClass symbol;
	UnicodeString code = GetSafeText();
	UnicodeString symbolName = ResourceUpdates->Worker.GetSymbolAt(FileIdentifier, posToCheck, resourceFinder, symbol, code);
	if (symbolName.isEmpty()) {
		int startPos = WordStartPosition(posToCheck, true);
		int endPos = WordEndPosition(posToCheck, true);
		symbolName = GetSafeSubstring(startPos, endPos);	
	}
	return StringHelperClass::IcuToWx(symbolName);
}

void mvceditor::CodeControlClass::HandleAutoCompletion() {
	if (Document->CanAutoComplete()) {
		int currentPos = GetCurrentPos();
		int startPos = WordStartPosition(currentPos, true);
		int endPos = WordEndPosition(currentPos, true);
		UnicodeString symbol = 	GetSafeSubstring(startPos, endPos);
		UnicodeString code = GetSafeSubstring(0, currentPos + 1);
		
		wxString fileName = CurrentFilename;
		std::vector<wxString> autoCompleteList = Document->HandleAutoComplete(fileName, code, symbol);
		if (!autoCompleteList.empty()) {
			
			// scintilla needs the keywords sorted.
			sort(autoCompleteList.begin(), autoCompleteList.end());
			wxString list;
			for (size_t i = 0; i < autoCompleteList.size(); ++i) {
				list += wxT(" ");
				list += autoCompleteList[i];
			}
			AutoCompSetMaxWidth(0);
			int wordLength = currentPos - startPos;
			AutoCompShow(wordLength, list);
		}
	}
}

bool mvceditor::CodeControlClass::PositionedAtVariable(int pos) {
	int start = WordStartPosition(pos, true);
	return GetCharAt(start) == wxT('$');
}

void mvceditor::CodeControlClass::HandleCallTip(wxChar ch, bool force) {
	if (!ResourceUpdates) {
		return;
	}
	
	// this function deliberately uses scintilla positions (bytes) instead of 
	// converting over to unicode text. be careful.
	int currentPos = GetCurrentPos();
	if (InCommentOrStringStyle(currentPos) && wxT(')') == ch) {
		CallTipCancel();
	}
	if (force || wxT('(') == ch) {
		
		// back up to the last function call "(" then get the function name, do not get the open parenthesis
		/// we are always going to do the call tip for the nearest function ie. when
		// a line is  
		// Func1('hello', Func2('bye'
		// we are always going to show the call tip for Func2 (if the cursor is after 'bye')
		// make sure we don't go past the last statement
		while (currentPos >= 0) {
			char c = GetCharAt(currentPos);
			if (!InCommentOrStringStyle(currentPos)) {
				if ('(' == c) {
					break;
				}
				if (';' == c) {
					currentPos = -1;
					break;
				}
			}
			currentPos--;
		}
		
		if (currentPos >= 0) {
			wxString symbol = GetSymbolAt(currentPos);
			CurrentSignature = wxT("");
			ResourceFinderClass* globalResourceFinder = Project->GetResourceFinder();
			if (ResourceUpdates->Worker.PrepareAll(globalResourceFinder, symbol)) {
				
				// highly unlikely that there is more than one match since we searched for a full name (lookup succeeded).
				// use CollectNearMatchResources to handle the case when  the method is inherited
				if (ResourceUpdates->Worker.CollectNearMatchResourcesFromAll(globalResourceFinder)) {
					std::vector<mvceditor::ResourceClass> matches = ResourceUpdates->Worker.Matches(globalResourceFinder);
					if (matches.size() == 1) {
						mvceditor::ResourceClass resource = matches[0];
						UnicodeString fullQualifiedResource =  resource.Resource;
						CurrentSignature = mvceditor::StringHelperClass::IcuToWx(resource.Signature);
					}
				}
			}
			if (!CurrentSignature.IsEmpty()) {
				CallTipShow(GetCurrentPos(), CurrentSignature);
			}
		}
	}
	if (CallTipActive()) {
		
		// highlight the 1st, 2nd, 3rd or 4th parameter of the call tip depending on where the cursors currently is.
		// If the cursor is in the 2nd argument, then highlight the 2nd parameter and so on...
		int startOfArguments = GetCurrentPos();
		int commaCount = 0;
		while (startOfArguments >= 0) {
			char c = GetCharAt(startOfArguments);
			if (!InCommentOrStringStyle(startOfArguments)) {
				if ('(' == c) {
					break;
				}
				if (';' == c) {
					startOfArguments = -1;
					break;
				}
				if (',' == c) {
					commaCount++;
				}
			}
			startOfArguments--;
		}
		if (startOfArguments >= 0) {
			int startHighlightPos = CurrentSignature.find(wxT('('));
			
			// sometimes the previous call tip is active, as in for example this line
			//   $m->getB()->work(
			// in this case we need to be careful
			if(startHighlightPos >= 0) {
				int endHighlightPos = CurrentSignature.find(wxT(','), startHighlightPos);
				
				// no comma =  highlight the only param, if signature has no params, then 
				// nothing will get highlighted since its all whitespace
				if (endHighlightPos < 0) {
					endHighlightPos = CurrentSignature.length() - 1;
				}
				while (commaCount > 0) {
					startHighlightPos = endHighlightPos;
					endHighlightPos = CurrentSignature.find(wxT(','), startHighlightPos + 1);
					if (-1 == endHighlightPos) {
						endHighlightPos = CurrentSignature.length() - 1;
					}
					--commaCount;
				}
				CallTipSetHighlight(startHighlightPos, endHighlightPos);
			}
		}
	}
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

bool mvceditor::CodeControlClass::InCommentOrStringStyle(int posToCheck) {
	int style = GetStyleAt(posToCheck);
	int prevStyle = GetStyleAt(posToCheck - 1);

	// dont match braces inside strings or comments. for some reason when styling line comments (//)
	// the last character is styled as default but the characters before are styled correctly (wxSTC_HPHP_COMMENTLINE)
	// so lets check the previous character in that case
	return wxSTC_HPHP_HSTRING == style || wxSTC_HPHP_SIMPLESTRING == style || wxSTC_HPHP_COMMENT == style
		|| wxSTC_HPHP_COMMENTLINE == style || wxSTC_HPHP_COMMENTLINE == prevStyle;
}

void mvceditor::CodeControlClass::MatchBraces(int posToCheck) {
	if (!InCommentOrStringStyle(posToCheck)) {
		wxChar c1 = GetCharAt(posToCheck),
		            c2 = GetCharAt(posToCheck - 1);
		if (wxT('{') == c1 || wxT('}') == c1 || wxT('(') == c1 || wxT(')') == c1 || wxT('[') == c1 || wxT(']') == c1) {
			posToCheck = posToCheck;
		}
		else if (wxT('{') == c2 || wxT('}') == c2 || wxT('(') == c2 || wxT(')') == c2 || wxT('[') == c2 || wxT(']') == c2) {
			posToCheck = posToCheck - 1;
		}
		else  {
			posToCheck = -1;
		}
		if (posToCheck >= 0) {
			int pos = 	BraceMatch(posToCheck);
			if (wxSTC_INVALID_POSITION == pos) {
				BraceBadLight(posToCheck);
			}
			else {
				BraceHighlight(posToCheck, pos);
			}
		}
		else {
			BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
		}
	}
	else {
		BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
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
	wxFileName name(file);
	wxString ext = name.GetExt();
	if (ext.CmpNoCase(wxT("sql")) == 0) {
		SetDocumentMode(mvceditor::CodeControlClass::SQL);
	}
	else if (ext.CmpNoCase(wxT("php")) == 0 || 
			ext.CmpNoCase(wxT("phtml")) == 0 || 
			ext.CmpNoCase(wxT("html")) == 0) {
				
		// TODO: someway for the user to change this
		// *.inc endings are common
		SetDocumentMode(mvceditor::CodeControlClass::PHP);
	}
	else if (ext.CmpNoCase(wxT("css")) == 0) {
		SetDocumentMode(mvceditor::CodeControlClass::CSS);
	}
	else {
		SetDocumentMode(mvceditor::CodeControlClass::TEXT);
	}
}

void mvceditor::CodeControlClass::ApplyPreferences() {
	SetMargin();
	if (Document) {
		delete Document;
		Document = NULL;
	}
	if (mvceditor::CodeControlClass::SQL == DocumentMode) {
		Document = new mvceditor::SqlDocumentClass(Project, CurrentInfo);
		SetCodeControlOptions(CodeControlOptions.SqlStyles);
		SetSqlOptions();		
		
	}
	else if (mvceditor::CodeControlClass::PHP == DocumentMode) {
		Document = new mvceditor::PhpDocumentClass(Project, ResourceUpdates);
		SetCodeControlOptions(CodeControlOptions.PhpStyles);
		SetPhpOptions();
		if (ResourceUpdates) {
			ResourceUpdates->Register(FileIdentifier, this);
		}
	}
	else if (mvceditor::CodeControlClass::CSS == DocumentMode) {
		Document = new mvceditor::CssDocumentClass();
		SetCodeControlOptions(CodeControlOptions.CssStyles);
		SetCssOptions();
		
	}
	else {
		std::vector<mvceditor::StylePreferenceClass> noStyles;
		SetCodeControlOptions(noStyles);
		Document = new mvceditor::TextDocumentClass();
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
	AutoCompStops(wxT("!@#$%^&*()_+-=[]\\{}|;'\",./<?"));
	AutoCompSetSeparator(' ');
	AutoCompSetFillUps(wxT("(["));
	AutoCompSetIgnoreCase(true);
	SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$"));
	
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
				wxSTC_STYLE_DEFAULT,
				wxSTC_HJ_START, wxSTC_HJ_DEFAULT, wxSTC_HJ_COMMENT,
				wxSTC_HJ_COMMENTLINE, wxSTC_HJ_COMMENTDOC, wxSTC_HJ_NUMBER,
				wxSTC_HJ_WORD, wxSTC_HJ_KEYWORD, wxSTC_HJ_DOUBLESTRING,
				wxSTC_HJ_SINGLESTRING, wxSTC_HJ_SYMBOLS, wxSTC_HJ_STRINGEOL,
				wxSTC_HJ_REGEX
			};
			for (int j = 0; j < 14; ++j) {
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
	int charEndIndex = 0;
	
	// pos, endPos are byte offsets into the UTF-8 string, need to convert to char numbers
	int documentLength = GetTextLength();
	char* buf = new char[documentLength];
		
	// GET_TEXT  message
	SendMsg(2182, documentLength, (long)buf);
	charStartIndex = mvceditor::StringHelperClass::Utf8PosToChar(buf, documentLength, pos);
	charEndIndex = mvceditor::StringHelperClass::Utf8PosToChar(buf, documentLength, endPos);
	
	UnicodeString word = GetSafeSubstring(pos, endPos);
	if (!word.isEmpty()) {
		WordHighlightFinder.Expression = word;
		WordHighlightFinder.Mode = mvceditor::FinderClass::EXACT;
		WordHighlightFinder.CaseSensitive = true;
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

	// Let the frame handle it because we want plugins to have menu items
	if (frame) {
		frame->ProcessEvent(event);
	}
	else {
		event.Skip();
	}
}

UnicodeString mvceditor::CodeControlClass::GetSafeText() {
	
	// copied from the implementation of GetText method in stc.cpp 
    int len  = GetTextLength();
	wxMemoryBuffer mbuf(len + 1);   // leave room for the null...
	char* buf = (char*)mbuf.GetWriteBuf(len + 1);
	SendMsg(2182, len + 1, (long)buf);
	mbuf.UngetWriteBuf(len);
	mbuf.AppendByte(0);
	UnicodeString str(' ', len, 0);
	int32_t written = 0;
	UErrorCode error = U_ZERO_ERROR;
	u_strFromUTF8(str.getBuffer(len + 1), len, &written, (const char*)mbuf, len, &error);
	str.releaseBuffer(written);
	assert(U_SUCCESS(error));
	return str;
}

UnicodeString mvceditor::CodeControlClass::GetSafeSubstring(int startPos, int endPos) {
	wxString s = GetTextRange(startPos, endPos);
	UnicodeString ret = mvceditor::StringHelperClass::wxToIcu(s);
	return ret;
}

void mvceditor::CodeControlClass::OnIdle(wxIdleEvent& event) {
	if (!IsNew() && !ModifiedDialogOpen) {
		if (wxFileName::FileExists(CurrentFilename)) {
			wxFileName file(CurrentFilename);
			wxDateTime modifiedDateTime = file.GetModificationTime();
			if (modifiedDateTime.IsLaterThan(FileOpenedDateTime)) {
				ModifiedDialogOpen = true;
				wxString message = CurrentFilename;
				message += _("\n\nFile has been modified externally. Reload file and lose any changes?\n");
				message += _("Yes will reload file, No will allow you to override the file.");
				int res = wxMessageBox(message, _("Warning"), 
					wxYES_NO | wxICON_QUESTION, this);
				if (wxYES == res) {
					LoadAndTrackFile(CurrentFilename);
				}
				else {

					// so that next idle event user does not get asked the same question again
					FileOpenedDateTime = modifiedDateTime;
				}
				ModifiedDialogOpen = false;
			}
		}
		// else file has been removed.  most likely the user know about it so don't bother telling them
		// explictly checking for file existence because GetModificationTime requires the file to exist
	}
	
	// moving match braces here from UpdateUi because when I put this code in UpdateUi there
	// is a bad flicker in GTK when the braces are highlighted.
	// Code folding seems to make the flicker appear all the time.
	MatchBraces(GetCurrentPos());
	event.Skip();
}

void mvceditor::CodeControlClass::OnKeyDown(wxKeyEvent& event) {
	UndoHighlight();
	
	// if already parsing then dont do anything
	if (ResourceUpdates->GetThread() && ResourceUpdates->GetThread()->IsRunning()) {
		event.Skip();
		return;
	}
	// only update the dirty bit on a letter, number of backspace
	// keypress.  disregard shortcut keystrokes
	// we want to keep the re-parsings to a minimum
	// we dont need to reparse when user is adding a 
	// comment or a constant string
	if (!InCommentOrStringStyle(GetCurrentPos())) {
		int modifiers = event.GetModifiers();
		if (modifiers == wxMOD_SHIFT || modifiers == wxMOD_NONE) {
			int keyCode = event.GetKeyCode();
			if ((keyCode >= 'a'  && keyCode <= 'z') ||
					(keyCode >= 'A'  && keyCode <= 'Z') ||
					(keyCode >= '0'  && keyCode <= '9') ||
					'_' == keyCode || 
					'{' == keyCode || 
					'}' == keyCode ||
					'(' == keyCode ||
					')' == keyCode ||
					WXK_DELETE == keyCode ||
					WXK_BACK == keyCode) {
				NeedToUpdateResources = true;
			}
		}
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
			int utf8Start = mvceditor::StringHelperClass::CharToUtf8Pos(buf, documentLength, matchStart);
			int utf8End = mvceditor::StringHelperClass::CharToUtf8Pos(buf, documentLength, matchStart + matchLength);

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
			int utf8Start = mvceditor::StringHelperClass::CharToUtf8Pos(buf, documentLength, matchStart);
			int utf8End = mvceditor::StringHelperClass::CharToUtf8Pos(buf, documentLength, matchStart + matchLength);

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

void mvceditor::CodeControlClass::MarkLintError(const mvceditor::LintResultsClass& result) {
	
	// positions in scintilla are byte offsets. convert chars to bytes so we can mark
	// the squigglies properly
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
		int byteNumber = mvceditor::StringHelperClass::CharToUtf8Pos(buf, documentLength, charNumber);
		StartStyling(byteNumber, WordHighlightStyle);
		SetStyling(errorLength, WordHighlightStyle);

		GotoPos(byteNumber);

		delete[] buf;
	}
	Colourise(0, -1);	
}

void mvceditor::CodeControlClass::ClearLintErrors() {
	MarkerDeleteAll(LINT_RESULT_MARKER);
	StartStyling(0, WordHighlightStyle);
	SetStyling(GetLength(), 0);
}

void mvceditor::CodeControlClass::OnClose(wxCloseEvent& event) {
	delete Document;
	Document = NULL;
	Timer.Stop();
	if (ResourceUpdates) {
		ResourceUpdates->Unregister(FileIdentifier);
	}
	event.Skip();
}

void mvceditor::CodeControlClass::SetCurrentInfo(const mvceditor::DatabaseInfoClass& currentInfo) {
	CurrentInfo.Copy(currentInfo);
	
	// if SQL document is active we need to change the currentInfo in that object
	// but since C++ does has poor RTTI we dont know what type Document pointer currently is
	// for now just refresh everything which will update CurrentInfo
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
	if (!ResourceUpdates) {
		event.Skip();
		return;
	}
	/*
	 * do not use wxTipWindow 
	 * there is a crash bug  with wxTipWindow
	 * 
	 * http://trac.wxwidgets.org/ticket/11125
	 * 
	mvceditor::ResourceFinderClass* globalResourceFinder = Project->GetResourceFinder();
	if (DocumentMode == PHP && globalResourceFinder) {
		int pos = event.GetPosition();
		wxString symbol = GetSymbolAt(pos);
		if (!symbol.IsEmpty()) {
			if (ResourceUpdates->PrepareAll(globalResourceFinder, symbol)) {
				if (ResourceUpdates->CollectFullyQualifiedResourceFromAll(globalResourceFinder)) {
					std::vector<mvceditor::ResourceFinderClass*> finders = ResourceUpdates->Iterator(globalResourceFinder);
					for (size_t r = 0; r < finders.size(); r++) {
						mvceditor::ResourceFinderClass* finder = finders[r];
						mvceditor::ResourceClass resource = finder->GetResourceMatch(0);					
						bool found = FALSE != resource.Identifier.isEmpty();
						if (found) {
							wxString msg = mvceditor::StringHelperClass::IcuToWx(resource.Identifier);
							if (resource.Type == mvceditor::ResourceClass::FUNCTION) {
								msg += wxT("\n\n");
								msg += mvceditor::StringHelperClass::IcuToWx(resource.ReturnType);
								msg += wxT(" ");
								msg += mvceditor::StringHelperClass::IcuToWx(resource.Signature);
							}
							else if (resource.Type == mvceditor::ResourceClass::METHOD) {
								msg += wxT("\n\n");
								msg += mvceditor::StringHelperClass::IcuToWx(resource.ReturnType);
								msg += wxT(" ");
								msg += mvceditor::StringHelperClass::IcuToWx(resource.Signature);
							}
							else {
								msg += wxT("\n\n");
								msg += mvceditor::StringHelperClass::IcuToWx(resource.Signature);
							}
							if (!resource.Comment.isEmpty()) {
								msg += wxT("\n\n");
								msg += mvceditor::StringHelperClass::IcuToWx(resource.Comment);
							}
							if (!msg.IsEmpty()) {
								new wxTipWindow(this, msg, 
									wxCoord(400), NULL);
							}
							break;
						}
					}
				}
			}
		}
	}*/
	event.Skip();
}

void mvceditor::CodeControlClass::OnDwellEnd(wxStyledTextEvent& event) {

}

void mvceditor::CodeControlClass::OnResourceUpdateComplete(wxCommandEvent& event) {
	Timer.Start();
	event.Skip();
}
void mvceditor::CodeControlClass::OnTimer(wxTimerEvent& event) {
	if (NeedToUpdateResources && ResourceUpdates && (!ResourceUpdates->GetThread() || !ResourceUpdates->GetThread()->IsRunning())) {
		UnicodeString text = GetSafeText();
		wxThreadError error = ResourceUpdates->StartBackgroundUpdate(FileIdentifier, text);
		if (wxTHREAD_RUNNING == error) {
			wxMessageBox(_("Indexing is already taking place. Please wait."), wxT("Warning"), wxICON_EXCLAMATION);
		}
		else if (wxTHREAD_NO_RESOURCE == error) {
			wxMessageBox(_("Your system is low on resources. Try again later."), wxT("Warning"), wxICON_EXCLAMATION);
		}
		NeedToUpdateResources = false;
		Timer.Stop();
	}
	event.Skip();
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
	EVT_CLOSE(mvceditor::CodeControlClass::OnClose)
	EVT_STC_DWELLSTART(wxID_ANY, mvceditor::CodeControlClass::OnDwellStart)
	EVT_STC_DWELLEND(wxID_ANY, mvceditor::CodeControlClass::OnDwellEnd)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::CodeControlClass::OnResourceUpdateComplete)
	EVT_TIMER(wxID_ANY, mvceditor::CodeControlClass::OnTimer)
END_EVENT_TABLE()
