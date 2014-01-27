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
 
void mvceditor::FillWithFileType(wxImageList& imgList) {
	imgList.Add(mvceditor::IconImageAsset(wxT("document-php")));
	imgList.Add(mvceditor::IconImageAsset(wxT("document-sql")));
	imgList.Add(mvceditor::IconImageAsset(wxT("document-css")));
	imgList.Add(mvceditor::IconImageAsset(wxT("document-javascript")));
	imgList.Add(mvceditor::IconImageAsset(wxT("document-config")));
	imgList.Add(mvceditor::IconImageAsset(wxT("document-config")));
	imgList.Add(mvceditor::IconImageAsset(wxT("document-yaml")));
	imgList.Add(mvceditor::IconImageAsset(wxT("document-xml")));
	imgList.Add(mvceditor::IconImageAsset(wxT("document-ruby")));
	imgList.Add(mvceditor::IconImageAsset(wxT("document-lua")));
	imgList.Add(mvceditor::IconImageAsset(wxT("document-markdown")));
	imgList.Add(mvceditor::IconImageAsset(wxT("document-bash")));
	imgList.Add(mvceditor::IconImageAsset(wxT("document-diff")));
	imgList.Add(mvceditor::IconImageAsset(wxT("document-text")));
	imgList.Add(mvceditor::IconImageAsset(wxT("document-blank")));
}
