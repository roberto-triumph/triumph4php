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
#include <language_php/TemplateFileTagClass.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <globals/Sqlite.h>
#include <wx/tokenzr.h>
#include <string>
#include <vector>

t4p::TemplateFileTagClass::TemplateFileTagClass()
	:  FullPath()
	, Variables() {
}

t4p::TemplateFileTagClass::TemplateFileTagClass(const t4p::TemplateFileTagClass& src)
	: FullPath()
	, Variables() {
	Copy(src);
}

void t4p::TemplateFileTagClass::Copy(const t4p::TemplateFileTagClass& src) {
	// copy is thread-safe, wxString needs to be cloned
	FullPath = src.FullPath.c_str();
	Variables.clear();
	for (size_t i = 0; i < src.Variables.size(); ++i) {
		Variables.push_back(src.Variables[i].c_str());
	}
}

t4p::TemplateFileTagClass& t4p::TemplateFileTagClass::operator=(const t4p::TemplateFileTagClass& src) {
	Copy(src);
	return *this;
}

void t4p::TemplateFileTagClass::Init(const wxString& fullPath, const std::vector<wxString>& variables) {
	FullPath = fullPath;
	Variables = variables;
}

t4p::TemplateFileTagFinderClass::TemplateFileTagFinderClass()
	: Session()
	, IsInitialized(false) {
}

void t4p::TemplateFileTagFinderClass::Init(const wxFileName& detectorDbFileName) {
	IsInitialized = false;
	try {
		Session.close();
		std::string stdDbName = t4p::WxToChar(detectorDbFileName.GetFullPath());

		// we should be able to open this since it has been created by
		// the DetectorCacheDbVersionActionClass
		Session.open(*soci::factory_sqlite3(), stdDbName);
		IsInitialized = true;
	} catch (std::exception& e) {
		wxString msg = t4p::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
}

std::vector<t4p::TemplateFileTagClass> t4p::TemplateFileTagFinderClass::All() {
	std::vector<t4p::TemplateFileTagClass> templates;
	if (!IsInitialized) {
		return templates;
	}

	std::string fullPath,
				variables;
	soci::statement stmt = (Session.prepare <<
		"SELECT full_path, variables FROM template_file_tags;",
		soci::into(fullPath), soci::into(variables)
	);
	if (stmt.execute(true)) {
		do {
			t4p::TemplateFileTagClass templateFile;
			templateFile.FullPath = t4p::CharToWx(fullPath.c_str());
			wxString wxVariables = t4p::CharToWx(variables.c_str());
			wxStringTokenizer tok(wxVariables, wxT(","));
			while (tok.HasMoreTokens()) {
				templateFile.Variables.push_back(tok.NextToken());
			}
			templates.push_back(templateFile);
		} while (stmt.fetch());
	}
	return templates;
}
