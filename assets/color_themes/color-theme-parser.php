<?php
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

/**
 * this file will parse out theme files and create a c++ source code file.
 * The source code file can then be copied into the Triumph project to
 * add new color themes to the editor.
 * Adding a new theme:
 * 1. Go to http://eclipsecolorthemes.org and pick theme.
 * 2. Download the theme's "Eclipse Color Theme (XML) - for Eclipse Color Theme Plugin" 
 *    file and place in resources/color_themes
 * 3. run this script. Copy and paste the output into the file
 *    src/code_control/CodeControlStyles.cpp (yes, overwrite the entire
 *    file)
 * 4. Recompile triumph4php
 * 5. Go to Edit -> Preferences, switch to the "Styles & Colors" tab
 *    The new theme will show up in the Themes drop down.
 * 
 * The generated C++ code will contain 1 function per theme, as well
 * as 1 function to list theme names, and 1 function to set the
 * colors for a theme.
 *
 * The color themes can be downloaded http://eclipsecolorthemes.org.
 */

$opts = getopt('h', array('help'));
if (isset($opts['h']) || isset($opts['help'])) {
	echo <<<HELP
This file will parse out theme files and create a c++ source code file.

The source code file can then be copied into the triumph4php project to
add new color themes to the editor.
Adding a new theme:

1. Go to http://eclipsecolorthemes.org and pick theme.
2. Download the theme's "Eclipse Color Theme (XML) - for Eclipse Color Theme Plugin" 
   file and place in resources/color_themes
3. run this script. Copy and paste the output into the file
   src/code_control/CodeControlStyles.cpp (yes, overwrite the entire
   file)
4. Recompile triump4php
5. Go to Edit -> Preferences, switch to the "Styles & Colors" tab
   The new theme will show up in the Themes drop down.

The generated C++ code will contain 1 function per theme, as well
as 1 function to list theme names, and 1 function to set the
colors for a theme.
The color themes can be downloaded http://eclipsecolorthemes.org.


HELP;
	exit(0);
}

/**
 * function to make a theme name suitable to be part of a C++ function
 * name. this file will generate one c++ function for each theme, thus
 * we need to make a theme a valid c++ identifier
 *
 * @return string valid c++ identifier
 */
function themeFuncName($strTheme) {
	$strTheme = str_replace(' ', '_', $strTheme);
	$strTheme = str_replace('.', '', $strTheme);
	$strTheme = str_replace('+', '', $strTheme);
	$strTheme = str_replace('-', '', $strTheme);
	return $strTheme;
}

$rootDir = realpath(dirname(__FILE__) . DIRECTORY_SEPARATOR . '..' . DIRECTORY_SEPARATOR . '..');


$themesDir = dirname(__FILE__) . DIRECTORY_SEPARATOR;
$strCode = '';
$arrThemes = array();
foreach (glob("{$themesDir}/*.xml") as $file ) {
	$root = simplexml_load_file($file);

	$name = ucfirst(strtolower(($root['name'])));
	$arrThemes[] = $name;

	$defaultBackground = $root->background['color'];
	$defaultForeground = $root->foreground['color'];
	$string = $root->string['color'];
	$keyword = $root->keyword['color'];
	$number = $root->number['color'];
	$variable = $root->localVariable['color'];
	$commentMultiLine = $root->multiLineComment['color'];
	$commentSingle = $root->singleLineComment['color'];
	$variableInsideString = $root->localVariable['color'];
	$complexVariable = $root->localVariable['color'];
	$operator = $root->operator['color'];
	$matchingBrace = $root->staticField['color'];
	$mismatchedBrace = "#FF0000";
	$lineNumberMargin = $root->lineNumber['color'];
	$identationGuide = $root->lineNumber['color']; // indentation guide same as line number
	$caret = $root->lineNumber['color'];
	$caretLine = $root->currentLine['color'];
	$selectionForeground = $root->selectionForeground['color'];
	$selectionBackground = $root->selectionBackground['color'];
	$codeFolding = $root->lineNumber['color'];
	$rightMarginLine = $root->lineNumber['color'];
	$matchHighlight = '#FFFF00';
	
	if (!$variable) {
		$variable = $defaultForeground;
	}
	if (!$variableInsideString) {
		$variableInsideString = $defaultForeground;
	}
	if (!$complexVariable) {
		$complexVariable = $defaultForeground;
	}
	if (!$matchingBrace) {
		$matchingBrace = $defaultForeground;
	}
	
	
	$strFunc = themeFuncName($name);
	$strCode .= <<<CODE
	
static void SetTo{$strFunc}ThemePhp(t4p::CodeControlOptionsClass& options);
static void SetTo{$strFunc}ThemeSql(t4p::CodeControlOptionsClass& options);
static void SetTo{$strFunc}ThemeCss(t4p::CodeControlOptionsClass& options);
static void SetTo{$strFunc}ThemeJs(t4p::CodeControlOptionsClass& options);
static void SetTo{$strFunc}ThemeConfig(t4p::CodeControlOptionsClass& options);
static void SetTo{$strFunc}ThemeCrontab(t4p::CodeControlOptionsClass& options);
static void SetTo{$strFunc}ThemeYaml(t4p::CodeControlOptionsClass& options);
static void SetTo{$strFunc}ThemeRuby(t4p::CodeControlOptionsClass& options);
static void SetTo{$strFunc}ThemeLua(t4p::CodeControlOptionsClass& options);
static void SetTo{$strFunc}ThemeMarkdown(t4p::CodeControlOptionsClass& options);
static void SetTo{$strFunc}ThemeBash(t4p::CodeControlOptionsClass& options);
static void SetTo{$strFunc}ThemeDiff(t4p::CodeControlOptionsClass& options);

static void SetTo{$strFunc}Theme(t4p::CodeControlOptionsClass& options) {
	wxPlatformInfo platform;
	wxString fontName;
	int os = platform.GetOperatingSystemId();
	int fontSize = 10;

	//ATTN: different OSs have different fonts
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
		fontSize = 10;
	}
	else if (os == wxOS_UNIX_LINUX) {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
		fontSize = 10;
	}
	else if (os == wxOS_MAC_OSX_DARWIN) {
		fontName = wxT("Monaco");
		fontSize = 12;
	}
	wxFont font(wxFontInfo(fontSize).AntiAliased(true).FaceName(fontName));
	
	for (size_t i = 0; i < options.PhpStyles.size(); ++i) {
		options.PhpStyles[i].Font = font;
		options.PhpStyles[i].Color = wxColour(wxT("{$defaultForeground}"));
		options.PhpStyles[i].BackgroundColor = wxColour(wxT("{$defaultBackground}"));
		options.PhpStyles[i].IsBold = false;
		options.PhpStyles[i].IsItalic = false;
	}
	
	for (size_t i = 0; i < options.SqlStyles.size(); ++i) {
		options.SqlStyles[i].Font = font;
		options.SqlStyles[i].Color = wxColour(wxT("{$defaultForeground}"));
		options.SqlStyles[i].BackgroundColor = wxColour(wxT("{$defaultBackground}"));
		options.SqlStyles[i].IsBold = false;
		options.SqlStyles[i].IsItalic = false;
	}
	
	for (size_t i = 0; i < options.CssStyles.size(); ++i) {
		options.CssStyles[i].Font = font;
		options.CssStyles[i].Color = wxColour(wxT("{$defaultForeground}"));
		options.CssStyles[i].BackgroundColor = wxColour(wxT("{$defaultBackground}"));
		options.CssStyles[i].IsBold = false;
		options.CssStyles[i].IsItalic = false;
	}
	
	for (size_t i = 0; i < options.JsStyles.size(); ++i) {
		options.JsStyles[i].Font = font;
		options.JsStyles[i].Color = wxColour(wxT("{$defaultForeground}"));
		options.JsStyles[i].BackgroundColor = wxColour(wxT("{$defaultBackground}"));
		options.JsStyles[i].IsBold = false;
		options.JsStyles[i].IsItalic = false;
	}
	
	for (size_t i = 0; i < options.ConfigStyles.size(); ++i) {
		options.ConfigStyles[i].Font = font;
		options.ConfigStyles[i].Color = wxColour(wxT("{$defaultForeground}"));
		options.ConfigStyles[i].BackgroundColor = wxColour(wxT("{$defaultBackground}"));
		options.ConfigStyles[i].IsBold = false;
		options.ConfigStyles[i].IsItalic = false;
	}
	
	for (size_t i = 0; i < options.CrontabStyles.size(); ++i) {
		options.CrontabStyles[i].Font = font;
		options.CrontabStyles[i].Color = wxColour(wxT("{$defaultForeground}"));
		options.CrontabStyles[i].BackgroundColor = wxColour(wxT("{$defaultBackground}"));
		options.CrontabStyles[i].IsBold = false;
		options.CrontabStyles[i].IsItalic = false;
	}
	
	for (size_t i = 0; i < options.YamlStyles.size(); ++i) {
		options.YamlStyles[i].Font = font;
		options.YamlStyles[i].Color = wxColour(wxT("{$defaultForeground}"));
		options.YamlStyles[i].BackgroundColor = wxColour(wxT("{$defaultBackground}"));
		options.YamlStyles[i].IsBold = false;
		options.YamlStyles[i].IsItalic = false;
	}
	
	for (size_t i = 0; i < options.RubyStyles.size(); ++i) {
		options.RubyStyles[i].Font = font;
		options.RubyStyles[i].Color = wxColour(wxT("{$defaultForeground}"));
		options.RubyStyles[i].BackgroundColor = wxColour(wxT("{$defaultBackground}"));
		options.RubyStyles[i].IsBold = false;
		options.RubyStyles[i].IsItalic = false;
	}
	
	for (size_t i = 0; i < options.LuaStyles.size(); ++i) {
		options.LuaStyles[i].Font = font;
		options.LuaStyles[i].Color = wxColour(wxT("{$defaultForeground}"));
		options.LuaStyles[i].BackgroundColor = wxColour(wxT("{$defaultBackground}"));
		options.LuaStyles[i].IsBold = false;
		options.LuaStyles[i].IsItalic = false;
	}
	
	for (size_t i = 0; i < options.MarkdownStyles.size(); ++i) {
		options.MarkdownStyles[i].Font = font;
		options.MarkdownStyles[i].Color = wxColour(wxT("{$defaultForeground}"));
		options.MarkdownStyles[i].BackgroundColor = wxColour(wxT("{$defaultBackground}"));
		options.MarkdownStyles[i].IsBold = false;
		options.MarkdownStyles[i].IsItalic = false;
	}
	
	for (size_t i = 0; i < options.BashStyles.size(); ++i) {
		options.BashStyles[i].Font = font;
		options.BashStyles[i].Color = wxColour(wxT("{$defaultForeground}"));
		options.BashStyles[i].BackgroundColor = wxColour(wxT("{$defaultBackground}"));
		options.BashStyles[i].IsBold = false;
		options.BashStyles[i].IsItalic = false;
	}
	
	for (size_t i = 0; i < options.DiffStyles.size(); ++i) {
		options.DiffStyles[i].Font = font;
		options.DiffStyles[i].Color = wxColour(wxT("{$defaultForeground}"));
		options.DiffStyles[i].BackgroundColor = wxColour(wxT("{$defaultBackground}"));
		options.DiffStyles[i].IsBold = false;
		options.DiffStyles[i].IsItalic = false;
	}
	
	
	SetTo{$strFunc}ThemePhp(options);
	SetTo{$strFunc}ThemeSql(options);
	SetTo{$strFunc}ThemeCss(options);
	SetTo{$strFunc}ThemeJs(options);
	SetTo{$strFunc}ThemeConfig(options);
	SetTo{$strFunc}ThemeCrontab(options);
	SetTo{$strFunc}ThemeYaml(options);
	SetTo{$strFunc}ThemeRuby(options);
	SetTo{$strFunc}ThemeLua(options);
	SetTo{$strFunc}ThemeMarkdown(options);
	SetTo{$strFunc}ThemeBash(options);
	SetTo{$strFunc}ThemeDiff(options);
}

static void SetTo{$strFunc}ThemePhp(t4p::CodeControlOptionsClass& options) {
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_SIMPLESTRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_NUMBER).Color = wxColour(wxT("{$number}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_VARIABLE).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMPLEX_VARIABLE).Color = wxColour(wxT("{$complexVariable}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_COMMENTLINE).Color = wxColour(wxT("{$commentSingle}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_HSTRING_VARIABLE).Color = wxColour(wxT("{$variableInsideString}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).Color = wxColour(wxT("{$operator}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HPHP_OPERATOR).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("{$matchingBrace}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("{$mismatchedBrace}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("{$lineNumberMargin}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.PhpStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).Color = wxColour(wxT("{$caret}"));
	options.FindByStcStyle(options.PhpStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.PhpStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("{$caretLine}"));
	options.FindByStcStyle(options.PhpStyles, t4p::CodeControlOptionsClass::T4P_STYLE_SELECTION).BackgroundColor = wxColour(wxT("{$selectionBackground}"));
	options.FindByStcStyle(options.PhpStyles, t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("{$matchHighlight}"));	

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("{$defaultForeground}"));
	
	// javascript embedded inside HTML
	options.FindByStcStyle(options.PhpStyles, wxSTC_HJ_START).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HJ_DEFAULT).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HJ_COMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HJ_COMMENTLINE).Color = wxColour(wxT("{$commentSingle}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HJ_COMMENTDOC).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HJ_NUMBER).Color = wxColour(wxT("{$number}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HJ_WORD).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HJ_KEYWORD).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HJ_WORD).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_HJ_DOUBLESTRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HJ_SINGLESTRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HJ_SYMBOLS).Color = wxColour(wxT("{$operator}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HJ_REGEX).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_HJ_STRINGEOL).Color = wxColour(wxT("{$string}"));

	// extra HTML / XML styles for the HTML lexer
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SGML_DEFAULT).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SGML_1ST_PARAM).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SGML_1ST_PARAM_COMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SGML_BLOCK_DEFAULT).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SGML_COMMAND).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SGML_COMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SGML_DOUBLESTRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SGML_ENTITY).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SGML_ERROR).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SGML_SIMPLESTRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SGML_SPECIAL).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_XCCOMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_XMLEND).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_XMLSTART).Color = wxColour(wxT("{$defaultForeground}"));
}

static void SetTo{$strFunc}ThemeJs(t4p::CodeControlOptionsClass& options) {
	
	// javascript (in its own file) scintilla c lexer is used of c,c++, java, and javascript
	options.FindByStcStyle(options.JsStyles, wxSTC_C_DEFAULT).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINE).Color = wxColour(wxT("{$commentSingle}"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTDOC).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_NUMBER).Color = wxColour(wxT("{$number}"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_C_IDENTIFIER).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_CHARACTER).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_OPERATOR).Color = wxColour(wxT("{$operator}"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_STRINGEOL).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_REGEX).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.JsStyles, wxSTC_C_COMMENTLINEDOC).Color = wxColour(wxT("{$commentMultiLine}"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).Color = wxColour(wxT("{$keyword}"));	
	options.FindByStcStyle(options.JsStyles, wxSTC_C_WORD2).IsBold = true;
	options.FindByStcStyle(options.JsStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("{$matchingBrace}"));
	options.FindByStcStyle(options.JsStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("{$mismatchedBrace}"));
	options.FindByStcStyle(options.JsStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("{$lineNumberMargin}"));
	options.FindByStcStyle(options.JsStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.JsStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).Color = wxColour(wxT("{$caret}"));
	options.FindByStcStyle(options.JsStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.JsStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.JsStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("{$caretLine}"));
	options.FindByStcStyle(options.JsStyles, t4p::CodeControlOptionsClass::T4P_STYLE_SELECTION).BackgroundColor = wxColour(wxT("{$selectionBackground}"));
	options.FindByStcStyle(options.JsStyles, t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("{$matchHighlight}"));
}

static void SetTo{$strFunc}ThemeSql(t4p::CodeControlOptionsClass& options) {	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINE).Color = wxColour(wxT("{$commentSingle}"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTDOC).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD).IsBold = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_STRING).Color = wxColour(wxT("{$string}"));;
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_CHARACTER).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_COMMENTLINEDOC).Color = wxColour(wxT("{$commentMultiLine}"));	
	options.FindByStcStyle(options.SqlStyles, wxSTC_SQL_WORD2).IsItalic = true;
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("{$matchingBrace}"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("{$mismatchedBrace}"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("{$lineNumberMargin}"));
	options.FindByStcStyle(options.SqlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.SqlStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).Color = wxColour(wxT("{$caret}"));
	options.FindByStcStyle(options.SqlStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.SqlStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.SqlStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("{$caretLine}"));
	options.FindByStcStyle(options.SqlStyles, t4p::CodeControlOptionsClass::T4P_STYLE_SELECTION).BackgroundColor = wxColour(wxT("{$selectionBackground}"));
	options.FindByStcStyle(options.SqlStyles, t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("{$matchHighlight}"));
}

static void SetTo{$strFunc}ThemeCss(t4p::CodeControlOptionsClass& options) {
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).IsBold = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_PSEUDOCLASS).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_SINGLESTRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_IDENTIFIER).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_UNKNOWN_PSEUDOCLASS).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("{$matchingBrace}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("{$mismatchedBrace}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("{$lineNumberMargin}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.CssStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).Color = wxColour(wxT("{$caret}"));
	options.FindByStcStyle(options.CssStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.CssStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.CssStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("{$caretLine}"));
	options.FindByStcStyle(options.CssStyles, t4p::CodeControlOptionsClass::T4P_STYLE_SELECTION).BackgroundColor = wxColour(wxT("{$selectionBackground}"));
	options.FindByStcStyle(options.CssStyles, t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("{$matchHighlight}"));
}

static void SetTo{$strFunc}ThemeConfig(t4p::CodeControlOptionsClass& options) {
	options.FindByStcStyle(options.ConfigStyles, wxSTC_CONF_DEFAULT).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.ConfigStyles, wxSTC_CONF_COMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.ConfigStyles, wxSTC_CONF_DIRECTIVE).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.ConfigStyles, wxSTC_CONF_EXTENSION).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.ConfigStyles, wxSTC_CONF_IDENTIFIER).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.ConfigStyles, wxSTC_CONF_IP).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.ConfigStyles, wxSTC_CONF_NUMBER).Color = wxColour(wxT("{$number}"));
	options.FindByStcStyle(options.ConfigStyles, wxSTC_CONF_OPERATOR).Color = wxColour(wxT("{$operator}"));
	options.FindByStcStyle(options.ConfigStyles, wxSTC_CONF_PARAMETER).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.ConfigStyles, wxSTC_CONF_STRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.ConfigStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("{$matchingBrace}"));
	options.FindByStcStyle(options.ConfigStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("{$mismatchedBrace}"));
	options.FindByStcStyle(options.ConfigStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("{$lineNumberMargin}"));
	options.FindByStcStyle(options.ConfigStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.ConfigStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).Color = wxColour(wxT("{$caret}"));
	options.FindByStcStyle(options.ConfigStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.ConfigStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.ConfigStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("{$caretLine}"));
	options.FindByStcStyle(options.ConfigStyles, t4p::CodeControlOptionsClass::T4P_STYLE_SELECTION).BackgroundColor = wxColour(wxT("{$selectionBackground}"));
	options.FindByStcStyle(options.ConfigStyles, t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("{$matchHighlight}"));
}

static void SetTo{$strFunc}ThemeCrontab(t4p::CodeControlOptionsClass& options) {
	options.FindByStcStyle(options.CrontabStyles, wxSTC_NNCRONTAB_DEFAULT).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.CrontabStyles, wxSTC_NNCRONTAB_ASTERISK).Color = wxColour(wxT("{$operator}"));
	options.FindByStcStyle(options.CrontabStyles, wxSTC_NNCRONTAB_COMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.CrontabStyles, wxSTC_NNCRONTAB_ENVIRONMENT).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.CrontabStyles, wxSTC_NNCRONTAB_IDENTIFIER).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.CrontabStyles, wxSTC_NNCRONTAB_KEYWORD).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.CrontabStyles, wxSTC_NNCRONTAB_MODIFIER).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.CrontabStyles, wxSTC_NNCRONTAB_NUMBER).Color = wxColour(wxT("{$number}"));
	options.FindByStcStyle(options.CrontabStyles, wxSTC_NNCRONTAB_SECTION).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.CrontabStyles, wxSTC_NNCRONTAB_STRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.CrontabStyles, wxSTC_NNCRONTAB_TASK).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.CrontabStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("{$matchingBrace}"));
	options.FindByStcStyle(options.CrontabStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("{$mismatchedBrace}"));
	options.FindByStcStyle(options.CrontabStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("{$lineNumberMargin}"));
	options.FindByStcStyle(options.CrontabStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.CrontabStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).Color = wxColour(wxT("{$caret}"));
	options.FindByStcStyle(options.CrontabStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.CrontabStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.CrontabStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("{$caretLine}"));
	options.FindByStcStyle(options.CrontabStyles, t4p::CodeControlOptionsClass::T4P_STYLE_SELECTION).BackgroundColor = wxColour(wxT("{$selectionBackground}"));
	options.FindByStcStyle(options.CrontabStyles, t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("{$matchHighlight}"));
}

static void SetTo{$strFunc}ThemeYaml(t4p::CodeControlOptionsClass& options) {	
	options.FindByStcStyle(options.YamlStyles, wxSTC_YAML_DEFAULT).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.YamlStyles, wxSTC_YAML_COMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.YamlStyles, wxSTC_YAML_DOCUMENT).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.YamlStyles, wxSTC_YAML_ERROR).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.YamlStyles, wxSTC_YAML_IDENTIFIER).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.YamlStyles, wxSTC_YAML_KEYWORD).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.YamlStyles, wxSTC_YAML_NUMBER).Color = wxColour(wxT("{$number}"));
	options.FindByStcStyle(options.YamlStyles, wxSTC_YAML_OPERATOR).Color = wxColour(wxT("{$operator}"));
	options.FindByStcStyle(options.YamlStyles, wxSTC_YAML_REFERENCE).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.YamlStyles, wxSTC_YAML_TEXT).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.YamlStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("{$matchingBrace}"));
	options.FindByStcStyle(options.YamlStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("{$mismatchedBrace}"));
	options.FindByStcStyle(options.YamlStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("{$lineNumberMargin}"));
	options.FindByStcStyle(options.YamlStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.YamlStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).Color = wxColour(wxT("{$caret}"));
	options.FindByStcStyle(options.YamlStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.YamlStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.YamlStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("{$caretLine}"));
	options.FindByStcStyle(options.YamlStyles, t4p::CodeControlOptionsClass::T4P_STYLE_SELECTION).BackgroundColor = wxColour(wxT("{$selectionBackground}"));
	options.FindByStcStyle(options.YamlStyles, t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("{$matchHighlight}"));
}

static void SetTo{$strFunc}ThemeRuby(t4p::CodeControlOptionsClass& options) {		
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_DEFAULT).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_BACKTICKS).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_CHARACTER).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_CLASSNAME).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_CLASS_VAR).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_COMMENTLINE).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_DATASECTION).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_DEFNAME).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_ERROR).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_GLOBAL).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_HERE_DELIM).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_HERE_Q).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_HERE_QQ).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_HERE_QX).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_IDENTIFIER).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_INSTANCE_VAR).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_MODULE_NAME).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_NUMBER).Color = wxColour(wxT("{$number}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_OPERATOR).Color = wxColour(wxT("{$operator}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_POD).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_REGEX).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_STDERR).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_STDIN).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_STDOUT).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_STRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_STRING_Q).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_STRING_QQ).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_STRING_QR).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_STRING_QW).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_STRING_QX).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_SYMBOL).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_UPPER_BOUND).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_WORD).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_RB_WORD_DEMOTED).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("{$matchingBrace}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("{$mismatchedBrace}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("{$lineNumberMargin}"));
	options.FindByStcStyle(options.RubyStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.RubyStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).Color = wxColour(wxT("{$caret}"));
	options.FindByStcStyle(options.RubyStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.RubyStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.RubyStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("{$caretLine}"));
	options.FindByStcStyle(options.RubyStyles, t4p::CodeControlOptionsClass::T4P_STYLE_SELECTION).BackgroundColor = wxColour(wxT("{$selectionBackground}"));
	options.FindByStcStyle(options.RubyStyles, t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("{$matchHighlight}"));
}

static void SetTo{$strFunc}ThemeLua(t4p::CodeControlOptionsClass& options) {
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_DEFAULT).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_CHARACTER).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_COMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_COMMENTDOC).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_COMMENTLINE).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_IDENTIFIER).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_LABEL).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_LITERALSTRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_NUMBER).Color = wxColour(wxT("{$number}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_OPERATOR).Color = wxColour(wxT("{$operator}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_PREPROCESSOR).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_STRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_STRINGEOL).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_WORD).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_WORD2).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_WORD3).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_WORD4).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_WORD5).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_WORD6).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_WORD7).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_LUA_WORD8).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("{$matchingBrace}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("{$mismatchedBrace}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("{$lineNumberMargin}"));
	options.FindByStcStyle(options.LuaStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.LuaStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).Color = wxColour(wxT("{$caret}"));
	options.FindByStcStyle(options.LuaStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.LuaStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.LuaStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("{$caretLine}"));
	options.FindByStcStyle(options.LuaStyles, t4p::CodeControlOptionsClass::T4P_STYLE_SELECTION).BackgroundColor = wxColour(wxT("{$selectionBackground}"));
	options.FindByStcStyle(options.LuaStyles, t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("{$matchHighlight}"));
}

static void SetTo{$strFunc}ThemeMarkdown(t4p::CodeControlOptionsClass& options) {
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_DEFAULT).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_BLOCKQUOTE).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_CODE).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_CODE2).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_CODEBK).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_EM1).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_EM2).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_HEADER1).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_HEADER2).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_HEADER3).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_HEADER4).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_HEADER5).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_HEADER6).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_HRULE).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_LINE_BEGIN).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_LINK).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_OLIST_ITEM).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_PRECHAR).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_STRIKEOUT).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_STRONG1).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_STRONG2).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_MARKDOWN_ULIST_ITEM).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("{$matchingBrace}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("{$mismatchedBrace}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("{$lineNumberMargin}"));
	options.FindByStcStyle(options.MarkdownStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.MarkdownStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).Color = wxColour(wxT("{$caret}"));
	options.FindByStcStyle(options.MarkdownStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.MarkdownStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.MarkdownStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("{$caretLine}"));
	options.FindByStcStyle(options.MarkdownStyles, t4p::CodeControlOptionsClass::T4P_STYLE_SELECTION).BackgroundColor = wxColour(wxT("{$selectionBackground}"));
	options.FindByStcStyle(options.MarkdownStyles, t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("{$matchHighlight}"));
}

static void SetTo{$strFunc}ThemeBash(t4p::CodeControlOptionsClass& options) {		
	options.FindByStcStyle(options.BashStyles, wxSTC_SH_DEFAULT).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_SH_BACKTICKS).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_SH_CHARACTER).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_SH_COMMENTLINE).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_SH_ERROR).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_SH_HERE_DELIM).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_SH_IDENTIFIER).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_SH_NUMBER).Color = wxColour(wxT("{$number}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_SH_OPERATOR).Color = wxColour(wxT("{$operator}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_SH_PARAM).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_SH_SCALAR).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_SH_STRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_SH_WORD).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("{$matchingBrace}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("{$mismatchedBrace}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("{$lineNumberMargin}"));
	options.FindByStcStyle(options.BashStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.BashStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).Color = wxColour(wxT("{$caret}"));
	options.FindByStcStyle(options.BashStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.BashStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.BashStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("{$caretLine}"));
	options.FindByStcStyle(options.BashStyles, t4p::CodeControlOptionsClass::T4P_STYLE_SELECTION).BackgroundColor = wxColour(wxT("{$selectionBackground}"));
	options.FindByStcStyle(options.BashStyles, t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("{$matchHighlight}"));
}

static void SetTo{$strFunc}ThemeDiff(t4p::CodeControlOptionsClass& options) {	
	options.FindByStcStyle(options.DiffStyles, wxSTC_DIFF_DEFAULT).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.DiffStyles, wxSTC_DIFF_ADDED).Color = wxColour(wxT("#0000FF"));
	options.FindByStcStyle(options.DiffStyles, wxSTC_DIFF_CHANGED).Color = wxColour(wxT("#00FF00"));
	options.FindByStcStyle(options.DiffStyles, wxSTC_DIFF_COMMAND).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.DiffStyles, wxSTC_DIFF_COMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.DiffStyles, wxSTC_DIFF_DELETED).Color = wxColour(wxT("#FF0000"));
	options.FindByStcStyle(options.DiffStyles, wxSTC_DIFF_HEADER).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.DiffStyles, wxSTC_DIFF_POSITION).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.DiffStyles, wxSTC_STYLE_BRACELIGHT).Color = wxColour(wxT("{$matchingBrace}"));
	options.FindByStcStyle(options.DiffStyles, wxSTC_STYLE_BRACEBAD).Color = wxColour(wxT("{$mismatchedBrace}"));
	options.FindByStcStyle(options.DiffStyles, wxSTC_STYLE_LINENUMBER).Color = wxColour(wxT("{$lineNumberMargin}"));
	options.FindByStcStyle(options.DiffStyles, wxSTC_STYLE_LINENUMBER).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.DiffStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).Color = wxColour(wxT("{$caret}"));
	options.FindByStcStyle(options.DiffStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.DiffStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.DiffStyles, t4p::CodeControlOptionsClass::T4P_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("{$caretLine}"));
	options.FindByStcStyle(options.DiffStyles, t4p::CodeControlOptionsClass::T4P_STYLE_SELECTION).BackgroundColor = wxColour(wxT("{$selectionBackground}"));
	options.FindByStcStyle(options.DiffStyles, t4p::CodeControlOptionsClass::T4P_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("{$matchHighlight}"));	
}

CODE;

}

$strFunc = themeFuncName($arrThemes[0]);
$strCode .= <<<CODE

void t4p::CodeControlStylesSetTheme(t4p::CodeControlOptionsClass& options, const wxString& theme) {
	if (theme == wxT("{$arrThemes[0]}")) {
		SetTo{$strFunc}Theme(options);
	}
CODE;
	for ($i = 1; $i < count($arrThemes); $i++) {
		$strFunc = themeFuncName($arrThemes[$i]);
		$strCode .= <<<CODE

	else if (theme == wxT("{$arrThemes[$i]}")) {
		SetTo{$strFunc}Theme(options);
	}
CODE;
}
$strCode .= "\n}\n";

$strCode .= <<<CODE
wxArrayString t4p::CodeControlStylesGetThemes() {
	wxArrayString themes;
	
CODE;
	for ($i = 0; $i < count($arrThemes); $i++) {
		$strCode .= <<<CODE
	themes.Add(wxT("{$arrThemes[$i]}"));

CODE;
	}

$strCode .= "\treturn themes;";	
$strCode .= "\n}\n";


$strMarker = '// *** ALL CODE BELOW IS GENERATED AUTOMATICALLY BY resources/color_themes/color-theme-parser.cpp *** //';
$strSrcFile = $rootDir . DIRECTORY_SEPARATOR . 'src' .  DIRECTORY_SEPARATOR . 
	'code_control' . DIRECTORY_SEPARATOR . 'CodeControlStyles.cpp';
$strContents = file_get_contents($strSrcFile);

$strNewCode = '';
$iStart = strpos($strContents, $strMarker, 0);
if ($iStart > 0) {
	$strNewCode = substr($strContents, 0, $iStart) . 
	"\n" . 
	$strMarker . "\n\n" .
	$strCode;
}

echo $strNewCode;
