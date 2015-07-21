/**
 * @copyright  2014 Roberto Perpuly
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

#include "globals/FileTypeClass.h"
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <vector>

/**
 * @return exploded list of wildcards, split by semicolon
 */
static std::vector<wxString> Explode(const wxString& extensionsString) {
	std::vector<wxString> wildcards;
	wxStringTokenizer tokenizer(extensionsString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		wildcards.push_back(wildcard);
	}
	return wildcards;
}

/**
 * Checks the given file against the given set of wildcards.
 * @return TRUE if the fullPath matches at least one of
 *          the wildcards.
 */
static bool HasExtension(const wxString& fullPath, const wxString& extensionsString) {
	std::vector<wxString> wildcards = Explode(extensionsString);
	bool found = false;
	wxFileName file(fullPath);
	wxString nameOnly = file.IsOk() ? file.GetFullName() : fullPath;
	for (size_t i = 0; i < wildcards.size(); ++i) {
		wxString wildcard = wildcards[i];
		if (wxMatchWild(wildcard, nameOnly, false)) {
			found = true;
			break;
		}
	}
	return found;
}

t4p::FileTypeClass::FileTypeClass()
: PhpFileExtensionsString(wxT("*.php;*.phtml;*.html;*.php3"))
, CssFileExtensionsString(wxT("*.css"))
, SqlFileExtensionsString(wxT("*.sql"))
, JsFileExtensionsString(wxT("*.js"))
, ConfigFileExtensionsString(wxT("*.conf;*.ini;.htaccess"))
, CrontabFileExtensionsString(wxT("crontab"))
, YamlFileExtensionsString(wxT("*.yml"))
, XmlFileExtensionsString(wxT("*.xml"))
, RubyFileExtensionsString(wxT("*.rb;Capfile"))
, LuaFileExtensionsString(wxT("*.lua"))
, MarkdownFileExtensionsString(wxT("*.md"))
, BashFileExtensionsString(wxT("*.sh"))
, DiffFileExtensionsString(wxT("*.diff;*.patch"))
, MiscFileExtensionsString(wxT("*.json;*.twig;*.txt")) {
}

t4p::FileTypeClass::FileTypeClass(const t4p::FileTypeClass& src)
: PhpFileExtensionsString(wxT("*.php;*.phtml;*.html;*.php3"))
, CssFileExtensionsString(wxT("*.css"))
, SqlFileExtensionsString(wxT("*.sql"))
, JsFileExtensionsString(wxT("*.js"))
, ConfigFileExtensionsString(wxT("*.conf;*.ini;.htaccess"))
, CrontabFileExtensionsString(wxT("crontab"))
, YamlFileExtensionsString(wxT("*.yml"))
, XmlFileExtensionsString(wxT("*.xml"))
, RubyFileExtensionsString(wxT("*.rb;Capfile"))
, LuaFileExtensionsString(wxT("*.lua"))
, MarkdownFileExtensionsString(wxT("*.md"))
, BashFileExtensionsString(wxT("*.sh"))
, DiffFileExtensionsString(wxT("*.diff;*.patch"))
, MiscFileExtensionsString(wxT("*.json;*.twig;*.txt")) {
	Copy(src);
}

t4p::FileTypeClass& t4p::FileTypeClass::operator=(const t4p::FileTypeClass& src) {
	Copy(src);
	return *this;
}

void t4p::FileTypeClass::Copy(const t4p::FileTypeClass& src) {
	// copy in a thread-safe way
	CssFileExtensionsString        = CssFileExtensionsString.c_str();
	PhpFileExtensionsString        = PhpFileExtensionsString.c_str();
	SqlFileExtensionsString        = SqlFileExtensionsString.c_str();
	JsFileExtensionsString         = JsFileExtensionsString.c_str();
	ConfigFileExtensionsString     = ConfigFileExtensionsString.c_str();
	CrontabFileExtensionsString    = CrontabFileExtensionsString.c_str();
	YamlFileExtensionsString       = YamlFileExtensionsString.c_str();
	XmlFileExtensionsString        = XmlFileExtensionsString.c_str();
	RubyFileExtensionsString       = RubyFileExtensionsString.c_str();
	LuaFileExtensionsString        = LuaFileExtensionsString.c_str();
	MarkdownFileExtensionsString   = MarkdownFileExtensionsString.c_str();
	BashFileExtensionsString       = BashFileExtensionsString.c_str();
	DiffFileExtensionsString       = DiffFileExtensionsString.c_str();
}

std::vector<wxString> t4p::FileTypeClass::GetPhpFileExtensions() const {
	return Explode(PhpFileExtensionsString);
}

std::vector<wxString> t4p::FileTypeClass::GetCssFileExtensions() const {
	return Explode(CssFileExtensionsString);
}

std::vector<wxString> t4p::FileTypeClass::GetSqlFileExtensions() const {
	return Explode(SqlFileExtensionsString);
}

std::vector<wxString> t4p::FileTypeClass::GetJsFileExtensions() const {
	return Explode(JsFileExtensionsString);
}

std::vector<wxString> t4p::FileTypeClass::GetConfigFileExtensions() const {
	return Explode(ConfigFileExtensionsString);
}

std::vector<wxString> t4p::FileTypeClass::GetCrontabFileExtensions() const {
	return Explode(CrontabFileExtensionsString);
}

std::vector<wxString> t4p::FileTypeClass::GetYamlFileExtensions() const {
	return Explode(YamlFileExtensionsString);
}

std::vector<wxString> t4p::FileTypeClass::GetRubyFileExtensions() const {
	return Explode(RubyFileExtensionsString);
}

std::vector<wxString> t4p::FileTypeClass::GetLuaFileExtensions() const {
	return Explode(LuaFileExtensionsString);
}

std::vector<wxString> t4p::FileTypeClass::GetMarkdownFileExtensions() const {
	return Explode(MarkdownFileExtensionsString);
}

std::vector<wxString> t4p::FileTypeClass::GetBashFileExtensions() const {
	return Explode(BashFileExtensionsString);
}

std::vector<wxString> t4p::FileTypeClass::GetDiffFileExtensions() const {
	return Explode(DiffFileExtensionsString);
}

std::vector<wxString> t4p::FileTypeClass::GetMiscFileExtensions() const {
	return Explode(MiscFileExtensionsString);
}

std::vector<wxString> t4p::FileTypeClass::GetNonPhpFileExtensions() const {
	wxString allNotPhpWildcards;
	allNotPhpWildcards += CssFileExtensionsString;
	allNotPhpWildcards += wxT(";") + SqlFileExtensionsString;
	allNotPhpWildcards += wxT(";") + JsFileExtensionsString;
	allNotPhpWildcards += wxT(";") + ConfigFileExtensionsString;
	allNotPhpWildcards += wxT(";") + CrontabFileExtensionsString;
	allNotPhpWildcards += wxT(";") + YamlFileExtensionsString;
	allNotPhpWildcards += wxT(";") + XmlFileExtensionsString;
	allNotPhpWildcards += wxT(";") + RubyFileExtensionsString;
	allNotPhpWildcards += wxT(";") + LuaFileExtensionsString;
	allNotPhpWildcards += wxT(";") + MarkdownFileExtensionsString;
	allNotPhpWildcards += wxT(";") + BashFileExtensionsString;
	allNotPhpWildcards += wxT(";") + DiffFileExtensionsString;
	allNotPhpWildcards += wxT(";") + MiscFileExtensionsString;
	return Explode(allNotPhpWildcards);
}

std::vector<wxString> t4p::FileTypeClass::GetAllSourceFileExtensions() const {
	return Explode(GetAllSourceFileExtensionsString());
}

wxString t4p::FileTypeClass::GetAllSourceFileExtensionsString() const {
	wxString allWildcards;
	allWildcards += PhpFileExtensionsString;
	allWildcards += wxT(";") + CssFileExtensionsString;
	allWildcards += wxT(";") + SqlFileExtensionsString;
	allWildcards += wxT(";") + JsFileExtensionsString;
	allWildcards += wxT(";") + ConfigFileExtensionsString;
	allWildcards += wxT(";") + CrontabFileExtensionsString;
	allWildcards += wxT(";") + YamlFileExtensionsString;
	allWildcards += wxT(";") + XmlFileExtensionsString;
	allWildcards += wxT(";") + RubyFileExtensionsString;
	allWildcards += wxT(";") + LuaFileExtensionsString;
	allWildcards += wxT(";") + MarkdownFileExtensionsString;
	allWildcards += wxT(";") + BashFileExtensionsString;
	allWildcards += wxT(";") + DiffFileExtensionsString;
	allWildcards += wxT(";") + MiscFileExtensionsString;
	return allWildcards;
}

bool t4p::FileTypeClass::HasAPhpExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, PhpFileExtensionsString);
}

bool t4p::FileTypeClass::HasABashExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, BashFileExtensionsString);
}

bool t4p::FileTypeClass::HasAConfigExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, ConfigFileExtensionsString);
}

bool t4p::FileTypeClass::HasACrontabExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, CrontabFileExtensionsString);
}

bool t4p::FileTypeClass::HasACssExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, CssFileExtensionsString);
}

bool t4p::FileTypeClass::HasADiffExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, DiffFileExtensionsString);
}

bool t4p::FileTypeClass::HasAJsExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, JsFileExtensionsString);
}

bool t4p::FileTypeClass::HasALuaExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, LuaFileExtensionsString);
}

bool t4p::FileTypeClass::HasAMarkdownExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, MarkdownFileExtensionsString);
}

bool t4p::FileTypeClass::HasAMiscExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, MiscFileExtensionsString);
}

bool t4p::FileTypeClass::HasARubyExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, RubyFileExtensionsString);
}

bool t4p::FileTypeClass::HasASqlExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, SqlFileExtensionsString);
}

bool t4p::FileTypeClass::HasAXmlExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, XmlFileExtensionsString);
}

bool t4p::FileTypeClass::HasAYamlExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, YamlFileExtensionsString);
}

bool t4p::FileTypeClass::HasAnyExtension(const wxString& fullPath) const {
	return HasAPhpExtension(fullPath)
		|| HasASqlExtension(fullPath)
		|| HasAJsExtension(fullPath)
		|| HasACssExtension(fullPath)
		|| HasAConfigExtension(fullPath)
		|| HasACrontabExtension(fullPath)
		|| HasAYamlExtension(fullPath)
		|| HasAXmlExtension(fullPath)
		|| HasARubyExtension(fullPath)
		|| HasALuaExtension(fullPath)
		|| HasAMarkdownExtension(fullPath)
		|| HasABashExtension(fullPath)
		|| HasADiffExtension(fullPath)
		|| HasAMiscExtension(fullPath);
}

