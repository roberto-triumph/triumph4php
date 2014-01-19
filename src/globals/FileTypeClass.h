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
 
#ifndef __MVCEDITOR_FILETYPECLASS_H__
#define __MVCEDITOR_FILETYPECLASS_H__

#include <wx/string.h>
#include <vector>

namespace mvceditor
{

/**
 * This class holds the wildcards for all of the 
 * different file types that are treated specially by 
 * MVC Editor. There is one class total, not one class per
 * file type.
 */
class FileTypeClass {

public:

	/**
	 * Serialized file filters strings from the config
	 * these strings contain multitple wildcards, use
	 * the corresponding GetXXXFileExtensions()
	 * to get the wildcards in a list
	 */
	wxString PhpFileExtensionsString;
	wxString CssFileExtensionsString;
	wxString SqlFileExtensionsString;
	wxString JsFileExtensionsString;
	wxString ConfigFileExtensionsString;
	wxString CrontabFileExtensionsString;
	wxString YamlFileExtensionsString;
	wxString XmlFileExtensionsString;
	wxString RubyFileExtensionsString;
	wxString LuaFileExtensionsString;
	wxString MarkdownFileExtensionsString;
	wxString BashFileExtensionsString;
	wxString DiffFileExtensionsString;
	
	/**
	 * Serialized miscalleneous file filters string from the config
	 * basically any files that we want to open in the editor, like
	 * text files, twig files, json files, etc...
	 */
	wxString MiscFileExtensionsString;

	FileTypeClass();
	
	/**
	 * The methods below returns the list of wildcard file extensions for
	 * the specified file type, each wildcard on its own string
	 */
	 
	std::vector<wxString> GetPhpFileExtensions() const;
	std::vector<wxString> GetCssFileExtensions() const;
	std::vector<wxString> GetSqlFileExtensions() const;
	std::vector<wxString> GetJsFileExtensions() const;
	std::vector<wxString> GetConfigFileExtensions() const;
	std::vector<wxString> GetCrontabFileExtensions() const;
	std::vector<wxString> GetYamlFileExtensions() const;
	std::vector<wxString> GetRubyFileExtensions() const;
	std::vector<wxString> GetLuaFileExtensions() const;
	std::vector<wxString> GetMarkdownFileExtensions() const;
	std::vector<wxString> GetBashFileExtensions() const;
	std::vector<wxString> GetDiffFileExtensions() const;
	std::vector<wxString> GetMiscFileExtensions() const;
	
	/**
	 * Returns the all file extensions exception PHP file extensions
	 * @return std::vector<wxString> a copy of the file extensions
	 */
	std::vector<wxString> GetNonPhpFileExtensions() const;

	/**
	 * Returns the all file extensions that we want the editor to open
	 *   php, css, sql, etc...
	 * @return std::vector<wxString> a copy of the file extensions
	 */
	std::vector<wxString> GetAllSourceFileExtensions() const;
	
	/**
	 * @return all of the file extensions, concatenated with semicolon
	 */
	wxString GetAllSourceFileExtensionsString() const;

	/**
	 * The methods below return TRUE 
	 * @return TRUE if given full path is a file of the specified
	 * type, as determined only by the appropriate file extensions 
	 * wilcard. For example, HasAPhpExtension returns TRUE if
	 * fullPath has one of the extensions configure in PhpExtensionsString
	 */
	 
	bool HasAPhpExtension(const wxString& fullPath) const;
	bool HasASqlExtension(const wxString& fullPath) const;
	bool HasAJsExtension(const wxString& fullPath) const;
	bool HasACssExtension(const wxString& fullPath) const;
	bool HasAConfigExtension(const wxString& fullPath) const;
	bool HasACrontabExtension(const wxString& fullPath) const;
	bool HasAYamlExtension(const wxString& fullPath) const;
	bool HasAXmlExtension(const wxString& fullPath) const;
	bool HasARubyExtension(const wxString& fullPath) const;
	bool HasALuaExtension(const wxString& fullPath) const;
	bool HasAMarkdownExtension(const wxString& fullPath) const;
	bool HasABashExtension(const wxString& fullPath) const;
	bool HasADiffExtension(const wxString& fullPath) const;
	bool HasAMiscExtension(const wxString& fullPath) const;

};

}

#endif // __MVCEDITOR_FILETYPECLASS_H__
