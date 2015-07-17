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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <language_php/ConfigTagClass.h>
#include <globals/String.h>
#include <globals/Errors.h>

t4p::ConfigTagClass::ConfigTagClass() 
	: Label()
	, ConfigFileName() {

}	

t4p::ConfigTagClass::ConfigTagClass(const t4p::ConfigTagClass& src)
	: Label()
	, ConfigFileName() {
	Copy(src);
}

void t4p::ConfigTagClass::Copy(const t4p::ConfigTagClass& src) {
	
	// make sure wxString copy in thread-safe manner
	Label = src.Label.c_str();
	ConfigFileName.Assign(src.ConfigFileName.GetFullPath().c_str());
}

t4p::ConfigTagClass& t4p::ConfigTagClass::operator=(const t4p::ConfigTagClass& src) {
	Copy(src);
	return *this;
}

wxString t4p::ConfigTagClass::MenuLabel() const {
	wxString label(Label);
	label.Replace(wxT("&"), wxT("&&"));
	return label;
}

t4p::ConfigTagFinderClass::ConfigTagFinderClass(soci::session& session)
: SqliteFinderClass(session) {

}

std::vector<t4p::ConfigTagClass> t4p::ConfigTagFinderClass::All(const std::vector<wxFileName>& sourceDirectories) {
	std::vector<t4p::ConfigTagClass> allConfigTags;
	if (sourceDirectories.empty()) {
		return allConfigTags;
	}
	
	std::vector<std::string> stdSourceDirectories;
	
	std::string label,
		fullPath;
	std::string sql = "SELECT label, full_path FROM config_tags JOIN sources ON(sources.source_id = config_tags.source_id) ";
	sql += "WHERE directory IN(";
	for (size_t i = 0; i < sourceDirectories.size(); ++i) {
		stdSourceDirectories.push_back(t4p::WxToChar(sourceDirectories[i].GetPathWithSep()));
		if (0 == i) {
			sql += "?";
		}
		else {
			sql += ",?";
		}
	}
	sql += ")";
	
	try {
		soci::statement stmt = Session.prepare << sql;
		stmt.exchange(soci::into(label));
		stmt.exchange(soci::into(fullPath)); 
		for (size_t i = 0; i < stdSourceDirectories.size(); ++i) {
			stmt.exchange(soci::use(stdSourceDirectories[i]));
		}
		stmt.define_and_bind();
		if (stmt.execute(true)) {
			do {
				t4p::ConfigTagClass configTag;
				configTag.Label = t4p::CharToWx(label.c_str());
				configTag.ConfigFileName.Assign(t4p::CharToWx(fullPath.c_str()));

				allConfigTags.push_back(configTag);

			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
		wxString msg = t4p::CharToWx(e.what());
		wxUnusedVar(msg);
		t4p::EditorLogError(t4p::ERR_TAG_READ, msg);
	}
	return allConfigTags;
}
