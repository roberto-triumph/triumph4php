/**
 * The MIT License
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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <code_control/CodeControlStyles.h>
#include <wx/stc/stc.h>


void mvceditor::CodeControlStylesInit(mvceditor::CodeControlOptionsClass& options) {	
	wxOperatingSystemId os = wxGetOsVersion();
	if (wxOS_WINDOWS == os) {  
		options.LineEndingMode = wxSTC_EOL_CRLF;
	}
	else if (wxOS_MAC == os) {
		options.LineEndingMode = wxSTC_EOL_CR;
	}
	else {
		options.LineEndingMode = wxSTC_EOL_LF;	
	}
	int index = 0;
	mvceditor::StylePreferenceClass pref;
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_HPHP_DEFAULT, "PHP Default");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_HPHP_HSTRING, "PHP Double Quoted String");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_HPHP_SIMPLESTRING, "PHP Single Quoted String");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_HPHP_WORD, "PHP Keyword");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_HPHP_NUMBER, "PHP Number Constant");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_HPHP_VARIABLE, "PHP Variable");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_HPHP_COMMENT, "PHP Multi Line Comment");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_HPHP_COMMENTLINE, "PHP Line Comment");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_HPHP_HSTRING_VARIABLE, "PHP Variable Inside Double Quoted String");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_HPHP_COMPLEX_VARIABLE, "PHP Complex Variable");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_HPHP_OPERATOR, "PHP Operator");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_STYLE_BRACELIGHT, "PHP Matching Brace");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_STYLE_BRACEBAD, "PHP Mismatched Brace");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_STYLE_LINENUMBER, "PHP Line Number Margin");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_STYLE_INDENTGUIDE, "PHP Indentation Guides");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET, "PHP Caret");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE, "PHP Current Line");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION, "PHP Selection");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING, "PHP Code Folding Margin");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN, "PHP Right Margin Vertical Line");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT, "PHP Highlight Matches");

	// HTML lexer
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_H_DEFAULT, "HTML Default");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_H_TAG, "HTML Known Tag");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_H_TAGUNKNOWN, "HTML Unknown Tag");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_H_ATTRIBUTE, "HTML Attribute");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_H_ATTRIBUTEUNKNOWN, "HTML Unknown Attribute");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_H_NUMBER, "HTML Number");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_H_DOUBLESTRING, "HTML Double Quoted String");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_H_SINGLESTRING, "HTML Single Quoted String");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_H_OTHER, "HTML Other");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_H_COMMENT, "HTML Comment");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_H_ENTITY, "HTML Entity");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_H_VALUE, "HTML Value");
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_H_QUESTION,  "HTML Embedded Script Start"); // <?php start tag
	options.PhpStyles.push_back(pref); options.PhpStyles[index++].Control(wxSTC_H_TAGEND, "HTML Embedded Script End"); // script end tag ?>	
	
	// javascript lexer
	// note that scintilla the CPP lexer is also the Javascript lexer
	index = 0;
	options.JsStyles.push_back(pref); options.JsStyles[index++].Control(wxSTC_C_DEFAULT, "Javascript Start");
	options.JsStyles.push_back(pref); options.JsStyles[index++].Control(wxSTC_C_COMMENT, "Javascript Multi Line Comment");
	options.JsStyles.push_back(pref); options.JsStyles[index++].Control(wxSTC_C_COMMENTLINE, "Javascript Line Comment");
	options.JsStyles.push_back(pref); options.JsStyles[index++].Control(wxSTC_C_COMMENTDOC, "Javascript Doc Comment");
	options.JsStyles.push_back(pref); options.JsStyles[index++].Control(wxSTC_C_NUMBER, "Javascript Number");
	options.JsStyles.push_back(pref); options.JsStyles[index++].Control(wxSTC_C_WORD, "Javascript Keyword");
	options.JsStyles.push_back(pref); options.JsStyles[index++].Control(wxSTC_C_IDENTIFIER, "Javascript Identifier");
	options.JsStyles.push_back(pref); options.JsStyles[index++].Control(wxSTC_C_STRING, "Javascript Double Quoted String");
	options.JsStyles.push_back(pref); options.JsStyles[index++].Control(wxSTC_C_CHARACTER, "Javascript Single Quoted String");
	options.JsStyles.push_back(pref); options.JsStyles[index++].Control(wxSTC_C_OPERATOR, "Javascript Operators");
	options.JsStyles.push_back(pref); options.JsStyles[index++].Control(wxSTC_C_STRINGEOL, "Javascript End-Of-Line");
	options.JsStyles.push_back(pref); options.JsStyles[index++].Control(wxSTC_C_REGEX, "Javascript Regular Expression");
	options.JsStyles.push_back(pref); options.JsStyles[index++].Control(wxSTC_C_COMMENTLINEDOC, "Javascript Singe Line Doc Comment");
	options.JsStyles.push_back(pref); options.JsStyles[index++].Control(wxSTC_C_WORD2, "Javascript Keyword 2");

	index = 0;
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_DEFAULT, "SQL Default");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_COMMENT, "SQL Comment");  // starts with "/*"
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_COMMENTLINE, "SQL Single Line Comment"); // starts with "--"
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_COMMENTDOC, "SQL Doc Comment"); // starts with or '/**' or '/*!' (doxygen)
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_NUMBER, "SQL Number");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_WORD, "SQL Keyword");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_STRING, "SQL String");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_CHARACTER, "SQL Character");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_OPERATOR, "SQL Operator");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_IDENTIFIER, "SQL Identifier");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_COMMENTLINEDOC, "SQL Single Line Doc Comment"); // starts with '#'
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_WORD2, "SQL Database Objects"); //table, column names that match the DB
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_COMMENTDOCKEYWORD, "SQL Stored Procedure Keyword"); // keyword on stored procedure
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_COMMENTDOCKEYWORDERROR, "SQL Doxygen Comment Error"); // doxygen error
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_USER1, "SQL User Keywords 1");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_USER2, "SQL User Keywords 2");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_USER3, "SQL User Keywords 3");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_USER4, "SQL User Keywords 4");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_SQL_QUOTEDIDENTIFIER, "SQL Quoted Identifier");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_STYLE_BRACELIGHT, "SQL Matching Brace");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_STYLE_BRACEBAD, "SQL Mismatched Brace");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_STYLE_LINENUMBER, "SQL Line Number Margin");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(wxSTC_STYLE_INDENTGUIDE, "SQL Indentation Guides");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET, "SQL Caret");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE, "SQL Current Line");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION, "SQL Selection");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING, "SQL Code Folding Margin");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN, "SQL Right Margin Vertical Line");
	options.SqlStyles.push_back(pref); options.SqlStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT, "SQL Highlight Matches");

	index = 0;
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_DEFAULT, "CSS Default");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_TAG, "CSS Tag Selector");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_CLASS, "CSS Class Selector");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_PSEUDOCLASS, "CSS Pseudo Class Selector");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_UNKNOWN_PSEUDOCLASS, "CSS Unknown Pseudo Class Selector");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_OPERATOR, "CSS Operator");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_IDENTIFIER, "CSS Property");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_UNKNOWN_IDENTIFIER, "CSS Unknown Property");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_VALUE, "CSS Value");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_COMMENT, "CSS Comment"); 
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_ID, "CSS ID Selector");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_IMPORTANT, "CSS Important");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_DIRECTIVE, "CSS Directive");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_DOUBLESTRING, "CSS Double Quoted String");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_SINGLESTRING, "CSS Single Quoted String");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_IDENTIFIER2, "CSS 2 Property");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_CSS_ATTRIBUTE, "CSS Attribute Selector");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_STYLE_BRACELIGHT, "CSS Matching Brace");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_STYLE_BRACEBAD, "CSS Mismatched Brace");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_STYLE_LINENUMBER, "CSS Line Number Margin");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(wxSTC_STYLE_INDENTGUIDE, "CSS Indentation Guides");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET, "CSS Caret");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE, "CSS Current Line");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION, "CSS Selection");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING, "CSS Code Folding Margin");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN, "CSS Right Margin Vertical Line");
	options.CssStyles.push_back(pref); options.CssStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT, "CSS Highlight Matches");
	
	mvceditor::CodeControlStylesSetTheme(options, mvceditor::CodeControlStylesGetThemes()[0]);
}






// *** ALL CODE BELOW IS GENERATED AUTOMATICALLY BY resources/color_themes/color-theme-parser.cpp *** //

static void SetToOblivionTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#D8D8D8"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#1E1E1E"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#D8D8D8"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#1E1E1E"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#D8D8D8"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#1E1E1E"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#D8D8D8"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#1E1E1E"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#FFC600"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#FFC600"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#7FB347"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#C7DD0C"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#C7DD0C"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#EFC090"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#2A2A2A"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#FFC600"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#FFC600"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#C7DD0C"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#D8D8D8"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#C7DD0C"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#C7DD0C"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#C7DD0C"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#7FB347"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#FFC600"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#FFC600"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#FFC600"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#C7DD0C"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#FFFFFF"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#C7DD0C"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#C7DD0C"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#C7DD0C"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#FFC600"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#FFC600"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#C7DD0C"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#EFC090"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#C7DD0C"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#FFC600"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#FFC600"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#EFC090"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToRettaTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#F8E1AA"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#F8E1AA"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#F8E1AA"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#F8E1AA"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#D6C248"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#D6C248"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#E79E3C"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#D6C248"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#83786E"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#83786E"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#D6C248"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#F8E1A3"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#C97138"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#C97138"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#2A2A2A"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#527D5D"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#D6C248"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#D6C248"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#83786E"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#F8E1AA"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#83786E"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#83786E"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#83786E"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#D6C248"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#E79E3C"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#D6C248"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#D6C248"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#D6C248"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#D6C248"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#83786E"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#E79E3C"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#83786E"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#83786E"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#83786E"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#E79E3C"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#D6C248"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#D6C248"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#83786E"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#F8E1A3"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#C97138"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#C97138"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#527D5D"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#E79E3C"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#83786E"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#D6C248"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#D6C248"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#F8E1A3"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#C97138"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#C97138"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#F8E1AA"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#527D5D"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToSolarized_lightTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#657A81"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#FDF6E3"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#657A81"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#FDF6E3"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#657A81"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#FDF6E3"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#657A81"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#FDF6E3"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#2AA198"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#B58900"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#93A1A1"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FDF6E3"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FDF6E3"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#FDF6E3"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#ECE7D5"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#657A81"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#93A1A1"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#B58900"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#586E75"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#B58900"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#93A1A1"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#B58900"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#2AA198"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#586E75"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FDF6E3"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FDF6E3"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#FDF6E3"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#ECE7D5"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#B58900"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FDF6E3"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FDF6E3"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#657A81"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#FDF6E3"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#ECE7D5"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToSolarized_darkTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#839496"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#002B36"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#839496"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#002B36"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#839496"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#002B36"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#839496"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#002B36"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#2AA198"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#B58900"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#268BD2"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#268BD2"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#268BD2"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#B58900"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#678CB1"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#657B83"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#002B36"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#657B83"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#002B36"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#839496"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#002B36"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#073642"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#268BD2"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#839496"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#839496"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#839496"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#839496"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#B58900"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#268BD2"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#B58900"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#268BD2"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#586E75"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#B58900"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#B58900"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#2AA198"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#586E75"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#678CB1"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#657B83"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#002B36"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#657B83"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#002B36"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#839496"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#002B36"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#073642"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#B58900"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#839496"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#586E75"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#839496"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#839496"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#839496"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#839496"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#839496"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#839496"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#2AA198"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#839496"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#839496"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#678CB1"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#657B83"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#002B36"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#657B83"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#002B36"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#839496"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#002B36"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#073642"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToGedit_original_oblivionTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#d3d7cf"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#2e3436"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#d3d7cf"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#2e3436"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#d3d7cf"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#2e3436"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#d3d7cf"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#2e3436"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#edd400"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#edd400"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#ce5c00"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#729fcf"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#729fcf"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#888a85"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#888a85"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#729fcf"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#EFC090"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#555753"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#2e3436"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#555753"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#2e3436"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#d3d7cf"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#555753"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#888a85"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#729fcf"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#d3d7cf"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#edd400"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#edd400"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#888a85"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#d3d7cf"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#d3d7cf"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#d3d7cf"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#888a85"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#888a85"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#888a85"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#ce5c00"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#729fcf"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#edd400"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#edd400"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#edd400"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#729fcf"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#888a85"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#FFFFFF"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#888a85"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#888a85"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#888a85"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#edd400"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#edd400"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#888a85"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#EFC090"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#555753"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#2e3436"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#555753"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#2e3436"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#d3d7cf"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#2e3436"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#888a85"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#d3d7cf"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#888a85"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#d3d7cf"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#edd400"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#d3d7cf"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#d3d7cf"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#d3d7cf"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#d3d7cf"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#d3d7cf"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#edd400"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#d3d7cf"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#d3d7cf"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#EFC090"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#555753"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#2e3436"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#555753"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#2e3436"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#d3d7cf"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#2e3436"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#888a85"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToWombatTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#f6f3e8"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#242424"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#f6f3e8"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#242424"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#f6f3e8"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#242424"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#f6f3e8"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#242424"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#95e454"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#95e454"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#8ac6f2"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#f08080"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#99968b"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#99968b"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#f3f6ee"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#93A2CC"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#656565"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#242424"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#656565"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#242424"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#f6f3e8"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#656565"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#898941"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#f6f3e8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#95e454"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#95e454"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#99968b"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#f6f3e8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#f6f3e8"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#f6f3e8"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#99968b"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#99968b"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#99968b"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#f08080"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#8ac6f2"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#95e454"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#95e454"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#f3f6ee"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#95e454"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#99968b"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#8ac6f2"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#99968b"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#99968b"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#99968b"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#8ac6f2"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#95e454"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#95e454"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#99968b"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#93A2CC"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#656565"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#242424"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#656565"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#242424"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#f6f3e8"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#242424"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#898941"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#8ac6f2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#f6f3e8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#99968b"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#f6f3e8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#95e454"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#f6f3e8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#f6f3e8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#f6f3e8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#f6f3e8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#f6f3e8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#95e454"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#f6f3e8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#f6f3e8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#93A2CC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#656565"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#242424"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#656565"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#242424"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#f6f3e8"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#242424"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#898941"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToBlack_pastelTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#C0C0C0"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#C0C0C0"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#C0C0C0"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#C0C0C0"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#c78d9b"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#82677E"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#E8E2B7"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#678CB1"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#2F393C"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#95bed8"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#C0C0C0"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#82677E"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#E8E2B7"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#7D8C93"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#82677E"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#82677E"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#c78d9b"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#7D8C93"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#678CB1"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#95bed8"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#82677E"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#678CB1"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#95bed8"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToVisual_studioTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#FFFFFF"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#FFFFFF"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#FFFFFF"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#FFFFFF"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#990000"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#990000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#3333ff"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#7FB347"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#00cc00"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#00cc00"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#EFC090"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#807575"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#757575"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#990000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#990000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#00cc00"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#FFFFFF"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#00cc00"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#00cc00"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#00cc00"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#7FB347"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#3333ff"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#990000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#990000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#990000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#00cc00"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#3333ff"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#00cc00"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#00cc00"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#00cc00"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#3333ff"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#990000"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#990000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#00cc00"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#EFC090"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#757575"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#3333ff"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#00cc00"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#990000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#990000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#EFC090"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#757575"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToObsidianTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#E0E2E4"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#293134"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#E0E2E4"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#293134"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#E0E2E4"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#293134"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#E0E2E4"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#293134"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#EC7600"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#EC7600"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#93C763"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#FFCD22"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#E8E2B7"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#678CB1"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#293134"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#293134"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#2F393C"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#804000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#EC7600"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#EC7600"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#E0E2E4"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#FFCD22"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#93C763"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#EC7600"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#EC7600"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#E8E2B7"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#EC7600"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#7D8C93"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#93C763"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#93C763"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#EC7600"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#EC7600"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#7D8C93"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#678CB1"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#293134"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#293134"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#293134"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#804000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#93C763"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#EC7600"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#EC7600"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#678CB1"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#293134"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#293134"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#293134"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#804000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToAs_visual_studio_2010Theme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#000000"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#000000"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#000000"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#000000"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#A31515"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#A31515"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#EEEEEE"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#EEEEEE"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#A31515"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#A31515"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#000000"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#A31515"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#A31515"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#A31515"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#008000"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#0000FF"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#A31515"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#A31515"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#008000"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#EEEEEE"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#A31515"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#A31515"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#EEEEEE"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToHavenjarkTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#C0B6A8"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#2D3639"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#C0B6A8"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#2D3639"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#C0B6A8"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#2D3639"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#C0B6A8"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#2D3639"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#CC9393"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#A38474"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#B9A185"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#A19A83"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#A19A83"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#AEAEAE"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#AEAEAE"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#A19A83"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#F0EFD0"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#93A2CC"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#2D3639"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#2D3639"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#C0B6A8"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#00001F"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#2A4750"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#A19A83"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#C0B6A8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#AEAEAE"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#C0B6A8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#C0B6A8"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#C0B6A8"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#AEAEAE"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#AEAEAE"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#AEAEAE"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#B9A185"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#A38474"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#A19A83"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#F0EFD0"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#A19A83"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#AEAEAE"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#A38474"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#AEAEAE"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#AEAEAE"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#AEAEAE"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#A38474"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#CC9393"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#AEAEAE"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#93A2CC"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#2D3639"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#2D3639"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#C0B6A8"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#2D3639"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#2A4750"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#A38474"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#C0B6A8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#AEAEAE"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#C0B6A8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#C0B6A8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#C0B6A8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#C0B6A8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#C0B6A8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#C0B6A8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#C0B6A8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#C0B6A8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#93A2CC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#2D3639"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#2D3639"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#C0B6A8"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#2D3639"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#2A4750"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToTangoTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#000000"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#000000"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#000000"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#000000"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#92679a"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#92679a"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#688046"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#801f91"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#17608f"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#17608f"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#885d3b"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#EEEEEE"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#EEEEEE"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#92679a"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#92679a"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#17608f"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#000000"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#17608f"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#17608f"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#17608f"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#801f91"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#688046"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#92679a"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#92679a"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#92679a"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#17608f"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#688046"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#17608f"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#17608f"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#17608f"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#688046"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#92679a"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#92679a"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#17608f"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#885d3b"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#EEEEEE"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#688046"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#17608f"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#92679a"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#92679a"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#885d3b"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#EEEEEE"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToSchussTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#430400"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#430400"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#430400"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#430400"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#585545"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#585545"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#606060"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#d0321f"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#2b6488"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#2b6488"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#d5d9e5"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#d7d3cc"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#2b6488"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#5f97a9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#464646"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#430400"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#fff7cd"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#f4fdff"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#2b6488"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#430400"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#585545"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#585545"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#d5d9e5"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#430400"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#430400"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#430400"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#d5d9e5"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#d7d3cc"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#d5d9e5"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#d0321f"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#606060"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#2b6488"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#585545"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#585545"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#5f97a9"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#585545"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#2b6488"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#d5d9e5"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#606060"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#d5d9e5"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#d7d3cc"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#d5d9e5"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#606060"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#585545"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#585545"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#d5d9e5"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#464646"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#430400"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#f4fdff"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#606060"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#430400"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#d5d9e5"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#430400"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#585545"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#430400"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#430400"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#430400"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#430400"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#430400"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#585545"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#430400"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#430400"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#464646"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#430400"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#f4fdff"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToNotepad_defaultTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#000000"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#FEFCF5"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#000000"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#FEFCF5"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#000000"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#FEFCF5"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#000000"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#FEFCF5"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#808080"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#808080"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#FF8000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#000080"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#000080"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#000080"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#8000FF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#800040"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#808080"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FEFCF5"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#808080"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FEFCF5"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#EEEEEE"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#EEEEEE"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#000080"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#808080"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#808080"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#000000"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#FF8000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#000080"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#808080"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#808080"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#8000FF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#808080"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#000080"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#008000"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#0000FF"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#808080"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#808080"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#008000"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#800040"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#808080"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FEFCF5"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#808080"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FEFCF5"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#FEFCF5"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#EEEEEE"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#008000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#808080"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#808080"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#800040"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#808080"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FEFCF5"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#808080"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FEFCF5"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#FEFCF5"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#EEEEEE"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToVibrant_inkTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#FFFFFF"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#191919"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#FFFFFF"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#191919"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#FFFFFF"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#191919"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#FFFFFF"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#191919"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#477488"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#477488"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#EC691E"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#477488"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#3C758D"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#3C758D"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#8C3FC8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#8146A2"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#3C758D"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#191919"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#191919"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#222220"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#414C3B"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#3C758D"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#477488"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#477488"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#8C3FC8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#FFFFFF"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#8C3FC8"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#8146A2"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#8C3FC8"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#477488"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#EC691E"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#3C758D"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#477488"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#477488"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#477488"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#3C758D"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#8C3FC8"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#EC691E"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#8C3FC8"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#8146A2"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#8C3FC8"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#EC691E"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#477488"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#477488"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#8C3FC8"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#191919"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#191919"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#191919"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#414C3B"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#EC691E"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#8C3FC8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#477488"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#477488"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#191919"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#191919"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#191919"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#414C3B"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToRecogneyesTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#D0D0D0"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#101020"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#D0D0D0"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#101020"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#D0D0D0"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#101020"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#D0D0D0"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#101020"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#DC78DC"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#DC78DC"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#00D0D0"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#FFFF00"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#00E000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#00E000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#EFC090"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#2B91AF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#101020"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#2B91AF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#101020"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#202030"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#DC78DC"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#DC78DC"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#00E000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#D0D0D0"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#00E000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#00E000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#00E000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#FFFF00"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#00D0D0"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#DC78DC"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#DC78DC"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#DC78DC"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#00E000"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#00D0D0"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#00E000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#00E000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#00E000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#00D0D0"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#DC78DC"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#DC78DC"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#00E000"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#EFC090"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#2B91AF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#101020"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#2B91AF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#101020"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#101020"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#00D0D0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#00E000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#DC78DC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#DC78DC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#EFC090"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#2B91AF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#101020"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#2B91AF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#101020"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#D0D0D0"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#101020"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToMrTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#333333"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#333333"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#333333"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#333333"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#FFFFFF"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#CC0000"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#CC0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#0066FF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#0066FF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#FF9900"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#FF9900"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#0066FF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#552200"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#0066FF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#CC0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#CC0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#FF9900"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#333333"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#FF9900"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#FF9900"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#FF9900"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#0066FF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#CC0000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#CC0000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#CC0000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#0066FF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#FF9900"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#0000FF"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#FF9900"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#FF9900"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#FF9900"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#CC0000"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#CC0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#FF9900"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#552200"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#FF9900"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#CC0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#CC0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#552200"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToSunburstTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#F9F9F9"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#F9F9F9"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#F9F9F9"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#F9F9F9"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#76BA53"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#76BA53"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#EA9C77"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#4B9CE9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#4B9CE9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#A8A8A8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#A8A8A8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#4B9CE9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#4B9CE9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#2F2F2F"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#DDF0FF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#4B9CE9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#76BA53"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#76BA53"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#A8A8A8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#F9F9F9"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#A8A8A8"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#A8A8A8"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#A8A8A8"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#EA9C77"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#4B9CE9"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#76BA53"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#76BA53"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#76BA53"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#4B9CE9"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#A8A8A8"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#EA9C77"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#A8A8A8"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#A8A8A8"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#A8A8A8"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#EA9C77"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#76BA53"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#76BA53"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#A8A8A8"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#4B9CE9"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#DDF0FF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#EA9C77"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#A8A8A8"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#76BA53"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#76BA53"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#4B9CE9"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#F9F9F9"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#DDF0FF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToInkpotTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#CFBFAD"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#1F1F27"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#CFBFAD"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#1F1F27"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#CFBFAD"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#1F1F27"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#CFBFAD"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#1F1F27"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#FFCD8B"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#FFCD8B"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#808BED"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#FFCD8B"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#CD8B00"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#CD8B00"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#2B91AF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#1F1F27"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#2B91AF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#1F1F27"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#2D2D44"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#8B8BFF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#FFCD8B"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#FFCD8B"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#CD8B00"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#CFBFAD"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#CD8B00"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#CD8B00"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#CD8B00"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#FFCD8B"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#808BED"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#FFCD8B"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#FFCD8B"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#FFCD8B"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#CD8B00"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#808BED"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#CD8B00"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#CD8B00"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#CD8B00"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#808BED"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#FFCD8B"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#FFCD8B"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#CD8B00"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#2B91AF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#1F1F27"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#2B91AF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#1F1F27"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#1F1F27"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#8B8BFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#808BED"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#CD8B00"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#FFCD8B"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#FFCD8B"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#2B91AF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#1F1F27"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#2B91AF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#1F1F27"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#1F1F27"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#8B8BFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToMinimalTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#000000"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#ffffff"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#000000"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#ffffff"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#000000"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#ffffff"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#000000"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#ffffff"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#333333"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#334466"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#334466"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#05314d"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#ffffff"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#ffffff"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#aaccff"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#Efefff"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#334466"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#000000"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#334466"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#334466"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#334466"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#334466"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#5c8198"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#334466"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#334466"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#334466"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#333333"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#334466"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#05314d"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#ffffff"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#ffffff"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#ffffff"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#Efefff"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#5c8198"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#334466"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#05314d"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#ffffff"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#ffffff"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#ffffff"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#Efefff"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToNightlion_aptana_themeTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#E2E2E2"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#1E1E1E"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#E2E2E2"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#1E1E1E"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#E2E2E2"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#1E1E1E"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#E2E2E2"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#1E1E1E"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#CC9393"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#8DCBE2"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#EAB882"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#73879B"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#F0EFD0"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#93A2CC"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#E2E2E2"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#505050"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#364656"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#E2E2E2"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#73879B"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#E2E2E2"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#E2E2E2"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#E2E2E2"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#73879B"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#73879B"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#EAB882"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#8DCBE2"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#F0EFD0"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#73879B"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#8DCBE2"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#73879B"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#73879B"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#8DCBE2"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#CC9393"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#73879B"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#93A2CC"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#E2E2E2"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#364656"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#8DCBE2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#E2E2E2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#73879B"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#E2E2E2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#E2E2E2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#E2E2E2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#E2E2E2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#E2E2E2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#E2E2E2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#E2E2E2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#E2E2E2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#93A2CC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#C0C0C0"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#E2E2E2"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#1E1E1E"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#364656"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToMonokaiTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#F8F8F2"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#272822"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#F8F8F2"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#272822"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#F8F8F2"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#272822"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#F8F8F2"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#272822"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#E6DB74"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#E6DB74"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#66CCB3"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#7FB347"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#75715e"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#75715E"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#EFC090"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#3E3D32"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#757575"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#E6DB74"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#E6DB74"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#75715e"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#F8F8F2"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#75715e"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#75715E"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#75715e"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#7FB347"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#66CCB3"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#E6DB74"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#E6DB74"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#D8D8D8"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#E6DB74"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#79ABFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#75715e"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#66CCB3"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#75715e"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#75715E"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#75715e"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#66CCB3"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#E6DB74"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#E6DB74"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#75715e"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#EFC090"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#757575"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#66CCB3"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#75715e"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#E6DB74"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#E6DB74"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#EFC090"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#F8F8F2"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#757575"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToSublime_text_2Theme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#CFBFAD"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#272822"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#CFBFAD"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#272822"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#CFBFAD"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#272822"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#CFBFAD"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#272822"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#ECE47E"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#ECE47E"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#FF007F"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#C48CFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#FF007F"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#5B5A4E"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#CC9900"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#ECE47E"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#ECE47E"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#CFBFAD"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#C48CFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#FF007F"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#ECE47E"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#ECE47E"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#FF007F"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#ECE47E"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#FFFFFF"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#FF007F"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#FF007F"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#ECE47E"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#ECE47E"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#FFFFFF"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#CC9900"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#FF007F"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#ECE47E"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#ECE47E"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#CFBFAD"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#272822"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#CC9900"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToPastelTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#E0E2E4"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#1f2223"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#E0E2E4"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#1f2223"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#E0E2E4"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#1f2223"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#E0E2E4"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#1f2223"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#c78d9b"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#a57b61"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#E8E2B7"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#678CB1"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#1f2223"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#1f2223"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#2F393C"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#95bed8"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#E0E2E4"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#a57b61"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#E8E2B7"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#7D8C93"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#a57b61"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#a57b61"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#c78d9b"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#7D8C93"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#678CB1"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#1f2223"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#1f2223"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#1f2223"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#95bed8"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#a57b61"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#7D8C93"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#c78d9b"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#678CB1"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#1f2223"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#81969A"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#1f2223"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#E0E2E4"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#1f2223"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#95bed8"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToZenburn_highcontrastTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#DCDCCC"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#1F1F1F"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#DCDCCC"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#1F1F1F"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#DCDCCC"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#1F1F1F"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#DCDCCC"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#1F1F1F"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#CC9393"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#F0DFAF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#8CD0D3"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#F0EFD0"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#93A2CC"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#9FAFAF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#1F1F1F"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#9FAFAF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#1F1F1F"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#DCDCCC"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#505050"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#71D3B4"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#DCDCCC"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#DCDCCC"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#DCDCCC"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#DCDCCC"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#8CD0D3"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#F0DFAF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#F0EFD0"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#D4C4A9"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#7F9F7F"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#F0DFAF"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#F0DFAF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#CC9393"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#7F9F7F"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#93A2CC"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#9FAFAF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#1F1F1F"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#9FAFAF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#1F1F1F"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#DCDCCC"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#1F1F1F"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#71D3B4"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#F0DFAF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#DCDCCC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#DCDCCC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#DCDCCC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#DCDCCC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#DCDCCC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#DCDCCC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#DCDCCC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#DCDCCC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#DCDCCC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#93A2CC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#9FAFAF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#1F1F1F"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#9FAFAF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#1F1F1F"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#DCDCCC"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#1F1F1F"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#71D3B4"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToFrontenddevTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#FFFFFF"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#FFFFFF"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#FFFFFF"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#FFFFFF"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#000000"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#00a40f"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#00a40f"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#F7C527"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#F7C527"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#F7C527"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#222220"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#F7C527"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#00a40f"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#00a40f"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#FFFFFF"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#F7C527"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#00a40f"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#00a40f"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#00a40f"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#F7C527"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#666666"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#999999"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#00a40f"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#00a40f"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#666666"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#666666"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#00a40f"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#00a40f"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#999999"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#FFFFFF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#333333"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}
static void SetToRoboticketTheme(mvceditor::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
				
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("#585858"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("#F5F5F5"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("#585858"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("#F5F5F5"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("#585858"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("#F5F5F5"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("#585858"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("#F5F5F5"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#317ECC"));	
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#317ECC"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#295F94"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#AF0F91"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#55aa55"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#55aa55"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#AD95AF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#AD95AF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#55aa55"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#885D3B"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#AFBFCF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#F5F5F5"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#AFBFCF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#F5F5F5"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#585858"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#E0E0FF"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#BDD8F2"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#55aa55"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#585858"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#317ECC"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#317ECC"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#AD95AF"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#585858"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#585858"));
	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("#585858"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("#AD95AF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("#AD95AF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("#AD95AF"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("#AF0F91"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("#295F94"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("#55aa55"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("#317ECC"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("#317ECC"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("#317ECC"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("#55aa55"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("#AD95AF"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("#295F94"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#AD95AF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#AD95AF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#AD95AF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#295F94"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#317ECC"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#317ECC"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("#AD95AF"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#885D3B"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#AFBFCF"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#F5F5F5"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#AFBFCF"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#F5F5F5"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#585858"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#F5F5F5"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#BDD8F2"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#295F94"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#585858"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#AD95AF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#585858"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#317ECC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#585858"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#585858"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#585858"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#585858"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#585858"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#317ECC"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#585858"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#585858"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#885D3B"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#AFBFCF"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#F5F5F5"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#AFBFCF"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#F5F5F5"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#585858"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#F5F5F5"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#BDD8F2"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("#FFFF00"));
}

void mvceditor::CodeControlStylesSetTheme(mvceditor::CodeControlOptionsClass& options, const wxString& theme) {
	if (theme == wxT("Oblivion")) {
		SetToOblivionTheme(options);
	}
	else if (theme == wxT("Retta")) {
		SetToRettaTheme(options);
	}
	else if (theme == wxT("Solarized light")) {
		SetToSolarized_lightTheme(options);
	}
	else if (theme == wxT("Solarized dark.")) {
		SetToSolarized_darkTheme(options);
	}
	else if (theme == wxT("Gedit original oblivion")) {
		SetToGedit_original_oblivionTheme(options);
	}
	else if (theme == wxT("Wombat")) {
		SetToWombatTheme(options);
	}
	else if (theme == wxT("Black pastel")) {
		SetToBlack_pastelTheme(options);
	}
	else if (theme == wxT("Visual studio")) {
		SetToVisual_studioTheme(options);
	}
	else if (theme == wxT("Obsidian")) {
		SetToObsidianTheme(options);
	}
	else if (theme == wxT("As visual studio 2010")) {
		SetToAs_visual_studio_2010Theme(options);
	}
	else if (theme == wxT("Havenjark")) {
		SetToHavenjarkTheme(options);
	}
	else if (theme == wxT("Tango")) {
		SetToTangoTheme(options);
	}
	else if (theme == wxT("Schuss")) {
		SetToSchussTheme(options);
	}
	else if (theme == wxT("Notepad++ default")) {
		SetToNotepad_defaultTheme(options);
	}
	else if (theme == wxT("Vibrant ink")) {
		SetToVibrant_inkTheme(options);
	}
	else if (theme == wxT("Recogneyes")) {
		SetToRecogneyesTheme(options);
	}
	else if (theme == wxT("Mr")) {
		SetToMrTheme(options);
	}
	else if (theme == wxT("Sunburst")) {
		SetToSunburstTheme(options);
	}
	else if (theme == wxT("Inkpot")) {
		SetToInkpotTheme(options);
	}
	else if (theme == wxT("Minimal")) {
		SetToMinimalTheme(options);
	}
	else if (theme == wxT("Nightlion aptana theme")) {
		SetToNightlion_aptana_themeTheme(options);
	}
	else if (theme == wxT("Monokai")) {
		SetToMonokaiTheme(options);
	}
	else if (theme == wxT("Sublime text 2")) {
		SetToSublime_text_2Theme(options);
	}
	else if (theme == wxT("Pastel")) {
		SetToPastelTheme(options);
	}
	else if (theme == wxT("Zenburn high-contrast")) {
		SetToZenburn_highcontrastTheme(options);
	}
	else if (theme == wxT("Frontenddev")) {
		SetToFrontenddevTheme(options);
	}
	else if (theme == wxT("Roboticket")) {
		SetToRoboticketTheme(options);
	}
}
wxArrayString mvceditor::CodeControlStylesGetThemes() {
	wxArrayString themes;
		themes.Add(wxT("Oblivion"));
	themes.Add(wxT("Retta"));
	themes.Add(wxT("Solarized light"));
	themes.Add(wxT("Solarized dark."));
	themes.Add(wxT("Gedit original oblivion"));
	themes.Add(wxT("Wombat"));
	themes.Add(wxT("Black pastel"));
	themes.Add(wxT("Visual studio"));
	themes.Add(wxT("Obsidian"));
	themes.Add(wxT("As visual studio 2010"));
	themes.Add(wxT("Havenjark"));
	themes.Add(wxT("Tango"));
	themes.Add(wxT("Schuss"));
	themes.Add(wxT("Notepad++ default"));
	themes.Add(wxT("Vibrant ink"));
	themes.Add(wxT("Recogneyes"));
	themes.Add(wxT("Mr"));
	themes.Add(wxT("Sunburst"));
	themes.Add(wxT("Inkpot"));
	themes.Add(wxT("Minimal"));
	themes.Add(wxT("Nightlion aptana theme"));
	themes.Add(wxT("Monokai"));
	themes.Add(wxT("Sublime text 2"));
	themes.Add(wxT("Pastel"));
	themes.Add(wxT("Zenburn high-contrast"));
	themes.Add(wxT("Frontenddev"));
	themes.Add(wxT("Roboticket"));
	return themes;
}
