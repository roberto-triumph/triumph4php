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
#include <wx/string.h>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/config.h>

#ifndef __CODECONTROLLASS_H___
#define __CODECONTROLLASS_H___

namespace mvceditor {
	
/**
 * This class will act as a bridge between the preferences dialog and the CodeControlClass; it will
 * contain logic to simplify the font selection.
 */
class CodeControlOptionsClass {

public:

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
	
	CodeControlOptionsClass();
	
	/**
	 * Changes the font, color, bold, and italic settings for the given style.
	 * 
	 * @param int style index into the CodeStyles array
	 * @param wxFont font the new font
	 * @param wxColor color the new foreground color
	 * @param wxColor backgroundColor the new background color
	 * @param bool isBold the new setting for bold
	 * @param bool isItalic the new setting for italic
	 */
	void ChangeStyle(int style, wxFont font, wxColor color, wxColor backgroundColor, bool isBold, bool isItalic);

	/**
	 * Get the font, color, bold, and italic settings for the given style. Will NOT get the preferences
	 * that have been changed (by calling ChangeStyle()), UNTIL CommitChanges() method gest called.
	 * 
	 * @param int style one of the wxSTC_HPHP_* constants, or wxSTC_STYLE_*
	 * @param wxFont the font will be written to this variable
	 * @param wxColor color the color will be written to this variable
	 * @param wxColor backgroundColor the background color will be written to this variable
	 * @param bool isBold the  setting for bold will be written to this variable
	 * @param bool isItalic the setting for italic will be written to this variable
	 * @return bool if style exists and is a valid style
	 */
	bool GetStyleByStcConstant(int style, wxFont& font, wxColor& color, wxColor& backgroundColor, bool& isBold, 
		bool& isItalic) const;
	
	/**
	 * Get the font, color, bold, and italic settings for the given style THAT IS CURRENTLY BEING EDITED.
	 * 
	 * @param int style index into the CodeStyles array
	 * @param wxFont the font will be written to this variable
	 * @param wxColor color the color will be written to this variable
	 * @param wxColor backgroundColor the color will be written to this variable
	 * @param bool isBold the  setting for bold will be written to this variable
	 * @param bool isItalic the setting for italic will be written to this variable
	 * @return bool if style exists and is a valid style
	 */
	bool GetEditStyle(int style, wxFont& font, wxColor& color, wxColor& backgroundColor, bool& isBold, 
		bool& isItalic) const;
	
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
	 * Get the array index of the given style. Note that this function is to be used for
	 * static data, as a failed assertion is made when the style is not valid.
	 * 
	 * @param int style 
	 * @return int the index of the style in the preferences array.  
	 */
	int StyleIndex(int style) const;
	
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
	 * number of styles that can be edited.
	 * 
	 * @var int
	 */
	static const int STYLE_COUNT = 54;
	
	/**
	 * Mapping of array index to wxSTC_HPHP_*, wxSTC_STYLE_* constant, eliminating the need for IFs or
	 * CASE statements.
	 */
	static int ArrayIndexToStcConstant[STYLE_COUNT];	

private:

	struct StylePreference {
		wxFont Font;
		wxColor Color;
		wxColor BackgroundColor;
		bool IsBold;
		bool IsItalic;
	};
	
	/**
	 * Stores the preferences to for each style 
	 */
	StylePreference StylePreferences[STYLE_COUNT];
	
	/**
	 * Stores the preferences being edited.  They are stored separately so that if the user
	 * happens to cancel the operation, the original settings are left intact.
	 */
	StylePreference EditedStylePreferences[STYLE_COUNT];
	
};

}

#endif