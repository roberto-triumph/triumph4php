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
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
 #include <widgets/CodeControlOptionsClass.h>
 #include <wx/stc/stc.h>
 

mvceditor::CodeControlOptionsClass::CodeControlOptionsClass() {
	SpacesPerIndent = 0;
	TabWidth = 4;
	IndentUsingTabs = true;
	EnableCodeFolding = true;
	EnableAutomaticLineIndentation = true;
	EnableLineNumbers = true;
	EnableIndentationGuides = false;
	EnableLineEndings = false;
	EnableAutoCompletion = true;
	wxOperatingSystemId os = wxGetOsVersion();
	if (wxOS_WINDOWS == os) {  
		LineEndingMode = wxSTC_EOL_CRLF;
	}
	else if (wxOS_MAC == os) {
		LineEndingMode = wxSTC_EOL_CR;
	}
	else {
		LineEndingMode = wxSTC_EOL_LF;	
		
		// there is a bad flicker that happens when Code Folding and matching braces are enabled ...
		// turn off code folding in linux by default.
		// ATTN: fixed in new versions of Scintilla?
		EnableCodeFolding = false;
	}
	RightMargin = 0;
	
	int index = 0;
	mvceditor::StylePreferenceClass pref;
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_HPHP_DEFAULT, "PHP Default");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_HPHP_HSTRING, "PHP Double Quoted String");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_HPHP_SIMPLESTRING, "PHP Single Quoted String");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_HPHP_WORD, "PHP Keyword");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_HPHP_NUMBER, "PHP Number Constant");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_HPHP_VARIABLE, "PHP Variable");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_HPHP_COMMENT, "PHP Multi Line Comment");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_HPHP_COMMENTLINE, "PHP Line Comment");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_HPHP_HSTRING_VARIABLE, "PHP Variable Inside Double Quoted String");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_HPHP_COMPLEX_VARIABLE, "PHP Complex Variable");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_HPHP_OPERATOR, "PHP Operator");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_STYLE_BRACELIGHT, "PHP Matching Brace");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_STYLE_BRACEBAD, "PHP Mismatched Brace");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_STYLE_LINENUMBER, "PHP Line Number Margin");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_STYLE_INDENTGUIDE, "PHP Indentation Guides");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET, "PHP Caret");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE, "PHP Current Line");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION, "PHP Selection");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING, "PHP Code Folding Margin");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN, "PHP Right Margin Vertical Line");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT, "PHP Highlight Matches");

	// HTML lexer
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_H_DEFAULT, "HTML Default");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_H_TAG, "HTML Known Tag");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_H_TAGUNKNOWN, "HTML Unknown Tag");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_H_ATTRIBUTE, "HTML Attribute");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_H_ATTRIBUTEUNKNOWN, "HTML Unknown Attribute");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_H_NUMBER, "HTML Number");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_H_DOUBLESTRING, "HTML Double Quoted String");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_H_SINGLESTRING, "HTML Single Quoted String");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_H_OTHER, "HTML Other");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_H_COMMENT, "HTML Comment");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_H_ENTITY, "HTML Entity");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_H_VALUE, "HTML Value");
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_H_QUESTION,  "HTML Embedded Script Start"); // <?php start tag
	PhpStyles.push_back(pref); PhpStyles[index++].Control(wxSTC_H_TAGEND, "HTML Embedded Script End"); // script end tag ?>	
	
	index = 0;
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_DEFAULT, "SQL Default");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_COMMENT, "SQL Comment");  // starts with "/*"
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_COMMENTLINE, "SQL Single Line Comment"); // starts with "--"
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_COMMENTDOC, "SQL Doc Comment"); // starts with or '/**' or '/*!' (doxygen)
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_NUMBER, "SQL Number");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_WORD, "SQL Keyword");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_STRING, "SQL String");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_CHARACTER, "SQL Character");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_OPERATOR, "SQL Operator");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_IDENTIFIER, "SQL Identifier");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_COMMENTLINEDOC, "SQL Single Line Doc Comment"); // starts with '#'
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_WORD2, "SQL Database Objects"); //table, column names that match the DB
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_COMMENTDOCKEYWORD, "SQL Stored Procedure Keyword"); // keyword on stored procedure
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_COMMENTDOCKEYWORDERROR, "SQL Doxygen Comment Error"); // doxygen error
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_USER1, "SQL User Keywords 1");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_USER2, "SQL User Keywords 2");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_USER3, "SQL User Keywords 3");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_USER4, "SQL User Keywords 4");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_SQL_QUOTEDIDENTIFIER, "SQL Quoted Identifier");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_STYLE_BRACELIGHT, "SQL Matching Brace");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_STYLE_BRACEBAD, "SQL Mismatched Brace");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_STYLE_LINENUMBER, "SQL Line Number Margin");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(wxSTC_STYLE_INDENTGUIDE, "SQL Indentation Guides");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET, "SQL Caret");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE, "SQL Current Line");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION, "SQL Selection");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING, "SQL Code Folding Margin");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN, "SQL Right Margin Vertical Line");
	SqlStyles.push_back(pref); SqlStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT, "SQL Highlight Matches");

	index = 0;
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_DEFAULT, "CSS Default");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_TAG, "CSS Tag Selector");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_CLASS, "CSS Class Selector");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_PSEUDOCLASS, "CSS Pseudo Class Selector");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_UNKNOWN_PSEUDOCLASS, "CSS Unknown Pseudo Class Selector");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_OPERATOR, "CSS Operator");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_IDENTIFIER, "CSS Property");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_UNKNOWN_IDENTIFIER, "CSS Unknown Property");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_VALUE, "CSS Value");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_COMMENT, "CSS Comment"); 
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_ID, "CSS ID Selector");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_IMPORTANT, "CSS Important");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_DIRECTIVE, "CSS Directive");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_DOUBLESTRING, "CSS Double Quoted String");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_SINGLESTRING, "CSS Single Quoted String");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_IDENTIFIER2, "CSS 2 Property");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_CSS_ATTRIBUTE, "CSS Attribute Selector");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_STYLE_BRACELIGHT, "CSS Matching Brace");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_STYLE_BRACEBAD, "CSS Mismatched Brace");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_STYLE_LINENUMBER, "CSS Line Number Margin");
	CssStyles.push_back(pref); CssStyles[index++].Control(wxSTC_STYLE_INDENTGUIDE, "CSS Indentation Guides");
	CssStyles.push_back(pref); CssStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET, "CSS Caret");
	CssStyles.push_back(pref); CssStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE, "CSS Current Line");
	CssStyles.push_back(pref); CssStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION, "CSS Selection");
	CssStyles.push_back(pref); CssStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING, "CSS Code Folding Margin");
	CssStyles.push_back(pref); CssStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN, "CSS Right Margin Vertical Line");
	CssStyles.push_back(pref); CssStyles[index++].Control(CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT, "CSS Highlight Matches");
	
	StartEditMode();
	SetToLightTheme();
	CommitChanges();
}

void mvceditor::CodeControlOptionsClass::CommitChanges() {
	for (size_t i = 0; i < PhpStyles.size(); ++i) {
		PhpStyles[i].Copy(EditedPhpStyles[i]);
	}
	for (size_t i = 0; i < SqlStyles.size(); ++i) {
		SqlStyles[i].Copy(EditedSqlStyles[i]);
	}
	for (size_t i = 0; i < CssStyles.size(); ++i) {
		CssStyles[i].Copy(EditedCssStyles[i]);
	}
}

void mvceditor::CodeControlOptionsClass::StartEditMode() {
	
	// make sure that the original and edited vectors are the same size
	EditedPhpStyles.resize(PhpStyles.size());
	EditedSqlStyles.resize(SqlStyles.size());
	EditedCssStyles.resize(CssStyles.size());
	
	for (size_t i = 0; i < PhpStyles.size(); ++i) {
		EditedPhpStyles[i].Copy(PhpStyles[i]);
	}
	for (size_t i = 0; i < SqlStyles.size(); ++i) {
		EditedSqlStyles[i].Copy(SqlStyles[i]);
	}
	for (size_t i = 0; i < CssStyles.size(); ++i) {
		EditedCssStyles[i].Copy(CssStyles[i]);
	}
}

mvceditor::StylePreferenceClass& mvceditor::CodeControlOptionsClass::FindByStcStyle(std::vector<mvceditor::StylePreferenceClass>& styles, int stcStyle) const {
	for (size_t i = 0; i < styles.size(); ++i) {
		if (styles[i].StcStyle == stcStyle) {
			return styles[i];
		}
	}
	return styles[0];
}

void mvceditor::CodeControlOptionsClass::SetToLightTheme() {
	
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
	for (size_t i = 0; i < EditedPhpStyles.size(); ++i) {
		EditedPhpStyles[i].Font = font;
		EditedPhpStyles[i].Color = *wxBLACK;
		EditedPhpStyles[i].BackgroundColor = *wxWHITE;
		EditedPhpStyles[i].IsBold = false;
		EditedPhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < EditedSqlStyles.size(); ++i) {
		EditedSqlStyles[i].Font = font;
		EditedSqlStyles[i].Color = *wxBLACK;
		EditedSqlStyles[i].BackgroundColor = *wxWHITE;
		EditedSqlStyles[i].IsBold = false;
		EditedSqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < EditedCssStyles.size(); ++i) {
		EditedCssStyles[i].Font = font;
		EditedCssStyles[i].Color = *wxBLACK;
		EditedCssStyles[i].BackgroundColor = *wxWHITE;
		EditedCssStyles[i].IsBold = false;
		EditedCssStyles[i].IsItalic = false;
	}

	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(163, 21, 21);	
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(163, 21, 21);
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_WORD).Color = *wxBLUE;	
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_NUMBER).Color = wxTheColourDatabase->Find(wxT("DARK GRAY"));
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_COMMENT).Color = wxColor(0, 128, 0);
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColor(0, 128, 0);
	FindByStcStyle(EditedPhpStyles, wxSTC_STYLE_BRACELIGHT).Color = *wxGREEN;
	FindByStcStyle(EditedPhpStyles, wxSTC_STYLE_BRACEBAD).Color = *wxRED;
	FindByStcStyle(EditedPhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(0, 64, 128);
	FindByStcStyle(EditedPhpStyles, wxSTC_STYLE_INDENTGUIDE).Color = wxColour(wxT("#D3D3D3"));
	FindByStcStyle(EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = *wxBLACK;	
	FindByStcStyle(EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(192, 192, 192);
	FindByStcStyle(EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = *wxBLUE;
	FindByStcStyle(EditedPhpStyles, wxSTC_H_TAG).Color = wxColour(163, 21, 21);
	FindByStcStyle(EditedPhpStyles, wxSTC_H_ATTRIBUTE).Color = *wxRED;
	FindByStcStyle(EditedPhpStyles, wxSTC_H_DOUBLESTRING).Color = *wxBLUE;
	FindByStcStyle(EditedPhpStyles, wxSTC_H_SINGLESTRING).Color = *wxBLUE;
	FindByStcStyle(EditedPhpStyles, wxSTC_H_COMMENT).Color = wxColour(0, 128, 0);
	
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_COMMENT).Color = wxColor(0, 128, 0);
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColor(0, 128, 0);
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColor(0, 128, 0);
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_WORD).Color = *wxBLUE;
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_WORD2).Color = wxColour(163, 21, 21);
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_STRING).Color = wxColour(163, 21, 21);
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(163, 21, 21);
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColor(0, 128, 0);	
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	FindByStcStyle(EditedSqlStyles, wxSTC_STYLE_BRACELIGHT).Color = *wxGREEN;
	FindByStcStyle(EditedSqlStyles, wxSTC_STYLE_BRACEBAD).Color = *wxRED;
	FindByStcStyle(EditedSqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(0, 64, 128);
	FindByStcStyle(EditedSqlStyles, wxSTC_STYLE_INDENTGUIDE).Color = wxColour(wxT("#D3D3D3"));
	FindByStcStyle(EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = *wxBLACK;	
	FindByStcStyle(EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(192, 192, 192);
	FindByStcStyle(EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = *wxBLUE;
	
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_ATTRIBUTE).Color = *wxBLUE;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_CLASS).Color = *wxBLUE;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_COMMENT).Color = wxColor(0, 128, 0);
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_DIRECTIVE).Color = *wxBLUE;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(163, 21, 21);
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_ID).Color = *wxBLUE;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_IDENTIFIER).Color = *wxRED;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_IDENTIFIER2).Color = *wxRED;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_PSEUDOCLASS).Color = *wxRED;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(163, 21, 21);
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = *wxRED;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = *wxRED;
	FindByStcStyle(EditedCssStyles, wxSTC_STYLE_BRACELIGHT).Color = *wxGREEN;
	FindByStcStyle(EditedCssStyles, wxSTC_STYLE_BRACEBAD).Color = *wxRED;
	FindByStcStyle(EditedCssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(0, 64, 128);
	FindByStcStyle(EditedCssStyles, wxSTC_STYLE_INDENTGUIDE).Color = wxColour(wxT("#D3D3D3"));
	FindByStcStyle(EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = *wxBLACK;	
	FindByStcStyle(EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(192, 192, 192);
	FindByStcStyle(EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = *wxBLUE;
}

void mvceditor::CodeControlOptionsClass::SetToDarkTheme() {
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
	for (size_t i = 0; i < EditedPhpStyles.size(); ++i) {
		EditedPhpStyles[i].Font = font;
		EditedPhpStyles[i].Color = wxColour(wxT("#DCDCCC"));
		EditedPhpStyles[i].BackgroundColor = wxColour(wxT("#1F1F1F"));
		EditedPhpStyles[i].IsBold = false;
		EditedPhpStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < EditedSqlStyles.size(); ++i) {
		EditedSqlStyles[i].Font = font;
		EditedSqlStyles[i].Color = wxColour(wxT("#DCDCCC"));
		EditedSqlStyles[i].BackgroundColor = wxColour(wxT("#1F1F1F"));
		EditedSqlStyles[i].IsBold = false;
		EditedSqlStyles[i].IsItalic = false;
	}
	for (size_t i = 0; i < EditedCssStyles.size(); ++i) {
		EditedCssStyles[i].Font = font;
		EditedCssStyles[i].Color = wxColour(wxT("#DCDCCC"));
		EditedCssStyles[i].BackgroundColor = wxColour(wxT("#1F1F1F"));
		EditedCssStyles[i].IsBold = false;
		EditedCssStyles[i].IsItalic = false;
	}

	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("#CC9393"));	
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("#CC9393"));
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("#DFC47D"));
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("#8CD0D3"));
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("#CEDF99"));
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("#CEDF99"));
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("#7F9F7F"));
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("#7F9F7F"));
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("#CC9393"));
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("#9F9D6D"));
	FindByStcStyle(EditedPhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	FindByStcStyle(EditedPhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#AE0000"));
	FindByStcStyle(EditedPhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	FindByStcStyle(EditedPhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#8A8A8A"));
	FindByStcStyle(EditedPhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	FindByStcStyle(EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#D74100"));
	FindByStcStyle(EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#D74100"));
	FindByStcStyle(EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	FindByStcStyle(EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	FindByStcStyle(EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#646464"));
	FindByStcStyle(EditedPhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxTheColourDatabase->Find(wxT("YELLOW"));
	FindByStcStyle(EditedPhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("#DFC47D"));
	FindByStcStyle(EditedPhpStyles, wxSTC_H_TAG).IsBold = true;
	FindByStcStyle(EditedPhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("#DFC47D"));
	FindByStcStyle(EditedPhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	FindByStcStyle(EditedPhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("#CC9393"));
	FindByStcStyle(EditedPhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("#CC9393"));
	FindByStcStyle(EditedPhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("#7F9F7F"));
	FindByStcStyle(EditedPhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("#DFC47D"));
	FindByStcStyle(EditedPhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("#DFC47D"));

	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("#7F9F7F"));
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("#7F9F7F"));
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("#7F9F7F"));
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("#DFC47D"));
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_WORD).IsBold = true;
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("#CC9393"));;
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("#CC9393"));
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColor(0, 128, 0);	
	FindByStcStyle(EditedSqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	FindByStcStyle(EditedSqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#AE0000"));
	FindByStcStyle(EditedSqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	FindByStcStyle(EditedSqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#8A8A8A"));
	FindByStcStyle(EditedSqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	FindByStcStyle(EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#D74100"));
	FindByStcStyle(EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#D74100"));
	FindByStcStyle(EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	FindByStcStyle(EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	FindByStcStyle(EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#646464"));
	FindByStcStyle(EditedSqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxTheColourDatabase->Find(wxT("YELLOW"));
	
	
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("#CEDF99"));
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("#CEDF99"));
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("#7F9F7F"));
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("#DFC47D"));
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("#CC9393"));
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("#CEDF99"));
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("#DFC47D"));
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("#DFC47D"));
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("#DFC47D"));
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("#DFC47D"));
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("#CC9393"));
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("#DFC47D"));
	FindByStcStyle(EditedCssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("#DFC47D"));
	FindByStcStyle(EditedCssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("#AE0000"));
	FindByStcStyle(EditedCssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("#FF0000"));
	FindByStcStyle(EditedCssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("#8A8A8A"));
	FindByStcStyle(EditedCssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("#000000"));
	FindByStcStyle(EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("#D74100"));
	FindByStcStyle(EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("#D74100"));
	FindByStcStyle(EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("#000000"));
	FindByStcStyle(EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("#000000"));
	FindByStcStyle(EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("#646464"));
	FindByStcStyle(EditedCssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxTheColourDatabase->Find(wxT("YELLOW"));
}

void mvceditor::CodeControlOptionsClass::Load(wxConfigBase* config) {
	config->Read(wxT("EditorBehavior/SpacesPerIndent"), &SpacesPerIndent);
	config->Read(wxT("EditorBehavior/TabWidth"), &TabWidth);
	config->Read(wxT("EditorBehavior/RightMargin"), &RightMargin);
	config->Read(wxT("EditorBehavior/LineEndingMode"), &LineEndingMode);
	config->Read(wxT("EditorBehavior/IndentUsingTabs"), &IndentUsingTabs);
	config->Read(wxT("EditorBehavior/EnableCodeFolding"), &EnableCodeFolding);
	config->Read(wxT("EditorBehavior/EnableAutomaticLineIndentation"), &EnableAutomaticLineIndentation);
	config->Read(wxT("EditorBehavior/EnableLineNumbers"), &EnableLineNumbers);
	config->Read(wxT("EditorBehavior/EnableIndentationGuides"), &EnableIndentationGuides);
	config->Read(wxT("EditorBehavior/EnableLineEndings"), &EnableLineEndings);
	config->Read(wxT("EditorBehavior/EnableAutoCompletion"), &EnableAutoCompletion);
	for (size_t i = 0; i < PhpStyles.size(); ++i) {
		PhpStyles[i].Read(config);
	}
	for (size_t i = 0; i < SqlStyles.size(); ++i) {
		SqlStyles[i].Read(config);
	}
	for (size_t i = 0; i < CssStyles.size(); ++i) {
		CssStyles[i].Read(config);
	}
}
	 
void mvceditor::CodeControlOptionsClass::Save(wxConfigBase* config) {
	config->Write(wxT("EditorBehavior/SpacesPerIndent"), SpacesPerIndent);
	config->Write(wxT("EditorBehavior/TabWidth"), TabWidth);
	config->Write(wxT("EditorBehavior/RightMargin"), RightMargin);
	config->Write(wxT("EditorBehavior/LineEndingMode"), LineEndingMode);
	config->Write(wxT("EditorBehavior/IndentUsingTabs"), IndentUsingTabs);
	config->Write(wxT("EditorBehavior/EnableCodeFolding"), EnableCodeFolding);
	config->Write(wxT("EditorBehavior/EnableAutomaticLineIndentation"), EnableAutomaticLineIndentation);
	config->Write(wxT("EditorBehavior/EnableLineNumbers"), EnableLineNumbers);
	config->Write(wxT("EditorBehavior/EnableIndentationGuides"), EnableIndentationGuides);
	config->Write(wxT("EditorBehavior/EnableLineEndings"), EnableLineEndings);
	config->Write(wxT("EditorBehavior/EnableAutoCompletion"), EnableAutoCompletion);
	for (size_t i = 0; i < PhpStyles.size(); ++i) {
		PhpStyles[i].Write(config);
	}
	for (size_t i = 0; i < SqlStyles.size(); ++i) {
		SqlStyles[i].Write(config);
	}
	for (size_t i = 0; i < CssStyles.size(); ++i) {
		CssStyles[i].Write(config);
	}
}

mvceditor::StylePreferenceClass::StylePreferenceClass() 
	: Font()
	, Color()
	, BackgroundColor()
	, Name(0)
	, StcStyle(0)
	, IsBold(false)
	, IsItalic(false) {
		
}

void mvceditor::StylePreferenceClass::Copy(const mvceditor::StylePreferenceClass& src) {
	Font = src.Font;
	Color = src.Color;
	BackgroundColor = src.BackgroundColor;
	IsBold = src.IsBold;
	IsItalic = src.IsItalic;
	StcStyle = src.StcStyle;
	Name = src.Name;
}

bool mvceditor::StylePreferenceClass::Read(wxConfigBase* config) {
	wxString fontString;
	bool ret = true;
	wxString name = wxString::FromAscii(Name);
	name.Replace(wxT(" "), wxT("_"));
	wxString baseKey = wxString::Format(wxT("EditorBehavior/Style_%s"), name.c_str());
	ret &= config->Read(baseKey + wxT("/Font"), &fontString);
	if (ret) {
		Font.SetNativeFontInfo(fontString);
	}
	wxString colorString;
	ret &= config->Read(baseKey + wxT("/Color"), &colorString);
	Color.Set(colorString);
	ret &= config->Read(baseKey + wxT("/BackgroundColor"), &colorString);
	BackgroundColor.Set(colorString);
	ret &= config->Read(baseKey + wxT("/IsBold"), &IsBold);
	ret &= config->Read(baseKey + wxT("/IsItalic"), &IsItalic);
	return ret;
}

bool mvceditor::StylePreferenceClass::Write(wxConfigBase* config) {
	bool ret = true;
	wxString name = wxString::FromAscii(Name);
	name.Replace(wxT(" "), wxT("_"));
	wxString baseKey = wxString::Format(wxT("EditorBehavior/Style_%s"), name.c_str());
	wxString fontString = Font.GetNativeFontInfoDesc();
	ret &= config->Write(baseKey + wxT("/Font"), fontString);
	ret &= config->Write(baseKey + wxT("/Color"), Color.GetAsString(wxC2S_HTML_SYNTAX));
	ret &= config->Write(baseKey + wxT("/BackgroundColor"), BackgroundColor.GetAsString(wxC2S_HTML_SYNTAX));
	ret &= config->Write(baseKey + wxT("/IsBold"), IsBold);
	ret &= config->Write(baseKey + wxT("/IsItalic"), IsItalic);
	return ret;
}

void mvceditor::StylePreferenceClass::Control(int stcStyle, const char* name) {
	Name = name;
	StcStyle = stcStyle;
}