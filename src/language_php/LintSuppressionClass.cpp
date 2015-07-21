/**
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#include "language_php/LintSuppressionClass.h"
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <vector>
#include "globals/String.h"

/**
 * serialize a SuppressionRuleClass::Types to a string
 */
static wxString RuleTypeString(t4p::SuppressionRuleClass::Types type)  {
	switch (type) {
		case t4p::SuppressionRuleClass::SKIP_UNKNOWN_CLASS:
			return wxT("SKIP_UNKNOWN_CLASS");
		case t4p::SuppressionRuleClass::SKIP_UNKNOWN_METHOD:
			return wxT("SKIP_UNKNOWN_METHOD");
		case t4p::SuppressionRuleClass::SKIP_UNKNOWN_PROPERTY:
			return wxT("SKIP_UNKNOWN_PROPERTY");
		case t4p::SuppressionRuleClass::SKIP_UNKNOWN_FUNCTION:
			return wxT("SKIP_UNKNOWN_FUNCTION");
		case t4p::SuppressionRuleClass::SKIP_UNINITIALIZED_VAR:
			return wxT("SKIP_UNINITIALIZED_VAR");
		case t4p::SuppressionRuleClass::SKIP_FUNCTION_ARGUMENT_MISMATCH:
			return wxT("SKIP_FUNCTION_ARGUMENT_MISMATCH");
		case t4p::SuppressionRuleClass::SKIP_ALL:
			return wxT("SKIP_ALL");
	}
	return wxT("");
}

/**
 * de-serialize a SuppressionRuleClass::Types from a string
 * @param good will be set to TRUE when the string is a valid type
 */
static t4p::SuppressionRuleClass::Types RuleTypeFromName(const wxString& name, bool& good)  {
	 if (name.CmpNoCase(wxT("SKIP_UNKNOWN_CLASS")) == 0) {
		 good = true;
		 return t4p::SuppressionRuleClass::SKIP_UNKNOWN_CLASS;
	 }
	 if (name.CmpNoCase(wxT("SKIP_UNKNOWN_METHOD")) == 0) {
		 good = true;
		 return t4p::SuppressionRuleClass::SKIP_UNKNOWN_METHOD;
	 }
	 if (name.CmpNoCase(wxT("SKIP_UNKNOWN_PROPERTY")) == 0) {
		 good = true;
		 return t4p::SuppressionRuleClass::SKIP_UNKNOWN_PROPERTY;
	 }
	 if (name.CmpNoCase(wxT("SKIP_UNKNOWN_FUNCTION")) == 0) {
		 good = true;
		 return t4p::SuppressionRuleClass::SKIP_UNKNOWN_FUNCTION;
	 }
	 if (name.CmpNoCase(wxT("SKIP_UNINITIALIZED_VAR")) == 0) {
		 good = true;
		 return t4p::SuppressionRuleClass::SKIP_UNINITIALIZED_VAR;
	 }
	 if (name.CmpNoCase(wxT("SKIP_FUNCTION_ARGUMENT_MISMATCH")) == 0) {
		 good = true;
		 return t4p::SuppressionRuleClass::SKIP_FUNCTION_ARGUMENT_MISMATCH;
	 }
	 if (name.CmpNoCase(wxT("SKIP_ALL")) == 0) {
		 good = true;
		 return t4p::SuppressionRuleClass::SKIP_ALL;
	 }
	 return  t4p::SuppressionRuleClass::SKIP_ALL;
}

t4p::SuppressionRuleClass::SuppressionRuleClass()
: Type(SKIP_ALL)
, Target()
, Location() {
}

t4p::SuppressionRuleClass::SuppressionRuleClass(const t4p::SuppressionRuleClass& src)
: Type(SKIP_ALL)
, Target()
, Location() {
	Copy(src);
}

void t4p::SuppressionRuleClass::SkipAllRule(const wxFileName& location) {
	Type = SKIP_ALL;
	Target = UNICODE_STRING_SIMPLE("");

	// make thread safe
	if (location.IsDir()) {
		Location.AssignDir(location.GetPath());
	} else {
		Location.Assign(location.GetFullPath());
	}
}

void t4p::SuppressionRuleClass::SkipUnknownClassRule(const wxFileName& location,
	const UnicodeString& className) {
	Type = SKIP_UNKNOWN_CLASS;
	Target = className;

	// make thread safe
	if (location.IsDir()) {
		Location.AssignDir(location.GetPath());
	} else {
		Location.Assign(location.GetFullPath());
	}
}

void t4p::SuppressionRuleClass::SkipUnknownMethodRule(const wxFileName& location,
	const UnicodeString& methodName) {
	Type = SKIP_UNKNOWN_METHOD;
	Target = methodName;

	// make thread safe
	if (location.IsDir()) {
		Location.AssignDir(location.GetPath());
	} else {
		Location.Assign(location.GetFullPath());
	}
}

void t4p::SuppressionRuleClass::SkipUnknownFunctionRule(const wxFileName& location,
		const UnicodeString& functionName) {
	Type = SKIP_UNKNOWN_FUNCTION;
	Target = functionName;

	// make thread safe
	if (location.IsDir()) {
		Location.AssignDir(location.GetPath());
	} else {
		Location.Assign(location.GetFullPath());
	}
}

t4p::SuppressionRuleClass& t4p::SuppressionRuleClass::operator=(const t4p::SuppressionRuleClass& src) {
	Copy(src);
	return *this;
}

void t4p::SuppressionRuleClass::Copy(const t4p::SuppressionRuleClass& src) {
	Type = src.Type;
	Target = src.Target;

	// make thread safe
	if (src.Location.IsDir()) {
		Location.AssignDir(src.Location.GetPath());
	} else {
		Location.Assign(src.Location.GetFullPath());
	}
}

t4p::LintSuppressionClass::LintSuppressionClass()
: Rules() {
}

bool t4p::LintSuppressionClass::Init(const wxFileName& suppressionFile, std::vector<UnicodeString>& errors) {
	if (!suppressionFile.FileExists()) {
		errors.push_back(UNICODE_STRING_SIMPLE("suppressions file does not exist"));
		return false;
	}
	wxFFileInputStream fstream(suppressionFile.GetFullPath(), wxT("rb"));
	if (!fstream.Ok()) {
		return false;
	}
	Rules.clear();
	wxTextInputStream txt(fstream);
	wxStringTokenizer tok;
	while (!fstream.Eof()) {
		// each supression is in its own line, in comma-separated form
		//
		// type,target, location
		//
		// lines starting with # are ignored (treated as comments)
		// examples:
		//
		// SKIP_UNKNOWN_CLASS,Couchbase,/home/user/www/project
		// SKIP_ALL,,/home/user/www/project/vendor
		//
		//
		wxString line = txt.ReadLine();
		line.Trim(false).Trim(true);
		if (!line.IsEmpty() && line.GetChar(0) != '#') {
			tok.SetString(line, wxT(","), wxTOKEN_DEFAULT);
			wxString typeStr = tok.GetNextToken();
			wxString targetStr = tok.GetNextToken();
			wxString locationStr = tok.GetNextToken();
			bool goodType = false;

			typeStr.Trim(false).Trim(true);
			targetStr.Trim(false).Trim(true);
			locationStr.Trim(false).Trim(true);

			t4p::SuppressionRuleClass::Types type = RuleTypeFromName(typeStr, goodType);
			if (!typeStr.IsEmpty() && !locationStr.IsEmpty() && goodType) {
				t4p::SuppressionRuleClass rule;
				rule.Type = type;
				rule.Target = t4p::WxToIcu(targetStr);
				if (wxFileName::FileExists(locationStr)) {
					rule.Location.Assign(locationStr);
					Rules.push_back(rule);
				} else if (wxFileName::DirExists(locationStr)) {
					rule.Location.AssignDir(locationStr);
					Rules.push_back(rule);
				}
			} else if (!typeStr.IsEmpty()) {
				wxString wxErr = wxT("suppression rule type cannot be empty in line: ") + line;
				UnicodeString error = t4p::WxToIcu(wxErr);
				errors.push_back(error);
			} else if (!goodType) {
				wxString wxErr = wxT("suppression rule type is not valid: ") + typeStr + wxT(" in line: ") + line;
				UnicodeString error = t4p::WxToIcu(wxErr);
				errors.push_back(error);
			} else if (!locationStr.IsEmpty()) {
				wxString wxErr = wxT("suppression rule location cannot be empty in line: ") + line;
				UnicodeString error = t4p::WxToIcu(wxErr);
				errors.push_back(error);
			}
		}
	}
	return !Rules.empty();
}

bool t4p::LintSuppressionClass::Save(const wxFileName& suppressionFile) {
	wxFFileOutputStream fstream(suppressionFile.GetFullPath(), wxT("wb"));
	if (!fstream.Ok()) {
		return false;
	}
	wxTextOutputStream txt(fstream);

	txt.WriteString("# each supression is in its own line, in comma-separated form\n");
	txt.WriteString("# \n");
	txt.WriteString("# type, target, location\n");
	txt.WriteString("# \n");
	txt.WriteString("# examples:\n");
	txt.WriteString("# \n");
	txt.WriteString("# SKIP_UNKNOWN_CLASS,Couchbase,/home/user/www/project\n");
	txt.WriteString("# SKIP_ALL,,/home/user/www/project/vendor\n");
	txt.WriteString("# \n");
	txt.WriteString("# \n");
	txt.WriteString("# Possible types:\n");
	txt.WriteString("# SKIP_UNKNOWN_CLASS, SKIP_UNKNOWN_METHOD, SKIP_UNKNOWN_PROPERTY, SKIP_UNKNOWN_FUNCTION, \n");
	txt.WriteString("# SKIP_UNINITIALIZED_VAR, SKIP_FUNCTION_ARGUMENT_MISMATCH, SKIP_ALL\n");
	txt.WriteString("# \n");
	std::vector<t4p::SuppressionRuleClass>::const_iterator rule;
	for (rule = Rules.begin(); rule != Rules.end(); ++rule) {
		wxString typeStr = RuleTypeString(rule->Type);
		wxString targetStr = t4p::IcuToWx(rule->Target);
		wxString locationStr;
		if (rule->Location.FileExists()) {
			locationStr = rule->Location.GetFullPath();
		} else if (rule->Location.DirExists()) {
			locationStr = rule->Location.GetPathWithSep();
		}

		txt.WriteString(typeStr);
		txt.WriteString(",");
		txt.WriteString(targetStr);
		txt.WriteString(",");
		txt.WriteString(locationStr);
		txt.WriteString("\n");
	}
	return true;
}

void t4p::LintSuppressionClass::Add(const t4p::SuppressionRuleClass& rule) {
	Rules.push_back(rule);
}

bool t4p::LintSuppressionClass::ShouldIgnore(const wxFileName& file, const UnicodeString& target,
		t4p::SuppressionRuleClass::Types type) const {
	bool ignore = false;
	std::vector<t4p::SuppressionRuleClass>::const_iterator rule;
	wxString fullPathToCheck = file.GetFullPath();

	for (rule = Rules.begin(); rule != Rules.end(); ++rule) {
		bool matchesType = false;
		bool matchesLocation = false;
		bool matchesTarget  = false;

		// account for the skip_all rule
		if (rule->Type == type || rule->Type == t4p::SuppressionRuleClass::SKIP_ALL) {
			matchesType = true;
		}

		if (matchesType) {
			// does the rule match the given file?
			// if the rule location is a dir, then is the file in the dir?
			wxString rulePath;
			if (rule->Location.IsDir()) {
				rulePath = rule->Location.GetPathWithSep();
			} else {
				rulePath = rule->Location.GetFullPath();
			}
			matchesLocation = fullPathToCheck.Find(rulePath) == 0;
		}

		// for the skip_all rule, we don't need to look at target
		if (matchesType && matchesLocation && rule->Type == t4p::SuppressionRuleClass::SKIP_ALL) {
			matchesTarget = true;
		}

		if (matchesType && matchesLocation && rule->Target.caseCompare(target, 0) == 0) {
			matchesTarget = true;
		}

		if (matchesType && matchesLocation && matchesTarget) {
			ignore = true;
			break;
		}
	}
	return ignore;
}

bool t4p::LintSuppressionClass::AddSkipAllRuleForDirectory(const wxFileName& dir) {
	bool isFound = false;
	wxString dirStr = dir.GetPathWithSep();
	std::vector<t4p::SuppressionRuleClass>::const_iterator rule = Rules.begin();
	while (rule != Rules.end() && !isFound) {
		if (rule->Location.IsDir()) {
			wxString location = rule->Location.GetPathWithSep();
			if (location == dirStr) {
				isFound = true;
			}
		}
		++rule;
	}

	if (!isFound) {
		t4p::SuppressionRuleClass newRule;
		newRule.SkipAllRule(dir);
		Add(newRule);
	}
	return !isFound;
}

bool t4p::LintSuppressionClass::RemoveRulesForDirectory(const wxFileName& dir) {
	std::vector<t4p::SuppressionRuleClass>::iterator rule = Rules.begin();
	wxString dirStr = dir.GetPathWithSep();
	int removeCount = 0;
	while (rule != Rules.end()) {
		bool isRemoved = false;
		if (rule->Location.IsDir()) {
			// what should happen to rules that the user manually created
			// that are in sub-drectories of the given dir? should we
			// remove those too? for now, we are not
			wxString location = rule->Location.GetPathWithSep();
			if (location == dirStr) {
				isRemoved = true;
				rule = Rules.erase(rule);
				removeCount++;
			}
		}

		if (!isRemoved) {
			++rule;
		}
	}
	return removeCount  > 0;
}
