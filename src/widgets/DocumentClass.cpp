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
#include <widgets/DocumentClass.h>
#include <windows/StringHelperClass.h>
#include <wx/tokenzr.h>


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



mvceditor::TextDocumentClass::TextDocumentClass() {
	
}

mvceditor::TextDocumentClass::~TextDocumentClass() {
	
}

bool mvceditor::TextDocumentClass::CanAutoComplete() {
	return false;
}

std::vector<wxString> mvceditor::TextDocumentClass::HandleAutoComplete(const wxString& fileName, const UnicodeString& code, const UnicodeString& word) {	
	std::vector<wxString> ret;
	return ret;
}


mvceditor::PhpDocumentClass::PhpDocumentClass(mvceditor::ProjectClass* project, mvceditor::ResourceUpdateThreadClass* resourceUpdates)
	: TextDocumentClass()
	, LanguageDiscovery()
	, SymbolTable()
	, Project(project)
	, ResourceUpdates(resourceUpdates) {
}

bool mvceditor::PhpDocumentClass::CanAutoComplete() {
	return true;
}

std::vector<wxString> mvceditor::PhpDocumentClass::HandleAutoComplete(const wxString& fileName, const UnicodeString& code, const UnicodeString& word) {	
	std::vector<wxString> ret;
	if (LanguageDiscovery.Open(code)) {
		mvceditor::LanguageDiscoveryClass::Syntax syntax = LanguageDiscovery.at(code.length() - 1);
		switch (syntax) {
		case mvceditor::LanguageDiscoveryClass::SYNTAX_PHP_SCRIPT:
		case mvceditor::LanguageDiscoveryClass::SYNTAX_PHP_BACKTICK:
		case mvceditor::LanguageDiscoveryClass::SYNTAX_PHP_DOUBLE_QUOTE_STRING:
		case mvceditor::LanguageDiscoveryClass::SYNTAX_PHP_HEREDOC:
		case mvceditor::LanguageDiscoveryClass::SYNTAX_PHP_LINE_COMMENT:
		case mvceditor::LanguageDiscoveryClass::SYNTAX_PHP_MULTI_LINE_COMMENT:
		case mvceditor::LanguageDiscoveryClass::SYNTAX_PHP_NOWDOC:
		case mvceditor::LanguageDiscoveryClass::SYNTAX_PHP_SINGLE_QUOTE_STRING:
			ret = HandleAutoCompletionPhp(fileName, code, word, syntax);
			break;
		case mvceditor::LanguageDiscoveryClass::SYNTAX_HTML:
		case mvceditor::LanguageDiscoveryClass::SYNTAX_HTML_TAG:
		case mvceditor::LanguageDiscoveryClass::SYNTAX_HTML_ATTRIBUTE:
		case mvceditor::LanguageDiscoveryClass::SYNTAX_HTML_ATTRIBUTE_DOUBLE_QUOTE_VALUE:
		case mvceditor::LanguageDiscoveryClass::SYNTAX_HTML_ATTRIBUTE_SINGLE_QUOTE_VALUE:
		case mvceditor::LanguageDiscoveryClass::SYNTAX_HTML_ENTITY:
			ret = HandleAutoCompletionHtml(code, word, syntax);
			break;
		}
		LanguageDiscovery.Close();
	}	
	return ret;
}

std::vector<wxString> mvceditor::PhpDocumentClass::HandleAutoCompletionHtml(const UnicodeString& code, const UnicodeString& word, mvceditor::LanguageDiscoveryClass::Syntax syntax) {
	std::vector<wxString> autoCompleteList;
	if (word.length() < 1) {
		 return autoCompleteList;
	}
	wxStringTokenizer tokenizer(wxT(""));
	if (mvceditor::LanguageDiscoveryClass::SYNTAX_HTML_ATTRIBUTE == syntax) {
		tokenizer.SetString(HTML_ATTRIBUTE_NAMES, wxT(" "), wxTOKEN_STRTOK);
	}
	else if (mvceditor::LanguageDiscoveryClass::SYNTAX_HTML_TAG == syntax) {
		tokenizer.SetString(HTML_TAG_NAMES, wxT(" "), wxTOKEN_STRTOK);
	}
	wxString symbol = mvceditor::StringHelperClass::IcuToWx(word);
	while (tokenizer.HasMoreTokens()) {
		wxString it = tokenizer.NextToken();
		if (it.StartsWith(symbol)) {
			autoCompleteList.push_back(it);
		}
	}
	return autoCompleteList;
}

std::vector<wxString> mvceditor::PhpDocumentClass::HandleAutoCompletionPhp(const wxString& fileName, const UnicodeString& code, const UnicodeString& word, mvceditor::LanguageDiscoveryClass::Syntax syntax) {
	std::vector<wxString> autoCompleteList;
	if (!ResourceUpdates) {
		return autoCompleteList;
	}
		
	// word will start with a '$" if its a variable
	//the word will contain the $ in case of variables since "$" is a word characters (via wxStyledTextCtrl::SetWordCharacters() call)
	if (word.charAt(0) == '$') {
		
		//+1 = do not take the '$' into account since SymbolTable drops the '$' 
		UnicodeString symbolName(word, 1);
		std::vector<UnicodeString> variables = ResourceUpdates->Worker.GetVariablesInScope(fileName, code.length() - 1, code);
		for (size_t i = 0; i < variables.size(); ++i) {
			if (0 == variables[i].indexOf(symbolName)) {
				autoCompleteList.push_back(wxT("$") + StringHelperClass::IcuToWx(variables[i]));
			}
		}
	}
	else {
		mvceditor::ResourceFinderClass* globalResourceFinder = Project->GetResourceFinder();
		
		// look up the type of the word (is the word in the context of a class, method or function ?
		// SymbolTable resolves stuff like parent:: and self:: as well we don't need to do it here
		mvceditor::SymbolClass symbol;
		UnicodeString symbolName = ResourceUpdates->Worker.GetSymbolAt(fileName, code.length() - 1, globalResourceFinder, symbol, code);
		
		
		// get all other resources that start like the word
		wxString wxSymbol = mvceditor::StringHelperClass::IcuToWx(symbolName);
		if (ResourceUpdates->Worker.PrepareAll(globalResourceFinder, wxSymbol)) {
			if (ResourceUpdates->Worker.CollectNearMatchResourcesFromAll(globalResourceFinder)) {
				std::vector<mvceditor::ResourceClass> matches = ResourceUpdates->Worker.Matches(globalResourceFinder);
				for (size_t i = 0; i < matches.size(); ++i) {

					
				}
			}
		}

		 // when completing method names, do NOT include keywords
		if (globalResourceFinder->GetResourceType() != ResourceFinderClass::CLASS_NAME_METHOD_NAME) {
			std::vector<wxString> keywordMatches = CollectNearMatchKeywords(wxSymbol);
			for (size_t i = 0; i < keywordMatches.size(); ++i) {
				autoCompleteList.push_back(keywordMatches[i]);
			}
		}
	}
	return autoCompleteList;
}

std::vector<wxString> mvceditor::PhpDocumentClass::CollectNearMatchKeywords(wxString resource) {
	resource = resource.Lower();
	std::vector<wxString> matchedKeywords;
	wxString keywords = GetPhpKeywords();
	wxStringTokenizer tokens(keywords, wxT(" "));
	while (tokens.HasMoreTokens()) {
		wxString keyword = tokens.GetNextToken();
		if (0 == keyword.Find(resource)) {
			matchedKeywords.push_back(keyword);
		}
	}
	return matchedKeywords;
}

wxString mvceditor::PhpDocumentClass::GetPhpKeywords() const {
	return Project->GetPhpKeywords();
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

std::vector<wxString> mvceditor::SqlDocumentClass::HandleAutoComplete(const wxString& fileName, const UnicodeString& code, const UnicodeString& word) {
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
	// TODO: mysql information_schema stuff does not show up under code completion
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

mvceditor::CssDocumentClass::CssDocumentClass() 
	: TextDocumentClass() {
		
}

bool mvceditor::CssDocumentClass::CanAutoComplete() {
	return false;
}

std::vector<wxString> mvceditor::CssDocumentClass::HandleAutoComplete(const wxString& fileName, const UnicodeString& code, const UnicodeString& word) {
	std::vector<wxString> autoCompleteList;
	return autoCompleteList;
}


wxString mvceditor::CssDocumentClass::GetCssKeywords() const {
	return CSS_KEYWORDS;
}

wxString mvceditor::CssDocumentClass::GetCssPseudoClasses() const {
	return CSS_PSEUDOCLASSES;
}