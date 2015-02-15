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
#include <features/JavascriptFeatureClass.h>

static bool InCommentOrStringStyle(wxStyledTextCtrl* ctrl, int posToCheck) {
	int style = ctrl->GetStyleAt(posToCheck);

	// dont match braces inside strings or comments.
	return wxSTC_C_COMMENT == style || wxSTC_C_STRING == style || wxSTC_C_CHARACTER == style;
}


t4p::JavascriptCodeCompletionProviderClass::JavascriptCodeCompletionProviderClass()
: CodeCompletionProviderClass() {
	
}

bool t4p::JavascriptCodeCompletionProviderClass::DoesSupport(t4p::FileType type) {
	
	// not yet implemented
	return false;
}

void t4p::JavascriptCodeCompletionProviderClass::Provide(t4p::CodeControlClass* ctrl, std::vector<t4p::CodeCompletionItemClass>& suggestions, wxString& completeStatus) {
	
}

t4p::JavascriptBraceMatchStylerClass::JavascriptBraceMatchStylerClass()
: BraceMatchStylerClass() {
	
}

bool t4p::JavascriptBraceMatchStylerClass::DoesSupport(t4p::FileType type) {
	return t4p::FILE_TYPE_JS == type;
}

void t4p::JavascriptBraceMatchStylerClass::Style(t4p::CodeControlClass* ctrl, int posToCheck) {
	if (!InCommentOrStringStyle(ctrl, posToCheck)) {
		wxChar c2 = ctrl->GetCharAt(posToCheck - 1);
		if (wxT('(') == c2 || wxT(')') == c2 || wxT('[') == c2 || wxT(']') == c2 || wxT('{') == c2 || wxT('}') == c2) {
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

t4p::JavascriptFeatureClass::JavascriptFeatureClass(t4p::AppClass& app)
: FeatureClass(app)
, JavascriptCompletionProvider() 
, BraceStyler() {
	
}

void t4p::JavascriptFeatureClass::OnAppFileOpened(t4p::CodeControlEventClass& event) {
	event.GetCodeControl()->RegisterCompletionProvider(&JavascriptCompletionProvider);
	event.GetCodeControl()->RegisterBraceMatchStyler(&BraceStyler);
}

BEGIN_EVENT_TABLE(t4p::JavascriptFeatureClass, t4p::FeatureClass)
	EVT_APP_FILE_NEW(t4p::JavascriptFeatureClass::OnAppFileOpened)
	EVT_APP_FILE_OPEN(t4p::JavascriptFeatureClass::OnAppFileOpened)
END_EVENT_TABLE()
