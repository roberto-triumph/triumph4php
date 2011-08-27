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
 * @author     $Author: robertop2004@gmail.com $
 * @date       $Date: 2011-08-20 13:19:24 -0700 (Sat, 20 Aug 2011) $
 * @version    $Rev: 598 $ 
 */
#include <widgets/CodeControlClass.h>
#include <windows/StringHelperClass.h>
#include <wx/filename.h>
#include <wx/stc/stc.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include <unicode/ustring.h>
#include <algorithm>

// IMPLEMENTATION NOTE:
// Take care when using positions given by Scintilla.  Scintilla gives positions in bytes while wxString and UnicodeString
// use character positions. Take caution when using methods like GetCurrentPos(), WordStartPosition(), WordEndPosition()
// This causes problems when Scintilla is handling UTF-8 documents.
// There is a method called GetSafeSubString() that will help you in this regard.

// want to support both HTML4 and HTML5, using both sets of keywords.
static const wxString HTML_KEYWORDS = wxString::FromAscii(

	// HTML4 tag names, found at http://www.w3.org/TR/html4/index/elements.html
	"a abbr acronym address applet area b base basefont bdo big blockquote body br button caption center cite code col colgroup " 
	"dd del dfn dir div dl dt em fieldset font form frame frameset h1 h2 h3 h4 h5 h6 head hr html i iframe img input ins isindex kbd "
	"label legend li link map menu meta noframes noscript object ol optgroup option p param pre q s samp script select small span strike "
	"strong style sub sup table tbody td textarea tfoot th thead title tr tt u ul var"

	// HTML5 tag names found at http://www.w3.org/TR/html5/index.html#elements-1
	// but only the ones not already above
	"!doctype article aside audio b base bdi canvas cite code "
	"command datalist details embed figcaption figure footer "
	"header hgroup keygen mark "
	"meter nav output progress rp rt ruby section "
	"source summary time track video wbr"

	// HTML4 attributes, found at http://www.w3.org/TR/html4/index/attributes.html
	"abbr accept-charset accept accesskey action align alink alt archive axis background bgcolor border cellpadding "
	"cellspacing char charoff charset checked cite class classid clear code codebase codetype color cols colspan compact "
	"content coords data datetime declare defer dir disabled enctype face for frame frameborder headers href hreflang "
	"hspace http-equiv id ismap label lang language link longdesc marginheight marginwidth maxlength media method multiple "
	"name nohref noresize noshade nowrap object "
	"onblur onchange onclick ondblclick onfocus onkeydown onkeypress onkeyup "
	"onload onmousedown onmousemove onmouseout onmouseover onmouseup onreset onselect onsubmit onunload "
	"profile prompt "
	"readonly rel rev rows rowspan rules scheme scope scrolling selected shape shape size span src standby start style "
	"summary tabindex target text title type usemap valign value valuetype version vlink vspace width"

	// includes HTML5 attributes, found at http://www.w3.org/TR/html5/index.html#attributes-1
	// but only the ones not already above
	"async autocomplete autofocus autoplay challenge "
	"contenteditable contextmenu controls default dirname "
	"draggable dropzone form formaction formenctype formmethod formnovalidate formtarget height hidden high "
	"icon keytype kind list loop low manifest max mediagroup "
	"min novalidate open optimum pattern placeholder poster preload pubdate radiogroup required reversed rows "
	"sandbox spellcheck scoped seamless sizes srcdoc srclang step "
	"wrap "
	"onabort onafterprint onbeforeprint onbeforeunload oncanplay oncanplaythrough oncontextmenu oncuechange "
	"ondrag ondragend ondragenter ondragleave ondragover ondragstart ondrop ondurationchange onemptied onended onerror "
	"onhashchange oninput oninvalid onloadeddata onloadedmetadata onloadstart "
	"onmessage onmousewheel onoffline ononline onpagehide onpageshow onpause "
	"onplay onplaying onpopstate onprogress onratechange onreadystatechange onredo onresize onscroll onseeked onseeking "
	"onshow onstalled onstorage onsubmit onsuspend ontimeupdate onundo onunload onvolumechange onwaiting"
);
static const wxString JAVASCRIPT_KEYWORDS = wxT("");

// margin 0 is taken up by line numbers, margin 1 is taken up by code folding. use
// margin 2 for lint error markers
static const int LINT_RESULT_MARKER = 2;
static const int LINT_RESULT_MARGIN = 2;

// we'll use up the first available marker & indicator for showing parse results
static const int INDICATOR = 0;

// 128 => 8th bit on since first 7 bits of style bits are for the lexer
static const int INDICATOR_STYLE = 128;

// wxSTC_HPHP_DEFAULT			PHP Default
// wxSTC_HPHP_HSTRING			PHP Double quoted String
// wxSTC_HPHP_SIMPLESTRING		PHP Single quoted string
// wxSTC_HPHP_WORD				PHP Keyword
// wxSTC_HPHP_NUMBER			PHP Number constant
// wxSTC_HPHP_VARIABLE			PHP Variable
// wxSTC_HPHP_COMMENT			PHP Comment
// wxSTC_HPHP_COMMENTLINE		PHP One line comment
// wxSTC_HPHP_HSTRING_VARIABLE 	PHP Variable in double quoted string
// wxSTC_HPHP_OPERATOR			PHP Operator
// wxSTC_STYLE_BRACELIGHT		Matching Braces
// wxSTC_STYLE_BRACEBAD			Mismatched brace
// wxSTC_STYLE_LINENUMBER		Line number margin
// wxSTC_STYLE_INDENT_GUIDE		Indentation guides
	
int mvceditor::CodeControlOptionsClass::ArrayIndexToStcConstant[] = {
	wxSTC_HPHP_DEFAULT,
	wxSTC_HPHP_HSTRING,
	wxSTC_HPHP_SIMPLESTRING,
	wxSTC_HPHP_WORD,
	wxSTC_HPHP_NUMBER,
	wxSTC_HPHP_VARIABLE,
	wxSTC_HPHP_COMMENT,
	wxSTC_HPHP_COMMENTLINE,
	wxSTC_HPHP_HSTRING_VARIABLE,
	wxSTC_HPHP_COMPLEX_VARIABLE,
	wxSTC_HPHP_OPERATOR,
	wxSTC_STYLE_BRACELIGHT,
	wxSTC_STYLE_BRACEBAD,
	wxSTC_STYLE_LINENUMBER,
	wxSTC_STYLE_INDENTGUIDE,
	CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET,
	CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE,
	CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION,
	CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING,
	CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN,
	CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT,

	// HTML lexer
	wxSTC_H_DEFAULT,
	wxSTC_H_TAG,
	wxSTC_H_TAGUNKNOWN,
	wxSTC_H_ATTRIBUTE,
	wxSTC_H_ATTRIBUTEUNKNOWN,
	wxSTC_H_NUMBER,
	wxSTC_H_DOUBLESTRING,
	wxSTC_H_SINGLESTRING,
	wxSTC_H_OTHER,
	wxSTC_H_COMMENT,
	wxSTC_H_ENTITY,
	wxSTC_H_VALUE,
	wxSTC_H_QUESTION, // <?php start tag
	wxSTC_H_TAGEND, // script end tag ?>

	// SQL lexer 
	wxSTC_SQL_DEFAULT,
	wxSTC_SQL_COMMENT,
	wxSTC_SQL_COMMENTLINE,
	wxSTC_SQL_COMMENTDOC,
	wxSTC_SQL_NUMBER,
	wxSTC_SQL_WORD,
	wxSTC_SQL_STRING,
	wxSTC_SQL_CHARACTER,
	wxSTC_SQL_OPERATOR,
	wxSTC_SQL_IDENTIFIER,
	wxSTC_SQL_COMMENTLINEDOC,
	wxSTC_SQL_WORD2,
	wxSTC_SQL_COMMENTDOCKEYWORD,
	wxSTC_SQL_COMMENTDOCKEYWORDERROR,
	wxSTC_SQL_USER1,
	wxSTC_SQL_USER2,
	wxSTC_SQL_USER3,
	wxSTC_SQL_USER4,
	wxSTC_SQL_QUOTEDIDENTIFIER
};

mvceditor::CodeControlClass::CodeControlClass(wxWindow* parent, CodeControlOptionsClass& options,  ProjectClass* project, 
			int id, const wxPoint& position, const wxSize& size, long style,
			const wxString& name)
		: wxStyledTextCtrl(parent, id, position, size, style, name)
		, SymbolTable()
		, CurrentFilename()
		, CurrentSignature()
		, CodeControlOptions(options)
		, WordHighlightFinder()
		, WordHighlightWord()
		, LanguageDiscovery()
		, WordHighlightPreviousIndex(-1)
		, WordHighlightNextIndex(-1)
		, Project(project)
		, ModifiedDialogOpen(false)
		, WordHighlightIsWordHighlighted(false) {
	SetLexer(wxSTC_LEX_HTML);
	UsePopUp(false);

	// 7 = as per scintilla docs, HTML lexer uses 7 bits for styles
	SetStyleBits(7);
	AutoCompStops(wxT("!@#$%^&*()_+-=[]\\{}|;':\",./<>?"));
	AutoCompSetSeparator(' ');
	AutoCompSetFillUps(wxT("(["));
	SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$"));
	wxString keywords = Project->GetPhpKeywords();
	SetKeyWords(4, keywords);
	SetYCaretPolicy(wxSTC_CARET_EVEN, 0);
	
	SetMarginType(LINT_RESULT_MARGIN, wxSTC_MARGIN_SYMBOL);
	SetMarginWidth(LINT_RESULT_MARGIN, 16);
	SetMarginSensitive(LINT_RESULT_MARGIN, false);
	SetMarginMask(LINT_RESULT_MARGIN, ~wxSTC_MASK_FOLDERS);

	MarkerDefine(LINT_RESULT_MARKER, wxSTC_MARK_ARROW, *wxRED, *wxRED);
	ApplyPreferences();
}

bool mvceditor::CodeControlClass::LoadPhpFile(const wxString& filename) {
	UnicodeString fileContents;
	wxFileName file(filename);
	// not using LoadFile() because it does not correctly handle files with high ascii characters
	if (file.IsOk() && file.IsFileReadable()) {
		FindInFilesClass::FileContents(filename, fileContents);
		SetText(StringHelperClass::IcuToWx(fileContents));
		EmptyUndoBuffer();
        SetSavePoint();
		int lexer = wxSTC_LEX_HTML;
		if (file.GetExt().CompareTo(wxT("sql")) == 0) {
			lexer = wxSTC_LEX_SQL;
		}
		SetLexer(lexer);
		Colourise(0, -1);
		CurrentFilename = filename;
		FileOpenedDateTime = file.GetModificationTime();
		return true;
	}
	return false;
}

void mvceditor::CodeControlClass::Revert() {
	if (!IsNew()) {
		LoadPhpFile(CurrentFilename);
	}
}

bool mvceditor::CodeControlClass::IsNew() const {
	return CurrentFilename.empty();
}

bool mvceditor::CodeControlClass::SavePhpFile(const wxString newFilename) {
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
		mvceditor::ResourceFinderClass* finder = Project->GetResourceFinder();
		if (finder != NULL) {
			finder->Walk(CurrentFilename);
		}
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
		HandleAutoCompletion(false);
	}
	HandleCallTip(ch);
	
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
	int pos = WordStartPosition(GetCurrentPos(), true);
	int endPos = WordEndPosition(GetCurrentPos(), true);
	UnicodeString symbol = GetSafeSubstring(pos, endPos);
	UnicodeString codeText = GetSafeText();
	ResourceFinderClass* resourceFinder = Project->GetResourceFinder();
	wxString fileName = CurrentFilename.IsEmpty() ? wxT("Untitled") : CurrentFilename;
	resourceFinder->BuildResourceCacheForFile(fileName, codeText);
	SymbolTable.CreateSymbols(codeText);
	SymbolClass::Types type;
	UnicodeString objectType,
		objectMember,
		comment;
	if (SymbolTable.Lookup(endPos, *resourceFinder, type, objectType, objectMember, comment)) {
		bool isObjectMethodOrProperty = SymbolClass::OBJECT == type ||SymbolClass::METHOD == type || SymbolClass::PROPERTY == type;
		if (isObjectMethodOrProperty)  {
			// even if objectType is empty, symbol will be something like '::METHOD' which the 
			// ResourceFinder will interpret to look for methods only (which is what we want here)
			symbol = objectType + UNICODE_STRING_SIMPLE("::") + objectMember;
		}
		else {
			symbol = objectType;
		}
	}
	
	return StringHelperClass::IcuToWx(symbol);
}

void mvceditor::CodeControlClass::HandleAutoCompletion(bool force) {
	wxLongLong now = wxGetLocalTimeMillis();
	
	// if user is typing really fast dont bother helping them. however, if force is true, it means
	// the user asked for auto completion, in which case always perform.
	int currentPos = GetCurrentPos();
	if ((now - LastCharAddedTime) > 400 || force) {
		int charStart;
		int charEnd;
		UnicodeString code = GetSafeSubstring(0, currentPos, &charStart, &charEnd);
		if (LanguageDiscovery.Open(code)) {                     
			mvceditor::LanguageDiscoveryClass::Syntax syntax = LanguageDiscovery.at(charEnd - 1);
			switch (syntax) {
			case mvceditor::LanguageDiscoveryClass::PHP_SCRIPT:
				HandleAutoCompletionPhp(force);
				break;
			case mvceditor::LanguageDiscoveryClass::HTML:
				HandleAutoCompletionHtml(force);
				break;
			}
		}
	}
	LastCharAddedTime = wxGetLocalTimeMillis();
}

void mvceditor::CodeControlClass::HandleAutoCompletionHtml(bool force) {
      
	// for now only complete when inside the angle brackets
	int currentPos = GetCurrentPos();
	int start = WordStartPosition(currentPos, true);
	int wordLength = currentPos - start;
	UnicodeString s = GetSafeSubstring(start - 1, start);
	if (s.compare(UNICODE_STRING("<", 1)) == 0) {
     
		// scintilla needs the keywords sorted.
		// split all of the HTML keywords into a vector so we can sort
		// them and concatenate them
		std::vector<wxString> autoCompleteList;
		wxStringTokenizer tokenizer;
		tokenizer.SetString(HTML_KEYWORDS, wxT(" "), wxTOKEN_STRTOK);
		while (tokenizer.HasMoreTokens()) {
			wxString it = tokenizer.NextToken();
			autoCompleteList.push_back(it);
		}
		if (!autoCompleteList.empty()) {
			sort(autoCompleteList.begin(), autoCompleteList.end());
			wxString list;
			for (size_t i = 0; i < autoCompleteList.size(); ++i) {
				list += wxT(" ");
				list += autoCompleteList[i];
			}
			AutoCompSetMaxWidth(0);
			AutoCompShow(wordLength, list);
		}
	}
}

void mvceditor::CodeControlClass::HandleAutoCompletionPhp(bool force) {
	int currentPos = GetCurrentPos();
	int wordLength = currentPos - WordStartPosition(currentPos, true);
	std::vector<wxString> autoCompleteList;
	if (force || wordLength > 3) {
		if (PositionedAtVariable(currentPos)) {
			SymbolTable.CreateSymbols(GetSafeText());
			int pos = WordStartPosition(currentPos, true);

			//+1 = do not take the '$' into account
			UnicodeString symbol = GetSafeSubstring(pos + 1, pos + 1 + wordLength - 1);
			std::vector<UnicodeString> variables = SymbolTable.GetVariablesInScope(currentPos);
			for (size_t i = 0; i < variables.size(); ++i) {
				if (0 == variables[i].indexOf(symbol)) {
					autoCompleteList.push_back(wxT("$") + StringHelperClass::IcuToWx(variables[i]));
				}
			}
		}
		else {
			wxString symbol = GetCurrentSymbol();
			LastCharAddedTime = wxGetLocalTimeMillis();
			ResourceFinderClass* resourceFinder = Project->GetResourceFinder();
			if (resourceFinder->Prepare(symbol)) {
				resourceFinder->CollectNearMatchResources();
				for (size_t i = 0; i < resourceFinder->GetResourceMatchCount(); ++i) {
					wxString s = mvceditor::StringHelperClass::IcuToWx(resourceFinder->GetResourceMatch(i).Identifier);
					autoCompleteList.push_back(s);
				}
			}

			 // when completing method names, do NOT include keywords
			if (resourceFinder->GetResourceType() != ResourceFinderClass::CLASS_NAME_METHOD_NAME) {
				std::vector<wxString> keywordMatches = CollectNearMatchKeywords(symbol);
				for (size_t i = 0; i < keywordMatches.size(); ++i) {
					autoCompleteList.push_back(keywordMatches[i]);
				}
			}
		}
		if (!autoCompleteList.empty()) {
			sort(autoCompleteList.begin(), autoCompleteList.end());
			wxString list;
			for (size_t i = 0; i < autoCompleteList.size(); ++i) {
				list += wxT(" ");
				list += autoCompleteList[i];
			}
			AutoCompSetMaxWidth(0);
			AutoCompShow(wordLength, list);
		}
	}
}

bool mvceditor::CodeControlClass::PositionedAtVariable(int pos) {
	int start = WordStartPosition(pos, true);
	return GetCharAt(start) == wxT('$');
}

void mvceditor::CodeControlClass::HandleCallTip(wxChar ch) {
	int currentPos = GetCurrentPos();
	if (wxT(')') == ch) {
		CallTipCancel();
	}
	if (wxT('(') == ch) {
		
		// when getting the word, do not get the open parenthesis
		UnicodeString uniText = GetSafeText();
		wxString text = StringHelperClass::IcuToWx(uniText);
		ResourceFinderClass* resourceFinder = Project->GetResourceFinder();
		CurrentSignature = wxT("");
		wxString fileName = CurrentFilename.IsEmpty() ? wxT("Untitled") : CurrentFilename;
		resourceFinder->BuildResourceCacheForFile(fileName, uniText);
		currentPos = text.rfind(wxT("("), currentPos);
		SymbolTable.CreateSymbols(GetSafeText());
		SymbolClass::Types type;
		UnicodeString objectType,
			objectMember,
			comment;
		if (SymbolTable.Lookup(currentPos, *resourceFinder, type, objectType, objectMember, comment)) {
			wxString resource = SymbolClass::OBJECT == type ||SymbolClass::METHOD == type ||SymbolClass::PROPERTY == type ? 
				StringHelperClass::IcuToWx(objectType) + wxT("::") + StringHelperClass::IcuToWx(objectMember) : StringHelperClass::IcuToWx(objectType);
			if (resourceFinder->Prepare(resource)) {
				if (resourceFinder->CollectNearMatchResources()) {
					
					// highly unlikely that there is more than one match since we searched for a full name (lookup succeeded).
					UnicodeString fullQualifiedResource =  resourceFinder->GetResourceMatch(0).Resource;
					CurrentSignature = mvceditor::StringHelperClass::IcuToWx(resourceFinder->GetResourceSignature(fullQualifiedResource));
				}
			}
		}
		else {
			
			 // when getting the word, do not get the open parenthesis
			int lastOpenParenthesis = text.rfind(wxT('('),currentPos);
			int lastSemicolon = text.rfind(wxT(';'), currentPos);
			if (lastOpenParenthesis > lastSemicolon) {
				int pos = WordStartPosition(lastOpenParenthesis - 1, true);
				wxString word = text.substr(pos, lastOpenParenthesis - pos);
				CurrentSignature = mvceditor::StringHelperClass::IcuToWx(resourceFinder->GetResourceSignature(
					mvceditor::StringHelperClass::wxToIcu(word)));
				if (!CurrentSignature.IsEmpty()) {
						CallTipShow(GetCurrentPos(), CurrentSignature);
				}
			}
		}
		if (!CurrentSignature.IsEmpty()) {
			CallTipShow(GetCurrentPos(), CurrentSignature);
		}
	}
	if (CallTipActive()) {
		
		// highlight the 1st, 2nd, 3rd or 4th parameter of the call tip depending on where the cursors currently is.
		// If the cursor is in the 2nd argument, then highlight the 2nd parameter and so on...
		wxString text = GetText();
		int lastOpenParenthesis = text.rfind(wxT('('), currentPos);
		int lastSemicolon = text.rfind(wxT(';'), currentPos);
		if (lastOpenParenthesis > lastSemicolon) {
			int commaCount = 0;
			wxString args = text.substr(lastOpenParenthesis, currentPos - lastOpenParenthesis);
			for (size_t i = 0; i < args.length(); ++i) {
				if (args[i] == wxT(',')) {
					commaCount++;
				}
			}
			int startHighlightPos = CurrentSignature.find(wxT('('));
			
			// sometimes the previous call tip is active, as in for exampele this line
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

std::vector<wxString> mvceditor::CodeControlClass::CollectNearMatchKeywords(wxString resource) {
	resource = resource.Lower();
	std::vector<wxString> matchedKeywords;
	wxString keywords = Project->GetPhpKeywords();
	wxStringTokenizer tokens(keywords, wxT(" "));
	while (tokens.HasMoreTokens()) {
		wxString keyword = tokens.GetNextToken();
		if (0 == keyword.Find(resource)) {
			matchedKeywords.push_back(keyword);
		}
	}
	return matchedKeywords;
}

void mvceditor::CodeControlClass::OnUpdateUi(wxStyledTextEvent &event) {
	HandleCallTip(0);
	event.Skip();
}

void mvceditor::CodeControlClass::OnMarginClick(wxStyledTextEvent& event) {
	if (event.GetMargin() == CodeControlOptionsClass::MARGIN_CODE_FOLDING) {
		int line = LineFromPosition(event.GetPosition());
		ToggleFold(line);
	}
}

void mvceditor::CodeControlClass::MatchBraces(int posToCheck) {

	int style = GetStyleAt(posToCheck);
	int prevStyle = GetStyleAt(posToCheck - 1);

	// dont match braces inside strings or comments. for some reason when styling line comments (//)
	// the last character is styled as default but the characters before are styled correctly (wxSTC_HPHP_COMMENTLINE)
	// so lets check the previous character in that case
	if (wxSTC_HPHP_HSTRING != style && wxSTC_HPHP_SIMPLESTRING != style && wxSTC_HPHP_COMMENT != style
	        && wxSTC_HPHP_COMMENTLINE != style && wxSTC_HPHP_COMMENTLINE != prevStyle) {
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

void mvceditor::CodeControlClass::SetPhpOptions() {
	wxFont font;
	wxColor color,
		backgroundColor;
	bool isBold,
		isItalic;
	for (int i = 0; i < CodeControlOptionsClass::STYLE_COUNT; ++i) {
		int style = CodeControlOptionsClass::ArrayIndexToStcConstant[i];
		if (CodeControlOptions.GetStyleByStcConstant(style, font, color, backgroundColor, isBold, isItalic)) {
			switch (style) {
				case CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET:
					SetCaretForeground(color);
					break;
				case CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE:
					SetCaretLineVisible(true);
					SetCaretLineBackground(backgroundColor);
					break;
				case CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION:
					SetSelForeground(true, color);
					SetSelBackground(true, backgroundColor);
					break;
				case CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING:
					if (CodeControlOptions.EnableCodeFolding) {
						SetFoldMarginColour(true, backgroundColor);
						SetFoldMarginHiColour(true, backgroundColor);
						MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS, backgroundColor, color);
						MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS, backgroundColor, color);
						MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE, backgroundColor, color);
						MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER, backgroundColor, color);
						MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUSCONNECTED, backgroundColor, color);
						MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, backgroundColor, color);
						MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER, backgroundColor, color);
					}
				case CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN:
					if (CodeControlOptions.RightMargin > 0) {
						SetEdgeColour(color);
					}
					break;
				case mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT:
					// since we need to share one indicator with the matching word highlight
					// and the parse errors indicators; we will set this setting when the 
					// user initiates the matching word feature
					break;
				default:
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
						for (int i = 0; i < 28; ++i) {
							StyleSetFont(styles[i], font);
							StyleSetForeground(styles[i], color);
							StyleSetBackground(styles[i], backgroundColor);
							StyleSetBold(styles[i], isBold);
							StyleSetItalic(styles[i], isItalic);
						}
					}
					StyleSetFont(style, font);
					StyleSetForeground(style, color);
					StyleSetBackground(style, backgroundColor);
					StyleSetBold(style, isBold);
					StyleSetItalic(style, isItalic);
					break;
			}
		}
		
	}
}

void mvceditor::CodeControlClass::ApplyPreferences() {
	SetHtmlOptions();
	SetMargin();
	SetPhpOptions();
	SetJavascriptOptions();
	SetCodeControlOptions();
}

void mvceditor::CodeControlClass::SetHtmlOptions() {
	
	// Some languages, such as HTML may contain embedded languages, VBScript
	// and JavaScript are common for HTML. For HTML, key word set 0 is for HTML,
	// 1 is for JavaScript and 2 is for VBScript, 3 is for Python, 4 is for PHP
	// and 5 is for SGML and DTD keywords
	SetKeyWords(0, HTML_KEYWORDS);
}

void mvceditor::CodeControlClass::SetJavascriptOptions() {
	SetKeyWords(1, JAVASCRIPT_KEYWORDS);
}

void mvceditor::CodeControlClass::SetCodeControlOptions() {
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
}

void  mvceditor::CodeControlClass::OnDoubleClick(wxStyledTextEvent& event) {
	int pos = WordStartPosition(GetCurrentPos(), true);
	int endPos = WordEndPosition(GetCurrentPos(), true);

	// we must share the indicator between highlight words functionality and
	// parse errors functionality.  This is because the HTML lexer uses 7 of the
	// eight style bits, leaving only one bit for indicators
	wxFont font;
	wxColor color;
	wxColor backgroundColor;
	bool isBold;
	bool isItalic;
	CodeControlOptions.GetStyleByStcConstant(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT, 
		font, color, backgroundColor, isBold, isItalic);
	IndicatorSetStyle(INDICATOR,  wxSTC_INDIC_ROUNDBOX);
	IndicatorSetForeground(INDICATOR, color);

	// remove any parse error indicators. if we don't do this the 
	// parse error will get highlighted like a match.
	StartStyling(0, INDICATOR_STYLE);
	SetStyling(GetTextLength(), 0);
	
	int charStartIndex = 0;
	int charEndIndex = 0;
	// pos, endPos are byte offsets into the UTF-8 string, need to convert to char numbers
	UnicodeString word = GetSafeSubstring(pos, endPos, &charStartIndex, &charEndIndex);
	if (!word.isEmpty()) {
		WordHighlightFinder.Expression = word;
		WordHighlightFinder.Mode = mvceditor::FinderClass::EXACT;
		WordHighlightFinder.CaseSensitive = true;
		if (WordHighlightFinder.Prepare()) {
			WordHighlightPreviousIndex = charStartIndex;
			WordHighlightNextIndex = charStartIndex;
		}
	}
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

UnicodeString mvceditor::CodeControlClass::GetSafeSubstring(int startPos, int endPos, int *charStartIndex, int *charEndIndex) {
	
	// not a very efficient function, but at least it's correct
	
	int documentLength = GetTextLength();
	char* buf = new char[documentLength];
	
	// GET_TEXT  message
	SendMsg(2182, documentLength, (long)buf);
	
	// pos, endPos are byte offsets into the UTF-8 string, need to convert to char numbers
	int startIndex = mvceditor::StringHelperClass::Utf8PosToChar(buf, documentLength, startPos);
	UnicodeString codeText = GetSafeText();
	int endIndex;
	if (endPos >= documentLength) {
		
		// caller wants the entire document
		endIndex = codeText.length();		
	}
	else {
		endIndex =  mvceditor::StringHelperClass::Utf8PosToChar(buf, documentLength, endPos);
	}
	UnicodeString word(codeText, startIndex, endIndex - startIndex);
	delete[] buf;
	if (charStartIndex != NULL) {
		*charStartIndex = startIndex;
	}
	if (charEndIndex != NULL) {
		*charEndIndex = endIndex;
	}
	return word;
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
					LoadPhpFile(CurrentFilename);
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
		
		StartStyling(0, INDICATOR_STYLE);
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

			StartStyling(utf8Start, INDICATOR_STYLE);
			SetStyling(utf8End - utf8Start, INDICATOR_STYLE);
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

			StartStyling(utf8Start, INDICATOR_STYLE);
			SetStyling(utf8End - utf8Start, INDICATOR_STYLE);
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
		StartStyling(byteNumber, INDICATOR_STYLE);
		SetStyling(errorLength, INDICATOR_STYLE);

		GotoPos(byteNumber);

		delete[] buf;
	}
	Colourise(0, -1);	
}

void mvceditor::CodeControlClass::ClearLintErrors() {
	MarkerDeleteAll(LINT_RESULT_MARKER);
	StartStyling(0, INDICATOR_STYLE);
	SetStyling(GetLength(), 0);
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
END_EVENT_TABLE()
