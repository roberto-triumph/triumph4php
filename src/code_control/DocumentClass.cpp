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
#include <code_control/DocumentClass.h>
#include <code_control/CodeControlClass.h>
#include <windows/StringHelperClass.h>
#include <windows/StringHelperClass.h>
#include <MvcEditorAssets.h>
#include <wx/tokenzr.h>
#include <wx/filename.h>
#include <algorithm>
#include <unicode/ustring.h>

static const wxString PHP_KEYWORDS = wxString::FromAscii(
	"php if else elseif do while for foreach switch "
	"case break continue default function return public private protected "
	"class interface extends implements static final const true false "
	"NULL global array echo empty eval exit isset list print unset __LINE__ "
	"__FILE__ __DIR__ __FUNCTION__ __CLASS__ __METHOD__ __NAMESPACE__ "
	"require require_once include include_once stdClass parent self abstract "
	"clone namespace use as new bool boolean float double real string int "
	"integer var endif endwhile endfor endforeach endswitch"
);

static const wxString PHP_54_KEYWORDS = wxString::FromAscii(
	"trait callable"
);


// want to support both HTML4 and HTML5, using both sets of keywords.
static const wxString HTML_TAG_NAMES = wxString::FromAscii(

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
);
	
static const wxString HTML_ATTRIBUTE_NAMES = wxString::FromAscii(

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

static const wxString MYSQL_KEYWORDS = wxString::FromAscii(

	// MySQL version 5.6 keywords from
	//http://dev.mysql.com/doc/mysqld-version-reference/en/mysqld-version-reference-reservedwords-5-6.html
	"accessible add all alter analyze and as asc asensitive before between bigint binary blob both by call cascade "
	"case change char character check collate column condition constraint continue convert create cross current_date "
	"current_time current_timestamp current_user cursor database databases day_hour day_microsecond day_minute "
	"day_second dec decimal declare default delayed delete desc describe deterministic distinct distinctrow div "
	"double drop dual each else elseif enclosed escaped exists exit explain false fetch float float4 float8 for "
	"force foreign from fulltext general grant group having high_priority hour_microsecond hour_minute hour_second "
	"if ignore ignore_server_ids in index infile inner inout insensitive insert int int1 int2 int3 int4 int8 integer "
	"interval into is iterate join key keys kill leading leave left like limit linear lines load localtime "
	"localtimestamp lock long longblob longtext loop low_priority master_bind master_heartbeat_period "
	"master_ssl_verify_server_cert match maxvalue mediumblob mediumint mediumtext middleint minute_microsecond "
	"minute_second mod modifies natural not no_write_to_binlog null numeric on one_shot optimize option "
	"optionally or order out outer outfile partition precision primary procedure purge range read reads read_write "
	"real references regexp release rename repeat replace require resignal restrict return revoke right rlike "
	"schema schemas second_microsecond select sensitive separator set show signal slow smallint spatial specific "
	"sql sqlexception sqlstate sqlwarning sql_big_result sql_calc_found_rows sql_small_result ssl "
	"starting straight_join table terminated then tinyblob tinyint tinytext to trailing trigger true undo "
	"union unique unlock unsigned update usage use using utc_date utc_time utc_timestamp values varbinary "
	"varchar varcharacter varying when where while with write xor year_month zerofill"
);

// these are actually CSS 1, CSS 2 and CSS 3 keywords
// got these from http://code.google.com/p/scite-files/wiki/bettercsspropertiesfile
static const wxString CSS_KEYWORDS = wxString::FromAscii(
	"ascent azimuth background background-attachment background-color background-image background-position "
	"background-repeat background-size baseline bbox border border-bottom border-bottom-color "
	"border-bottom-left-radius border-bottom-right-radius border-bottom-style border-bottom-width border-collapse " 
	"border-color border-color border-left border-left-color border-left-style border-left-width border-radius "
	"border-right border-right-color border-right-style border-right-width border-spacing border-style "
	"border-style border-top border-top-left-radius border-top-right-radius border-top-style border-top-width "
	"border-width bottom box-shadow cap-height caption-side centerline clear clip color column-count column-gap "
	"column-rule column-rule-color column-rule-style column-rule-width columns column-width content counter-increment "
	"counter-reset cue cue-after cue-before cursor definition-src descent direction display elevation empty-cells "
	"float font font-family font-size font-size-adjust font-stretch font-style font-variant font-weight height left "
	"letter-spacing line-height list-style list-style-image list-style-position list-style-type margin margin-bottom "
	"margin-left margin-right margin-top marker-offset marks mathline max-height max-width min-height min-width "
	"opacity order-top-color orphans outline outline-color outline-style outline-width overflow padding padding-bottom "
	"padding-left padding-right padding-top page page-break-after page-break-before page-break-inside panose-1 pause "
	"pause-after pause-before pitch pitch-range play-during position quotes resize richness right size slope speak "
	"speak-header speak-numeral speak-punctuation speech-rate src stemh stemv stress table-layout text-align "
	"text-decoration text-indent text-shadow text-transform top topline unicode-bidi unicode-range units-per-em "
	"vertical-align visibility voice-family volume white-space widows width widths word-spacing word-wrap x-height z-index"
);

// these are for CSS 1, 2, and 3
// got these from http://code.google.com/p/scite-files/wiki/bettercsspropertiesfile
static const wxString CSS_PSEUDOCLASSES = wxString::FromAscii(
	"active checked disabled empty enabled first first-child first-of-type focus hover invalid lang last-child "
	"last-of-type left link not nth-child nth-last-child nth-last-of-type nth-of-type only-child only-of-type "
	"optional required right root target valid visited"
);

static const wxString JAVASCRIPT_KEYWORDS = wxT("");

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
	AUTOCOMP_IMAGE_PUBLIC_MEMBER
};


/**
 * @return the signature of the resource at the given index.
 * signature is in a format that is ready for the Scintilla call tip (with up or down arrows as appropriate)
 */
static wxString PhpCallTipSignature(size_t index, const std::vector<mvceditor::ResourceClass>& resources) {
	wxString callTip;
	size_t size = resources.size();
	if (index >= size) {
		return callTip;
	}
	wxString sig = mvceditor::StringHelperClass::IcuToWx(resources[index].Signature);
	if (size == 1) {
		callTip = sig;
	}
	else {
		callTip = wxString::Format(wxT("\001 %u of %u \002 "), index + 1, size) + sig;
	}
	return callTip;
}


mvceditor::TextDocumentClass::TextDocumentClass() {
	Ctrl = NULL;
}

mvceditor::TextDocumentClass::~TextDocumentClass() {
	if (Ctrl) {
		DetachFromControl(Ctrl);
	}
}

void mvceditor::TextDocumentClass::SetControl(CodeControlClass* ctrl) {
	if (Ctrl) {
		DetachFromControl(Ctrl);
	}
	Ctrl = ctrl;
	AttachToControl(Ctrl);
}

bool mvceditor::TextDocumentClass::CanAutoComplete() {
	return false;
}

void mvceditor::TextDocumentClass::HandleAutoCompletion(wxString& completeStatus) {
}

void mvceditor::TextDocumentClass::HandleCallTip(wxChar ch, bool force) {
}

std::vector<mvceditor::ResourceClass> mvceditor::TextDocumentClass::GetCurrentSymbolResource() {

	// plain text docs don't have structure
	std::vector<mvceditor::ResourceClass> resources;
	return resources;
}

void mvceditor::TextDocumentClass::FileOpened(wxString fileName) {
}

void mvceditor::TextDocumentClass::MatchBraces(int posToCheck) {
}

UnicodeString mvceditor::TextDocumentClass::GetSafeText() {
	
	// copied from the implementation of GetText method in stc.cpp 
    int len  = Ctrl->GetTextLength();
	wxMemoryBuffer mbuf(len + 1);   // leave room for the null...
	char* buf = (char*)mbuf.GetWriteBuf(len + 1);
	Ctrl->SendMsg(2182, len + 1, (long)buf);
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

UnicodeString mvceditor::TextDocumentClass::GetSafeSubstring(int startPos, int endPos) {
	wxString s = Ctrl->GetTextRange(startPos, endPos);
	UnicodeString ret = mvceditor::StringHelperClass::wxToIcu(s);
	return ret;
}

void mvceditor::TextDocumentClass::AttachToControl(CodeControlClass* ctrl) {

}

void mvceditor::TextDocumentClass::DetachFromControl(CodeControlClass* ctrl) {

}

mvceditor::PhpDocumentClass::PhpDocumentClass(mvceditor::StructsClass* structs, mvceditor::RunningThreadsClass& runningThreads)
	: wxEvtHandler()
	, TextDocumentClass()
	, LanguageDiscovery()
	, Parser()
	, Lexer()
	, ScopeFinder()
	, CurrentCallTipResources()
	, AutoCompletionResourceMatches()
	, Timer()
	, FileIdentifier()
	, Structs(structs)
	, ResourceCacheUpdateThread(&structs->ResourceCache, *this, runningThreads)
	, CurrentCallTipIndex(0)
	, NeedToUpdateResources(false) 
	, AreImagesRegistered(false) {
	Timer.SetOwner(this);
	Timer.Start(500, false);

	// need to give this new file unique name so because the
	// resource updates object needs a unique name
	wxLongLong time = wxGetLocalTimeMillis();
	FileIdentifier = wxString::Format(wxT("File_%s"), time.ToString().c_str());
}

mvceditor::PhpDocumentClass::~PhpDocumentClass() {
	Timer.Stop();
	if (Structs) {
		Structs->ResourceCache.Unregister(FileIdentifier);
	}	
}


void mvceditor::PhpDocumentClass::AttachToControl(CodeControlClass* ctrl) {
	ctrl->SetModEventMask(wxSTC_MOD_INSERTTEXT | wxSTC_MOD_DELETETEXT);
		
	// using Connect instead of Event tables because call EVT_STC_CALLTIP_CLICK macro
	// contains a syntax error
	ctrl->Connect(wxID_ANY, wxID_ANY, wxEVT_STC_CALLTIP_CLICK, 
		(wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent(wxStyledTextEventFunction, &mvceditor::PhpDocumentClass::OnCallTipClick),
		NULL, this);
	ctrl->Connect(wxID_ANY, wxID_ANY, wxEVT_STC_MODIFIED, 
		(wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent(wxStyledTextEventFunction, &mvceditor::PhpDocumentClass::OnModification),
		NULL, this);
	ctrl->Connect(wxID_ANY, wxID_ANY, wxEVT_STC_AUTOCOMP_SELECTION,
		(wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent(wxStyledTextEventFunction, &mvceditor::PhpDocumentClass::OnAutoCompletionSelected),
		NULL, this);

	// do this so that when a new untitled file is created that code completion still works.
	FileOpened(FileIdentifier);
}

void mvceditor::PhpDocumentClass::DetachFromControl(CodeControlClass* ctrl) {
	ctrl->Disconnect(wxID_ANY, wxID_ANY, wxEVT_STC_CALLTIP_CLICK, 
		(wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent(wxStyledTextEventFunction, &mvceditor::PhpDocumentClass::OnCallTipClick),
		NULL, this);
	ctrl->Disconnect(wxID_ANY, wxID_ANY, wxEVT_STC_MODIFIED, 
		(wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent(wxStyledTextEventFunction, &mvceditor::PhpDocumentClass::OnModification),
		NULL, this);
	ctrl->Disconnect(wxID_ANY, wxID_ANY, wxEVT_STC_AUTOCOMP_SELECTION,
		(wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent(wxStyledTextEventFunction, &mvceditor::PhpDocumentClass::OnAutoCompletionSelected),
		NULL, this);
	ctrl->ClearRegisteredImages();
}

bool mvceditor::PhpDocumentClass::CanAutoComplete() {
	return true;
}

void mvceditor::PhpDocumentClass::HandleAutoCompletion(wxString& completeStatus) {	
	AutoCompletionResourceMatches.clear();
	int currentPos = Ctrl->GetCurrentPos();
	int startPos = Ctrl->WordStartPosition(currentPos, true);
	int endPos = Ctrl->WordEndPosition(currentPos, true);
	UnicodeString word = GetSafeSubstring(startPos, endPos);
	UnicodeString code = GetSafeSubstring(0, currentPos);
	std::vector<wxString> ret;
	if (LanguageDiscovery.Open(code)) {
		pelet::LanguageDiscoveryClass::Syntax syntax = LanguageDiscovery.at(code.length() - 1);
		switch (syntax) {
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_SCRIPT:
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_BACKTICK:
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_DOUBLE_QUOTE_STRING:
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_HEREDOC:
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_LINE_COMMENT:
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_MULTI_LINE_COMMENT:
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_NOWDOC:
		case pelet::LanguageDiscoveryClass::SYNTAX_PHP_SINGLE_QUOTE_STRING:
			HandleAutoCompletionPhp(code, completeStatus);
			break;
		case pelet::LanguageDiscoveryClass::SYNTAX_HTML:
		case pelet::LanguageDiscoveryClass::SYNTAX_HTML_TAG:
		case pelet::LanguageDiscoveryClass::SYNTAX_HTML_ATTRIBUTE:
		case pelet::LanguageDiscoveryClass::SYNTAX_HTML_ATTRIBUTE_DOUBLE_QUOTE_VALUE:
		case pelet::LanguageDiscoveryClass::SYNTAX_HTML_ATTRIBUTE_SINGLE_QUOTE_VALUE:
		case pelet::LanguageDiscoveryClass::SYNTAX_HTML_ENTITY:
			HandleAutoCompletionHtml(word, syntax, completeStatus);
			break;
		default:
			break;
		}
		LanguageDiscovery.Close();
	}
}

void mvceditor::PhpDocumentClass::HandleAutoCompletionHtml(const UnicodeString& word, 
														   pelet::LanguageDiscoveryClass::Syntax syntax,
														   wxString& completeStatus) {
	if (word.length() < 1) {
		 return;
	}
	std::vector<wxString> autoCompleteList;
	wxStringTokenizer tokenizer(wxT(""));
	if (pelet::LanguageDiscoveryClass::SYNTAX_HTML_ATTRIBUTE == syntax) {
		tokenizer.SetString(HTML_ATTRIBUTE_NAMES, wxT(" "), wxTOKEN_STRTOK);
	}
	else if (pelet::LanguageDiscoveryClass::SYNTAX_HTML_TAG == syntax) {
		tokenizer.SetString(HTML_TAG_NAMES, wxT(" "), wxTOKEN_STRTOK);
	}
	wxString symbol = mvceditor::StringHelperClass::IcuToWx(word);
	while (tokenizer.HasMoreTokens()) {
		wxString it = tokenizer.NextToken();
		if (it.StartsWith(symbol)) {
			autoCompleteList.push_back(it);
		}
	}
	if (!autoCompleteList.empty()) {
			
		// scintilla needs the keywords sorted.
		std::sort(autoCompleteList.begin(), autoCompleteList.end());
		wxString list;
		for (size_t i = 0; i < autoCompleteList.size(); ++i) {
			list += autoCompleteList[i];
			if (i < (autoCompleteList.size() - 1)) {
				list += (wxChar)Ctrl->AutoCompGetSeparator();

			}
		}
		Ctrl->AutoCompSetMaxWidth(0);
		int currentPos = Ctrl->GetCurrentPos();
		int startPos = Ctrl->WordStartPosition(currentPos, true);
		int wordLength = currentPos - startPos;
		Ctrl->AutoCompShow(wordLength, list);
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

void mvceditor::PhpDocumentClass::HandleAutoCompletionPhp(const UnicodeString& code, wxString& completeStatus) {
	if (!Structs) {
		return;
	}
	if (!AreImagesRegistered) {
		RegisterAutoCompletionImages();
	}
	Lexer.SetVersion(Structs->Environment.Php.Version);
	Parser.SetVersion(Structs->Environment.Php.Version);
	ScopeFinder.SetVersion(Structs->Environment.Php.Version);
	
	std::vector<wxString> autoCompleteList;
	std::vector<UnicodeString> variableMatches;
	int expressionPos = code.length() - 1;
	UnicodeString lastExpression = Lexer.LastExpression(code);
	pelet::ScopeClass scope;
	pelet::ExpressionClass parsedExpression(scope);
	pelet::ScopeClass expressionScope;
	mvceditor::SymbolTableMatchErrorClass error;
	
	bool doDuckTyping = Ctrl->CodeControlOptions.EnableDynamicAutoCompletion;
	if (!lastExpression.isEmpty()) {
		Parser.ParseExpression(lastExpression, parsedExpression);
		ScopeFinder.GetScopeString(code, expressionPos, expressionScope);
		Structs->ResourceCache.ExpressionCompletionMatches(FileIdentifier, parsedExpression, expressionScope,
				variableMatches, AutoCompletionResourceMatches, doDuckTyping, error);
		if (!variableMatches.empty()) {
			for (size_t i = 0; i < variableMatches.size(); ++i) {
				wxString postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_VARIABLE);
				autoCompleteList.push_back(mvceditor::StringHelperClass::IcuToWx(variableMatches[i]) + postFix);
			}
		}
		else if (parsedExpression.ChainList.size() == 1) {
			
			// a bunch of function, define, or class names
			for (size_t i = 0; i < AutoCompletionResourceMatches.size(); ++i) {
				mvceditor::ResourceClass res = AutoCompletionResourceMatches[i];
				wxString postFix;
				if (mvceditor::ResourceClass::DEFINE == res.Type) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_VARIABLE);
				}
				else if (mvceditor::ResourceClass::FUNCTION == res.Type) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_FUNCTION);
				}
				else if (mvceditor::ResourceClass::CLASS == res.Type) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_CLASS);
				}
				autoCompleteList.push_back(mvceditor::StringHelperClass::IcuToWx(res.Identifier) + postFix);
			}

			// when completing standalone function names, also include keyword matches
			std::vector<wxString> keywordMatches = CollectNearMatchKeywords(mvceditor::StringHelperClass::IcuToWx(parsedExpression.ChainList[0].Name));
			for (size_t i = 0; i < keywordMatches.size(); ++i) {
				wxString postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_KEYWORD);
				autoCompleteList.push_back(keywordMatches[i] + postFix);
			}
		}
		else if (!AutoCompletionResourceMatches.empty()) {
			
			// an object / function "chain"
			for (size_t i = 0; i < AutoCompletionResourceMatches.size(); ++i) {
				mvceditor::ResourceClass res = AutoCompletionResourceMatches[i];
				wxString comp = mvceditor::StringHelperClass::IcuToWx(res.Identifier);
				wxString postFix;
				if (mvceditor::ResourceClass::MEMBER == res.Type && res.IsPrivate) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_PRIVATE_MEMBER);
				}
				else if (mvceditor::ResourceClass::MEMBER == res.Type && res.IsProtected) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_PROTECTED_MEMBER);
				}
				else if (mvceditor::ResourceClass::MEMBER == res.Type) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_PUBLIC_MEMBER);
				}
				else if (mvceditor::ResourceClass::METHOD == res.Type && res.IsPrivate) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_PRIVATE_METHOD);
				}
				else if (mvceditor::ResourceClass::METHOD == res.Type && res.IsProtected) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_PROTECTED_METHOD);
				}
				else if (mvceditor::ResourceClass::METHOD == res.Type) {
					postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_PUBLIC_METHOD);
				}
				autoCompleteList.push_back(comp + postFix);
			}
		}

		// auto complete any template variables
		for (size_t i = 0; i < Structs->CurrentViewInfos.size(); ++i) {
			mvceditor::ViewInfoClass viewInfo = Structs->CurrentViewInfos[i];
			wxFileName f1(viewInfo.FileName);
			wxFileName f2(Ctrl->GetFileName());
			if (f1 == f2) {
				for (size_t j = 0; j < viewInfo.TemplateVariables.size(); ++j) {
					if (viewInfo.TemplateVariables[j].Find(mvceditor::StringHelperClass::IcuToWx(lastExpression)) == 0) {
						wxString postFix = wxString::Format(wxT("?%d"), AUTOCOMP_IMAGE_VARIABLE);
						autoCompleteList.push_back(viewInfo.TemplateVariables[j] + postFix);
					}
				}
			}
		}
	}

	if (!autoCompleteList.empty()) {
			
		// scintilla needs the keywords sorted.
		sort(autoCompleteList.begin(), autoCompleteList.end());
		wxString list;
		for (size_t i = 0; i < autoCompleteList.size(); ++i) {
			list += autoCompleteList[i];
			if (i < (autoCompleteList.size() - 1)) {
				list += (wxChar)Ctrl->AutoCompGetSeparator();
			}
		}
		Ctrl->AutoCompSetMaxWidth(0);
		int currentPos = Ctrl->GetCurrentPos();
		int startPos = Ctrl->WordStartPosition(currentPos, true);
		int wordLength = currentPos - startPos;
		Ctrl->AutoCompShow(wordLength, list);
	}
	else {
		HandleAutoCompletionPhpStatus(error, lastExpression, parsedExpression,
			expressionScope, completeStatus);
	}
}

void mvceditor::PhpDocumentClass::HandleAutoCompletionPhpStatus(
		const mvceditor::SymbolTableMatchErrorClass& error, 
		const UnicodeString& lastExpression,
		const pelet::ExpressionClass& parsedExpression,
		const pelet::ScopeClass& expressionScope,
		wxString& completeStatus) {
	if (lastExpression.isEmpty()) {
		completeStatus = _("Nothing to complete");
	}
	else if (lastExpression.startsWith(UNICODE_STRING_SIMPLE("$")) && parsedExpression.ChainList.size() <= 1) {
		completeStatus = _("No matching variables for: ");
		completeStatus += mvceditor::StringHelperClass::IcuToWx(lastExpression);
		completeStatus +=  _(" in scope: ");
		completeStatus += mvceditor::StringHelperClass::IcuToWx(expressionScope.ClassName);
		completeStatus += _("::");
		completeStatus += mvceditor::StringHelperClass::IcuToWx(expressionScope.MethodName);
	}
	else if (parsedExpression.ChainList.size() == 1) {
		completeStatus = _("No matching class, function, define, or keyword for: \"");
		completeStatus += mvceditor::StringHelperClass::IcuToWx(lastExpression);
		completeStatus += wxT("\"");
	}
	else if (AutoCompletionResourceMatches.empty()) {
		if (mvceditor::SymbolTableMatchErrorClass::PARENT_ERROR == error.Type) {
			completeStatus = _("parent not valid for scope: ");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(expressionScope.ClassName);
			completeStatus += _("::");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(expressionScope.MethodName);
		}
		else if (mvceditor::SymbolTableMatchErrorClass::STATIC_ERROR == error.Type) {
			completeStatus = _("Cannot access protected or private static member \"");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(error.ErrorLexeme);
			completeStatus += _("\" in class: ");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(error.ErrorClass);
		}
		else if (mvceditor::SymbolTableMatchErrorClass::TYPE_RESOLUTION_ERROR == error.Type) {
			completeStatus = _("Could not resolve type for \"");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(error.ErrorLexeme);
			completeStatus += wxT("\"");
		}
		else if (mvceditor::SymbolTableMatchErrorClass::UNKNOWN_RESOURCE == error.Type) {
			if (parsedExpression.FirstValue() == UNICODE_STRING_SIMPLE("$this")) {
				completeStatus = _("No public, protected, or private member matches for \"");
			}
			else {
				completeStatus = _("No public member matches for \"");
			}
			completeStatus += mvceditor::StringHelperClass::IcuToWx(error.ErrorLexeme);
			completeStatus += _("\" in class: ");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(error.ErrorClass);
		}
		else if (mvceditor::SymbolTableMatchErrorClass::UNKNOWN_STATIC_RESOURCE == error.Type) {
			completeStatus = _("No static member matches for \"");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(error.ErrorLexeme);
			completeStatus += _("\" in class: ");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(error.ErrorClass);
		}
		else if (mvceditor::SymbolTableMatchErrorClass::VISIBILITY_ERROR == error.Type) {
			completeStatus = _("Cannot access protected or private member \"");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(error.ErrorLexeme);
			completeStatus += _("\" in class: ");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(error.ErrorClass);
		}
		else if (mvceditor::SymbolTableMatchErrorClass::ARRAY_ERROR == error.Type && !error.ErrorClass.isEmpty()) {
			completeStatus = _("Cannot use object operator for array returned by \"");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(error.ErrorClass);
			completeStatus += _("::");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(error.ErrorLexeme);
		}
		else if (mvceditor::SymbolTableMatchErrorClass::ARRAY_ERROR == error.Type) {
			completeStatus = _("Cannot use object operator for array variable ");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(error.ErrorLexeme);
		}
		else if (mvceditor::SymbolTableMatchErrorClass::PRIMITIVE_ERROR == error.Type && !error.ErrorClass.isEmpty()) {
			completeStatus = _("Cannot use object operator for primitive returned by \"");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(error.ErrorClass);
			completeStatus += _("::");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(error.ErrorLexeme);
		}
		else if (mvceditor::SymbolTableMatchErrorClass::PRIMITIVE_ERROR == error.Type) {
			completeStatus = _("Cannot use object operator for primitive variable \"");
			completeStatus += mvceditor::StringHelperClass::IcuToWx(error.ErrorLexeme);
		}
	}
}

void mvceditor::PhpDocumentClass::HandleCallTip(wxChar ch, bool force) {
	if (!Structs) {
		return;
	}
	
	// this function deliberately uses scintilla positions (bytes) instead of 
	// converting over to unicode text. be careful.
	int currentPos = Ctrl->GetCurrentPos();
	if (InCommentOrStringStyle(currentPos) && wxT(')') == ch) {
		Ctrl->CallTipCancel();
	}
	if (force || wxT('(') == ch) {
		
		// back up to the last function call "(" then get the function name, do not get the open parenthesis
		/// we are always going to do the call tip for the nearest function ie. when
		// a line is  
		// Func1('hello', Func2('bye'
		// we are always going to show the call tip for Func2 (if the cursor is after 'bye')
		// make sure we don't go past the last statement
		bool hasMethodCall = false;
		while (currentPos >= 0) {
			char c = Ctrl->GetCharAt(currentPos);
			if (!InCommentOrStringStyle(currentPos)) {
				if ('(' == c) {
					hasMethodCall = true;
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
			CurrentCallTipResources.clear();
			CurrentCallTipIndex = 0;
			std::vector<mvceditor::ResourceClass> matches = GetSymbolAt(currentPos);
			for (size_t i = 0; i < matches.size(); ++i) {
				mvceditor::ResourceClass resource = matches[i];
				if (mvceditor::ResourceClass::FUNCTION == resource.Type || mvceditor::ResourceClass::METHOD == resource.Type) {
					CurrentCallTipResources.push_back(resource);
				}
			}
			if (CurrentCallTipResources.empty() && hasMethodCall && !matches.empty()) {

				// may be the constructor is being called.
				// when the constructor is called; the matched symbol is the class name and not a method name
				// here we will look for the constructor
				// search for all methods of the class
				UnicodeString className = matches[0].ClassName;
				wxString constructorResourceSearch = mvceditor::StringHelperClass::IcuToWx(className);
				constructorResourceSearch += wxT("::");
				std::vector<mvceditor::ResourceClass> matches = Structs->ResourceCache.PrepareAndCollectNearMatchResourcesFromAll(constructorResourceSearch);
				for (size_t i = 0; i < matches.size(); ++i) {
					mvceditor::ResourceClass res = matches[i];
					if (mvceditor::ResourceClass::METHOD == res.Type && UNICODE_STRING_SIMPLE("__construct") == res.Identifier) {
						CurrentCallTipResources.push_back(res);
					}
					else if (mvceditor::ResourceClass::METHOD == res.Type && className == res.Identifier) {
						CurrentCallTipResources.push_back(res);
					}
				}
			}
			size_t matchCount = CurrentCallTipResources.size();
			if (matchCount > 0) {				
				wxString callTip = PhpCallTipSignature(CurrentCallTipIndex, CurrentCallTipResources);
				Ctrl->CallTipShow(Ctrl->GetCurrentPos(), callTip);
			}
		}
	}
	if (Ctrl->CallTipActive()) {
		
		// highlight the 1st, 2nd, 3rd or 4th parameter of the call tip depending on where the cursors currently is.
		// If the cursor is in the 2nd argument, then highlight the 2nd parameter and so on...
		int startOfArguments = Ctrl->GetCurrentPos();
		int commaCount = 0;
		while (startOfArguments >= 0) {
			char c = Ctrl->GetCharAt(startOfArguments);
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
		if (startOfArguments >= 0 && !CurrentCallTipResources.empty() && CurrentCallTipIndex < CurrentCallTipResources.size()) {
			wxString currentSignature = mvceditor::StringHelperClass::IcuToWx(CurrentCallTipResources[CurrentCallTipIndex].Signature);
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
					Ctrl->CallTipSetHighlight(startHighlightPos, endHighlightPos);
				}
			}
		}
	}
}

std::vector<mvceditor::ResourceClass> mvceditor::PhpDocumentClass::GetCurrentSymbolResource() {
	
	// if the cursor is in the middle of an identifier, find the end of the
	// current identifier; that way we can know the full name of the resource we want
	// to get
	int currentPos = Ctrl->GetCurrentPos();
	int endPos = Ctrl->WordEndPosition(currentPos, true);
	
	UnicodeString code = GetSafeSubstring(0, endPos);
	
	std::vector<mvceditor::ResourceClass> matches;
	pelet::LexicalAnalyzerClass lexer;
	pelet::ParserClass parser;
	pelet::ScopeClass scope;
	pelet::ExpressionClass parsedExpression(scope);
	mvceditor::ScopeFinderClass scopeFinder;
	pelet::ScopeClass scopeResult;
	
	UnicodeString lastExpression = lexer.LastExpression(code);
	bool doDuckTyping = true;
	if (Structs && !lastExpression.isEmpty()) {
		parser.ParseExpression(lastExpression, parsedExpression);
		scopeFinder.GetScopeString(code, endPos, scopeResult);

		// for now do nothing with error
		mvceditor::SymbolTableMatchErrorClass error;
		Structs->ResourceCache.ResourceMatches(FileIdentifier, parsedExpression, scopeResult, matches, 
			doDuckTyping, true, error);
	}
	return matches;
}

void mvceditor::PhpDocumentClass::FileOpened(wxString fileName) {
	
	// in case the file name was changed and current file name is no  longer valid
	if (Structs) {
		Structs->ResourceCache.Unregister(FileIdentifier);
	}

	// important to set the fileIdentifier to the name;
	// the ResourceCache object will need to know what files are being edited so it can mark them as 'dirty'
	FileIdentifier = fileName;

	if (Structs) {
		Structs->ResourceCache.Register(FileIdentifier, false);
	}
	// trigger the resource cache
	// so that code completion works when the file is first opened
	NeedToUpdateResources = true;
}

void mvceditor::PhpDocumentClass::MatchBraces(int posToCheck) {
	if (!InCommentOrStringStyle(posToCheck)) {
		wxChar c1 = Ctrl->GetCharAt(posToCheck),
		            c2 = Ctrl->GetCharAt(posToCheck - 1);
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
			int pos = Ctrl->BraceMatch(posToCheck);
			if (wxSTC_INVALID_POSITION == pos) {
				Ctrl->BraceBadLight(posToCheck);
			}
			else {
				Ctrl->BraceHighlight(posToCheck, pos);
			}
		}
		else {
			Ctrl->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
		}
	}
	else {
		Ctrl->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
	}
}

void mvceditor::PhpDocumentClass::OnModification(wxStyledTextEvent& event) {

	// if already parsing then dont do anything
	if (!Structs || ResourceCacheUpdateThread.IsRunning()) {
		event.Skip();
		return;
	}
	if (!Ctrl || Ctrl != event.GetEventObject()) {
		event.Skip();
		return;
	}

	// we want to keep the re-parsings to a minimum
	// we dont need to reparse when user is adding a 
	// comment or a constant string
	if (!InCommentOrStringStyle(Ctrl->GetCurrentPos())) {

		// trigger reparsing only when a variable has been added or removed.
		// We can detect when a variable is added by looking for 
		// 1. when a statement ends ";"
		// 2. a scope ends (to handle "if ($row = mysql_fetch_array()) {}" 
		// 3. user pastes a block of text
		// 4. user tweaks a variable name (ie change "$user" to "$allUsers")
		// 5. user tweaks a function / class name (change "function work" to "function asyncWork")
		//
		// We can detect when a variable is removed ...
		// 1. user deletes a block of text
		// 2. user deletes char by char until we hit the beginning of a statement (if the
		//    cursor is positioned right after a semicolon (with whitespace being insignigicant)
		// 3. user cuts a block of text
		// 4. user tweaks a variable name (ie change "$user" to "$allUsers")
		// 5. user tweaks a function / class name (change "function work" to "function asyncWork")
		//
		// Also we will need to look for cases where the user fixes unmatched parenthesis / braces
		// because these may fix lint errors (and allow symbol table to be built).
		//
		// it looks like all text operations (type in text, cut / copy/ paste) are funneled
		// through the wxSTC_MOD_INSERTTEXT and wxSTC_MOD_DELETETEXT events so we just need 
		// to inspect what text is being added / removed here
		int mod = event.GetModificationType();
		if (mod & wxSTC_MOD_INSERTTEXT || mod & wxSTC_MOD_DELETETEXT) {
			wxString text = event.GetText();
			if (text.find_first_of(wxT(";{}()")) != std::string::npos) {
				NeedToUpdateResources = true;
			}
			else {

				// get the whole word that's positioned at the modified text
				// then check to see if it's a variable
				int wordStart = Ctrl->WordStartPosition(event.GetPosition(), true);
				int wordEnd = Ctrl->WordEndPosition(event.GetPosition(), true);
				wxString word = Ctrl->GetTextRange(wordStart, wordEnd);
				word.Trim(true);
				if (!word.IsEmpty() && word.StartsWith(wxT("$"))) {
					NeedToUpdateResources = true;
				}
				if (!word.IsEmpty()) {
					
					// get the previous word; if the previous word is the keyword
					// "function" or "class" then the user changed a function or class name
					int previousWordEnd = Ctrl->WordStartPosition(wordStart, false);
					int previousWordStart = Ctrl->WordStartPosition(previousWordEnd, true);
					wxString previousWord = Ctrl->GetTextRange(previousWordStart, previousWordEnd);
					previousWord.Trim(true);
					if (previousWord.CmpNoCase(wxT("function")) == 0 || previousWord.CmpNoCase(wxT("class")) == 0) {
						NeedToUpdateResources = true;
					}
				}
			}
		}
		
	}
	event.Skip();
}

std::vector<wxString> mvceditor::PhpDocumentClass::CollectNearMatchKeywords(wxString resource) {
	resource = resource.Lower();
	std::vector<wxString> matchedKeywords;
	wxString keywords = GetPhpKeywords();
	wxStringTokenizer tokens(keywords, wxT(" "));
	while (tokens.HasMoreTokens()) {
		wxString keyword = tokens.GetNextToken();
		wxString keywordLower = keyword.Lower();
		if (0 == keywordLower.Find(resource)) {
			matchedKeywords.push_back(keyword);
		}
	}
	return matchedKeywords;
}

std::vector<mvceditor::ResourceClass> mvceditor::PhpDocumentClass::GetSymbolAt(int posToCheck) {
	UnicodeString code = GetSafeSubstring(0, posToCheck);
	
	std::vector<mvceditor::ResourceClass> matches;
	pelet::LexicalAnalyzerClass lexer;
	pelet::ParserClass parser;
	pelet::ScopeClass scope;
	pelet::ExpressionClass parsedExpression(scope);
	mvceditor::ScopeFinderClass scopeFinder;
	pelet::ScopeClass expressionScope;
	
	UnicodeString lastExpression = lexer.LastExpression(code);
	UnicodeString resourceName;
	bool doDuckTyping = true;
	if (!lastExpression.isEmpty()) {
		parser.ParseExpression(lastExpression, parsedExpression);
		scopeFinder.GetScopeString(code, posToCheck, expressionScope);

		// for now do nothing with error
		mvceditor::SymbolTableMatchErrorClass error;
		Structs->ResourceCache.ResourceMatches(FileIdentifier, parsedExpression, expressionScope, matches, 
			doDuckTyping, true, error);
	}
	return matches;
}

bool mvceditor::PhpDocumentClass::InCommentOrStringStyle(int posToCheck) {
	int style = Ctrl->GetStyleAt(posToCheck);
	int prevStyle = Ctrl->GetStyleAt(posToCheck - 1);

	// dont match braces inside strings or comments. for some reason when styling line comments (//)
	// the last character is styled as default but the characters before are styled correctly (wxSTC_HPHP_COMMENTLINE)
	// so lets check the previous character in that case
	return wxSTC_HPHP_HSTRING == style || wxSTC_HPHP_SIMPLESTRING == style || wxSTC_HPHP_COMMENT == style
		|| wxSTC_HPHP_COMMENTLINE == style || wxSTC_HPHP_COMMENTLINE == prevStyle;
}

void mvceditor::PhpDocumentClass::OnCallTipClick(wxStyledTextEvent& evt) {
	if (evt.GetEventObject() == Ctrl) {
		if (!CurrentCallTipResources.empty()) {
			size_t resourcesSize = CurrentCallTipResources.size();
			int position = evt.GetPosition();
			wxString callTip;

			// up arrow. if already at the first choice, then loop around 
			// looping around looks better than hiding arrows; because hiding arrows changes the 
			// rendering position and make the call tip jump around when clicking up/down
			if (1 == position) {
				CurrentCallTipIndex = ((CurrentCallTipIndex - 1) >= 0 && (CurrentCallTipIndex - 1) < resourcesSize) ? CurrentCallTipIndex - 1 : resourcesSize - 1;				
				callTip =  PhpCallTipSignature(CurrentCallTipIndex, CurrentCallTipResources);
			}
			else if (2 == position) {

				// down arrow
				CurrentCallTipIndex = ((CurrentCallTipIndex + 1) >= 0 && (CurrentCallTipIndex + 1) < resourcesSize) ? CurrentCallTipIndex + 1 : 0;
				callTip = PhpCallTipSignature(CurrentCallTipIndex, CurrentCallTipResources);
			}
			if (!callTip.IsEmpty()) {
				Ctrl->CallTipCancel();
				Ctrl->CallTipShow(Ctrl->GetCurrentPos(), callTip);					
			}
		}
	}
	evt.Skip();
}

void mvceditor::PhpDocumentClass::OnResourceUpdateComplete(wxCommandEvent& event) {
	if(!Timer.Start(500, false)) {
		wxLogDebug(_("Could not start parsing timer."));
	}
	event.Skip();
}

void mvceditor::PhpDocumentClass::OnTimer(wxTimerEvent& event) {
	if (NeedToUpdateResources && Structs && !ResourceCacheUpdateThread.IsRunning()) {
		UnicodeString text = GetSafeText();

		// we need to differentiate between new and opened files (the 'true' arg)
		ResourceCacheUpdateThread.StartBackgroundUpdate(FileIdentifier, text, !wxFileName::FileExists(Ctrl->GetFileName()));

		// even if thread could not be started just prevent re-parsing until user 
		// modified the text
		NeedToUpdateResources = false;
		Timer.Stop();
	}
	event.Skip();
}

void mvceditor::PhpDocumentClass::RegisterAutoCompletionImages() {
	AreImagesRegistered = true;
	std::map<int, wxString> autoCompleteImages;
	autoCompleteImages[AUTOCOMP_IMAGE_CLASS] = wxT("round-blue");
	autoCompleteImages[AUTOCOMP_IMAGE_FUNCTION] = wxT("F-unction");
	autoCompleteImages[AUTOCOMP_IMAGE_KEYWORD] = wxT("inner-square-blue");
	autoCompleteImages[AUTOCOMP_IMAGE_PRIVATE_MEMBER] = wxT("dot-red");
	autoCompleteImages[AUTOCOMP_IMAGE_PRIVATE_METHOD] = wxT("F-unction-blue");
	autoCompleteImages[AUTOCOMP_IMAGE_PROTECTED_MEMBER] = wxT("dot-orange");
	autoCompleteImages[AUTOCOMP_IMAGE_PROTECTED_METHOD] = wxT("F-unction-blue");
	autoCompleteImages[AUTOCOMP_IMAGE_PUBLIC_MEMBER] = wxT("dot-blue");
	autoCompleteImages[AUTOCOMP_IMAGE_PUBLIC_METHOD] = wxT("F-unction");
	autoCompleteImages[AUTOCOMP_IMAGE_VARIABLE] = wxT("inner-square-black");
	for (std::map<int, wxString>::iterator it = autoCompleteImages.begin(); it != autoCompleteImages.end(); ++it) {
		wxFileName imgFileName = mvceditor::AutoCompleteImageAsset(it->second);
		wxBitmap bitmap(imgFileName.GetFullPath(), wxBITMAP_TYPE_XPM);
		Ctrl->RegisterImage(it->first, bitmap);
	}
}

void mvceditor::PhpDocumentClass::OnAutoCompletionSelected(wxStyledTextEvent& event) {
	if (!AutoCompletionResourceMatches.empty()) {
		UnicodeString selected = mvceditor::StringHelperClass::wxToIcu(event.GetText());
		
		bool handled = false;
		for (size_t i = 0; i < AutoCompletionResourceMatches.size(); ++i) {
			mvceditor::ResourceClass res = AutoCompletionResourceMatches[i];
			if (res.Identifier == selected) {

				// user had selected  a function /method name; let's add the 
				// parenthesis and show the call tip
				Ctrl->AutoCompCancel();
				wxString selected = event.GetText();
				int startPos = Ctrl->WordStartPosition(Ctrl->GetCurrentPos(), true);
				Ctrl->SetSelection(startPos, Ctrl->GetCurrentPos());
				if (mvceditor::ResourceClass::FUNCTION == res.Type || mvceditor::ResourceClass::METHOD == res.Type) {
					Ctrl->ReplaceSelection(selected + wxT("("));
					HandleCallTip(0, true);
				}
				else {
					Ctrl->ReplaceSelection(selected);
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
				
				Ctrl->AutoCompCancel();	
				wxString selected = event.GetText();
				int startPos = Ctrl->WordStartPosition(Ctrl->GetCurrentPos(), true);
				Ctrl->SetSelection(startPos, Ctrl->GetCurrentPos());
				Ctrl->ReplaceSelection(selected + wxT(";"));
			}
		}
	}
}

wxString mvceditor::PhpDocumentClass::GetPhpKeywords() const {
	if (pelet::PHP_54 == Structs->Environment.Php.Version) {
		return PHP_KEYWORDS + wxT(" ") + PHP_54_KEYWORDS;
	}
	return PHP_KEYWORDS;
}
wxString mvceditor::PhpDocumentClass::GetHtmlKeywords() const {
	return HTML_TAG_NAMES + wxT(" ") + HTML_ATTRIBUTE_NAMES;
}

wxString mvceditor::PhpDocumentClass::GetJavascriptKeywords() const {
	return JAVASCRIPT_KEYWORDS;
}

mvceditor::SqlDocumentClass::SqlDocumentClass(mvceditor::ProjectClass* project, const mvceditor::DatabaseInfoClass& currentInfo) 
	: TextDocumentClass() 
	, Project(project)
	, CurrentInfo(currentInfo) {
		
}

bool mvceditor::SqlDocumentClass::CanAutoComplete() {
	return true;
}

void mvceditor::SqlDocumentClass::HandleAutoCompletion(wxString& completeStatus) {
	int currentPos = Ctrl->GetCurrentPos();
	int startPos = Ctrl->WordStartPosition(currentPos, true);
	int endPos = Ctrl->WordEndPosition(currentPos, true);
	UnicodeString symbol = 	GetSafeSubstring(startPos, endPos);
	
	std::vector<wxString> autoCompleteList = HandleAutoCompletionMySql(symbol);
	if (!autoCompleteList.empty()) {
		
		// scintilla needs the keywords sorted.
		sort(autoCompleteList.begin(), autoCompleteList.end());
		wxString list;
		for (size_t i = 0; i < autoCompleteList.size(); ++i) {
			list += autoCompleteList[i];
			if (i < (autoCompleteList.size() - 1)) {
				list += (wxChar)Ctrl->AutoCompGetSeparator();
			}
		}
		Ctrl->AutoCompSetMaxWidth(0);
		int wordLength = currentPos - startPos;
		Ctrl->AutoCompShow(wordLength, list);
	}
}

std::vector<wxString> mvceditor::SqlDocumentClass::HandleAutoCompletionMySql(const UnicodeString& word) {
	std::vector<wxString> autoCompleteList;
	if (word.length() < 1) {
		return autoCompleteList;
	 }
	wxString symbol = mvceditor::StringHelperClass::IcuToWx(word);
	symbol = symbol.Lower();
	wxStringTokenizer tokenizer(wxT(""));
	tokenizer.SetString(MYSQL_KEYWORDS, wxT(" "), wxTOKEN_STRTOK);
	while (tokenizer.HasMoreTokens()) {
		wxString it = tokenizer.NextToken();
		if (it.StartsWith(symbol)) {
			
			// make keywords uppercase for SQL keywords
			autoCompleteList.push_back(it.Upper());
		}
	}
	
	// look at the meta data
	mvceditor::SqlResourceFinderClass* finder = Project->GetSqlResourceFinder();
	if (!CurrentInfo.Host.isEmpty()) {
		UnicodeString error;
		std::vector<UnicodeString> results = finder->FindTables(CurrentInfo, word);
		for (size_t i = 0; i < results.size(); i++) {
			wxString s = mvceditor::StringHelperClass::IcuToWx(results[i]);
			autoCompleteList.push_back(s);
		}
		results = finder->FindColumns(CurrentInfo, word);
		for (size_t i = 0; i < results.size(); i++) {
			wxString s = mvceditor::StringHelperClass::IcuToWx(results[i]);
			autoCompleteList.push_back(s);
		}
	}
	return autoCompleteList;
}

wxString mvceditor::SqlDocumentClass::GetMySqlKeywords() const {
	return MYSQL_KEYWORDS;
}

void mvceditor::SqlDocumentClass::MatchBraces(int posToCheck) {
	if (!InCommentOrStringStyle(posToCheck)) {
		wxChar c1 = Ctrl->GetCharAt(posToCheck),
		            c2 = Ctrl->GetCharAt(posToCheck - 1);
		if (wxT('(') == c1 || wxT(')') == c1) {
			posToCheck = posToCheck;
		}
		else if (wxT('(') == c2 || wxT(')') == c2) {
			posToCheck = posToCheck - 1;
		}
		else  {
			posToCheck = -1;
		}
		if (posToCheck >= 0) {
			int pos = Ctrl->BraceMatch(posToCheck);
			if (wxSTC_INVALID_POSITION == pos) {
				Ctrl->BraceBadLight(posToCheck);
			}
			else {
				Ctrl->BraceHighlight(posToCheck, pos);
			}
		}
		else {
			Ctrl->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
		}
	}
	else {
		Ctrl->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
	}
}

bool mvceditor::SqlDocumentClass::InCommentOrStringStyle(int posToCheck) {
	int style = Ctrl->GetStyleAt(posToCheck);
	int prevStyle = Ctrl->GetStyleAt(posToCheck - 1);

	// dont match braces inside strings or comments. for some reason when styling line comments (//)
	// the last character is styled as default but the characters before are styled correctly (wxSTC_SQL_COMMENTLINE)
	// so lets check the previous character in that case
	return wxSTC_SQL_COMMENT == style || wxSTC_SQL_COMMENTDOC == style || wxSTC_SQL_COMMENTLINE == prevStyle
		|| wxSTC_SQL_COMMENTLINEDOC == prevStyle || wxSTC_SQL_QUOTEDIDENTIFIER == style || wxSTC_SQL_STRING == style
		|| wxSTC_SQL_CHARACTER == style;
}

mvceditor::CssDocumentClass::CssDocumentClass() 
	: TextDocumentClass() {
		
}

bool mvceditor::CssDocumentClass::CanAutoComplete() {
	return false;
}


wxString mvceditor::CssDocumentClass::GetCssKeywords() const {
	return CSS_KEYWORDS;
}

wxString mvceditor::CssDocumentClass::GetCssPseudoClasses() const {
	return CSS_PSEUDOCLASSES;
}

void mvceditor::CssDocumentClass::MatchBraces(int posToCheck) {
	if (!InCommentOrStringStyle(posToCheck)) {
		wxChar c1 = Ctrl->GetCharAt(posToCheck),
		            c2 = Ctrl->GetCharAt(posToCheck - 1);
		if (wxT('(') == c1 || wxT(')') == c1 || wxT('[') == c1 || wxT(']') == c1 || wxT('{') == c1 || wxT('}') == c1) {
			posToCheck = posToCheck;
		}
		else if (wxT('(') == c2 || wxT(')') == c2 || wxT('[') == c2 || wxT(']') == c2 || wxT('{') == c2 || wxT('}') == c2) {
			posToCheck = posToCheck - 1;
		}
		else  {
			posToCheck = -1;
		}
		if (posToCheck >= 0) {
			int pos = Ctrl->BraceMatch(posToCheck);
			if (wxSTC_INVALID_POSITION == pos) {
				Ctrl->BraceBadLight(posToCheck);
			}
			else {
				Ctrl->BraceHighlight(posToCheck, pos);
			}
		}
		else {
			Ctrl->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
		}
	}
	else {
		Ctrl->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
	}
}

bool mvceditor::CssDocumentClass::InCommentOrStringStyle(int posToCheck) {
	int style = Ctrl->GetStyleAt(posToCheck);

	// dont match braces inside strings or comments.
	return wxSTC_CSS_COMMENT == style || wxSTC_CSS_DOUBLESTRING == style || wxSTC_CSS_SINGLESTRING == style;
}		

BEGIN_EVENT_TABLE(mvceditor::PhpDocumentClass, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::PhpDocumentClass::OnResourceUpdateComplete)
	EVT_TIMER(wxID_ANY, mvceditor::PhpDocumentClass::OnTimer)
END_EVENT_TABLE()