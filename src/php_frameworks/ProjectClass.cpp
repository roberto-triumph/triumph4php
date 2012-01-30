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
#include <php_frameworks/ProjectClass.h>
#include <wx/tokenzr.h>
#include <vector>

mvceditor::ProjectOptionsClass::ProjectOptionsClass()  
	: RootPath() {
}
	
mvceditor::ProjectOptionsClass::ProjectOptionsClass(const ProjectOptionsClass& other) { 
	RootPath = other.RootPath; 
}

mvceditor::ProjectClass::ProjectClass(const mvceditor::ProjectOptionsClass& options)
	: Options(options)
	, Databases()
	, PhpFileFilters()
	, CssFileFilters()
	, SqlFileFilters() {
}

wxString  mvceditor::ProjectClass::GetRootPath() const { 
	return Options.RootPath; 
}

mvceditor::SqlResourceFinderClass* mvceditor::ProjectClass::GetSqlResourceFinder() {
	return &SqlResourceFinder;
}

std::vector<mvceditor::DatabaseInfoClass> mvceditor::ProjectClass::DatabaseInfo() const {
	return Databases;
}

void mvceditor::ProjectClass::PushDatabaseInfo(const mvceditor::DatabaseInfoClass& info) {
	Databases.push_back(info);
}

wxString mvceditor::ProjectClass::GetPhpFileExtensionsString() const {
	wxString all;
	for (size_t i = 0; i < PhpFileFilters.size(); ++i) {
		all.Append(PhpFileFilters[i]);
		if (i < (PhpFileFilters.size() - 1)) {
			all.Append(wxT(';'));
		}
	}
	return all;
}

std::vector<wxString> mvceditor::ProjectClass::GetPhpFileExtensions() const {
	std::vector<wxString> cpy;
	for (size_t i = 0; i < PhpFileFilters.size(); ++i) {
		wxString s;
		s.Append(PhpFileFilters[i]);
		cpy.push_back(s);
	}
	return cpy;
}

void mvceditor::ProjectClass::SetPhpFileExtensionsString(wxString wildcardString) {
	wxStringTokenizer tokenizer(wildcardString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		PhpFileFilters.push_back(wildcard);
	}
}

wxString mvceditor::ProjectClass::GetCssFileExtensionsString() const {
	wxString all;
	for (size_t i = 0; i < CssFileFilters.size(); ++i) {
		all.Append(CssFileFilters[i]);
		if (i < (CssFileFilters.size() - 1)) {
			all.Append(wxT(';'));
		}
	}
	return all;
}

std::vector<wxString> mvceditor::ProjectClass::GetCssFileExtensions() const {
	std::vector<wxString> cpy;
	for (size_t i = 0; i < CssFileFilters.size(); ++i) {
		wxString s;
		s.Append(CssFileFilters[i]);
		cpy.push_back(s);
	}
	return cpy;
}

void mvceditor::ProjectClass::SetCssFileExtensionsString(wxString wildcardString) {
	wxStringTokenizer tokenizer(wildcardString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		CssFileFilters.push_back(wildcard);
	}
}

wxString mvceditor::ProjectClass::GetSqlFileExtensionsString() const {
	wxString all;
	for (size_t i = 0; i < SqlFileFilters.size(); ++i) {
		all.Append(SqlFileFilters[i]);
		if (i < (SqlFileFilters.size() - 1)) {
			all.Append(wxT(';'));
		}
	}
	return all;
}
std::vector<wxString> mvceditor::ProjectClass::GetSqlFileExtensions() const {
	std::vector<wxString> cpy;
	for (size_t i = 0; i <SqlFileFilters.size(); ++i) {
		wxString s;
		s.Append(SqlFileFilters[i]);
		cpy.push_back(s);
	}
	return cpy;
}

void mvceditor::ProjectClass::SetSqlFileExtensionsString(wxString wildcardString) {
	wxStringTokenizer tokenizer(wildcardString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		SqlFileFilters.push_back(wildcard);
	}
}