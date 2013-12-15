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
#include <globals/CodeControlOptionsClass.h>
#include <wx/platinfo.h>
#include <wx/utils.h>

mvceditor::CodeControlOptionsClass::CodeControlOptionsClass() 
: PhpStyles() 
, SqlStyles()
, CssStyles()
, CodeStyles() 
, SpacesPerIndent(0) 
, TabWidth(4) 
, RightMargin(0) 
, LineEndingMode(2)
, IndentUsingTabs(true) 
, EnableCodeFolding(true) 
, EnableAutomaticLineIndentation(true)
, EnableLineNumbers(true)
, EnableIndentationGuides(false) 
, EnableLineEndings(false) 
, EnableAutoCompletion(true)
, EnableDynamicAutoCompletion(false) 
, EnableWordWrap(false) 
, TrimTrailingSpaceBeforeSave(false) 
, RemoveTrailingBlankLinesBeforeSave(true) 
, EnableCallTipsOnMouseHover(true) {

}

mvceditor::CodeControlOptionsClass::CodeControlOptionsClass(const mvceditor::CodeControlOptionsClass& src) {
	Copy(src);
}

mvceditor::CodeControlOptionsClass& mvceditor::CodeControlOptionsClass::operator=(const mvceditor::CodeControlOptionsClass& src) {
	Copy(src);
	return *this;
}

void mvceditor::CodeControlOptionsClass::Copy(const mvceditor::CodeControlOptionsClass& src) {
	PhpStyles = src.PhpStyles; 
	SqlStyles = src.SqlStyles;
	CssStyles = src.CssStyles;
	CodeStyles = src.CodeStyles; 
	SpacesPerIndent = src.SpacesPerIndent;
	TabWidth = src.TabWidth;
	RightMargin = src.RightMargin;
	LineEndingMode = src.LineEndingMode;
	IndentUsingTabs = src.IndentUsingTabs;
	EnableCodeFolding = src.EnableCodeFolding;
	EnableAutomaticLineIndentation = src.EnableAutomaticLineIndentation;
	EnableLineNumbers = src.EnableLineNumbers;
	EnableIndentationGuides = src.EnableIndentationGuides;
	EnableLineEndings = src.EnableLineEndings;
	EnableAutoCompletion = src.EnableAutoCompletion;
	EnableDynamicAutoCompletion = src.EnableDynamicAutoCompletion;
	EnableWordWrap = src.EnableWordWrap;
	TrimTrailingSpaceBeforeSave = src.TrimTrailingSpaceBeforeSave;
	RemoveTrailingBlankLinesBeforeSave = src.RemoveTrailingBlankLinesBeforeSave; 
	EnableCallTipsOnMouseHover = src.EnableCallTipsOnMouseHover;
}

mvceditor::StylePreferenceClass& mvceditor::CodeControlOptionsClass::FindByStcStyle(std::vector<mvceditor::StylePreferenceClass>& styles, int stcStyle) const {
	for (size_t i = 0; i < styles.size(); ++i) {
		if (styles[i].StcStyle == stcStyle) {
			return styles[i];
		}
	}
	return styles[0];
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
	config->Read(wxT("EditorBehavior/EnableDynamicAutoCompletion"), &EnableDynamicAutoCompletion);
	config->Read(wxT("EditorBehavior/EnableWordWrap"), &EnableWordWrap);
	config->Read(wxT("EditorBehavior/TrimTrailingSpaceBeforeSave"), &TrimTrailingSpaceBeforeSave);
	config->Read(wxT("EditorBehavior/RemoveTrailingBlankLinesBeforeSave"), &RemoveTrailingBlankLinesBeforeSave);
	config->Read(wxT("EditorBehavior/EnableCallTipsOnMouseHover"), &EnableCallTipsOnMouseHover); 
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
	config->Write(wxT("EditorBehavior/EnableDynamicAutoCompletion"), EnableDynamicAutoCompletion);
	config->Write(wxT("EditorBehavior/EnableWordWrap"), EnableWordWrap);
	config->Write(wxT("EditorBehavior/TrimTrailingSpaceBeforeSave"), TrimTrailingSpaceBeforeSave);
	config->Write(wxT("EditorBehavior/RemoveTrailingBlankLinesBeforeSave"), RemoveTrailingBlankLinesBeforeSave);
	config->Write(wxT("EditorBehavior/EnableCallTipsOnMouseHover"), EnableCallTipsOnMouseHover);
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

mvceditor::StylePreferenceClass::StylePreferenceClass(const mvceditor::StylePreferenceClass& src)
: Font()
, Color()
, BackgroundColor()
, Name(0)
, StcStyle(0)
, IsBold(false)
, IsItalic(false) {
	Copy(src);
}

mvceditor::StylePreferenceClass& mvceditor::StylePreferenceClass::operator=(const mvceditor::StylePreferenceClass& src) {
	Copy(src);
	return *this;
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