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
 * The source code file can then be copied into the mvc editor project to
 * add new color themes to the editor.
 * Adding a new theme:
 * 1. Go to http://eclipsecolorthemes.org and pick theme.
 * 2. Download the theme's "Eclipse Color Theme (XML) - for Eclipse Color Theme Plugin" 
 *    file and place in resources/color_themes
 * 3. run this script. Copy and paste the output into the file
 *    src/code_control/CodeControlStyles.cpp (yes, overwrite the entire
 *    file)
 * 4. Recompile mvc-editor
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

The source code file can then be copied into the mvc editor project to
add new color themes to the editor.
Adding a new theme:

1. Go to http://eclipsecolorthemes.org and pick theme.
2. Download the theme's "Eclipse Color Theme (XML) - for Eclipse Color Theme Plugin" 
   file and place in resources/color_themes
3. run this script. Copy and paste the output into the file
   src/code_control/CodeControlStyles.cpp (yes, overwrite the entire
   file)
4. Recompile mvc-editor
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
foreach (glob("{$themesDir}\\*.xml") as $file ) {
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
static void SetTo{$strFunc}Theme(mvceditor::CodeControlOptionsClass& options) {
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
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("{$caret}"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("{$caretLine}"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("{$selectionBackground}"));
	options.FindByStcStyle(options.PhpStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("{$matchHighlight}"));

	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).Color = wxColour(wxT("{$variable}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAG).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_ATTRIBUTE).IsBold = true;
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_DOUBLESTRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_SINGLESTRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_COMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_QUESTION).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.PhpStyles, wxSTC_H_TAGEND).Color = wxColour(wxT("{$defaultForeground}"));
	
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
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("{$caret}"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("{$selectionBackground}"));
	options.FindByStcStyle(options.SqlStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("{$matchHighlight}"));


	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ATTRIBUTE).Color = wxColour(wxT("{$keyword}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_CLASS).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_COMMENT).Color = wxColour(wxT("{$commentMultiLine}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DIRECTIVE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_DOUBLESTRING).Color = wxColour(wxT("{$string}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_ID).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IDENTIFIER2).IsItalic = true;
	options.FindByStcStyle(options.CssStyles, wxSTC_CSS_IMPORTANT).Color = wxColour(wxT("{$defaultForeground}"));
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
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).Color = wxColour(wxT("{$caret}"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).Color = wxColour(wxT("{$defaultForeground}"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE).BackgroundColor = wxColour(wxT("{$defaultBackground}"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION).BackgroundColor = wxColour(wxT("{$selectionBackground}"));
	options.FindByStcStyle(options.CssStyles, mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT).Color = wxColour(wxT("{$matchHighlight}"));
}

CODE;

}

$strFunc = themeFuncName($arrThemes[0]);
$strCode .= <<<CODE

void mvceditor::CodeControlStylesSetTheme(mvceditor::CodeControlOptionsClass& options, const wxString& theme) {
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
wxArrayString mvceditor::CodeControlStylesGetThemes() {
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
