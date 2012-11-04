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
#include <main_frame/EditorBehaviorPanelClass.h>
#include <wx/valgen.h>

mvceditor::EditorBehaviorPanelClass::EditorBehaviorPanelClass(wxWindow* parent, mvceditor::CodeControlOptionsClass& options)
	: EditorBehaviorPanelGeneratedClass(parent) {
	wxGenericValidator enableCodeFoldingValidator(&options.EnableCodeFolding);
	EnableCodeFolding->SetValidator(enableCodeFoldingValidator);
	wxGenericValidator enableAutomaticIndentationValidator(&options.EnableAutomaticLineIndentation);
	EnableAutomaticLineIndentation->SetValidator(enableAutomaticIndentationValidator);
	wxGenericValidator enableLineNumbersValidator(&options.EnableLineNumbers);
	EnableLineNumbers->SetValidator(enableLineNumbersValidator);
	wxGenericValidator indentUsingTabsValidator(&options.IndentUsingTabs);
	IndentUsingTabs->SetValidator(indentUsingTabsValidator);
	wxGenericValidator tabWidthValidator(&options.TabWidth);
	TabWidth->SetValidator(tabWidthValidator);
	wxGenericValidator spacesPerIndentValidator(&options.SpacesPerIndent);
	SpacesPerIndent->SetValidator(spacesPerIndentValidator);
	TabWidth->Enable(options.IndentUsingTabs);
	SpacesPerIndent->Enable(!options.IndentUsingTabs);
	wxGenericValidator enableIndentationGuidesValidator(&options.EnableIndentationGuides);
	EnableIndentationGuides->SetValidator(enableIndentationGuidesValidator);
	wxGenericValidator rightMarginValidator(&options.RightMargin);
	RightMargin->SetValidator(rightMarginValidator);
	wxGenericValidator enableLineEndingsValidator(&options.EnableLineEndings);	
	EnableLineEndings->SetValidator(enableLineEndingsValidator);
	wxGenericValidator lineEndingModeValidator(&options.LineEndingMode);
	LineEndingMode->SetValidator(lineEndingModeValidator);
	wxGenericValidator autoCompletionValidator(&options.EnableAutoCompletion);
	EnableAutoCompletion->SetValidator(autoCompletionValidator);
	wxGenericValidator dynamicAutoCompletionValidator(&options.EnableDynamicAutoCompletion);
	EnableDynamicAutoCompletion->SetValidator(dynamicAutoCompletionValidator);
	wxGenericValidator wordWrapValidator(&options.EnableWordWrap);
	EnableWordWrap->SetValidator(wordWrapValidator);
	wxGenericValidator trimTrailingSpaceValidator(&options.TrimTrailingSpaceBeforeSave);
	TrimTrailingSpaceBeforeSave->SetValidator(trimTrailingSpaceValidator);
	wxGenericValidator removeTrailingBlankLinesValidator(&options.RemoveTrailingBlankLinesBeforeSave);
	RemoveTrailingBlankLines->SetValidator(removeTrailingBlankLinesValidator);
	wxGenericValidator callTipOnMouseHoverValidator(&options.EnableCallTipsOnMouseHover);
	EnableCallTipOnMouseHover->SetValidator(callTipOnMouseHoverValidator);
}

void mvceditor::EditorBehaviorPanelClass::OnIndentUsingSpaces(wxCommandEvent& event) {
	TabWidth->Enable(event.IsChecked());
	SpacesPerIndent->Enable(!event.IsChecked());
}

bool mvceditor::EditorBehaviorPanelClass::TransferDataToWindow() {
	if (wxWindow::TransferDataToWindow()) {
		
		// CodeControlOptionsClass disables right margin when RightMargin = 0
		EnableRightMargin->SetValue(RightMargin->GetValue() > 0);
		RightMargin->Enable(RightMargin->GetValue() > 0);
		return true;
	}
	return false;
}

bool mvceditor::EditorBehaviorPanelClass::TransferDataFromWindow() {
	
	// CodeControlOptionsClass disables right margin when RightMargin = 0
	if (!EnableRightMargin->IsChecked()) {
		RightMargin->SetValue(0);
	}	
	return wxWindow::TransferDataFromWindow();
}

void mvceditor::EditorBehaviorPanelClass::OnCheckRightMargin(wxCommandEvent& event) {
	RightMargin->Enable(event.IsChecked());
}