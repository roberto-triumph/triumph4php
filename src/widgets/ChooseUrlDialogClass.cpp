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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <widgets/ChooseUrlDialogClass.h>

mvceditor::ChooseUrlDialogClass::ChooseUrlDialogClass(wxWindow* parent, mvceditor::UrlResourceFinderClass& urls, mvceditor::UrlResourceClass& chosenUrl)
	: ChooseUrlDialogGeneratedClass(parent, wxID_ANY)
	, UrlResourceFinder(urls)
	, ChosenUrl(chosenUrl) {

	// get some urls to prepopulate the list
	std::vector<mvceditor::UrlResourceClass> allUrlResources;
	UrlResourceFinder.FilterUrls(wxT("http://"), allUrlResources);
	for (size_t i = 0; i < allUrlResources.size(); ++i) {
		UrlList->Append(allUrlResources[i].Url.BuildURI());
	}
	if (UrlList->GetCount() > 0) {
		UrlList->Select(0);
	}
	TransferDataToWindow();
	Filter->SetFocus();
}

void mvceditor::ChooseUrlDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		int index = UrlList->GetSelection();
		if (index >= 0) {
			wxURI selection(UrlList->GetString(index));
			UrlResourceFinder.FindByUrl(selection, ChosenUrl);

			// add the extra string also
			wxURI entireUri(ChosenUrl.Url.BuildURI() + ExtraText->GetValue());
			ChosenUrl.Url = entireUri;
		}
		EndModal(wxOK);
	}
}

void mvceditor::ChooseUrlDialogClass::OnListItemSelected(wxCommandEvent& event) {
	wxString finalUrl = UrlList->GetStringSelection() + ExtraText->GetValue();
	FinalUrlLabel->SetLabel(finalUrl);
	event.Skip();
}

void mvceditor::ChooseUrlDialogClass::OnFilterText(wxCommandEvent& event) {
	wxString filter = Filter->GetValue();
	UrlList->Freeze();
	if (filter.IsEmpty()) {
		
		// empty string =  no filter show all
		UrlList->Clear();
		std::vector<mvceditor::UrlResourceClass> allUrlResources;
		UrlResourceFinder.FilterUrls(wxT("http://"), allUrlResources);
		for (size_t i = 0; i < allUrlResources.size(); ++i) {
			UrlList->Append(allUrlResources[i].Url.BuildURI());
		}
		if (UrlList->GetCount() > 0) {
			UrlList->Select(0);
		}
	}
	else {
		std::vector<mvceditor::UrlResourceClass> filteredUrls;
		UrlResourceFinder.FilterUrls(filter, filteredUrls);
		UrlList->Clear();
		for (size_t i = 0; i < filteredUrls.size(); ++i) {
			UrlList->Append(filteredUrls[i].Url.BuildURI());
		}
		if (UrlList->GetCount() > 0) {
			UrlList->Select(0);
		}
	}
	UrlList->Thaw();
	wxString finalUrl = UrlList->GetStringSelection() + ExtraText->GetValue();
	FinalUrlLabel->SetLabel(finalUrl);
}

void mvceditor::ChooseUrlDialogClass::OnFilterTextEnter(wxCommandEvent& event) {
	OnOkButton(event);	
}

void mvceditor::ChooseUrlDialogClass::OnFilterKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_DOWN && event.GetModifiers() == wxMOD_NONE) {
		int selection = UrlList->GetSelection();
		if (selection >= 0 && selection < (int)(UrlList->GetCount() - 1)) {
			UrlList->SetSelection(selection  + 1);
		}
		Filter->SetFocus();
	}
	else if (event.GetKeyCode() == WXK_UP && event.GetModifiers() == wxMOD_NONE) {
		int selection = UrlList->GetSelection();
		if (selection > 0 && selection < (int)UrlList->GetCount()) {
			UrlList->SetSelection(selection  - 1);
		}
		Filter->SetFocus();
	}
	else {
		event.Skip();
	}
}

void mvceditor::ChooseUrlDialogClass::OnExtraText(wxCommandEvent& event) {
	wxString finalUrl = UrlList->GetStringSelection() + ExtraText->GetValue();
	FinalUrlLabel->SetLabel(finalUrl);
	event.Skip();
}

void mvceditor::ChooseUrlDialogClass::OnExtraChar(wxKeyEvent& event) {
	wxString finalUrl = UrlList->GetStringSelection() + ExtraText->GetValue();
	FinalUrlLabel->SetLabel(finalUrl);
	event.Skip();
}