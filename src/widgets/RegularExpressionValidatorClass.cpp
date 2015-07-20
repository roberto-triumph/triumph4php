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
#include <widgets/RegularExpressionValidatorClass.h>
#include <globals/String.h>
#include <search/FinderClass.h>
#include <unicode/unistr.h>
#include <unicode/regex.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/msgdlg.h>
#include <wx/clipbrd.h>

/**
 * Add a flag to the given regular expression string, taking care not to clobber existing flags
 *
 * @param wxComboBox& the textbox containing the regular expression (will be modified in place)
 * @param wxString flag the flag to add to the regular expression
 * @param int currentInsertionPoint the position of the cursor
 *        on Win32 GetInsertionPoint() returns 0 when the combo box is no
 *	      in focus; we must receive the position via an outside mechanism
 */
static void AddFlagToRegEx(wxComboBox* text, wxString flag, int currentInsertionPoint);

/**
 * Turns an ICU Error Code (from a regular expression compile error) into a human
 * friendly string.
 */
static wxString errorString(UErrorCode err) {
	wxString str;
	switch(err) {
	case U_REGEX_RULE_SYNTAX:
		str = _("Syntax error in regexp pattern.");
		break;
	case U_REGEX_BAD_ESCAPE_SEQUENCE:
		str = _("Unrecognized backslash escape sequence in pattern.");
		break;
	case U_REGEX_PROPERTY_SYNTAX:
		str = _("Incorrect Unicode property.");
		break;
	case U_REGEX_UNIMPLEMENTED:
		str = _("Use of regexp feature that is not yet implemented.");
		break;
	case U_REGEX_MISMATCHED_PAREN:
		str = _("Incorrectly nested parentheses in regexp pattern.");
		break;
	case U_REGEX_NUMBER_TOO_BIG:
		str = _("Decimal number is too large.");
		break;
	case U_REGEX_BAD_INTERVAL:
		str = _("Error in {min,max} interval.");
		break;
	case U_REGEX_MAX_LT_MIN:
		str = _("In {min,max}, max is less than min.");
		break;
	case U_REGEX_INVALID_BACK_REF:
		str = _("Back-reference to a non-existent capture group.");
		break;
	case U_REGEX_INVALID_FLAG:
		str = _("Invalid value for match mode flags.");
		break;
	case U_REGEX_LOOK_BEHIND_LIMIT:
		str = _("Look-Behind pattern matches must have a bounded maximum length.");
		break;
	case U_REGEX_SET_CONTAINS_STRING:
		str = _("Regexps cannot have UnicodeSets containing strings.");
		break;
	case U_REGEX_OCTAL_TOO_BIG :
		str = _("Octal character constants must be <= 0377.");
		break;
	case U_REGEX_MISSING_CLOSE_BRACKET:
		str = _("Missing closing bracket on a bracket expression.");
		break;
	case U_REGEX_INVALID_RANGE:
		str = _("In a character range [x-y], x is greater than y.");
		break;
	case U_REGEX_STACK_OVERFLOW:
		str = _("Regular expression backtrack stack overflow.");
		break;
	case U_REGEX_TIME_OUT:
		str = _("Maximum allowed match time exceeded.");
	break;
	case U_REGEX_STOPPED_BY_CALLER:
		str = _("Matching operation aborted by user callback fn.");
		break;
	default:
		break;
	}
	return str;
}

t4p::RegularExpressionValidatorClass::RegularExpressionValidatorClass(UnicodeString *data,
													  wxRadioBox* modeRadio)
	: wxValidator()
	, ModeRadio(modeRadio)
	, Data(data) {
}

t4p::RegularExpressionValidatorClass::RegularExpressionValidatorClass()
	: wxValidator()
	, ModeRadio(NULL)
	, Data(NULL) {
}

bool t4p::RegularExpressionValidatorClass::TransferFromWindow() {
	bool ret = false;
	if (!Data) {
		return false;
	}
	wxTextCtrl* t = wxDynamicCast(GetWindow(), wxTextCtrl);
	wxComboBox* combo = wxDynamicCast(GetWindow(), wxComboBox);
	UnicodeString regEx;
	if (t) {
		wxString val = t->GetValue();
		regEx = t4p::WxToIcu(val);
	} else if (combo) {
		wxString val = combo->GetValue();
		regEx = t4p::WxToIcu(val);
	}
	if (!regEx.isEmpty()) {
		ret = true;
		*Data = regEx;
	}
	return ret;
}

bool t4p::RegularExpressionValidatorClass::TransferToWindow() {
	bool ret = false;
	if (!Data) {
		return false;
	}
	wxTextCtrl* t = wxDynamicCast(GetWindow(), wxTextCtrl);
	wxComboBox* combo = wxDynamicCast(GetWindow(), wxComboBox);
	UnicodeString regEx;
	if (t) {
		wxString val = t4p::IcuToWx(*Data);
		t->SetValue(val);
		ret = true;
	} else if (combo) {
		wxString val = t4p::IcuToWx(*Data);
		combo->SetValue(val);
		ret = true;
	}
	return ret;
}

bool t4p::RegularExpressionValidatorClass::Validate(wxWindow *parent) {
	bool ret =  false;
	if (!ModeRadio || !Data) {
		return false;
	}
	if (ModeRadio->GetSelection() != t4p::FinderClass::REGULAR_EXPRESSION) {
		ret = true;
	} else {
		// need to validate the reg ex
		wxTextCtrl* t = wxDynamicCast(GetWindow(), wxTextCtrl);
		wxComboBox* combo = wxDynamicCast(GetWindow(), wxComboBox);
		UnicodeString regEx;
		if (t) {
			wxString val = t->GetValue();
			regEx = t4p::WxToIcu(val);
		} else if (combo) {
			wxString val = combo->GetValue();
			regEx = t4p::WxToIcu(val);
		}
		if (!regEx.isEmpty()) {
			UParseError parseError;
			UErrorCode status = U_ZERO_ERROR;
			RegexPattern* pattern = RegexPattern::compile(regEx, parseError, status);
			if (!pattern || U_FAILURE(status)) {
				wxString statusString = errorString(status);
				wxString errMsg = wxString::Format(_("Regular expression error on offset:%d"), parseError.offset);
				errMsg += wxT("\n");
				errMsg += statusString;
				wxMessageBox(errMsg, _(""), wxCENTER, parent);
			} else {
				ret = true;
			}
			if (pattern) {
				delete pattern;
			}
		} else {
			wxMessageBox(_("Regular expression cannot be empty."));
		}
	}
	return ret;
}

wxObject* t4p::RegularExpressionValidatorClass::Clone() const {
	t4p::RegularExpressionValidatorClass* validator = new t4p::RegularExpressionValidatorClass();
	validator->Data = Data;
	validator->ModeRadio = ModeRadio;
	return validator;
}

void t4p::PopulateRegExFindMenu(wxMenu& regExMenu, int menuStart) {
	regExMenu.Append(menuStart + ID_MENU_REG_EX_SEQUENCE_ZERO, _("*      Sequence of 0 or more"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_SEQUENCE_ONE, _("+      Sequence of 1 or more"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_ZERO_OR_ONE, _("?      Sequence of 0 or 1"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_SEQUENCE_EXACT, _("{m}    Sequence of m"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_SEQUENCE_AT_LEAST, _("{m,}   Sequence of at least m"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_SEQUENCE_BETWEEN, _("{m,n}  Sequence of m through n matches (inclusive)"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_BEGIN_OF_LINE, _("^      Beginning of line"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_END_OF_LINE, _("$      End of line"));
	regExMenu.AppendSeparator();
	regExMenu.Append(menuStart + ID_MENU_REG_EX_DIGIT, _("\\d    Decimal digit"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_WHITE_SPACE, _("\\s    White space character"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_ALPHANUMERIC, _("\\w    Alphanumeric (letter or digit)"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_NOT_DECIMAL, _("\\D    Not a decimal digit"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_NOT_WHITE_SPACE, _("\\S    Not a white space character"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_NOT_ALPHANUMERIC, _("\\W    Not an Alphanumeric (letter or digit)"));
	regExMenu.AppendSeparator();
	regExMenu.Append(menuStart + ID_MENU_REG_EX_CASE_SENSITIVE, _("(?i)    Case insensitive matching"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_COMMENT, _("(?x)    allow use of white space and #comments within patterns"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_DOT_ALL, _("(?s)    A dot (.) will match line terminator. \r\n will be treated as one character."));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_MULTI_LINE, _("(?m)    (^) and ($) will also match at the start and end of each line"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_UWORD, _("(?w)    word boundaries are found according to the definitions of word found in Unicode UAX 29"));
	regExMenu.AppendSeparator();
	regExMenu.Append(menuStart + ID_MENU_REG_EX_PHP_STRING, _("...    Matches a PHP single or double quoted string"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_PHP_VARIABLE, _("...    Matches a PHP variable"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_PHP_NUMBER, _("...    Matches a PHP number"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_PHP_WHITESPACE, _("\\s+    Matches PHP whitespace"));
}

void t4p::PopulateRegExReplaceMenu(wxMenu& regExMenu, int menuStart) {
	regExMenu.Append(menuStart + ID_MENU_REG_EX_REPLACE_MATCH_ONE,		_("$1      Match Group 1"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_REPLACE_MATCH_TWO,		_("$2      Match Group 2"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_REPLACE_MATCH_THREE,	_("$3      Match Group 3"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_REPLACE_MATCH_FOUR,		_("$4      Match Group 4"));
	regExMenu.Append(menuStart + ID_MENU_REG_EX_REPLACE_MATCH_FIVE,		_("$5      Match Group 5"));
}

void t4p::AddSymbolToRegularExpression(wxComboBox* text, int id, int currentInsertionPoint) {
	wxString symbols;
	if (id == ID_MENU_REG_EX_SEQUENCE_ZERO) {
		symbols = wxT("*");
	}
	if (id == ID_MENU_REG_EX_SEQUENCE_ONE) {
		symbols = wxT("+");
	}
	if (id == ID_MENU_REG_EX_ZERO_OR_ONE) {
		symbols = wxT("?");
	}
	if (id == ID_MENU_REG_EX_SEQUENCE_EXACT) {
		symbols = wxT("{m}");
	}
	if (id == ID_MENU_REG_EX_SEQUENCE_AT_LEAST) {
		symbols = wxT("{m,}");
	}
	if (id == ID_MENU_REG_EX_SEQUENCE_BETWEEN) {
		symbols = wxT("{m,n}");
	}
	if (id == ID_MENU_REG_EX_BEGIN_OF_LINE) {
		symbols = wxT("^");
	}
	if (id == ID_MENU_REG_EX_END_OF_LINE) {
		symbols = wxT("$");
	}
	if (id == ID_MENU_REG_EX_DIGIT) {
		symbols = wxT("\\d");
	}
	if (id == ID_MENU_REG_EX_WHITE_SPACE) {
		symbols = wxT("\\s");
	}
	if (id == ID_MENU_REG_EX_ALPHANUMERIC) {
		symbols = wxT("\\w");
	}
	if (id == ID_MENU_REG_EX_NOT_DECIMAL) {
		symbols = wxT("\\D");
	}
	if (id == ID_MENU_REG_EX_NOT_WHITE_SPACE) {
		symbols = wxT("\\S");
	}
	if (id == ID_MENU_REG_EX_NOT_ALPHANUMERIC) {
		symbols = wxT("\\W");
	}
	if (id == ID_MENU_REG_EX_PHP_NUMBER) {
		symbols = wxT("(-)?([0-9]+)|([0-9]*\\.[0-9]+)|([0-9]+\\.[0-9]*)");
	}
	if (id == ID_MENU_REG_EX_PHP_STRING) {
		symbols = wxT("('[^']*')|(\"[^\"]*\")");
	}
	if (id == ID_MENU_REG_EX_PHP_VARIABLE) {
		symbols = wxT("\\$[a-zA-Z_\\x7f-\\xff][a-zA-Z0-9_\\x7f-\\xff]*");
	}
	if (id == ID_MENU_REG_EX_PHP_WHITESPACE) {
		symbols = wxT("\\s+");
	}
	if (!symbols.IsEmpty()) {
		// would love to use wxComboBox::Replace() here, but on Win32 a error occurs
		// "Failed To Set Clipboard Data". Will need to do the replacement ourselves
		// also; on Win32 GetInsertionPoint() returns 0 when the combo box is no
		// in focus; we must receive the position via an outside mechanism
		wxString val = text->GetValue();
		wxString newVal = val.Mid(0, currentInsertionPoint) + symbols + val.Mid(currentInsertionPoint);
		text->SetValue(newVal);

		text->SetFocus();
		text->SetInsertionPoint(currentInsertionPoint + symbols.Length());
	} else if (id == ID_MENU_REG_EX_CASE_SENSITIVE) {
		// reg ex flags always go at the beginning
		AddFlagToRegEx(text, wxT("i"), currentInsertionPoint);
	} else if (id == ID_MENU_REG_EX_COMMENT) {
		AddFlagToRegEx(text, wxT("x"), currentInsertionPoint);
	} else if (id == ID_MENU_REG_EX_DOT_ALL) {
		AddFlagToRegEx(text, wxT("s"), currentInsertionPoint);
	} else if (id == ID_MENU_REG_EX_MULTI_LINE) {
		AddFlagToRegEx(text, wxT("m"), currentInsertionPoint);
	} else if (id == ID_MENU_REG_EX_UWORD) {
		AddFlagToRegEx(text, wxT("w"), currentInsertionPoint);
	}
}

void t4p::AddSymbolToReplaceRegularExpression(wxComboBox* text, int id, int currentInsertionPoint) {
	wxString symbols;
	if (id == ID_MENU_REG_EX_REPLACE_MATCH_ONE) {
		symbols = wxT("$1");
	}
	if (id == ID_MENU_REG_EX_REPLACE_MATCH_TWO) {
		symbols = wxT("$2");
	}
	if (id == ID_MENU_REG_EX_REPLACE_MATCH_THREE) {
		symbols = wxT("$3");
	}
	if (id == ID_MENU_REG_EX_REPLACE_MATCH_FOUR) {
		symbols = wxT("$4");
	}
	if (id == ID_MENU_REG_EX_REPLACE_MATCH_FIVE) {
		symbols = wxT("$5");
	}
	if (!symbols.IsEmpty()) {
		// would love to use wxComboBox::Replace() here, but on Win32 a error occurs
		// "Failed To Set Clipboard Data". Will need to do the replacement ourselves
		// also; on Win32 GetInsertionPoint() returns 0 when the combo box is no
		// in focus; we must receive the position via an outside mechanism
		wxString val = text->GetValue();
		wxString newVal = val.Mid(0, currentInsertionPoint) + symbols + val.Mid(currentInsertionPoint);
		text->SetValue(newVal);

		text->SetFocus();
		text->SetInsertionPoint(currentInsertionPoint + symbols.Length());
	}
}

void AddFlagToRegEx(wxComboBox* text, wxString flag, int currentInsertionPoint) {
	wxString value = text->GetValue();

	//meta syntax (?i) at the start of the regex. we need to put the new flag, but only if it is not already there
	wxString startFlag(wxT("(?"));
	if (0 == value.Find(startFlag)) {
		int afterMetasPos = value.Find(wxT(")"));
		if (afterMetasPos > 0) {
			wxString flags = value.SubString(0, afterMetasPos);
			if (wxNOT_FOUND == flags.Find(flag)) {
				// would love to use wxComboBox::Replace() here, but on Win32 a error occurs
				// "Failed To Set Clipboard Data". Will need to do the replacement ourselves
				wxString val = text->GetValue();
				wxString newVal = val.Mid(0, afterMetasPos - 1) + flag + val.Mid(afterMetasPos - 1);
				text->SetValue(newVal);

				text->SetFocus();
				text->SetInsertionPoint(currentInsertionPoint + flag.Length());
			}
		}
	} else {
		value = startFlag + flag + wxT(")") + value;
		text->SetValue(value);
	}
}
