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
#include <globals/ConfigTagClass.h>
#include <globals/String.h>
#include <globals/Errors.h>

mvceditor::ConfigTagClass::ConfigTagClass() 
	: Label()
	, ConfigFileName() {

}	

mvceditor::ConfigTagClass::ConfigTagClass(const mvceditor::ConfigTagClass& src)
	: Label()
	, ConfigFileName() {
	Copy(src);
}

void mvceditor::ConfigTagClass::Copy(const mvceditor::ConfigTagClass& src) {
	
	// make sure wxString copy in thread-safe manner
	Label = src.Label.c_str();
	ConfigFileName.Assign(src.ConfigFileName.GetFullPath().c_str());
}

mvceditor::ConfigTagClass& mvceditor::ConfigTagClass::operator=(const mvceditor::ConfigTagClass& src) {
	Copy(src);
	return *this;
}

wxString mvceditor::ConfigTagClass::MenuLabel() const {
	wxString label(Label);
	label.Replace(wxT("&"), wxT("&&"));
	return label;
}

mvceditor::ConfigTagFinderClass::ConfigTagFinderClass()
: SqliteFinderClass() {

}

std::vector<mvceditor::ConfigTagClass> mvceditor::ConfigTagFinderClass::All(const std::vector<wxFileName>& sourceDirectories) {
	std::vector<mvceditor::ConfigTagClass> allConfigTags;
	if (sourceDirectories.empty()) {
		return allConfigTags;
	}
	
	std::vector<std::string> stdSourceDirectories;
	
	std::string label,
		fullPath;
	std::string sql = "SELECT label, full_path FROM config_tags JOIN sources ON(sources.source_id = config_tags.source_id) ";
	sql += "WHERE directory IN(";
	for (size_t i = 0; i < sourceDirectories.size(); ++i) {
		stdSourceDirectories.push_back(mvceditor::WxToChar(sourceDirectories[i].GetPathWithSep()));
		if (0 == i) {
			sql += "?";
		}
		else {
			sql += ",?";
		}
	}
	sql += ")";
	
	try {
		soci::statement stmt = Session->prepare << sql;
		stmt.exchange(soci::into(label));
		stmt.exchange(soci::into(fullPath)); 
		for (size_t i = 0; i < stdSourceDirectories.size(); ++i) {
			stmt.exchange(soci::use(stdSourceDirectories[i]));
		}
		if (stmt.execute(true)) {
			do {
				mvceditor::ConfigTagClass configTag;
				configTag.Label = mvceditor::CharToWx(label.c_str());
				configTag.ConfigFileName.Assign(mvceditor::CharToWx(fullPath.c_str()));

				allConfigTags.push_back(configTag);

			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		mvceditor::EditorLogError(mvceditor::WARNING_OTHER, msg);
	}
	return allConfigTags;
}
