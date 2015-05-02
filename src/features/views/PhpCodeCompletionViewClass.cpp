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
#include <features/views/PhpCodeCompletionViewClass.h>
#include <features/PhpCodeCompletionFeatureClass.h>
#include <Triumph.h>
#include <wx/tokenzr.h>
#include <algorithm>
#include <language/Keywords.h>
#include <globals/Assets.h>

enum AutoCompletionImages {
	AUTOCOMP_IMAGE_VARIABLE = 1,
	AUTOCOMP_IMAGE_KEYWORD,
	AUTOCOMP_IMAGE_FUNCTION,
	AUTOCOMP_IMAGE_CLASS,
	AUTOCOMP_IMAGE_PRIVATE_METHOD,
	AUTOCOMP_IMAGE_PROTECTED_METHOD,
	AUTOCOMP_IMAGE_PUBLIC_METHOD,
	AUTOCOMP_IMAGE_PRIVATE_MEMBER,
	AUTOCOMP_IMAGE_PROTECTED_MEMBER,
	AUTOCOMP_IMAGE_PUBLIC_MEMBER,
	AUTOCOMP_IMAGE_CLASS_CONSTANT,
	AUTOCOMP_IMAGE_DEFINE
};

/**
 * @return the signature of the tag at the given index.
 * signature is in a format that is ready for the Scintilla call tip (with up or down arrows as appropriate)
 */
static wxString PhpCallTipSignature(size_t index, const std::vector<t4p::TagClass>& resources) {
	wxString callTip;
	size_t size = resources.size();
	if (index >= size) {
		return callTip;
	}
	wxString sig = t4p::IcuToWx(resources[index].Signature);
	if (size == 1) {
		callTip = sig;
	}
	else {
		callTip = wxString::Format(wxT("\001 %ld of %ld \002 "), index + 1, size) + sig;
	}
	return callTip;
}

/**
 * Check to see if the given position is at a PHP comment or style.
 * This is a quick-check that doesn't do any parsing it relies on the scintilla styling only
 * (this method will return true if the position is colored as a string or comment.)
 * This implementation should probably change in the future.
 *
 * @param ctrl the control to check
 * @param int posToCheck the scintilla position (byte) to check
 * @return bool TRUE if the position is at a PHP comment or PHP string
 */
static bool InCommentOrStringStyle(wxStyledTextCtrl* ctrl, int posToCheck) {
	int style = ctrl->GetStyleAt(posToCheck);
	int prevStyle = ctrl->GetStyleAt(posToCheck - 1);

	// dont match braces inside strings or comments. for some reason when styling line comments (//)
	// the last character is styled as default but the characters before are styled correctly (wxSTC_HPHP_COMMENTLINE)
	// so lets check the previous character in that case
	return wxSTC_HPHP_HSTRING == style || wxSTC_HPHP_SIMPLESTRING == style || wxSTC_HPHP_COMMENT == style
		|| wxSTC_HPHP_COMMENTLINE == style || wxSTC_HPHP_COMMENTLINE == prevStyle;
}

t4p::PhpCodeCompletionViewClass::PhpCodeCompletionViewClass(t4p::PhpCodeCompletionFeatureClass& feature)
: FeatureViewClass() 
, CodeCompletionProvider(feature.App.Globals) 
, CallTipProvider(feature.App.Globals) 
, BraceStyler() {
}

void t4p::PhpCodeCompletionViewClass::OnAutoCompletionSelected(wxStyledTextEvent& event) {
	CodeCompletionProvider.OnAutoCompletionSelected(event);
}


void t4p::PhpCodeCompletionViewClass::OnAppFileOpened(t4p::CodeControlEventClass& event) {
	CodeCompletionProvider.RegisterAutoCompletionImages(event.GetCodeControl());
	event.GetCodeControl()->RegisterCompletionProvider(&CodeCompletionProvider);
	event.GetCodeControl()->RegisterCallTipProvider(&CallTipProvider);
	event.GetCodeControl()->RegisterBraceMatchStyler(&BraceStyler);
}

void t4p::PhpCodeCompletionViewClass::OnCallTipClick(wxStyledTextEvent& event) {
	CallTipProvider.OnCallTipClick(event);
}

t4p::PhpCodeCompletionProviderClass::PhpCodeCompletionProviderClass(t4p::GlobalsClass& globals) 
: LanguageDiscovery()
, Parser()
, Lexer()
, ScopeFinder()
, Globals(globals)
, AutoCompletionResourceMatches() {
}

bool t4p::PhpCodeCompletionProviderClass::DoesSupport(t4p::FileType type) {
	return t4p::FILE_TYPE_PHP == type;
}
	

void t4p::PhpCodeCompletionProviderClass::Provide(t4p::CodeControlClass* ctrl, std::vector<t4p::CodeCompletionItemClass>& suggestions, wxString& completeStatus) {
	AutoCompletionResourceMatches.clear();
	int currentPos = ctrl->GetCurrentPos();
	int startPos = ctrl->WordStartPosition(currentPos, true);
	int endPos = ctrl->WordEndPosition(currentPos, true);
	UnicodeString word = ctrl->GetSafeSubstring(startPos, endPos);
	UnicodeString code = ctrl->GetSafeSubstring(0, currentPos);
	std::vector<wxString> ret;
	if (LanguageDiscovery.Open(code)) {
		pelet::LanguageDiscoveryClass::Syntax syntax = LanguageDiscovery.at(code.length() - 1);
		switch (syntax) {
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_SCRIPT:
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_BACKTICK:
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_DOUBLE_QUOTE_STRING:
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_HEREDOC:
			HandleAutoCompletionPhp(code, word, syntax, ctrl, completeStatus);
			break;
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_LINE_COMMENT:
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_MULTI_LINE_COMMENT:
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_NOWDOC:
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_SINGLE_QUOTE_STRING:
			HandleAutoCompletionString(word, ctrl, completeStatus);
			break;
		case pelet::LanguageDiscoveryClass::SYNTAX_HTML:
		case pelet::LanguageDiscoveryClass::SYNTAX_HTML_TAG:
		case pelet::LanguageDiscoveryClass::SYNTAX_HTML_ATTRIBUTE:
		case pelet::LanguageDiscoveryClass::SYNTAX_HTML_ATTRIBUTE_DOUBLE_QUOTE_VALUE:
		case pelet::LanguageDiscoveryClass::SYNTAX_HTML_ATTRIBUTE_SINGLE_QUOTE_VALUE:
		case pelet::LanguageDiscoveryClass::SYNTAX_HTML_ENTITY:
			HandleAutoCompletionHtml(word, syntax, ctrl, completeStatus);
			break;
		default:
			break;
		}
		LanguageDiscovery.Close();
	}
}

void t4p::PhpCodeCompletionProviderClass::HandleAutoCompletionString(const UnicodeString& word, wxStyledTextCtrl* ctrl, wxString& completeStats) {

	// look at the database meta data. look accross all connections since different tables
	// may be in different schemas used by the PHP app
	std::vector<wxString> autoCompleteList;
	AppendSqlTableNames(word, autoCompleteList);
	if (!autoCompleteList.empty()) {
		
		// scintilla needs the keywords sorted.
		std::sort(autoCompleteList.begin(), autoCompleteList.end());
		wxString list;
		for (size_t i = 0; i < autoCompleteList.size(); ++i) {
			list += autoCompleteList[i];
			if (i < (autoCompleteList.size() - 1)) {
				list += (wxChar)ctrl->AutoCompGetSeparator();

			}
		}
			ctrl->AutoCompSetMaxWidth(0);
		int currentPos = ctrl->GetCurrentPos();
		int startPos = ctrl->WordStartPosition(currentPos, true);
		int wordLength = currentPos - startPos;
		ctrl->AutoCompShow(wordLength, list);
	}
}

void t4p::PhpCodeCompletionProviderClass::AppendSqlTableNames(const UnicodeString& word, std::vector<wxString>& autoCompleteList) {
	for (size_t i = 0; i < Globals.DatabaseTags.size(); ++i) {
		t4p::DatabaseTagClass dbTag = Globals.DatabaseTags[i];
		if (dbTag.IsEnabled) {
			UnicodeString error;
			std::vector<UnicodeString> results = Globals.SqlResourceFinder.FindTables(dbTag, word);
			for (size_t i = 0; i < results.size(); i++) {
				wxString s = t4p::IcuToWx(results[i]);
				autoCompleteList.push_back(s);
			}
			results = Globals.SqlResourceFinder.FindColumns(dbTag, word);
			for (size_t i = 0; i < results.size(); i++) {
				wxString s = t4p::IcuToWx(results[i]);
				autoCompleteList.push_back(s);
			}
		}
	}
}

void t4p::PhpCodeCompletionProviderClass::HandleAutoCompletionHtml(const UnicodeString& word, 
														   pelet::LanguageDiscoveryClass::Syntax syntax,
														   wxStyledTextCtrl* ctrl,
														   wxString& completeStatus) {
	if (word.length() < 1) {
		 return;
	}
	std::vector<wxString> autoCompleteList;
	wxString symbol = t4p::IcuToWx(word);
	if (pelet::LanguageDiscoveryClass::SYNTAX_HTML_ATTRIBUTE == syntax) {
		t4p::KeywordsTokenizeMatch(t4p::KEYWORDS_HTML_ATTRIBUTE_NAMES, symbol, autoCompleteList);
	}
	else if (pelet::LanguageDiscoveryClass::SYNTAX_HTML_TAG == syntax) {
		t4p::KeywordsTokenizeMatch(t4p::KEYWORDS_HTML_TAG_NAMES, symbol, autoCompleteList);
	}
	if (!autoCompleteList.empty()) {
			
		// scintilla needs the keywords sorted.
		std::sort(autoCompleteList.begin(), autoCompleteList.end());
		wxString list;
		for (size_t i = 0; i < autoCompleteList.size(); ++i) {
			list += autoCompleteList[i];
			if (i < (autoCompleteList.size() - 1)) {
				list += (wxChar)ctrl->AutoCompGetSeparator();

			}
		}
		ctrl->AutoCompSetMaxWidth(0);
		int currentPos = ctrl->GetCurrentPos();
		int startPos = ctrl->WordStartPosition(currentPos, true);
		int wordLength = currentPos - startPos;
		ctrl->AutoCompShow(wordLength, list);
	}
	else if (pelet::LanguageDiscoveryClass::SYNTAX_HTML_ATTRIBUTE == syntax) {
		completeStatus = _("Unknown HTML Attribute: ");
		completeStatus += symbol;
	}
	else if (pelet::LanguageDiscoveryClass::SYNTAX_HTML_TAG == syntax) {
		completeStatus = _("Unknown HTML Tag: ");
		completeStatus += symbol;
	}
}

void t4p::PhpCodeCompletionProviderClass::HandleAutoCompletionPhp(const UnicodeString& code, const UnicodeString& word, 
	pelet::LanguageDiscoveryClass::Syntax syntax, t4p::CodeControlClass* ctrl,
	wxString& completeStatus) {
	
	Lexer.SetVersion(Globals.Environment.Php.Version);
	Parser.SetVersion(Globals.Environment.Php.Version);
	ScopeFinder.SetVersion(Globals.Environment.Php.Version);
	
	std::vector<wxFileName> sourceDirs = Globals.AllEnabledSourceDirectories();
	std::vector<wxString> autoCompleteList;
	std::vector<UnicodeString> variableMatches;
	int expressionPos = code.length() - 1;
	UnicodeString lastExpression = Lexer.LastExpression(code);
	pelet::ScopeClass scope;
	pelet::VariableClass parsedVariable(scope);
	pelet::ScopeClass variableScope;
	t4p::SymbolTableMatchErrorClass error;
	
	bool doDuckTyping = ctrl->CodeControlOptions.EnableDynamicAutoCompletion;
	if (!lastExpression.isEmpty()) {
		Parser.ParseExpression(lastExpression, parsedVariable);
		ScopeFinder.GetScopeString(code, expressionPos, variableScope);
		Globals.TagCache.ExpressionCompletionMatches(ctrl->GetIdString(), parsedVariable, variableScope, sourceDirs,
				variableMatches, AutoCompletionResourceMatches, doDuckTyping, error);
		if (!variableMatches.empty()) {
			for (size_t i = 0; i < variableMatches.size(); ++i) {
				wxString postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_VARIABLE);
				autoCompleteList.push_back(t4p::IcuToWx(variableMatches[i]) + postFix);
			}
		}
		else if (parsedVariable.ChainList.size() == 1) {
			
			// a bunch of function, define, or class names
			for (size_t i = 0; i < AutoCompletionResourceMatches.size(); ++i) {
				t4p::TagClass res = AutoCompletionResourceMatches[i];
				wxString postFix;
				if (t4p::TagClass::DEFINE == res.Type) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_DEFINE);
				}
				else if (t4p::TagClass::FUNCTION == res.Type) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_FUNCTION);
				}
				else if (t4p::TagClass::CLASS == res.Type) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_CLASS);
				}
				autoCompleteList.push_back(t4p::IcuToWx(res.Identifier) + postFix);
			}

			// when completing standalone function names, also include keyword matches
			std::vector<wxString> keywordMatches = CollectNearMatchKeywords(t4p::IcuToWx(parsedVariable.ChainList[0].Name));
			for (size_t i = 0; i < keywordMatches.size(); ++i) {
				wxString postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_KEYWORD);
				autoCompleteList.push_back(keywordMatches[i] + postFix);
			}
		}
		else if (!AutoCompletionResourceMatches.empty()) {
			
			// an object / function "chain"
			for (size_t i = 0; i < AutoCompletionResourceMatches.size(); ++i) {
				t4p::TagClass res = AutoCompletionResourceMatches[i];
				wxString comp = t4p::IcuToWx(res.Identifier);
				wxString postFix;
				if (t4p::TagClass::MEMBER == res.Type && res.IsPrivate) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_PRIVATE_MEMBER);
				}
				else if (t4p::TagClass::MEMBER == res.Type && res.IsProtected) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_PROTECTED_MEMBER);
				}
				else if (t4p::TagClass::MEMBER == res.Type) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_PUBLIC_MEMBER);
				}
				else if (t4p::TagClass::METHOD == res.Type && res.IsPrivate) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_PRIVATE_METHOD);
				}
				else if (t4p::TagClass::METHOD == res.Type && res.IsProtected) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_PROTECTED_METHOD);
				}
				else if (t4p::TagClass::METHOD == res.Type) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_PUBLIC_METHOD);
				}
				else if (t4p::TagClass::CLASS_CONSTANT == res.Type) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_CLASS_CONSTANT);
				}
				autoCompleteList.push_back(comp + postFix);
			}
		}
		// auto complete any template variables
		std::vector<t4p::TemplateFileTagClass> templateFiles = Globals.CurrentTemplates();
		std::vector<t4p::TemplateFileTagClass>::const_iterator templateFile;
		std::vector<wxString>::const_iterator variable;

		for (templateFile =  templateFiles.begin(); templateFile != templateFiles.end(); ++templateFile) {
			wxFileName f1(templateFile->FullPath);
			wxFileName f2(ctrl->GetFileName());
			if (f1 == f2) {
				for (variable = templateFile->Variables.begin(); variable != templateFile->Variables.end(); ++variable) {
					if (variable->Find(t4p::IcuToWx(lastExpression)) == 0) {
						wxString postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_VARIABLE);
						autoCompleteList.push_back(*variable + postFix);
					}
				}
			}
		}
		
		// in case of a double quoted string, complete SQL table names too
		// this is in addition to auto completing any variable names inside the string too
		if (pelet::LanguageDiscoveryClass::SYNTAX_PHP_DOUBLE_QUOTE_STRING == syntax) {
			AppendSqlTableNames(word, autoCompleteList);
		}
	}

	if (!autoCompleteList.empty()) {
			
		// scintilla needs the keywords sorted.
		sort(autoCompleteList.begin(), autoCompleteList.end());
		wxString list;
		for (size_t i = 0; i < autoCompleteList.size(); ++i) {
			list += autoCompleteList[i];
			if (i < (autoCompleteList.size() - 1)) {
				list += (wxChar)ctrl->AutoCompGetSeparator();
			}
		}
		ctrl->AutoCompSetMaxWidth(0);
		int currentPos = ctrl->GetCurrentPos();
		int startPos = ctrl->WordStartPosition(currentPos, true);
		int wordLength = currentPos - startPos;
		ctrl->AutoCompShow(wordLength, list);
	}
	else {
		HandleAutoCompletionPhpStatus(error, lastExpression, parsedVariable,
			variableScope, completeStatus);
	}
}

void t4p::PhpCodeCompletionProviderClass::HandleAutoCompletionPhpStatus(
		const t4p::SymbolTableMatchErrorClass& error, 
		const UnicodeString& lastExpression,
		const pelet::VariableClass& parsedVariable,
		const pelet::ScopeClass& variableScope,
		wxString& completeStatus) {
	if (lastExpression.isEmpty()) {
		completeStatus = _("Nothing to complete");
	}
	else if (lastExpression.startsWith(UNICODE_STRING_SIMPLE("$")) && parsedVariable.ChainList.size() <= 1) {
		completeStatus = _("No matching variables for: ");
		completeStatus += t4p::IcuToWx(lastExpression);
		completeStatus +=  _(" in scope: ");
		completeStatus += t4p::IcuToWx(variableScope.ClassName);
		completeStatus += _("::");
		completeStatus += t4p::IcuToWx(variableScope.MethodName);
	}
	else if (parsedVariable.ChainList.size() == 1) {
		completeStatus = _("No matching class, function, define, or keyword for: \"");
		completeStatus += t4p::IcuToWx(lastExpression);
		completeStatus += wxT("\"");
	}
	else if (AutoCompletionResourceMatches.empty()) {
		if (t4p::SymbolTableMatchErrorClass::PARENT_ERROR == error.Type) {
			completeStatus = _("parent not valid for scope: ");
			completeStatus += t4p::IcuToWx(variableScope.ClassName);
			completeStatus += _("::");
			completeStatus += t4p::IcuToWx(variableScope.MethodName);
		}
		else if (t4p::SymbolTableMatchErrorClass::STATIC_ERROR == error.Type) {
			completeStatus = _("Cannot access protected or private static member \"");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
			completeStatus += _("\" in class: ");
			completeStatus += t4p::IcuToWx(error.ErrorClass);
		}
		else if (t4p::SymbolTableMatchErrorClass::TYPE_RESOLUTION_ERROR == error.Type) {
			completeStatus = _("Could not resolve type for \"");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
			completeStatus += wxT("\"");
		}
		else if (t4p::SymbolTableMatchErrorClass::UNKNOWN_RESOURCE == error.Type) {
			if (!parsedVariable.ChainList.empty() &&
				parsedVariable.ChainList[0].Name == UNICODE_STRING_SIMPLE("$this")) {
				completeStatus = _("No public, protected, or private member matches for \"");
			}
			else {
				completeStatus = _("No public member matches for \"");
			}
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
			completeStatus += _("\" in class: ");
			completeStatus += t4p::IcuToWx(error.ErrorClass);
		}
		else if (t4p::SymbolTableMatchErrorClass::UNKNOWN_STATIC_RESOURCE == error.Type) {
			completeStatus = _("No static member matches for \"");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
			completeStatus += _("\" in class: ");
			completeStatus += t4p::IcuToWx(error.ErrorClass);
		}
		else if (t4p::SymbolTableMatchErrorClass::VISIBILITY_ERROR == error.Type) {
			completeStatus = _("Cannot access protected or private member \"");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
			completeStatus += _("\" in class: ");
			completeStatus += t4p::IcuToWx(error.ErrorClass);
		}
		else if (t4p::SymbolTableMatchErrorClass::ARRAY_ERROR == error.Type && !error.ErrorClass.isEmpty()) {
			completeStatus = _("Cannot use object operator for array returned by ");
			completeStatus += t4p::IcuToWx(error.ErrorClass);
			completeStatus += _("::");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
		}
		else if (t4p::SymbolTableMatchErrorClass::ARRAY_ERROR == error.Type) {
			completeStatus = _("Cannot use object operator for array variable ");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
		}
		else if (t4p::SymbolTableMatchErrorClass::PRIMITIVE_ERROR == error.Type && !error.ErrorClass.isEmpty()) {
			completeStatus = _("Cannot use object operator for primitive returned by ");
			completeStatus += t4p::IcuToWx(error.ErrorClass);
			completeStatus += _("::");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
		}
		else if (t4p::SymbolTableMatchErrorClass::PRIMITIVE_ERROR == error.Type) {
			completeStatus = _("Cannot use object operator for primitive variable ");
			completeStatus += t4p::IcuToWx(error.ErrorLexeme);
		}
	}
}

std::vector<wxString> t4p::PhpCodeCompletionProviderClass::CollectNearMatchKeywords(wxString tag) {
	tag = tag.Lower();
	std::vector<wxString> matchedKeywords;	
	t4p::KeywordsTokenizeMatch(
		t4p::KeywordsPhpAll(Globals.Environment.Php.Version),
		tag,
		matchedKeywords
	);
	return matchedKeywords;
}

void t4p::PhpCodeCompletionProviderClass::RegisterAutoCompletionImages(wxStyledTextCtrl* ctrl) {
	std::map<int, wxString> autoCompleteImages;
	autoCompleteImages[AUTOCOMP_IMAGE_CLASS] = wxT("class_black");
	autoCompleteImages[AUTOCOMP_IMAGE_FUNCTION] = wxT("function_black");
	autoCompleteImages[AUTOCOMP_IMAGE_KEYWORD] = wxT("keyword_black");
	autoCompleteImages[AUTOCOMP_IMAGE_PRIVATE_MEMBER] = wxT("property_black");
	autoCompleteImages[AUTOCOMP_IMAGE_PRIVATE_METHOD] = wxT("function_black");
	autoCompleteImages[AUTOCOMP_IMAGE_PROTECTED_MEMBER] = wxT("property_black");
	autoCompleteImages[AUTOCOMP_IMAGE_PROTECTED_METHOD] = wxT("property_black");
	autoCompleteImages[AUTOCOMP_IMAGE_PUBLIC_MEMBER] = wxT("property_black");
	autoCompleteImages[AUTOCOMP_IMAGE_PUBLIC_METHOD] = wxT("function_blue");
	autoCompleteImages[AUTOCOMP_IMAGE_VARIABLE] = wxT("inner_square_black");
	autoCompleteImages[AUTOCOMP_IMAGE_CLASS_CONSTANT] = wxT("class_constant_black");
	autoCompleteImages[AUTOCOMP_IMAGE_DEFINE] = wxT("define_black");
	for (std::map<int, wxString>::iterator it = autoCompleteImages.begin(); it != autoCompleteImages.end(); ++it) {
		wxBitmap bitmap = t4p::AutoCompleteImageAsset(it->second);
		ctrl->RegisterImage(it->first, bitmap);
	}
}

void t4p::PhpCodeCompletionProviderClass::OnAutoCompletionSelected(wxStyledTextEvent& event) {
	wxStyledTextCtrl* txtCtrl = wxDynamicCast(event.GetEventObject(), wxStyledTextCtrl);	
	if (!txtCtrl) {
		return;
	}
	t4p::CodeControlClass* ctrl = (t4p::CodeControlClass*)txtCtrl;
	
	if (!AutoCompletionResourceMatches.empty()) {
		UnicodeString selected = t4p::WxToIcu(event.GetText());
		
		bool handled = false;
		for (size_t i = 0; i < AutoCompletionResourceMatches.size(); ++i) {
			t4p::TagClass res = AutoCompletionResourceMatches[i];
			if (res.Identifier == selected) {

				// user had selected  a function /method name; let's add the 
				// parenthesis and show the call tip
				ctrl->AutoCompCancel();
				wxString selected = event.GetText();
				int startPos = ctrl->WordStartPosition(ctrl->GetCurrentPos(), true);
				ctrl->SetSelection(startPos, ctrl->GetCurrentPos());
				wxString status;
				if ((t4p::TagClass::FUNCTION == res.Type || t4p::TagClass::METHOD == res.Type) && !res.HasParameters()) {
					ctrl->ReplaceSelection(selected + wxT("()"));
					ctrl->HandleCallTip(0, true);
				}
				else if (t4p::TagClass::FUNCTION == res.Type || t4p::TagClass::METHOD == res.Type) {
					ctrl->ReplaceSelection(selected + wxT("("));
					ctrl->HandleCallTip(0, true);
				}
				else {
					ctrl->ReplaceSelection(selected);
				}
				handled = true;
				break;
			}
		}
		if (!handled) {

			// complete the PHP alternative syntax for control structures
			// ie endif endwhile endfor endforeach endswitch
			// Scintilla cannot handle semicolons in keywords; we will add the semicolon here
			if (selected.caseCompare(UNICODE_STRING_SIMPLE("endif"), 0) == 0 ||
				selected.caseCompare(UNICODE_STRING_SIMPLE("endwhile"), 0) == 0 ||
				selected.caseCompare(UNICODE_STRING_SIMPLE("endfor"), 0) == 0 ||
				selected.caseCompare(UNICODE_STRING_SIMPLE("endforeach"), 0) == 0 ||
				selected.caseCompare(UNICODE_STRING_SIMPLE("endswitch"), 0) == 0) {
				
				ctrl->AutoCompCancel();	
				wxString selected = event.GetText();
				int startPos = ctrl->WordStartPosition(ctrl->GetCurrentPos(), true);
				ctrl->SetSelection(startPos, ctrl->GetCurrentPos());
				ctrl->ReplaceSelection(selected + wxT(";"));
			}
		}
	}
}

t4p::PhpCallTipProviderClass::PhpCallTipProviderClass(t4p::GlobalsClass& globals)
: CallTipProviderClass() 
, Globals(globals) 
, CurrentCallTipResources() 
, CurrentCallTipIndex(0) {
	
}

bool t4p::PhpCallTipProviderClass::DoesSupport(t4p::FileType type) {
	return t4p::FILE_TYPE_PHP == type;
}

void t4p::PhpCallTipProviderClass::ProvideTip(t4p::CodeControlClass* ctrl, wxChar ch, bool force, wxString& status) {
	
	// this function deliberately uses scintilla positions (bytes) instead of 
	// converting over to unicode text. be careful.
	int currentPos = ctrl->GetCurrentPos();
	if (InCommentOrStringStyle(ctrl, currentPos) && wxT(')') == ch) {
		ctrl->CallTipCancel();
	}
	if (force || wxT('(') == ch) {
		
		// back up to the last function call "(" then get the function name, do not get the open parenthesis
		// we are always going to do the call tip for the nearest function ie. when
		// a line is  
		// Func1('hello', Func2('bye'
		// we are always going to show the call tip for Func2 (if the cursor is after 'bye')
		// make sure we don't go past the last statement
		// - But - 
		// in this case 
		// Func1('hello, Func2('bye')
		// we are going to show the call tip for Func1 since Func2 has been 
		// closed. we need to ignore matching parenthesis
		bool hasMethodCall = false;
		
		// seed matching parens in case the cursor is at a close parens
		// we show the call tip
		int matchingParens = 0;
		int startingPos = currentPos;
		while (currentPos >= 0) {
			char c = ctrl->GetCharAt(currentPos);
			if (!InCommentOrStringStyle(ctrl, currentPos)) {
				if (')' == c && startingPos != currentPos) {
					matchingParens++;
				}
				else if ('(' == c && matchingParens <= 0) {
					hasMethodCall = true;
					break;
				}
				else if ('(' == c) {
					matchingParens--;
				}
				else if (';' == c) {
					currentPos = -1;
					break;
				}
			}
			currentPos--;
		}
		
		if (currentPos >= 0) {
			CurrentCallTipResources.clear();
			CurrentCallTipIndex = 0;

			std::vector<t4p::TagClass> matches = Globals.TagCache.GetTagsAtPosition(
				ctrl->GetIdString(), ctrl->GetSafeText(), currentPos, 
				Globals.AllEnabledSourceDirectories(),
				Globals,
				status
			);
			
			for (size_t i = 0; i < matches.size(); ++i) {
				t4p::TagClass tag = matches[i];
				if (t4p::TagClass::FUNCTION == tag.Type || t4p::TagClass::METHOD == tag.Type) {
					CurrentCallTipResources.push_back(tag);
				}
			}
			if (CurrentCallTipResources.empty() && hasMethodCall && !matches.empty()) {

				// may be the constructor is being called.
				// when the constructor is called; the matched symbol is the class name and not a method name
				// here we will look for the constructor
				// search for all methods of the class
				UnicodeString constructorSearch = matches[0].FullyQualifiedClassName();
				constructorSearch += UNICODE_STRING_SIMPLE("::__construct");

				// search project and native tags 
				t4p::TagResultClass* result = Globals.TagCache.ExactTags(
					constructorSearch, Globals.AllEnabledSourceDirectories()
				);
				if (result->Empty()) {
					delete result;
					result = Globals.TagCache.ExactNativeTags(constructorSearch);
				}
				while (result->More()) {
					result->Next();
					t4p::TagClass res = result->Tag;
					if (t4p::TagClass::METHOD == res.Type && UNICODE_STRING_SIMPLE("__construct") == res.Identifier) {
						CurrentCallTipResources.push_back(res);
					}
				}
				delete result;
			}
			size_t matchCount = CurrentCallTipResources.size();
			if (matchCount > 0) {	
				wxString callTip = PhpCallTipSignature(CurrentCallTipIndex, CurrentCallTipResources);
				ctrl->CallTipShow(ctrl->GetCurrentPos(), callTip);
			}
		}
	}
	if (ctrl->CallTipActive()) {
		
		// highlight the 1st, 2nd, 3rd or 4th parameter of the call tip depending on where the cursors currently is.
		// If the cursor is in the 2nd argument, then highlight the 2nd parameter and so on...
		int startOfArguments = ctrl->GetCurrentPos();
		int commaCount = 0;
		while (startOfArguments >= 0) {
			char c = ctrl->GetCharAt(startOfArguments);
			if (!InCommentOrStringStyle(ctrl, startOfArguments)) {
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
		if (startOfArguments >= 0 && !CurrentCallTipResources.empty() && CurrentCallTipIndex < CurrentCallTipResources.size()) {
			wxString currentSignature = t4p::IcuToWx(CurrentCallTipResources[CurrentCallTipIndex].Signature);
			int startHighlightPos = currentSignature.find(wxT('('));
			
			// sometimes the previous call tip is active, as in for example this line
			//   $m->getB()->work(
			// in this case we need to be careful
			if(startHighlightPos >= 0) {
				int endHighlightPos = currentSignature.find(wxT(','), startHighlightPos);
				
				// no comma =  highlight the only param, if signature has no params, then 
				// nothing will get highlighted since its all whitespace
				if (endHighlightPos < 0) {
					endHighlightPos = currentSignature.length() - 1;
				}
				while (commaCount > 0) {
					startHighlightPos = endHighlightPos;
					endHighlightPos = currentSignature.find(wxT(','), startHighlightPos + 1);
					if (-1 == endHighlightPos) {
						endHighlightPos = currentSignature.length() - 1;
					}
					--commaCount;
				}
				if ((endHighlightPos - startHighlightPos) > 1) {
					ctrl->CallTipSetHighlight(startHighlightPos, endHighlightPos);
				}
			}
		}
	}
}

void t4p::PhpCallTipProviderClass::OnCallTipClick(wxStyledTextEvent& evt) {
	wxStyledTextCtrl* ctrl = wxDynamicCast(evt.GetEventObject(), wxStyledTextCtrl);

	if (!CurrentCallTipResources.empty()) {
		size_t resourcesSize = CurrentCallTipResources.size();
		int position = evt.GetPosition();
		wxString callTip;

		// up arrow. if already at the first choice, then loop around 
		// looping around looks better than hiding arrows; because hiding arrows changes the 
		// rendering position and make the call tip jump around when clicking up/down
		if (1 == position) {
			CurrentCallTipIndex = ((CurrentCallTipIndex >= 1) && (CurrentCallTipIndex - 1) < resourcesSize) ? CurrentCallTipIndex - 1 : resourcesSize - 1;				
			callTip =  PhpCallTipSignature(CurrentCallTipIndex, CurrentCallTipResources);
		}
		else if (2 == position) {

			// down arrow
			CurrentCallTipIndex = ((CurrentCallTipIndex + 1) < resourcesSize) ? CurrentCallTipIndex + 1 : 0;
			callTip = PhpCallTipSignature(CurrentCallTipIndex, CurrentCallTipResources);
		}
		if (!callTip.IsEmpty()) {
			ctrl->CallTipCancel();
			ctrl->CallTipShow(ctrl->GetCurrentPos(), callTip);					
		}
	}
	evt.Skip();
}

t4p::PhpBraceMatchStylerClass::PhpBraceMatchStylerClass()
: BraceMatchStylerClass() {
	
}

bool t4p::PhpBraceMatchStylerClass::DoesSupport(t4p::FileType type) {
	return t4p::FILE_TYPE_PHP == type;
}

void t4p::PhpBraceMatchStylerClass::Style(t4p::CodeControlClass* ctrl, int posToCheck) {
	if (!InCommentOrStringStyle(ctrl, posToCheck)) {
		wxChar c2 = ctrl->GetCharAt(posToCheck - 1);
		if (wxT('{') == c2 || wxT('}') == c2 || wxT('(') == c2 || wxT(')') == c2 || wxT('[') == c2 || wxT(']') == c2) {
			posToCheck = posToCheck - 1;
		}
		else  {
			posToCheck = -1;
		}
		if (posToCheck >= 0) {
			int pos = ctrl->BraceMatch(posToCheck);
			if (wxSTC_INVALID_POSITION == pos) {
				ctrl->BraceBadLight(posToCheck);
			}
			else {
				ctrl->BraceHighlight(posToCheck, pos);
			}
		}
		else {
			ctrl->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
		}
	}
	else {
		ctrl->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
	}
}

BEGIN_EVENT_TABLE(t4p::PhpCodeCompletionViewClass, t4p::FeatureClass)
	EVT_APP_FILE_OPEN(t4p::PhpCodeCompletionViewClass::OnAppFileOpened)
	EVT_APP_FILE_NEW(t4p::PhpCodeCompletionViewClass::OnAppFileOpened)
	EVT_STC_AUTOCOMP_SELECTION(wxID_ANY, t4p::PhpCodeCompletionViewClass::OnAutoCompletionSelected)
	EVT_STC_CALLTIP_CLICK(wxID_ANY, t4p::PhpCodeCompletionViewClass::OnCallTipClick)
END_EVENT_TABLE()

