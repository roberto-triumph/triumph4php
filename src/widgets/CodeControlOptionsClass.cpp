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
 * @author     $Author$
 * @date       $Date$
 * @version    $Rev$ 
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
	CodeStyles.Add(_("HTML Embedded Script End"));
	CodeStyles.Add(_("SQL Default"));
	CodeStyles.Add(_("SQL Comment"));  // starts with "/*"
	CodeStyles.Add(_("SQL Single Line Comment")); // starts with "--"
	CodeStyles.Add(_("SQL Doc Comment")); // starts with or '/**' or '/*!' (doxygen)
	CodeStyles.Add(_("SQL Number"));
	CodeStyles.Add(_("SQL Keyword"));
	CodeStyles.Add(_("SQL String"));
	CodeStyles.Add(_("SQL Character"));
	CodeStyles.Add(_("SQL Operator"));
	CodeStyles.Add(_("SQL Identifier"));
	CodeStyles.Add(_("SQL Single Line Doc Comment")); // starts with '#'
	CodeStyles.Add(_("SQL Database Objects")); //table, column names that match the DB
	CodeStyles.Add(_("SQL Stored Procedure Keyword")); // keyword on stored procedure
	CodeStyles.Add(_("SQL Doxygen Comment Error")); // doxygen error
	CodeStyles.Add(_("SQL User Keywords 1"));
	CodeStyles.Add(_("SQL User Keywords 2"));
	CodeStyles.Add(_("SQL User Keywords 3"));
	CodeStyles.Add(_("SQL User Keywords 4"));
	CodeStyles.Add(_("SQL Quoted Identifier"));

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
	for (int i = 0; i < CodeControlOptionsClass::STYLE_COUNT; ++i) {
		EditedStylePreferences[i].Font = font;
		EditedStylePreferences[i].Color = *wxBLACK;
		EditedStylePreferences[i].BackgroundColor = *wxWHITE;
		EditedStylePreferences[i].IsBold = false;
		EditedStylePreferences[i].IsItalic = false;
	}

	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING)].Color = wxColour(163, 21, 21);	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_SIMPLESTRING)].Color = wxColour(163, 21, 21);
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_WORD)].Color = *wxBLUE;	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_NUMBER)].Color = wxTheColourDatabase->Find(wxT("DARK GRAY"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENT)].Color = wxColor(0, 128, 0);
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENTLINE)].Color = wxColor(0, 128, 0);
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACELIGHT)].Color = *wxGREEN;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACEBAD)].Color = *wxRED;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_LINENUMBER)].Color = wxColour(0, 64, 128);
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_INDENTGUIDE)].Color = wxColour(wxT("#D3D3D3"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET)].BackgroundColor = *wxBLACK;	
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION)].BackgroundColor = wxColour(192, 192, 192);
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT)].Color = *wxBLUE;
	EditedStylePreferences[StyleIndex(wxSTC_H_TAG)].Color = wxColour(163, 21, 21);
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTE)].Color = *wxRED;
	EditedStylePreferences[StyleIndex(wxSTC_H_DOUBLESTRING)].Color = *wxBLUE;
	EditedStylePreferences[StyleIndex(wxSTC_H_SINGLESTRING)].Color = *wxBLUE;
	EditedStylePreferences[StyleIndex(wxSTC_H_COMMENT)].Color = wxColour(0, 128, 0);
	EditedStylePreferences[StyleIndex(wxSTC_SQL_COMMENT)].Color = wxColor(0, 128, 0);
	EditedStylePreferences[StyleIndex(wxSTC_SQL_COMMENTLINE)].Color = wxColor(0, 128, 0);
	EditedStylePreferences[StyleIndex(wxSTC_SQL_COMMENTDOC)].Color = wxColor(0, 128, 0);
	EditedStylePreferences[StyleIndex(wxSTC_SQL_WORD)].Color = *wxBLUE;
	EditedStylePreferences[StyleIndex(wxSTC_SQL_WORD2)].Color = wxColour(163, 21, 21);
	EditedStylePreferences[StyleIndex(wxSTC_SQL_STRING)].Color = wxColour(163, 21, 21);
	EditedStylePreferences[StyleIndex(wxSTC_SQL_CHARACTER)].Color = wxColour(163, 21, 21);
	EditedStylePreferences[StyleIndex(wxSTC_SQL_COMMENTLINEDOC)].Color = wxColor(0, 128, 0);	
	EditedStylePreferences[StyleIndex(wxSTC_SQL_WORD2)].IsItalic = true;
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

	for (int i = 0; i < CodeControlOptionsClass::STYLE_COUNT; ++i) {
		EditedStylePreferences[i].Font = font;
		EditedStylePreferences[i].Color = wxColour(wxT("#DCDCCC"));
		EditedStylePreferences[i].BackgroundColor = wxColour(wxT("#1F1F1F"));
		EditedStylePreferences[i].IsBold = false;
		EditedStylePreferences[i].IsItalic = false;
	}

	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING)].Color = wxColour(wxT("#CC9393"));	
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_SIMPLESTRING)].Color = wxColour(wxT("#CC9393"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_WORD)].Color = wxColour(wxT("#DFC47D"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_WORD)].IsBold = true;
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_NUMBER)].Color = wxColour(wxT("#8CD0D3"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_VARIABLE)].Color = wxColour(wxT("#CEDF99"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMPLEX_VARIABLE)].Color = wxColour(wxT("#CEDF99"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENT)].Color = wxColour(wxT("#7F9F7F"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_COMMENTLINE)].Color = wxColour(wxT("#7F9F7F"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_HSTRING_VARIABLE)].Color = wxColour(wxT("#CC9393"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_OPERATOR)].Color = wxColour(wxT("#9F9D6D"));
	EditedStylePreferences[StyleIndex(wxSTC_HPHP_OPERATOR)].IsBold = true;
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACELIGHT)].Color = wxColour(wxT("#AE0000"));
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_BRACEBAD)].Color = wxColour(wxT("#FF0000"));
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_LINENUMBER)].Color = wxColour(wxT("#8A8A8A"));
	EditedStylePreferences[StyleIndex(wxSTC_STYLE_LINENUMBER)].BackgroundColor = wxColour(wxT("#000000"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET)].Color = wxColour(wxT("#D74100"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET)].BackgroundColor = wxColour(wxT("#D74100"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE)].Color = wxColour(wxT("#000000"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE)].BackgroundColor = wxColour(wxT("#000000"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION)].BackgroundColor = wxColour(wxT("#646464"));
	EditedStylePreferences[StyleIndex(mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT)].Color = wxTheColourDatabase->Find(wxT("YELLOW"));
	EditedStylePreferences[StyleIndex(wxSTC_H_TAG)].Color = wxColour(wxT("#DFC47D"));
	EditedStylePreferences[StyleIndex(wxSTC_H_TAG)].IsBold = true;
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTE)].Color = wxColour(wxT("#DFC47D"));
	EditedStylePreferences[StyleIndex(wxSTC_H_ATTRIBUTE)].IsBold = true;
	EditedStylePreferences[StyleIndex(wxSTC_H_DOUBLESTRING)].Color = wxColour(wxT("#CC9393"));
	EditedStylePreferences[StyleIndex(wxSTC_H_SINGLESTRING)].Color = wxColour(wxT("#CC9393"));
	EditedStylePreferences[StyleIndex(wxSTC_H_COMMENT)].Color = wxColour(wxT("#7F9F7F"));
	EditedStylePreferences[StyleIndex(wxSTC_H_QUESTION)].Color = wxColour(wxT("#DFC47D"));
	EditedStylePreferences[StyleIndex(wxSTC_H_TAGEND)].Color = wxColour(wxT("#DFC47D"));

	EditedStylePreferences[StyleIndex(wxSTC_SQL_COMMENT)].Color = wxColour(wxT("#7F9F7F"));
	EditedStylePreferences[StyleIndex(wxSTC_SQL_COMMENTLINE)].Color = wxColour(wxT("#7F9F7F"));
	EditedStylePreferences[StyleIndex(wxSTC_SQL_COMMENTDOC)].Color = wxColour(wxT("#7F9F7F"));
	EditedStylePreferences[StyleIndex(wxSTC_SQL_WORD)].Color = wxColour(wxT("#DFC47D"));
	EditedStylePreferences[StyleIndex(wxSTC_SQL_WORD)].IsBold = true;
	EditedStylePreferences[StyleIndex(wxSTC_SQL_STRING)].Color = wxColour(wxT("#CC9393"));;
	EditedStylePreferences[StyleIndex(wxSTC_SQL_CHARACTER)].Color = wxColour(wxT("#CC9393"));
	EditedStylePreferences[StyleIndex(wxSTC_SQL_COMMENTLINEDOC)].Color = wxColor(0, 128, 0);	
	EditedStylePreferences[StyleIndex(wxSTC_SQL_WORD2)].IsItalic = true;
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