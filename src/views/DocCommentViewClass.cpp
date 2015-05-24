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
#include <views/DocCommentViewClass.h>
#include <features/DocCommentFeatureClass.h>
#include <Triumph.h>
#include <unicode/unistr.h>
#include <wx/tokenzr.h>
#include <wx/wupdlock.h>

/**
 * Turns a tag PHPDoc comment into a nicer format that is more suitable
 * to display. Any beginning '*'s are removed.
 *
 * Also, any HTML entities are handled (converted to its ascii equivalent)
 */
static wxString NiceDocText(const UnicodeString& comment) {
	wxString wxComment = t4p::IcuToWx(comment);
	wxComment = wxComment.Trim();

	// remove the beginning and ending '/*' and '*/'
	wxComment = wxComment.Mid(2, wxComment.Len() - 4);

	wxStringTokenizer tok(wxComment, wxT("\n\r"));
	wxString prettyComment;
	while (tok.HasMoreTokens()) {
		wxString line = tok.NextToken();

		// remove the beginning whitespace and '*'s
		size_t pos = 0;
		for (; pos < line.Len(); ++pos) {
			if (wxT(' ') != line[pos] && wxT('*') != line[pos] && wxT('\t') != line[pos]) {
				break;
			}
		}
		if (pos < line.Len()) {
			line = line.Mid(pos);
		}
		else {

			// an empty comment line
			line = wxT("");
		}

		// tag 'conversions'
		// taken from http://manual.phpdoc.org/HTMLSmartyConverter/HandS/phpDocumentor/tutorial_phpDocumentor.howto.pkg.html
		// the DocBlock Description details section
		// <b> -- emphasize/bold text
		// <code> -- Use this to surround php code, some converters will highlight it
		// <br> -- hard line break, may be ignored by some converters
		// <i> -- italicize/mark as important
		// <kbd> -- denote keyboard input/screen display
		// <li> -- list item
		// <ol> -- ordered list
		// <p> -- If used to enclose all paragraphs, otherwise it will be considered text
		// <pre> -- Preserve line breaks and spacing, and assume all tags are text (like XML's CDATA)
		// <samp> -- denote sample or examples (non-php)
		// <ul> -- unordered list
		// <var> -- denote a variable name
		//
		// will ignore all of the tags except '<br>', '<code>', <p>', '<pre>'
		// since we cannot format the Scintilla call tip window.
		// For the tags we don handle; just translate them to newlies for now.
		wxString remove[] = {
			wxT("<b>"), wxT("</b>"), wxT("<i>"), wxT("</i>"),
			wxT("<kbd>"), wxT("</kbd>"), wxT("<samp>"), wxT("</samp>"),
			wxT("<var>"), wxT("</var>"),
			wxT("")
		};
		for (int i = 0; !remove[i].IsEmpty(); ++i) {
			line.Replace(remove[i], wxT(""));
		}

		wxString toNewline[] =  {
			wxT("<code>"), wxT("</code>"),	wxT("<br>"), wxT("<br />"),
			wxT("<li>"), wxT("</li>"), wxT("<ol>"), wxT("</ol>"),
			wxT("<p>"), wxT("</p>"), wxT("<ul>"), wxT("</ul>"),
			wxT("")
		};
		for (int i = 0; !toNewline[i].IsEmpty(); ++i) {
			line.Replace(toNewline[i], wxT("\n"));
		}


		line.Replace(wxT("{@*}"), wxT("*/"));

		// replace tabs with spaces
		// do it here instead of in scintilla; we may want to change this later
		line.Replace(wxT("\t"), wxT("    "));

		// replace any html entitiies
		line.Replace(wxT("&lt;"), wxT("<"));
		line.Replace(wxT("&gt;"), wxT(">"));
		line.Replace(wxT("&nbsp;"), wxT(" "));
		line.Replace(wxT("&#039;"), wxT("'"));
		line.Replace(wxT("&quot;"), wxT("\""));

		prettyComment += line;
		prettyComment += wxT("\n");
	}
	return prettyComment;
}

/**
 * @return bool TRUE if the given code control window has a DocComment panel
 */
static bool CodeControlHasDocComment(wxWindow* parent) {
	return NULL != wxWindow::FindWindowByName(wxT("DocComment"), parent);
}

t4p::DocCommentViewClass::DocCommentViewClass(t4p::DocCommentFeatureClass& feature)
: FeatureViewClass()
, Feature(feature) {

}


void t4p::DocCommentViewClass::AddEditMenuItems(wxMenu* editMenu) {
	editMenu->Append(t4p::MENU_DOC_COMMENT + 0, _("Show Doc Comment"),
		_("Show PHP Doc comment of the symbol located at the current position"), wxITEM_NORMAL);
}

void t4p::DocCommentViewClass::OnShowDocComment(wxCommandEvent& event) {
	t4p::CodeControlClass* ctrl = GetCurrentCodeControl();
	if (ctrl) {

		// only show a DocComment if there isn't another DocComment shown on this code control
		if (!CodeControlHasDocComment(ctrl)) {
			ShowDocComment(ctrl, ctrl->GetCurrentPos());
		}
	}
}

void t4p::DocCommentViewClass::OnMotionAlt(wxCommandEvent& event) {
	if (!Feature.IsEnabled()) {
		return;
	}
	t4p::CodeControlClass* ctrl = (t4p::CodeControlClass*)event.GetEventObject();
	if (ctrl) {

		// only show a DocComment if there isn't another DocComment shown on this code control
		if (!CodeControlHasDocComment(ctrl)) {
			int pos = event.GetInt();
			if (pos >= 0) {
				ShowDocComment(ctrl, pos);
			}
		}
	}
}

void t4p::DocCommentViewClass::ShowDocComment(t4p::CodeControlClass* ctrl, int pos) {
	t4p::FileType type = ctrl->GetFileType();
	if (type != t4p::FILE_TYPE_PHP) {
		return;
	}

	// if the cursor is in the middle of an identifier, find the end of the
	// current identifier; that way we can know the full name of the tag we want
	// to get
	int endPos = ctrl->WordEndPosition(pos, true);
	pos = ctrl->WordStartPosition(pos, true);
	wxString matchError;
	std::vector<t4p::TagClass> matches = Feature.App.Globals.TagCache.GetTagsAtPosition(
		ctrl->GetIdString(), ctrl->GetSafeText(), endPos,
		Feature.App.Globals.AllEnabledSourceDirectories(),
		Feature.App.Globals,
		matchError
	);
	wxString msg;
	bool hasMatches = false;
	bool hasContent = false;
	bool isNativeTag = false;
	wxString funcName = wxT("");
	t4p::TagClass tag;
	if (!matches.empty()) {
		tag = matches[0];
		hasMatches = true;
		isNativeTag = tag.IsNative;
		funcName = t4p::IcuToWx(tag.Identifier);
		if (tag.Type == t4p::TagClass::FUNCTION) {
			msg = t4p::IcuToWx(tag.Identifier);
			msg += wxT("\n\n");
			msg += t4p::IcuToWx(tag.Signature);
			hasContent = true;
		}
		else if (tag.Type == t4p::TagClass::METHOD) {
			msg = t4p::IcuToWx(tag.ClassName);
			msg += wxT("::");
			msg += t4p::IcuToWx(tag.Identifier);
			msg += wxT("\n\n");
			msg += t4p::IcuToWx(tag.Signature);
			hasContent = true;
		}
		else if (tag.Type == t4p::TagClass::MEMBER || tag.Type == t4p::TagClass::CLASS_CONSTANT) {
			msg = t4p::IcuToWx(tag.ClassName);
			msg += wxT("::");
			msg += t4p::IcuToWx(tag.Identifier);
			msg += wxT("\n\n");
			msg += t4p::IcuToWx(tag.Signature);
			if (!tag.ReturnType.isEmpty()) {
				msg += wxT(" [ ");
				msg += t4p::IcuToWx(tag.ReturnType);
				msg += wxT(" ]");
			}
			hasContent = !tag.ReturnType.isEmpty();
		}
		else {
			msg = t4p::IcuToWx(tag.Identifier);
			msg += wxT("\n\n");
			msg += t4p::IcuToWx(tag.Signature);
		}
		if (!tag.Comment.isEmpty()) {
			msg += wxT("\n\n");
			msg += NiceDocText(tag.Comment);
		}
	}
	if (!hasMatches && !matchError.empty()) {
		GetStatusBarWithGauge()->SetColumn0Text(matchError);
	}
	else if (!hasMatches) {
		GetStatusBarWithGauge()->SetColumn0Text(wxString::Format(_("No match for %s"), ctrl->GetTextRange(pos, endPos).c_str()));
	}
	else if (!hasContent) {
		GetStatusBarWithGauge()->SetColumn0Text(wxString::Format(_("No content for %s"), t4p::IcuToWx(tag.Key).c_str()));
	}
	else {

		// freeze thaw the code control so that the call tip popup is
		// not drawn while its being moved into place
		// in linux, freezing already happens internally so we don't
		// want to do it here
		wxPlatformInfo info;
		if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT) {
			ctrl->Freeze();
		}
		wxPoint point = ctrl->PointFromPosition(pos);
		t4p::DocCommentPanelClass* panel = new t4p::DocCommentPanelClass(ctrl, *this);
		panel->SetPosition(point);
		panel->SetText(msg);
		if (isNativeTag) {
			panel->EnablePhpSiteLink(funcName);
		}
		panel->SetFocus();
		if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT) {
			ctrl->Thaw();
		}
	}
}

void t4p::DocCommentViewClass::OnPhpSiteLinkClick(wxHyperlinkEvent& event) {
	wxLaunchDefaultBrowser(event.GetURL());
}

t4p::DocCommentPanelClass::DocCommentPanelClass(wxWindow* parent, t4p::DocCommentViewClass& featureView)
: DocCommentPanelGeneratedClass(parent)
, FeatureView(featureView) {
	SetName(wxT("DocComment"));
	PhpSiteDocs->Hide();
}

void t4p::DocCommentPanelClass::SetText(const wxString& text) {
	Text->BeginTextColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
	Text->SetValue(text);
}

void t4p::DocCommentPanelClass::EnablePhpSiteLink(const wxString& functionName) {
	wxString url = wxT("http://php.net/") + functionName;
	PhpSiteDocs->SetURL(url);
	PhpSiteDocs->Show();
}

void t4p::DocCommentPanelClass::OnClose(wxHyperlinkEvent& event) {
	CallAfter(&t4p::DocCommentPanelClass::DoDestroy);
}

void t4p::DocCommentPanelClass::OnPhpSiteDocs(wxHyperlinkEvent& event) {
	CallAfter(&t4p::DocCommentPanelClass::DoDestroy);

	// post an event instead of handling letting the defauly handling take
	// place
	// this is because in MSW, DoDestroy gets called before the
	// default handling finishes; and this causes a crash
	wxPostEvent(&FeatureView, event);
}

void t4p::DocCommentPanelClass::OnKeyDown(wxKeyEvent& event) {

	// ESC == close the panel
	if (event.GetKeyCode() == WXK_ESCAPE) {
		CallAfter(&t4p::DocCommentPanelClass::DoDestroy);
		return;
	}
	event.Skip();
}

void t4p::DocCommentPanelClass::DoDestroy() {
	this->Destroy();
}


BEGIN_EVENT_TABLE(t4p::DocCommentViewClass, t4p::FeatureViewClass)
	EVT_MENU(t4p::MENU_DOC_COMMENT + 0, t4p::DocCommentViewClass::OnShowDocComment)
	EVT_COMMAND(wxID_ANY, t4p::EVT_MOTION_ALT, t4p::DocCommentViewClass::OnMotionAlt)
	EVT_HYPERLINK(ID_PHP_SITE_LINK, t4p::DocCommentViewClass::OnPhpSiteLinkClick)
END_EVENT_TABLE()
