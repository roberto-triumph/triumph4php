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
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef T4P_DOCCOMMENTVIEWCLASS_H__
#define T4P_DOCCOMMENTVIEWCLASS_H__

#include <views/FeatureViewClass.h>
#include <views/wxformbuilder/DocCommentFeatureForms.h>
#include <wx/hyperlink.h>

namespace t4p {

// forward declaration, defined in another file
class DocCommentFeatureClass;
class CodeControlClass;

class DocCommentViewClass : public t4p::FeatureViewClass {

public:

	DocCommentViewClass(t4p::DocCommentFeatureClass& feature);

	void AddEditMenuItems(wxMenu* editMenu);

private:

	void OnShowDocComment(wxCommandEvent& event);

	void OnMotionAlt(wxCommandEvent& event);

	void ShowDocComment(t4p::CodeControlClass* ctrl, int pos);

	void OnPhpSiteLinkClick(wxHyperlinkEvent& event);
	
	t4p::DocCommentFeatureClass& Feature;

	DECLARE_EVENT_TABLE()
};

/**
 * the panel that shows a PHP doc comment.
 */
class DocCommentPanelClass : public DocCommentPanelGeneratedClass {

public:

	DocCommentPanelClass(wxWindow* parent, t4p::DocCommentViewClass& featureView);

	void SetText(const wxString& text);
	
	void EnablePhpSiteLink(const wxString& functionName);

protected:

	void OnClose(wxHyperlinkEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	void OnPhpSiteDocs(wxHyperlinkEvent& event);
	void DoDestroy();

	t4p::DocCommentViewClass& FeatureView;
};


}

#endif
