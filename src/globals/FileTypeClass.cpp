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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
 
#include <globals/FileTypeClass.h>
#include <wx/tokenzr.h>
#include <wx/filefn.h> 

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
	for (size_t i = 0; i < wildcards.size(); ++i) {
		if (wxMatchWild(wildcards[i], fullPath)) {
			found = true;
			break;
		}
	}
	return found;
}
	
mvceditor::FileTypeClass::FileTypeClass()
: PhpFileExtensionsString(wxT("*.php"))
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
, MiscFileExtensionsString(wxT("*.json;*.twig;*.txt"))
{
}

std::vector<wxString> mvceditor::FileTypeClass::GetPhpFileExtensions() const {
	return Explode(PhpFileExtensionsString);
}

std::vector<wxString> mvceditor::FileTypeClass::GetCssFileExtensions() const {
	return Explode(CssFileExtensionsString);
}

std::vector<wxString> mvceditor::FileTypeClass::GetSqlFileExtensions() const {
	return Explode(SqlFileExtensionsString);
}

std::vector<wxString> mvceditor::FileTypeClass::GetJsFileExtensions() const {
	return Explode(JsFileExtensionsString);
}

std::vector<wxString> mvceditor::FileTypeClass::GetConfigFileExtensions() const {
	return Explode(ConfigFileExtensionsString);
}

std::vector<wxString> mvceditor::FileTypeClass::GetCrontabFileExtensions() const {
	return Explode(CrontabFileExtensionsString);
}

std::vector<wxString> mvceditor::FileTypeClass::GetYamlFileExtensions() const {
	return Explode(YamlFileExtensionsString);
}

std::vector<wxString> mvceditor::FileTypeClass::GetRubyFileExtensions() const {
	return Explode(RubyFileExtensionsString);
}

std::vector<wxString> mvceditor::FileTypeClass::GetLuaFileExtensions() const {
	return Explode(LuaFileExtensionsString);
}

std::vector<wxString> mvceditor::FileTypeClass::GetMarkdownFileExtensions() const {
	return Explode(MarkdownFileExtensionsString);
}

std::vector<wxString> mvceditor::FileTypeClass::GetBashFileExtensions() const {
	return Explode(BashFileExtensionsString);
}

std::vector<wxString> mvceditor::FileTypeClass::GetDiffFileExtensions() const {
	return Explode(DiffFileExtensionsString);
}

std::vector<wxString> mvceditor::FileTypeClass::GetMiscFileExtensions() const {
	return Explode(MiscFileExtensionsString);
}

std::vector<wxString> mvceditor::FileTypeClass::GetNonPhpFileExtensions() const {
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

std::vector<wxString> mvceditor::FileTypeClass::GetAllSourceFileExtensions() const {
	return Explode(GetAllSourceFileExtensionsString());
}

wxString mvceditor::FileTypeClass::GetAllSourceFileExtensionsString() const {
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

bool mvceditor::FileTypeClass::HasAPhpExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, PhpFileExtensionsString);
}

bool mvceditor::FileTypeClass::HasABashExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, BashFileExtensionsString);
}

bool mvceditor::FileTypeClass::HasAConfigExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, ConfigFileExtensionsString);
}

bool mvceditor::FileTypeClass::HasACrontabExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, CrontabFileExtensionsString);
}

bool mvceditor::FileTypeClass::HasACssExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, CssFileExtensionsString);
}

bool mvceditor::FileTypeClass::HasADiffExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, DiffFileExtensionsString);
}

bool mvceditor::FileTypeClass::HasAJsExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, JsFileExtensionsString);
}

bool mvceditor::FileTypeClass::HasALuaExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, LuaFileExtensionsString);
}

bool mvceditor::FileTypeClass::HasAMarkdownExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, MarkdownFileExtensionsString);
}

bool mvceditor::FileTypeClass::HasAMiscExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, MiscFileExtensionsString);
}

bool mvceditor::FileTypeClass::HasARubyExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, RubyFileExtensionsString);
}

bool mvceditor::FileTypeClass::HasASqlExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, SqlFileExtensionsString);
}

bool mvceditor::FileTypeClass::HasAXmlExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, XmlFileExtensionsString);
}

bool mvceditor::FileTypeClass::HasAYamlExtension(const wxString& fullPath) const {
	return HasExtension(fullPath, YamlFileExtensionsString);
}


