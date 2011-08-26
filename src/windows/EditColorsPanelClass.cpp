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
 * @author     $Author: robertop2004@gmail.com $
 * @date       $Date: 2011-06-10 17:19:31 -0700 (Fri, 10 Jun 2011) $
 * @version    $Rev: 555 $ 
 */
#include <windows/EditColorsPanelClass.h>
#include <widgets/CodeControlClass.h>

mvceditor::EditColorsPanelClass::EditColorsPanelClass(wxWindow* parent, mvceditor::CodeControlOptionsClass& options)
		: EditColorsPanelGeneratedClass(parent)
		, CodeControlOptions(options) {
	Styles->InsertItems(options.CodeStyles, 0);
	Styles->SetSelection(0);
	wxFont font;
	wxColor color,
		backgroundColor;
	bool isBold,
		isItalic;
	CodeControlOptions.StartEditMode();
	if (CodeControlOptions.GetEditStyle(0, font, color, backgroundColor, isBold, isItalic)) {
		Font->SetSelectedFont(font);
		ForegroundColor->SetColour(color);
		BackgroundColor->SetColour(backgroundColor);
		Bold->SetValue(isBold);
		Italic->SetValue(isItalic);
	}
}


void mvceditor::EditColorsPanelClass::OnListBox(wxCommandEvent& event) {
	int selected = event.GetSelection();
	wxFont font;
	wxColor color,
		backgroundColor;
	bool isBold,
		isItalic;
	if (CodeControlOptions.GetEditStyle(selected, font, color, backgroundColor, isBold, isItalic)) {
		Font->SetSelectedFont(font);
		ForegroundColor->SetColour(color);
		BackgroundColor->SetColour(backgroundColor);
		Bold->SetValue(isBold);
		Italic->SetValue(isItalic);
		int style = CodeControlOptionsClass::ArrayIndexToStcConstant[selected];
		switch (style) {
			case mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET:
			case mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CARET_LINE:
			case mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_SELECTION:
			case mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_CODE_FOLDING:
			case mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_RIGHT_MARGIN:
			case wxSTC_STYLE_INDENTGUIDE:
				Font->Enable(false);
				Bold->Enable(false);
				Italic->Enable(false);
				BackgroundColor->Enable(true);
				break;
			case mvceditor::CodeControlOptionsClass::MVC_EDITOR_STYLE_MATCH_HIGHLIGHT:
				Font->Enable(false);
				Bold->Enable(false);
				Italic->Enable(false);
				BackgroundColor->Enable(false);
				break;
			default:
				Font->Enable(true);
				Bold->Enable(true);
				Italic->Enable(true);
				BackgroundColor->Enable(true);
				break;
		}
	}
}


void mvceditor::EditColorsPanelClass::OnCheck(wxCommandEvent& event) {
	int selected = Styles->GetSelection();
	wxFont font;
	wxColor color,
		backgroundColor;
	bool isBold,
		isItalic;
	if (CodeControlOptions.GetEditStyle(selected, font, color, backgroundColor, isBold, isItalic)) {
		switch (event.GetId()) {
			case ID_BOLD:
				isBold = event.IsChecked();
				break;
			case ID_ITALIC:
				isItalic = event.IsChecked();
				break;
		}
		CodeControlOptions.ChangeStyle(selected, font, color, backgroundColor, isBold, isItalic);
	}
}

void mvceditor::EditColorsPanelClass::OnColorChanged(wxColourPickerEvent& event) {
	int selected = Styles->GetSelection();
	wxFont font;
	wxColor color,
		backgroundColor;
	bool isBold,
		isItalic;
	if (CodeControlOptions.GetEditStyle(selected, font, color, backgroundColor, isBold, isItalic)) {
		switch (event.GetId()) {
			case ID_FOREGROUND_COLOR:
				color = event.GetColour();
				break;
			case ID_BACKGROUND_COLOR:
				backgroundColor = event.GetColour();
				break;
		}
		CodeControlOptions.ChangeStyle(selected, font, color, backgroundColor, isBold, isItalic);
	}
}

void mvceditor::EditColorsPanelClass::OnFontChanged(wxFontPickerEvent& event) {
	int selected = Styles->GetSelection();
	wxFont font;
	wxColor color,
		backgroundColor;
	bool isBold,
		isItalic;
	if (CodeControlOptions.GetEditStyle(selected, font, color, backgroundColor, isBold, isItalic)) {
		font = event.GetFont();
		CodeControlOptions.ChangeStyle(selected, font, color, backgroundColor, isBold, isItalic);
	}
}

void mvceditor::EditColorsPanelClass::OnThemeChoice(wxCommandEvent& event) {
	int choice = event.GetSelection();
	wxCommandEvent listBoxEvent(wxEVT_COMMAND_LISTBOX_SELECTED, wxID_ANY);
	listBoxEvent.SetInt(Styles->GetSelection());
	switch (choice) {
		case 0:
			CodeControlOptions.SetToLightTheme();
			wxPostEvent(this, listBoxEvent);
			break;
		case 1:
			CodeControlOptions.SetToDarkTheme();
			wxPostEvent(this, listBoxEvent);
			break;
	}
}
