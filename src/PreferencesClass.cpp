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
#include <PreferencesClass.h>

#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/menuutils.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>
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
		// TODO: fixed in new versions of Scintilla?
		EnableCodeFolding = false;
	}
	RightMargin = 0;
	CodeStyles.Add(_("PHP Default"));
	CodeStyles.Add(_("PHP Double Quoted String"));
	CodeStyles.Add(_("PHP Single Quoted String"));
	CodeStyles.Add(_("PHP Keyword"));
	CodeStyles.Add(_("PHP Number Constant"));
	CodeStyles.Add(_("PHP Variable"));
	CodeStyles.Add(_("PHP Multi Line Comment"));
	CodeStyles.Add(_("PHP Line Comment"));
	CodeStyles.Add(_("PHP Variable Inside Double Quoted String"));
	CodeStyles.Add(_("PHP Complex Variable"));
	CodeStyles.Add(_("PHP Operator"));
	CodeStyles.Add(_("PHP Matching Brace"));
	CodeStyles.Add(_("PHP Mismatched Brace"));
	CodeStyles.Add(_("Line Number Margin"));
	CodeStyles.Add(_("Indentation Guides"));
	CodeStyles.Add(_("Caret"));
	CodeStyles.Add(_("Current Line"));
	CodeStyles.Add(_("Selection"));	
	CodeStyles.Add(_("Code Folding Margin"));
	CodeStyles.Add(_("Right Margin Vertical Line"));
	CodeStyles.Add(_("Highlight Matches"));
	CodeStyles.Add(_("HTML Default"));
	CodeStyles.Add(_("HTML Known Tag"));
	CodeStyles.Add(_("HTML Unknown Tag"));
	CodeStyles.Add(_("HTML Attribute"));
	CodeStyles.Add(_("HTML Unknown Attribute"));
	CodeStyles.Add(_("HTML Number"));
	CodeStyles.Add(_("HTML Double Quoted String"));
	CodeStyles.Add(_("HTML Single Quoted String"));
	CodeStyles.Add(_("HTML Other"));
	CodeStyles.Add(_("HTML Comment"));
	CodeStyles.Add(_("HTML Entity"));
	CodeStyles.Add(_("HTML Value"));
	CodeStyles.Add(_("HTML Embedded Script Start"));
	CodeStyles.Add(_("HTML Embedded Script End"));
	StartEditMode();
	SetToLightTheme();
	CommitChanges();
}

void mvceditor::CodeControlOptionsClass::ChangeStyle(int style, wxFont font, wxColor color, wxColor backgroundColor, bool isBold, 
	bool isItalic) {
	wxASSERT(style < mvceditor::CodeControlOptionsClass::STYLE_COUNT);
	EditedStylePreferences[style].Font = font;
	EditedStylePreferences[style].Color = color;
	EditedStylePreferences[style].BackgroundColor = backgroundColor;
	EditedStylePreferences[style].IsBold = isBold;
	EditedStylePreferences[style].IsItalic = isItalic;
}

void mvceditor::CodeControlOptionsClass::CommitChanges() {
	for (int i = 0; i < mvceditor::CodeControlOptionsClass::STYLE_COUNT; ++i) {
		StylePreferences[i].Font = EditedStylePreferences[i].Font;
		StylePreferences[i].Color = EditedStylePreferences[i].Color;
		StylePreferences[i].BackgroundColor = EditedStylePreferences[i].BackgroundColor;
		StylePreferences[i].IsBold = EditedStylePreferences[i].IsBold;
		StylePreferences[i].IsItalic = EditedStylePreferences[i].IsItalic;
	}
}

bool mvceditor::CodeControlOptionsClass::GetEditStyle(int style, wxFont& font, wxColor& color, wxColor& backgroundColor, 
	bool& isBold, bool& isItalic) const {
	if (style < mvceditor::CodeControlOptionsClass::STYLE_COUNT) {
		font = EditedStylePreferences[style].Font;
		color = EditedStylePreferences[style].Color;
		backgroundColor = EditedStylePreferences[style].BackgroundColor;
		isBold = EditedStylePreferences[style].IsBold;
		isItalic = EditedStylePreferences[style].IsItalic;
	}
	return style < mvceditor::CodeControlOptionsClass::STYLE_COUNT;
}

bool mvceditor::CodeControlOptionsClass::GetStyleByStcConstant(int style, wxFont& font, wxColor& color, wxColor& backgroundColor,
	bool& isBold, bool& isItalic) const {
	for (int i = 0; i < mvceditor::CodeControlOptionsClass::STYLE_COUNT; ++i) {
		if (style == ArrayIndexToStcConstant[i]) {
			font = StylePreferences[i].Font;
			color = StylePreferences[i].Color;
			backgroundColor = StylePreferences[i].BackgroundColor;
			isBold = StylePreferences[i].IsBold;
			isItalic = StylePreferences[i].IsItalic;
			return true;
		}
	}
	return false;
}

void mvceditor::CodeControlOptionsClass::StartEditMode() {
	for (int i = 0; i < mvceditor::CodeControlOptionsClass::STYLE_COUNT; ++i) {
		EditedStylePreferences[i].Font = StylePreferences[i].Font;
		EditedStylePreferences[i].Color = StylePreferences[i].Color;
		EditedStylePreferences[i].BackgroundColor = StylePreferences[i].BackgroundColor;
		EditedStylePreferences[i].IsBold = StylePreferences[i].IsBold;
		EditedStylePreferences[i].IsItalic = StylePreferences[i].IsItalic;
	}
}

int mvceditor::CodeControlOptionsClass::StyleIndex(int style) const {
	for (int i = 0; i < mvceditor::CodeControlOptionsClass::STYLE_COUNT; ++i) {
		if (style == ArrayIndexToStcConstant[i]) {
			return i;
		}
	}
	wxFAIL_MSG(wxString::Format(wxT("style: not valid"), style));
	return -1;
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
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_DEFAULT)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_DEFAULT)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_DEFAULT)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_DEFAULT)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_DEFAULT)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING)].Color = wxColour(163, 21, 21);
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_SIMPLESTRING)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_SIMPLESTRING)].Color = wxColour(163, 21, 21);
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_SIMPLESTRING)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_SIMPLESTRING)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_SIMPLESTRING)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_WORD)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_WORD)].Color = *wxBLUE;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_WORD)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_WORD)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_WORD)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_NUMBER)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_NUMBER)].Color = wxTheColourDatabase->Find(wxT("DARK GRAY"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_NUMBER)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_NUMBER)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_NUMBER)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_VARIABLE)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_VARIABLE)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_VARIABLE)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_VARIABLE)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_VARIABLE)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMPLEX_VARIABLE)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMPLEX_VARIABLE)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMPLEX_VARIABLE)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMPLEX_VARIABLE)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMPLEX_VARIABLE)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENT)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENT)].Color = wxColor(0, 128, 0);
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENT)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENT)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENT)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENTLINE)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENTLINE)].Color = wxColor(0, 128, 0);
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENTLINE)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENTLINE)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENTLINE)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING_VARIABLE)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING_VARIABLE)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING_VARIABLE)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING_VARIABLE)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING_VARIABLE)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_OPERATOR)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_OPERATOR)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_OPERATOR)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_OPERATOR)].IsBold = true;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_OPERATOR)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACELIGHT)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACELIGHT)].Color = *wxGREEN;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACELIGHT)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACELIGHT)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACELIGHT)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACEBAD)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACEBAD)].Color = *wxRED;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACEBAD)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACEBAD)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACEBAD)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_LINENUMBER)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_LINENUMBER)].Color = wxColour(0, 64, 128);
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_LINENUMBER)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_LINENUMBER)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_LINENUMBER)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_INDENTGUIDE)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_INDENTGUIDE)].Color = wxColour(wxT("#D3D3D3"));
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_INDENTGUIDE)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_INDENTGUIDE)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_INDENTGUIDE)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET)].Font = font;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET)].BackgroundColor = *wxBLACK;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET)].IsBold = false;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE)].Font = font;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE)].IsBold = false;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION)].Font = font;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION)].BackgroundColor = wxColour(192, 192, 192);
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION)].IsBold = false;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING)].Font = font;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING)].IsBold = false;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN)].Font = font;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN)].IsBold = false;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN)].IsItalic = false;

	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT)].Font = font;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT)].Color = *wxBLUE;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT)].IsBold = false;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_DEFAULT)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_DEFAULT)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(wxSTC_H_DEFAULT)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_H_DEFAULT)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_DEFAULT)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_TAG)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAG)].Color = wxColour(163, 21, 21);
	EditedStylePreferences[StyleIndex(wxSTC_H_TAG)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAG)].IsBold = true;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAG)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_TAGUNKNOWN)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGUNKNOWN)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGUNKNOWN)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGUNKNOWN)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGUNKNOWN)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTE)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTE)].Color = *wxRED;
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTE)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTE)].IsBold = true;
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTE)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTEUNKNOWN)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTEUNKNOWN)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTEUNKNOWN)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTEUNKNOWN)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTEUNKNOWN)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_NUMBER)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_NUMBER)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(wxSTC_H_NUMBER)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_H_NUMBER)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_NUMBER)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_DOUBLESTRING)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_DOUBLESTRING)].Color = *wxBLUE;
	EditedStylePreferences[StyleIndex(wxSTC_H_DOUBLESTRING)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_H_DOUBLESTRING)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_DOUBLESTRING)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_SINGLESTRING)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_SINGLESTRING)].Color = *wxBLUE;
	EditedStylePreferences[StyleIndex(wxSTC_H_SINGLESTRING)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_H_SINGLESTRING)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_SINGLESTRING)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_OTHER)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_OTHER)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(wxSTC_H_OTHER)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_H_OTHER)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_OTHER)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_COMMENT)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_COMMENT)].Color = wxColour(0, 128, 0);
	EditedStylePreferences[StyleIndex(wxSTC_H_COMMENT)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_H_COMMENT)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_COMMENT)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_ENTITY)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_ENTITY)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(wxSTC_H_ENTITY)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_H_ENTITY)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_ENTITY)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_VALUE)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_VALUE)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(wxSTC_H_VALUE)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_H_VALUE)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_VALUE)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_QUESTION)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_QUESTION)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(wxSTC_H_QUESTION)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_H_QUESTION)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_QUESTION)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_TAGEND)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGEND)].Color = *wxBLACK;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGEND)].BackgroundColor = *wxWHITE;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGEND)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGEND)].IsItalic = false;
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
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_DEFAULT)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_DEFAULT)].Color = wxColour(wxT("#DCDCCC"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_DEFAULT)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_DEFAULT)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_DEFAULT)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING)].Color = wxColour(wxT("#CC9393"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_SIMPLESTRING)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_SIMPLESTRING)].Color = wxColour(wxT("#CC9393"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_SIMPLESTRING)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_SIMPLESTRING)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_SIMPLESTRING)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_WORD)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_WORD)].Color = wxColour(wxT("#DFC47D"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_WORD)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_WORD)].IsBold = true;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_WORD)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_NUMBER)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_NUMBER)].Color = wxColour(wxT("#8CD0D3"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_NUMBER)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_NUMBER)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_NUMBER)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_VARIABLE)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_VARIABLE)].Color = wxColour(wxT("#CEDF99"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_VARIABLE)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_VARIABLE)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_VARIABLE)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMPLEX_VARIABLE)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMPLEX_VARIABLE)].Color = wxColour(wxT("#CEDF99"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMPLEX_VARIABLE)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMPLEX_VARIABLE)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMPLEX_VARIABLE)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENT)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENT)].Color = wxColour(wxT("#7F9F7F"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENT)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENT)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENT)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENTLINE)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENTLINE)].Color = wxColour(wxT("#7F9F7F"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENTLINE)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENTLINE)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENTLINE)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING_VARIABLE)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING_VARIABLE)].Color = wxColour(wxT("#CC9393"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING_VARIABLE)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING_VARIABLE)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING_VARIABLE)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_OPERATOR)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_OPERATOR)].Color = wxColour(wxT("#9F9D6D"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_OPERATOR)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_OPERATOR)].IsBold = true;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_OPERATOR)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACELIGHT)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACELIGHT)].Color = wxColour(wxT("#AE0000"));
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACELIGHT)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACELIGHT)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACELIGHT)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACEBAD)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACEBAD)].Color = wxColour(wxT("#FF0000"));
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACEBAD)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACEBAD)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACEBAD)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_LINENUMBER)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_LINENUMBER)].Color = wxColour(wxT("#8A8A8A"));
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_LINENUMBER)].BackgroundColor = wxColour(wxT("#000000"));
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_LINENUMBER)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_LINENUMBER)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_INDENTGUIDE)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_INDENTGUIDE)].Color = wxColour(wxT("#DCDCCC"));
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_INDENTGUIDE)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_INDENTGUIDE)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_INDENTGUIDE)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET)].Font = font;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET)].Color = wxColour(wxT("#D74100"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET)].BackgroundColor = wxColour(wxT("#D74100"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET)].IsBold = false;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE)].Font = font;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE)].Color = wxColour(wxT("#000000"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE)].BackgroundColor = wxColour(wxT("#000000"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE)].IsBold = false;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION)].Font = font;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION)].Color = wxColour(wxT("#DCDCCC"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION)].BackgroundColor = wxColour(wxT("#646464"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION)].IsBold = false;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING)].Font = font;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING)].Color = wxColour(wxT("#DCDCCC"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING)].IsBold = false;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING)].IsItalic = false;	
	
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN)].Font = font;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN)].Color = wxColour(wxT("#DCDCCC"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN)].IsBold = false;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN)].IsItalic = false;
	
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT)].Font = font;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT)].Color = wxTheColourDatabase->Find(wxT("YELLOW"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT)].IsBold = false;
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_DEFAULT)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_DEFAULT)].Color = wxColour(wxT("#DCDCCC"));
	EditedStylePreferences[StyleIndex(wxSTC_H_DEFAULT)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_DEFAULT)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_DEFAULT)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_TAG)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAG)].Color = wxColour(wxT("#DFC47D"));
	EditedStylePreferences[StyleIndex(wxSTC_H_TAG)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_TAG)].IsBold = true;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAG)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_TAGUNKNOWN)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGUNKNOWN)].Color = wxColour(wxT("#DCDCCC"));
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGUNKNOWN)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGUNKNOWN)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGUNKNOWN)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTE)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTE)].Color = wxColour(wxT("#DFC47D"));
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTE)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTE)].IsBold = true;
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTE)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTEUNKNOWN)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTEUNKNOWN)].Color = wxColour(wxT("#DCDCCC"));
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTEUNKNOWN)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTEUNKNOWN)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTEUNKNOWN)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_NUMBER)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_NUMBER)].Color = wxColour(wxT("#DCDCCC"));
	EditedStylePreferences[StyleIndex(wxSTC_H_NUMBER)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_NUMBER)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_NUMBER)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_DOUBLESTRING)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_DOUBLESTRING)].Color = wxColour(wxT("#CC9393"));
	EditedStylePreferences[StyleIndex(wxSTC_H_DOUBLESTRING)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_DOUBLESTRING)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_DOUBLESTRING)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_SINGLESTRING)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_SINGLESTRING)].Color = wxColour(wxT("#CC9393"));
	EditedStylePreferences[StyleIndex(wxSTC_H_SINGLESTRING)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_SINGLESTRING)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_SINGLESTRING)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_OTHER)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_OTHER)].Color = wxColour(wxT("#DCDCCC"));
	EditedStylePreferences[StyleIndex(wxSTC_H_OTHER)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_OTHER)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_OTHER)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_COMMENT)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_COMMENT)].Color = wxColour(wxT("#7F9F7F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_COMMENT)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_COMMENT)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_COMMENT)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_ENTITY)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_ENTITY)].Color = wxColour(wxT("#DCDCCC"));
	EditedStylePreferences[StyleIndex(wxSTC_H_ENTITY)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_ENTITY)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_ENTITY)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_VALUE)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_VALUE)].Color = wxColour(wxT("#DCDCCC"));
	EditedStylePreferences[StyleIndex(wxSTC_H_VALUE)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_VALUE)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_VALUE)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_QUESTION)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_QUESTION)].Color = wxColour(wxT("#DFC47D"));
	EditedStylePreferences[StyleIndex(wxSTC_H_QUESTION)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_QUESTION)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_QUESTION)].IsItalic = false;

	EditedStylePreferences[StyleIndex(wxSTC_H_TAGEND)].Font = font;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGEND)].Color = wxColour(wxT("#DFC47D"));
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGEND)].BackgroundColor = wxColour(wxT("#1F1F1F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGEND)].IsBold = false;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGEND)].IsItalic = false;
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
	for (int i = 0; i < mvceditor::CodeControlOptionsClass::STYLE_COUNT; ++i) {
		wxString fontString;
		if (config->Read(wxString::Format(wxT("EditorBehavior/Style_%i/Font"), i), &fontString)) {		
			StylePreferences[i].Font.SetNativeFontInfo(fontString);
		}
		wxString colorString;
		config->Read(wxString::Format(wxT("EditorBehavior/Style_%i/Color"), i), &colorString);
		StylePreferences[i].Color.Set(colorString);
		config->Read(wxString::Format(wxT("EditorBehavior/Style_%i/BackgroundColor"), i), &colorString);
		StylePreferences[i].BackgroundColor.Set(colorString);
		config->Read(wxString::Format(wxT("EditorBehavior/Style_%i/IsBold"), i), &StylePreferences[i].IsBold);
		config->Read(wxString::Format(wxT("EditorBehavior/Style_%i/IsItalic"), i), 
			&StylePreferences[i].IsItalic);
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
	for (int i = 0; i < mvceditor::CodeControlOptionsClass::STYLE_COUNT; ++i) {
		wxString fontString = StylePreferences[i].Font.GetNativeFontInfoDesc();
		config->Write(wxString::Format(wxT("EditorBehavior/Style_%i/Font"), i), fontString);
		config->Write(wxString::Format(wxT("EditorBehavior/Style_%i/Color"), i), 
			StylePreferences[i].Color.GetAsString(wxC2S_HTML_SYNTAX));
		config->Write(wxString::Format(wxT("EditorBehavior/Style_%i/BackgroundColor"), i), 
			StylePreferences[i].BackgroundColor.GetAsString(wxC2S_HTML_SYNTAX));
		config->Write(wxString::Format(wxT("EditorBehavior/Style_%i/IsBold"), i), StylePreferences[i].IsBold);
		config->Write(wxString::Format(wxT("EditorBehavior/Style_%i/IsItalic"), i), 
			StylePreferences[i].IsItalic);
	}
}


mvceditor::PreferencesClass::PreferencesClass()
	: CodeControlOptions()
	, KeyProfiles() {
	
}

mvceditor::PreferencesClass::~PreferencesClass() {
	//KeyProfileArray destructor deletes the pointers
}

void mvceditor::PreferencesClass::Load(wxFrame* frame) {
	CodeControlOptions.StartEditMode();
	CodeControlOptions.SetToLightTheme();
	CodeControlOptions.CommitChanges();
	KeyProfiles.Cleanup();
	wxConfigBase* config = wxConfigBase::Get();
	CodeControlOptions.Load(config);
	wxMenuBar* menuBar = frame->GetMenuBar();
	
	// before loading we must register in wxCmd arrays the various types
	// of commands we want wxCmd::Load to be able to recognize...	
	wxMenuCmd::Register(menuBar);
	int totalCmdCount = 0;
		
	// for some reason, must give absolute path else loading will saving & fail
	// COMMENT OUT THIS CODE TO SEE IF IT IS THE CAUSE OF CRASHES
	//KeyProfiles.Load(config, wxT("/Keyboard"));
	KeyProfiles.Load(config);
	for (int i = 0; i < KeyProfiles.GetCount(); ++i) {
		totalCmdCount += KeyProfiles.Item(i)->GetCmdCount();
	}
	
	// keybinder sets the config path, must reset it back to normal
	config->SetPath(wxT("/"));

	if (0 == totalCmdCount) {
		wxKeyProfile* profile = new wxKeyProfile(wxT("MVC Editor keyboard shortcuts"), wxT("MVC Editor keyboard shortcuts"));
		profile->ImportMenuBarCmd(menuBar);
		KeyProfiles.Add(profile);
		KeyProfiles.SetSelProfile(0);
	}
	LoadKeyboardShortcuts(frame);
}

void mvceditor::PreferencesClass::Save() {
	wxConfigBase* config = wxConfigBase::Get();
	CodeControlOptions.Save(config);
	
	// for some reason, must give absolute path else loading will saving & fail
	// COMMENT OUT THIS CODE TO SEE IF IT IS THE CAUSE OF CRASHES
	//KeyProfiles.Save(config, wxT("/Keyboard"), true);
	KeyProfiles.Save(config, wxT(""), true);
	
	// keybinder sets the config path, must reset it back to normal
	config->SetPath(wxT("/"));
	config->Flush();
}

void mvceditor::PreferencesClass::LoadKeyboardShortcuts(wxWindow* window) {
	KeyProfiles.DetachAll();
	KeyProfiles.GetSelProfile()->Enable(true);

	// not attaching recursively for now; since the child windows (the code editor widgets)
	// are temporary we cannot attach them to the profile otherwise when a code editor
	// is deleted the profile would have a dangling pointer.
	// however this means that the given window must be the frame
	KeyProfiles.GetSelProfile()->Attach(window);
}

void mvceditor::PreferencesClass::InitConfig() {
	wxStandardPaths paths;
	wxString appConfigFileName = paths.GetUserConfigDir() + wxFileName::GetPathSeparator() + wxT(".mvc_editor.ini");
	wxFileConfig* config = new wxFileConfig(wxT("mvc_editor"), wxEmptyString, appConfigFileName, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
	wxConfigBase::Set(config);
	// this config will be automatically deleted by wxWidgets at the end
}
