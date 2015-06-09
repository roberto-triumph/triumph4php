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
#include <widgets/FileTypeImageList.h>
#include <globals/Assets.h>
#include <globals/FileTypeClass.h>
 
void t4p::FileTypeImageList(wxImageList& imgList) {
	imgList.Add(t4p::BitmapImageAsset(wxT("document-php")));
	imgList.Add(t4p::BitmapImageAsset(wxT("document-sql")));
	imgList.Add(t4p::BitmapImageAsset(wxT("document-css")));
	imgList.Add(t4p::BitmapImageAsset(wxT("document-javascript")));
	imgList.Add(t4p::BitmapImageAsset(wxT("document-config")));
	imgList.Add(t4p::BitmapImageAsset(wxT("document-config")));
	imgList.Add(t4p::BitmapImageAsset(wxT("document-yaml")));
	imgList.Add(t4p::BitmapImageAsset(wxT("document-xml")));
	imgList.Add(t4p::BitmapImageAsset(wxT("document-ruby")));
	imgList.Add(t4p::BitmapImageAsset(wxT("document-lua")));
	imgList.Add(t4p::BitmapImageAsset(wxT("document-markdown")));
	imgList.Add(t4p::BitmapImageAsset(wxT("document-bash")));
	imgList.Add(t4p::BitmapImageAsset(wxT("document-diff")));
	imgList.Add(t4p::BitmapImageAsset(wxT("document-text")));
	imgList.Add(t4p::BitmapImageAsset(wxT("document-blank")));
}

int t4p::FileTypeImageId(const t4p::FileTypeClass& fileTypes, const wxFileName& fileName) {
	wxString fullPath = fileName.GetFullPath();
	if (fileTypes.HasAPhpExtension(fullPath)) {
		return t4p::IMGLIST_PHP;
	}
	if (fileTypes.HasASqlExtension(fullPath)) {
		return t4p::IMGLIST_SQL;
	} 
	if (fileTypes.HasACssExtension(fullPath)) {
		return t4p::IMGLIST_CSS;
	}
	if (fileTypes.HasAJsExtension(fullPath)) {
		return t4p::IMGLIST_JS;
	}
	if (fileTypes.HasAConfigExtension(fullPath)) {
		return t4p::IMGLIST_CONFIG;
	}
	if (fileTypes.HasACrontabExtension(fullPath)) {
		return t4p::IMGLIST_CRONTAB;
	} 
	if (fileTypes.HasAYamlExtension(fullPath)) {
		return t4p::IMGLIST_YAML;
	}
	if (fileTypes.HasAXmlExtension(fullPath)) {
		return t4p::IMGLIST_XML;
	}
	if (fileTypes.HasARubyExtension(fullPath)) {
		return t4p::IMGLIST_RUBY;
	}
	if (fileTypes.HasALuaExtension(fullPath)) {
		return t4p::IMGLIST_LUA;
	}
	if (fileTypes.HasAMarkdownExtension(fullPath)) {
		return t4p::IMGLIST_MARKDOWN;
	} 
	if (fileTypes.HasABashExtension(fullPath)) {
		return t4p::IMGLIST_BASH;
	}
	if (fileTypes.HasADiffExtension(fullPath)) {
		return t4p::IMGLIST_DIFF;
	}
	if (fileTypes.HasAMiscExtension(fullPath)) {
		return t4p::IMGLIST_MISC;
	}
	return t4p::IMGLIST_NONE;
}

int t4p::FileTypeImageIdFromType(t4p::FileType type) {
	switch (type) {
	case t4p::FILE_TYPE_PHP:
		return t4p::IMGLIST_PHP;
	case t4p::FILE_TYPE_SQL:
		return t4p::IMGLIST_SQL;
	case t4p::FILE_TYPE_CSS:
		return t4p::IMGLIST_CSS;
	case t4p::FILE_TYPE_JS:
		return t4p::IMGLIST_JS;
	case t4p::FILE_TYPE_CONFIG:
		return t4p::IMGLIST_CONFIG;
	case t4p::FILE_TYPE_CRONTAB:
		return t4p::IMGLIST_CRONTAB;
	case t4p::FILE_TYPE_YAML:
		return t4p::IMGLIST_YAML;
	case t4p::FILE_TYPE_XML:
		return t4p::IMGLIST_XML;
	case t4p::FILE_TYPE_RUBY:
		return t4p::IMGLIST_RUBY;
	case t4p::FILE_TYPE_LUA:
		return t4p::IMGLIST_LUA;
	case t4p::FILE_TYPE_MARKDOWN:
		return t4p::IMGLIST_MARKDOWN;
	case t4p::FILE_TYPE_BASH:
		return t4p::IMGLIST_BASH;
	case t4p::FILE_TYPE_DIFF:
		return t4p::IMGLIST_DIFF;
	case t4p::FILE_TYPE_TEXT:
		return t4p::IMGLIST_MISC;
	default:
		return t4p::IMGLIST_NONE;
	}
}
