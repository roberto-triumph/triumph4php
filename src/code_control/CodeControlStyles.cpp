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
	
	options.StartEditMode();
	CodeControlStylesSetToLightTheme(options);
	options.CommitChanges();
}

void mvceditor::CodeControlStylesSetToLightTheme(mvceditor::CodeControlOptionsClass& options) {
	
	// this theme is copied from Visual Studio settings
	// however, font name will depend based on OS
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	
	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		//default font: some websites say DejaVu Sans Mono is a good programming font
		fontName = wxT("DejaVu Sans Mono");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
	            fontName);
	for (size_t i = 0; i < options.EditedPhpStyles.size(); ++i) {
		options.EditedPhpStyles[i].Font = font;
		options.EditedPhpStyles[i].Color = *wxBLACK;
		options.EditedPhpStyles[i].BackgroundColor = *wxWHITE;
		options.EditedPhpStyles[i].IsBold = false;
		options.EditedPhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.EditedSqlStyles.size(); ++i) {
		options.EditedSqlStyles[i].Font = font;
		options.EditedSqlStyles[i].Color = *wxBLACK;
		options.EditedSqlStyles[i].BackgroundColor = *wxWHITE;
		options.EditedSqlStyles[i].IsBold = false;
		options.EditedSqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.EditedCssStyles.size(); ++i) {
		options.EditedCssStyles[i].Font = font;
		options.EditedCssStyles[i].Color = *wxBLACK;
		options.EditedCssStyles[i].BackgroundColor = *wxWHITE;
		options.EditedCssStyles[i].IsBold = false;
		options.EditedCssStyles[i].IsItalic = false;
	}

	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(163, 21, 21);	
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(163, 21, 21);
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_WORD).Color = *wxBLUE;	
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_NUMBER).Color = wxTheColourDatabase->Find(wxT("DARK GRAY"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_COMMENT).Color = wxColor(0, 128, 0);
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColor(0, 128, 0);
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_STYLE_BRACELIGHT).Color = *wxGREEN;
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_STYLE_BRACEBAD).Color = *wxRED;
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(0, 64, 128);
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_STYLE_INDENTGUIDE).Color = wxColour(wxT("#D3D3D3"));
	options.FindByStcStyle(options.EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = *wxBLACK;	
	options.FindByStcStyle(options.EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(192, 192, 192);
	options.FindByStcStyle(options.EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = *wxBLUE;
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_H_TAG).Color = wxColour(163, 21, 21);
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_H_ATTRIBUTE).Color = *wxRED;
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_H_DOUBLESTRING).Color = *wxBLUE;
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_H_SINGLESTRING).Color = *wxBLUE;
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_H_COMMENT).Color = wxColour(0, 128, 0);
	
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_COMMENT).Color = wxColor(0, 128, 0);
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColor(0, 128, 0);
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColor(0, 128, 0);
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_WORD).Color = *wxBLUE;
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_WORD2).Color = wxColour(163, 21, 21);
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_STRING).Color = wxColour(163, 21, 21);
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(163, 21, 21);
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColor(0, 128, 0);	
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_STYLE_BRACELIGHT).Color = *wxGREEN;
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_STYLE_BRACEBAD).Color = *wxRED;
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(0, 64, 128);
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_STYLE_INDENTGUIDE).Color = wxColour(wxT("#D3D3D3"));
	options.FindByStcStyle(options.EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = *wxBLACK;	
	options.FindByStcStyle(options.EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(192, 192, 192);
	options.FindByStcStyle(options.EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = *wxBLUE;
	
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_ATTRIBUTE).Color = *wxBLUE;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_CLASS).Color = *wxBLUE;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_COMMENT).Color = wxColor(0, 128, 0);
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_DIRECTIVE).Color = *wxBLUE;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(163, 21, 21);
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_ID).Color = *wxBLUE;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_IDENTIFIER).Color = *wxRED;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_IDENTIFIER2).Color = *wxRED;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_PSEUDOCLASS).Color = *wxRED;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(163, 21, 21);
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = *wxRED;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = *wxRED;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_STYLE_BRACELIGHT).Color = *wxGREEN;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_STYLE_BRACEBAD).Color = *wxRED;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(0, 64, 128);
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_STYLE_INDENTGUIDE).Color = wxColour(wxT("#D3D3D3"));
	options.FindByStcStyle(options.EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = *wxBLACK;	
	options.FindByStcStyle(options.EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(192, 192, 192);
	options.FindByStcStyle(options.EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = *wxBLUE;
}

void mvceditor::CodeControlStylesSetToDarkTheme(mvceditor::CodeControlOptionsClass& options) {
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

	// 100% props to the creator of the "zenburn" theme:
	// http://slinky.imukuppi.org/2006/10/31/just-some-alien-fruit-salad-to-keep-you-in-the-zone/
	for (size_t i = 0; i < options.EditedPhpStyles.size(); ++i) {
		options.EditedPhpStyles[i].Font = font;
		options.EditedPhpStyles[i].Color = wxColour(wxT("#DCDCCC"));
		options.EditedPhpStyles[i].BackgroundColor = wxColour(wxT("#1F1F1F"));
		options.EditedPhpStyles[i].IsBold = false;
		options.EditedPhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.EditedSqlStyles.size(); ++i) {
		options.EditedSqlStyles[i].Font = font;
		options.EditedSqlStyles[i].Color = wxColour(wxT("#DCDCCC"));
		options.EditedSqlStyles[i].BackgroundColor = wxColour(wxT("#1F1F1F"));
		options.EditedSqlStyles[i].IsBold = false;
		options.EditedSqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < options.EditedCssStyles.size(); ++i) {
		options.EditedCssStyles[i].Font = font;
		options.EditedCssStyles[i].Color = wxColour(wxT("#DCDCCC"));
		options.EditedCssStyles[i].BackgroundColor = wxColour(wxT("#1F1F1F"));
		options.EditedCssStyles[i].IsBold = false;
		options.EditedCssStyles[i].IsItalic = false;
	}

	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#CC9393"));	
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#DFC47D"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#8CD0D3"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#CEDF99"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#CEDF99"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#9F9D6D"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#AE0000"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#8A8A8A"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#D74100"));
	options.FindByStcStyle(options.EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#D74100"));
	options.FindByStcStyle(options.EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#646464"));
	options.FindByStcStyle(options.EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxTheColourDatabase->Find(wxT("YELLOW"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#DFC47D"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#DFC47D"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#DFC47D"));
	options.FindByStcStyle(options.EditedPhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#DFC47D"));

	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#DFC47D"));
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#CC9393"));;
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColor(0, 128, 0);	
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#AE0000"));
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#8A8A8A"));
	options.FindByStcStyle(options.EditedSqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#D74100"));
	options.FindByStcStyle(options.EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#D74100"));
	options.FindByStcStyle(options.EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#646464"));
	options.FindByStcStyle(options.EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxTheColourDatabase->Find(wxT("YELLOW"));
	
	
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#CEDF99"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#CEDF99"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#7F9F7F"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#DFC47D"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#CEDF99"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#DFC47D"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#DFC47D"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#DFC47D"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#DFC47D"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#CC9393"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#DFC47D"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#DFC47D"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#AE0000"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#8A8A8A"));
	options.FindByStcStyle(options.EditedCssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#D74100"));
	options.FindByStcStyle(options.EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#D74100"));
	options.FindByStcStyle(options.EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	options.FindByStcStyle(options.EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#646464"));
	options.FindByStcStyle(options.EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxTheColourDatabase->Find(wxT("YELLOW"));
}