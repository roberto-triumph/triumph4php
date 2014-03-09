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

t4p::CodeControlOptionsClass::CodeControlOptionsClass() 
: PhpStyles() 
, SqlStyles()
, CssStyles()
, JsStyles()
, ConfigStyles()
, CrontabStyles()
, YamlStyles()
, RubyStyles()
, LuaStyles()
, MarkdownStyles()
, BashStyles()
, DiffStyles()
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
, EnableCallTipsOnMouseHover(true) 
, EnableMultipleSelection(true)
, EnableVirtualSpace(false)
, EnableRectangularSelection(false)
, ShowWhitespace(false)
, Zoom(0)
{
	
}

t4p::CodeControlOptionsClass& t4p::CodeControlOptionsClass::operator=(const t4p::CodeControlOptionsClass& src) {
	Copy(src);
	return *this;
}

void t4p::CodeControlOptionsClass::CopyStyles(const t4p::CodeControlOptionsClass& src) {
	PhpStyles = src.PhpStyles; 
	SqlStyles = src.SqlStyles;
	CssStyles = src.CssStyles;
	JsStyles = src.JsStyles;
	ConfigStyles = src.ConfigStyles;
	CrontabStyles = src.CrontabStyles; 
	YamlStyles = src.YamlStyles;
	RubyStyles = src.RubyStyles;
	LuaStyles = src.LuaStyles;
	MarkdownStyles = src.MarkdownStyles;
	BashStyles = src.BashStyles;
	DiffStyles = src.DiffStyles;
}

void t4p::CodeControlOptionsClass::Copy(const t4p::CodeControlOptionsClass& src) {
	CopyStyles(src);

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
	EnableMultipleSelection = src.EnableMultipleSelection;
	EnableVirtualSpace = src.EnableVirtualSpace;
	EnableRectangularSelection = src.EnableRectangularSelection;
	ShowWhitespace = src.ShowWhitespace;
	Zoom = src.Zoom;
}

t4p::StylePreferenceClass& t4p::CodeControlOptionsClass::FindByStcStyle(std::vector<t4p::StylePreferenceClass>& styles, int stcStyle) const {
	for (size_t i = 0; i < styles.size(); ++i) {
		if (styles[i].StcStyle == stcStyle) {
			return styles[i];
		}
	}
	return styles[0];
}

std::vector<t4p::StylePreferenceClass> t4p::CodeControlOptionsClass::AllStyles() const {
	std::vector<t4p::StylePreferenceClass> all;
	
	all.insert(all.end(), PhpStyles.begin(), PhpStyles.end());
	all.insert(all.end(), SqlStyles.begin(), SqlStyles.end());
	all.insert(all.end(), CssStyles.begin(), CssStyles.end());
	all.insert(all.end(), JsStyles.begin(), JsStyles.end());
	all.insert(all.end(), ConfigStyles.begin(), ConfigStyles.end());
	all.insert(all.end(), CrontabStyles.begin(), CrontabStyles.end());
	all.insert(all.end(), YamlStyles.begin(), YamlStyles.end());
	all.insert(all.end(), RubyStyles.begin(), RubyStyles.end());
	all.insert(all.end(), LuaStyles.begin(), LuaStyles.end());
	all.insert(all.end(), MarkdownStyles.begin(), MarkdownStyles.end());
	all.insert(all.end(), BashStyles.begin(), BashStyles.end());
	all.insert(all.end(), DiffStyles.begin(), DiffStyles.end());
	
	return all;
}

static void ReadStyles(std::vector<t4p::StylePreferenceClass>& styles, wxConfigBase* config) {
	for (size_t i = 0; i < styles.size(); ++i) {
		styles[i].Read(config);
	}
}

void t4p::CodeControlOptionsClass::Load(wxConfigBase* config) {
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
	config->Read(wxT("EditorBehavior/EnableMultipleSelection"), &EnableMultipleSelection); 
	config->Read(wxT("EditorBehavior/EnableVirtualSpace"), &EnableVirtualSpace); 
	config->Read(wxT("EditorBehavior/EnableRectangularSelection"), &EnableRectangularSelection); 
	config->Read(wxT("EditorBehavior/Showhitespace"), &ShowWhitespace); 
	config->Read(wxT("EditorBehavior/Zoom"), &Zoom); 
		
	ReadStyles(PhpStyles, config);
	ReadStyles(SqlStyles, config);
	ReadStyles(CssStyles, config);
	ReadStyles(JsStyles, config);
	ReadStyles(CrontabStyles, config);
	ReadStyles(YamlStyles, config);
	ReadStyles(RubyStyles, config);
	ReadStyles(LuaStyles, config);
	ReadStyles(MarkdownStyles, config);
	ReadStyles(BashStyles, config);
	ReadStyles(DiffStyles, config);
}
	 
void t4p::CodeControlOptionsClass::Save(wxConfigBase* config) {
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
	config->Write(wxT("EditorBehavior/EnableMultipleSelection"), EnableMultipleSelection); 
	config->Write(wxT("EditorBehavior/EnableVirtualSpace"), EnableVirtualSpace); 
	config->Write(wxT("EditorBehavior/EnableRectangularSelection"), EnableRectangularSelection); 
	config->Write(wxT("EditorBehavior/Showhitespace"), ShowWhitespace); 
	config->Write(wxT("EditorBehavior/Zoom"), Zoom); 
	
	std::vector<t4p::StylePreferenceClass> allStyles = AllStyles();
	for (size_t i = 0; i < allStyles.size(); ++i) {
		allStyles[i].Write(config);
	}
}

t4p::StylePreferenceClass::StylePreferenceClass() 
: Font()
, Color()
, BackgroundColor()
, Name(0)
, StcStyle(0)
, IsBold(false)
, IsItalic(false) {
		
}

t4p::StylePreferenceClass::StylePreferenceClass(const t4p::StylePreferenceClass& src)
: Font()
, Color()
, BackgroundColor()
, Name(0)
, StcStyle(0)
, IsBold(false)
, IsItalic(false) {
	Copy(src);
}

t4p::StylePreferenceClass& t4p::StylePreferenceClass::operator=(const t4p::StylePreferenceClass& src) {
	Copy(src);
	return *this;
}

void t4p::StylePreferenceClass::Copy(const t4p::StylePreferenceClass& src) {
	Font = src.Font;
	Color = src.Color;
	BackgroundColor = src.BackgroundColor;
	IsBold = src.IsBold;
	IsItalic = src.IsItalic;
	StcStyle = src.StcStyle;
	Name = src.Name;
}

bool t4p::StylePreferenceClass::Read(wxConfigBase* config) {
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

bool t4p::StylePreferenceClass::Write(wxConfigBase* config) {
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

void t4p::StylePreferenceClass::Control(int stcStyle, const char* name) {
	Name = name;
	StcStyle = stcStyle;
}