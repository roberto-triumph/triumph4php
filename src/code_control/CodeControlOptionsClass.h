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
#include <wx/string.h>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/config.h>
#include <vector>

#ifndef __CODECONTROLLASS_H___
#define __CODECONTROLLASS_H___

namespace mvceditor {

/**
 * A class that holds the editor styles for syntax highlighting
 */
class StylePreferenceClass {
	
public:
	
	wxFont Font;
	wxColor Color;
	wxColor BackgroundColor;
	
	/**
	 * A name for this style. This is not
	 * nationalized, but it is human-friendly.
	 * Guaranteed to be unique.
	 */
	const char* Name;
	
	/**
	 * this is the value of the wxSTC_* constant that this
	 * style maps to.
	 * For example, for the PHP double quoted string, the value of this
	 * variable will be wxSTC_HPHP_HSTRING
	 * This could also be one of the STYLES enum of this class.
	 */
	int StcStyle;
	
	bool IsBold;
	bool IsItalic;
	
	StylePreferenceClass();
	
	/**
	 * Copy the properties from src to this object.
	 * After a call to this method; src and this will have the same values
	 * for each of the properties.
	 */
	void Copy(const StylePreferenceClass& src);
	
	/**
	 * Give a name to this style and set the constant that it
	 * controls.
	 */
	void Control(int stcStyle, const char* name);
	
	bool Read(wxConfigBase* config);
	
	bool Write(wxConfigBase* config);
};
	
/**
 * This class will act as a bridge between the preferences dialog and the CodeControlClass; it will
 * contain logic to simplify the font selection.
 */
class CodeControlOptionsClass {

public:

	/**
	 * Stores the preferences for each style; for syntax highlighting 
	 */
	std::vector<StylePreferenceClass> PhpStyles;
	std::vector<StylePreferenceClass> SqlStyles;
	std::vector<StylePreferenceClass> CssStyles;
	
	/**
	 * Stores the preferences being edited.  They are stored separately so that if the user
	 * happens to cancel the operation, the original settings are left intact.
	 */
	std::vector<StylePreferenceClass> EditedPhpStyles;
	std::vector<StylePreferenceClass> EditedSqlStyles;
	std::vector<StylePreferenceClass> EditedCssStyles;
	
	/**
	 *  This is the entire list of styles that can have font/color attached to.
	 *  @var wxArrayString
	 */
	wxArrayString CodeStyles;
	
	/**
	 * The number of spaces.  Only valid when IndentUsingTabs is false.
	 * 
	 * @var int
	 */
	int SpacesPerIndent;
	
	/**
	 * The number of columns to insert per TAB character. Only valid when IndentUsingTabs is true.
	 * @var int
	 */
	int TabWidth;
	
	/**
	 * The column at which to show a vertical line, useful for showing long lines. If zero, the 
	 * right margin is not shown.
	 * @var int
	 */
	int RightMargin;
	
	/**
	 * Line ending mode.
	 * 
	 * 0 = windows (\r\n)
	 * 1 = mac (\r)
	 * 2 = unix (\n)
	 */
	int LineEndingMode;
	
	/**
	 * Is true, then when the user enters a TAB character SpacesPerIndent number of spaces will be inserted instead of 
	 * a '\t'
	 * 
	 * @var bool
	 */
	bool IndentUsingTabs;
	
	/**
	 *  If true, the control will enable code folding.
	 * 
	 * @var bool
	 */
	bool EnableCodeFolding;
	
	/**
	 * If true, the control will automatically indent when the user hits the ENTER key
	 * @var bool
	 */
	bool EnableAutomaticLineIndentation;

	/**
	 * If true, line numbers will be shown in the left margin
	 * @var bool
	 */
	bool EnableLineNumbers;
	
	/**
	 * If true, indentation guides are shown.
	 * @var bool
	 */
	bool EnableIndentationGuides;
	
	/**
	 * if true, line endings are shown.
	 * @var bool
	 */
	bool EnableLineEndings;
	
	/**
	 * If true, Auto completion will be automatically triggered.  If false, the use
	 * can still manually trigger it
	 */
	bool EnableAutoCompletion;
	
	/**
	 * If true, Auto completion will allow for 'dynamic' matching; auto complete list 
	 * will show matching method / property names even if the variable could not be
	 * resolved.
	 */
	bool EnableDynamicAutoCompletion;

	/**
	 * If TRUE, lines wider than the window width will be continued on the following lines
	 */
	bool EnableWordWrap;

	/**
	 * If TRUE, the editor will remove any space characters from the last line before
	 * the document is saved.
	 */
	bool TrimTrailingSpaceBeforeSave;

	/**
	 * If TRUE, the editor will remove any trailing lines that are empty. This functionality
	 * will prevent the "headers already sent" problem when there are empty lines after
	 * a PHP closing tag.
	 */
	bool RemoveTrailingBlankLinesBeforeSave;

	CodeControlOptionsClass();

	/**
	 * Initializes the color schemes. 
	 */
	void Init();
	
	/**
	 * Prepares the edited preferences by copying them FROM the real settings. From this point on, all calls to 
	 * GetEditedStyle() will return the preferences that are set by the ChangeStyle() method. Note that this
	 * only affects style preferences and not the public properties.
	 */
	void StartEditMode();
	
	/**
	 * Promotes (copies) all edited preferences to the real settings. From this point on, all calls to 
	 * GetStyle() will return the preferences that were set in ChangeStyle() method. Note that this
	 * only affects style preferences and not the public properties.
	 */
	void CommitChanges();
	
	/**
	 * Set the color scheme to the default (dark on light) scheme
	 */
	void SetToLightTheme();

	/**
	 * Set the color scheme to the light on dark scheme
	 */
	void SetToDarkTheme();
	
	/**
	 * Load state from persistent storage
	 * 
	 * @param wxConfigBase* config the config
	 */
	void Load(wxConfigBase* config);
	 
	/**
	 * Save state to persistent storage
	 */
	void Save(wxConfigBase* config);
	
	/**
	 * This enum maps numbers to margins.  
	 */
	enum MARGINS {
		MARGIN_LINE_NUMBER = 0,
		MARGIN_CODE_FOLDING = 1	
	};
	
	/**
	 * This enum will be used to designate the style defintions for caret, current line, and
	 * selection.  This enum will be used so that caret, current line, and
	 * selection preferences can be stored in the Preferences array even though they are not stored in the
	 * Scintilla style bits.
	 */
	enum STYLES {
		MVC_EDITOR_STYLE_CARET = 512,
		MVC_EDITOR_STYLE_CARET_LINE,
		MVC_EDITOR_STYLE_SELECTION,
		MVC_EDITOR_STYLE_CODE_FOLDING,
		MVC_EDITOR_STYLE_RIGHT_MARGIN,
		MVC_EDITOR_STYLE_MATCH_HIGHLIGHT,
	};

	/**
	 * Get the array index of the given style. Note that this function is to be used for
	 * static data, as a failed assertion is made when the style is not valid.
	 * 
	 * @param int styles the style array to look into. one of static arrays PhpStyles, CssStyles, etc..
	 * @param int style the STC constant to look for
	 * @return int the index of the style in the preferences array.  
	 */
	StylePreferenceClass& FindByStcStyle(std::vector<StylePreferenceClass>& styles, int style) const;
	
};

}

#endif