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
#include <widgets/FinderValidatorClass.h>
#include <windows/StringHelperClass.h>
#include <unicode/unistr.h>
#include <unicode/regex.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/msgdlg.h>

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

mvceditor::FinderValidatorClass::FinderValidatorClass(mvceditor::FinderClass *finder,
													  wxRadioBox* modeRadio) 
	: wxValidator() 
	, Finder(finder)
	, ModeRadio(modeRadio) {
}

mvceditor::FinderValidatorClass::FinderValidatorClass() 
	: wxValidator()
	, Finder(NULL)
	, ModeRadio(NULL) {
}

bool mvceditor::FinderValidatorClass::TransferFromWindow() {
	bool ret = false;
	if (!Finder) {
		return false;
	}
	wxTextCtrl* t = wxDynamicCast(GetWindow(), wxTextCtrl);
	wxComboBox* combo = wxDynamicCast(GetWindow(), wxComboBox);
	UnicodeString regEx;
	if (t) {
		wxString val = t->GetValue();
		regEx = mvceditor::StringHelperClass::wxToIcu(val);
	}
	else if (combo) {
		wxString val = combo->GetValue();
		regEx = mvceditor::StringHelperClass::wxToIcu(val);
	}
	if (!regEx.isEmpty()) {
		ret = true;
		Finder->Expression = regEx;
	}
	return ret;
}

bool mvceditor::FinderValidatorClass::TransferToWindow() {
	bool ret = false;
	if (!Finder) {
		return false;
	}
	wxTextCtrl* t = wxDynamicCast(GetWindow(), wxTextCtrl);
	wxComboBox* combo = wxDynamicCast(GetWindow(), wxComboBox);
	UnicodeString regEx;
	if (t) {
		wxString val = mvceditor::StringHelperClass::IcuToWx(Finder->Expression);
		t->SetValue(val);
		ret = true;
	}
	else if (combo) {
		wxString val = mvceditor::StringHelperClass::IcuToWx(Finder->Expression);
		combo->SetValue(val);
		ret = true;
	}
	return ret;
}

bool mvceditor::FinderValidatorClass::Validate(wxWindow *parent) {
	bool ret =  false;
	if (!ModeRadio || !Finder) {
		return false;
	}
	if (ModeRadio->GetSelection() != mvceditor::FinderClass::REGULAR_EXPRESSION) {
		ret = true;
	}
	else {
		
		// need to validate the reg ex
		wxTextCtrl* t = wxDynamicCast(GetWindow(), wxTextCtrl);
		wxComboBox* combo = wxDynamicCast(GetWindow(), wxComboBox);
		UnicodeString regEx;
		if (t) {
			wxString val = t->GetValue();
			regEx = mvceditor::StringHelperClass::wxToIcu(val);
		}
		else if (combo) {
			wxString val = combo->GetValue();
			regEx = mvceditor::StringHelperClass::wxToIcu(val);
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
			}
			else {
				ret = true;
			}
			if (pattern) {
				delete pattern;
			}
		}
		else {
			wxMessageBox(_("Regular expression cannot be empty."));
		}
	}
	return ret;
}

wxObject* mvceditor::FinderValidatorClass::Clone() const {
	mvceditor::FinderValidatorClass* validator = new mvceditor::FinderValidatorClass();
	validator->Finder = Finder;
	validator->ModeRadio = ModeRadio;
	return validator;
}
