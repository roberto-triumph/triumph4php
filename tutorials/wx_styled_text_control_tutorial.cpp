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
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <wx/wx.h>
#include <wx/stc/stc.h>
#include <wx/regex.h>

/**
 * This tutorial uses the styled text control to open a source code file.
 * The text control will be made so that it colourises the PHP code
 * and also mark some text.
 */
class MyApp : public wxApp {
public:
	virtual bool OnInit();
};

class MyFrame: public wxFrame {
public:
	MyFrame();

private:
	wxStyledTextCtrl* TextCtrl;
	long wxColourAsLong(const wxColour& co);
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
	MyFrame* frame = new MyFrame();
	SetTopWindow(frame);
	frame->Show(true);	
	return true;
}

MyFrame::MyFrame() :
	wxFrame(NULL, wxID_ANY, wxT("STC test"), wxDefaultPosition, 
			wxSize(1024, 768)) {
	TextCtrl = new wxStyledTextCtrl(this, wxID_ANY);
	TextCtrl->SetFocus();

	// set all of the used styles to the same font
	// each token type has its own 'style' that can be changed
	// independently.
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
	            wxT("Courier New"));
	for (int i = wxSTC_H_DEFAULT; i <= wxSTC_H_QUESTION; i++) {
		TextCtrl->StyleSetFont(i, font);
	}
	TextCtrl->StyleSetFont(wxSTC_H_VALUE, font);

	for (int i = wxSTC_HPHP_DEFAULT; i <= wxSTC_HPHP_OPERATOR; i++) {
		TextCtrl->StyleSetFont(i, font);
	}
	TextCtrl->StyleSetFont(wxSTC_HPHP_COMPLEX_VARIABLE, font);


	// the lexer that highlights PHP code is the wxSTC_LEX_HTML.  this lexer can handle
	// HTML and any embedded server side language (PHP, ASP, Python, etc..).  Because of 
	// this, the lexer has more states than all other lexers; that's why we need
	// to set aside more style bits for this lexer.
	TextCtrl->SetLexer(wxSTC_LEX_HTML);
	TextCtrl->SetStyleBits(7);

	// make the keywords bold and blue.
	TextCtrl->StyleSetBold(wxSTC_HPHP_WORD, true);
	TextCtrl->StyleSetForeground(wxSTC_HPHP_WORD, *wxBLUE);
	TextCtrl->SetKeyWords(0, wxT("html body"));
	TextCtrl->SetKeyWords(4, wxString::FromAscii("php if else do while for foreach switch case " 
			"break default function return public private protected class "
			" interface extends implements static"));

	// make comments dark green 
	TextCtrl->StyleSetBold(wxSTC_HPHP_COMMENT, true);
	TextCtrl->StyleSetBold(wxSTC_HPHP_COMMENTLINE, true);
	TextCtrl->StyleSetForeground(wxSTC_HPHP_COMMENT, wxTheColourDatabase->Find(wxT("Dark Green")));
	TextCtrl->StyleSetBackground(wxSTC_HPHP_COMMENTLINE, wxTheColourDatabase->Find(wxT("Dark Green")));

	// intialize the markers.  this marker will be used to put arrows in the
	// left margin
	const int HIT_MARKER = 1;
	TextCtrl->MarkerDefine(HIT_MARKER, wxSTC_MARK_ARROW, *wxRED, *wxRED);
	
	TextCtrl->SetText(wxT("<?php\r\nfunction f() {\n\n}\n\n$s = \"this is a test this \";\r\n?>"));
	TextCtrl->Colourise(0, -1);

	// 128 => 8th bit on since first 7 bits of style bits are for the lexer
	// this means that there is only one bit left for indicators; resulting
	// in only 1 indicator being avaible when using scintilla with the HTML lexer.
	int STYLE_FIND = 128;
	int INDICATOR = 0;

	TextCtrl->IndicatorSetStyle(INDICATOR, wxSTC_INDIC_SQUIGGLE);
	TextCtrl->IndicatorSetForeground(INDICATOR, *wxRED);

	wxString text = TextCtrl->GetText();
	wxRegEx regEx(wxT("this"));
	if (regEx.IsValid()) {
		size_t matchStart, 
			matchLength;
		int lastHit = 0;
		while (regEx.Matches(text, 0)) {
			if (regEx.GetMatch(&matchStart, &matchLength, 0)) {
				printf("hit at %ld (length: %ld)\n", (lastHit + matchStart), matchLength);
				
				
				TextCtrl->StartStyling(lastHit + matchStart, STYLE_FIND);
				TextCtrl->SetStyling(matchLength, STYLE_FIND);

				// careful with positions; STC positions are byte offsets while 
				// most string functions from wxString or UnicodeString or std::string
				// are in characters. This means that we always need to do 
				// conversion from chars to bytes, otherwise line numbers will be off.  In this case, since the
				// text is ASCII it is OK.
				int line = TextCtrl->LineFromPosition(lastHit + matchStart);

				// in this demo the markes will stay forever; but in an application
				// we must explicitly call MarkerDelete to remove 
				// the markers.
				TextCtrl->MarkerAdd(line, HIT_MARKER);
			}
			lastHit = matchStart + matchLength;
			text = text.substr(lastHit, -1);
		}
		TextCtrl->Colourise(0, -1);
	}
}

