
#include <features/DocCommentFeatureClass.h>
#include <Triumph.h>
#include <unicode/unistr.h>
#include <wx/tokenzr.h>

/**
 * Turns a tag PHPDoc comment into a nicer format that is more suitable
 * to display. Any beginning '*'s are removed.
 *
 * Also, any HTML entities are handled (converted to its ascii equivalent)
 */
static wxString NiceDocText(const UnicodeString& comment) {
	wxString wxComment = mvceditor::IcuToWx(comment);
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

mvceditor::DocCommentFeatureClass::DocCommentFeatureClass(mvceditor::AppClass& app) 
: FeatureClass(app) {

}

void mvceditor::DocCommentFeatureClass::AddEditMenuItems(wxMenu* editMenu) {
	editMenu->Append(mvceditor::MENU_DOC_COMMENT + 0, _("Show Doc Comment"), 
		_("Show PHP Doc comment of the symbol located at the current position"), wxITEM_NORMAL);
}

void mvceditor::DocCommentFeatureClass::OnShowDocComment(wxCommandEvent& event) {
	mvceditor::CodeControlClass* ctrl = GetCurrentCodeControl();
	if (ctrl) {

		// only show a DocComment if there isn't another DocComment shown on this code control
		if (!CodeControlHasDocComment(ctrl)) {
			ShowDocComment(ctrl, ctrl->GetCurrentPos());
		}		
	}
}

void mvceditor::DocCommentFeatureClass::OnMotionAlt(wxCommandEvent& event) {
	if (!App.Preferences.CodeControlOptions.EnableCallTipsOnMouseHover) {
		return;
	}
	mvceditor::CodeControlClass* ctrl = (mvceditor::CodeControlClass*)event.GetEventObject();
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

void mvceditor::DocCommentFeatureClass::ShowDocComment(mvceditor::CodeControlClass* ctrl, int pos) {
	int documentMode = ctrl->GetDocumentMode();
	if (documentMode != mvceditor::CodeControlClass::PHP) {
		return;
	}
	
	// if the cursor is in the middle of an identifier, find the end of the
	// current identifier; that way we can know the full name of the tag we want
	// to get
	int endPos = ctrl->WordEndPosition(pos, true);
	pos = ctrl->WordStartPosition(pos, true);
	std::vector<mvceditor::TagClass> matches = ctrl->GetTagsAtPosition(endPos);
	wxString msg;
	bool hasMatches = false;
	bool hasContent = false;
	mvceditor::TagClass tag;
	if (!matches.empty()) {
		tag = matches[0];
		hasMatches = true;
		if (tag.Type == mvceditor::TagClass::FUNCTION) {
			msg = mvceditor::IcuToWx(tag.Identifier);
			msg += wxT("\n\n");
			msg += mvceditor::IcuToWx(tag.Signature);
			hasContent = true;
		}
		else if (tag.Type == mvceditor::TagClass::METHOD) {
			msg = mvceditor::IcuToWx(tag.ClassName);
			msg += wxT("::");
			msg += mvceditor::IcuToWx(tag.Identifier);
			msg += wxT("\n\n");
			msg += mvceditor::IcuToWx(tag.Signature);
			hasContent = true;
		}
		else if (tag.Type == mvceditor::TagClass::MEMBER || tag.Type == mvceditor::TagClass::CLASS_CONSTANT) {
			msg = mvceditor::IcuToWx(tag.ClassName);
			msg += wxT("::");
			msg += mvceditor::IcuToWx(tag.Identifier);
			msg += wxT("\n\n");
			msg += mvceditor::IcuToWx(tag.Signature);
			if (!tag.ReturnType.isEmpty()) {
				msg += wxT(" [ ");
				msg += mvceditor::IcuToWx(tag.ReturnType);
				msg += wxT(" ]");	
			}
			hasContent = !tag.ReturnType.isEmpty();
		}
		else {
			msg = mvceditor::IcuToWx(tag.Identifier);
			msg += wxT("\n\n");
			msg += mvceditor::IcuToWx(tag.Signature);
		}
		if (!tag.Comment.isEmpty()) {
			msg += wxT("\n\n");
			msg += NiceDocText(tag.Comment);
		}
	}
	if (!hasMatches) {
		GetStatusBarWithGauge()->SetColumn0Text(wxString::Format(_("No match for %s"), ctrl->GetTextRange(pos, endPos).c_str()));
	}
	else if (!hasContent) {
		GetStatusBarWithGauge()->SetColumn0Text(wxString::Format(_("No content for %s"), mvceditor::IcuToWx(tag.Key).c_str()));
	}
	else {

		// freeze thaw the code control so that the call tip popup is 
		// not drawn while its being moved into place
		ctrl->Freeze();
		wxPoint point = ctrl->PointFromPosition(pos);
		mvceditor::DocCommentPanelClass* panel = new mvceditor::DocCommentPanelClass(ctrl);
		panel->SetPosition(point);
		panel->SetText(msg);
		panel->SetFocus();
		ctrl->Thaw();
	}
}


mvceditor::DocCommentPanelClass::DocCommentPanelClass(wxWindow* parent) 
: DocCommentPanelGeneratedClass(parent) {
	SetName(wxT("DocComment"));
}

void mvceditor::DocCommentPanelClass::SetText(const wxString& text) {
	Text->SetValue(text);
}

void mvceditor::DocCommentPanelClass::OnClose(wxHyperlinkEvent& event) {
	CallAfter(&mvceditor::DocCommentPanelClass::DoDestroy);
}

void mvceditor::DocCommentPanelClass::OnKeyDown(wxKeyEvent& event) {

	// ESC == close the panel
	if (event.GetKeyCode() == WXK_ESCAPE) {
		CallAfter(&mvceditor::DocCommentPanelClass::DoDestroy);
		return;
	}
	event.Skip();
}

void mvceditor::DocCommentPanelClass::DoDestroy() {
	this->Destroy();
}


BEGIN_EVENT_TABLE(mvceditor::DocCommentFeatureClass, mvceditor::FeatureClass)
	EVT_MENU(mvceditor::MENU_DOC_COMMENT + 0, mvceditor::DocCommentFeatureClass::OnShowDocComment)
	EVT_COMMAND(wxID_ANY, mvceditor::EVT_MOTION_ALT, mvceditor::DocCommentFeatureClass::OnMotionAlt)
END_EVENT_TABLE()
