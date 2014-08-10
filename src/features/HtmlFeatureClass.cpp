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
#include <features/HtmlFeatureClass.h>

static bool InCssCommentOrStringStyle(wxStyledTextCtrl* ctrl, int posToCheck) {
	int style = ctrl->GetStyleAt(posToCheck);

	// dont match braces inside strings or comments.
	return wxSTC_CSS_COMMENT == style || wxSTC_CSS_DOUBLESTRING == style || wxSTC_CSS_SINGLESTRING == style;
}

t4p::HtmlCodeCompletionProviderClass::HtmlCodeCompletionProviderClass()
: CodeCompletionProviderClass() {
	
}

bool t4p::HtmlCodeCompletionProviderClass::DoesSupport(t4p::FileType type) {
	
	// not yet implemented
	return false;
}

void t4p::HtmlCodeCompletionProviderClass::Provide(t4p::CodeControlClass* ctrl, std::vector<t4p::CodeCompletionItemClass>& suggestions, wxString& completeStatus) {
	
}


t4p::CssBraceMatchStylerClass::CssBraceMatchStylerClass()
: BraceMatchStylerClass() {
	
}

bool t4p::CssBraceMatchStylerClass::DoesSupport(t4p::FileType type) {
	return t4p::FILE_TYPE_CSS == type;
}

void t4p::CssBraceMatchStylerClass::Style(t4p::CodeControlClass* ctrl, int posToCheck) {
	if (!InCssCommentOrStringStyle(ctrl, posToCheck)) {
		wxChar c1 = ctrl->GetCharAt(posToCheck),
		            c2 = ctrl->GetCharAt(posToCheck - 1);
		if (wxT('(') == c1 || wxT(')') == c1 || wxT('[') == c1 || wxT(']') == c1 || wxT('{') == c1 || wxT('}') == c1) {
			posToCheck = posToCheck;
		}
		else if (wxT('(') == c2 || wxT(')') == c2 || wxT('[') == c2 || wxT(']') == c2 || wxT('{') == c2 || wxT('}') == c2) {
			posToCheck = posToCheck - 1;
		}
		else  {
			posToCheck = -1;
		}
		if (posToCheck >= 0) {
			int pos = ctrl->BraceMatch(posToCheck);
			if (wxSTC_INVALID_POSITION == pos) {
				ctrl->BraceBadLight(posToCheck);
			}
			else {
				ctrl->BraceHighlight(posToCheck, pos);
			}
		}
		else {
			ctrl->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
		}
	}
	else {
		ctrl->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
	}
}

t4p::HtmlFeatureClass::HtmlFeatureClass(t4p::AppClass& app)
: FeatureClass(app)
, HtmlCompletionProvider() 
, CssBraceStyler() {
	
}

void t4p::HtmlFeatureClass::OnAppFileOpened(t4p::CodeControlEventClass& event) {
	event.GetCodeControl()->RegisterCompletionProvider(&HtmlCompletionProvider);
	event.GetCodeControl()->RegisterBraceMatchStyler(&CssBraceStyler);
}

BEGIN_EVENT_TABLE(t4p::HtmlFeatureClass, t4p::FeatureClass)
	EVT_APP_FILE_NEW(t4p::HtmlFeatureClass::OnAppFileOpened)
	EVT_APP_FILE_OPEN(t4p::HtmlFeatureClass::OnAppFileOpened)
END_EVENT_TABLE()
