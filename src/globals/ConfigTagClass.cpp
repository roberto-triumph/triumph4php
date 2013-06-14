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

std::vector<mvceditor::ConfigTagClass> mvceditor::ConfigTagFinderClass::All() {
	std::vector<mvceditor::ConfigTagClass> allConfigTags;
	std::vector<soci::session*>::iterator session;
	std::string label,
		fullPath;
	try {
		soci::statement stmt = (Session->prepare <<
			"SELECT label, full_path FROM config_tags",
			soci::into(label), soci::into(fullPath)
		);
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
