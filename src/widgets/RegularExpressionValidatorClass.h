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
 */
#ifndef __FINDERVALIDATORCLASS_H
#define __FINDERVALIDATORCLASS_H

#include <wx/validate.h>
#include <wx/radiobox.h>
#include <wx/combobox.h>
#include <wx/menu.h>
#include <unicode/unistr.h>

namespace t4p {

/**
 * This class is used to validate that a finder has a correctly formed regular expression
 * if the finder is in regex mode. Currently this validator
 * can handle either wxTextCtrl of wxComboBox only.
 */
class RegularExpressionValidatorClass : public wxValidator {

public:

	/**
	 * @param The string to validate / transfer. This class will not own this pointer.
	 * @param the mode radio button. this needs to be
	 *        checked so that we skip reg ex validation
	 *        when the expression is not a reg ex
	 */
	RegularExpressionValidatorClass(UnicodeString* data, wxRadioBox* modeRadio);

	virtual bool TransferToWindow();

	virtual bool TransferFromWindow();

	virtual bool Validate(wxWindow* parent);

	virtual wxObject* Clone() const;

private:

	RegularExpressionValidatorClass();

	/**
	 * The mode radio button; this needs to be
	 * checked so that we skip reg ex validation
	 * when the expression is not a reg ex
	 */
	wxRadioBox* ModeRadio;

	/**
	 * The object to validate. This class will not own this pointer.
	 */
	UnicodeString* Data;
};

/**
 * Add all of the regular expression flags to the given menu
 * @param regExMenu the popup menu
 * @param menuStart the start ID of the menu items
 */
void PopulateRegExFindMenu(wxMenu& regExMenu, int menuStart);

/**
 * Add all of the regular expression replacements to the given menu
 * @param regExMenu the popup menu
 * @param menuStart the start ID of the menu items
 */
void PopulateRegExReplaceMenu(wxMenu& regExMenu, int menuStart);

/**
 * Alter the given textbox (that contains a regular expression) depending on the user choice.
 * Note that the caret will be left at currentInsertionPoint
 *
 * @param wxComboBox* the combobox containing the regular expression (will be modified in place)
 * @param int menuId the ID that the user chose (one of the ID_MENU_REG_EX_ constants)
 * @param int currentInsertionPoint the position of the cursor
 *        on Win32 GetInsertionPoint() returns 0 when the combo box is no
 *	      in focus; we must receive the position via an outside mechanism
 */
void AddSymbolToRegularExpression(wxComboBox* text, const int id, int currentInsertionPoint);

/**
 * Alter the given textbox (that contains a regular expression replacement) depending on
 * the user choice. Note that the caret will be left at currentInsertionPoint
 *
 * @param wxComboBox* the combobox containing the regular expression (will be modified in place)
 * @param int menuId the ID that the user chose (one of the ID_MENU_REG_EX_ constants)
 * @param int currentInsertionPoint the position of the cursor
 *        on Win32 GetInsertionPoint() returns 0 when the combo box is no
 *	      in focus; we must receive the position via an outside mechanism
 */
void AddSymbolToReplaceRegularExpression(wxComboBox* text, int id, int currentInsertionPoint);

/**
 * These are the Menu IDs for the regular expression popup menu.
 * When connecting to events, use these IDs.
 */
enum RegExMenus {
	ID_MENU_REG_EX_SEQUENCE_ZERO = 1,
	ID_MENU_REG_EX_SEQUENCE_ONE,
	ID_MENU_REG_EX_ZERO_OR_ONE,
	ID_MENU_REG_EX_SEQUENCE_EXACT,
	ID_MENU_REG_EX_SEQUENCE_AT_LEAST,
	ID_MENU_REG_EX_SEQUENCE_BETWEEN,
	ID_MENU_REG_EX_BEGIN_OF_LINE,
	ID_MENU_REG_EX_END_OF_LINE,
	ID_MENU_REG_EX_DIGIT,
	ID_MENU_REG_EX_WHITE_SPACE,
	ID_MENU_REG_EX_ALPHANUMERIC,
	ID_MENU_REG_EX_NOT_DECIMAL,
	ID_MENU_REG_EX_NOT_WHITE_SPACE,
	ID_MENU_REG_EX_NOT_ALPHANUMERIC,
	ID_MENU_REG_EX_CASE_SENSITIVE,
	ID_MENU_REG_EX_COMMENT,
	ID_MENU_REG_EX_DOT_ALL,
	ID_MENU_REG_EX_MULTI_LINE,
	ID_MENU_REG_EX_UWORD,
	ID_MENU_REG_EX_PHP_STRING,
	ID_MENU_REG_EX_PHP_VARIABLE,
	ID_MENU_REG_EX_PHP_NUMBER,
	ID_MENU_REG_EX_PHP_WHITESPACE,

	ID_MENU_REG_EX_REPLACE_MATCH_ONE,
	ID_MENU_REG_EX_REPLACE_MATCH_TWO,
	ID_MENU_REG_EX_REPLACE_MATCH_THREE,
	ID_MENU_REG_EX_REPLACE_MATCH_FOUR,
	ID_MENU_REG_EX_REPLACE_MATCH_FIVE
};

}

#endif
