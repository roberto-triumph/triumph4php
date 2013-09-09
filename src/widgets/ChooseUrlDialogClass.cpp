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

mvceditor::ChooseUrlDialogClass::ChooseUrlDialogClass(wxWindow* parent, 
													  mvceditor::UrlTagFinderClass& urls,
													  const std::vector<mvceditor::ProjectClass>& projects,
													  mvceditor::UrlTagClass& chosenUrl)
	: ChooseUrlDialogGeneratedClass(parent, wxID_ANY)
	, UrlTagFinder(urls)
	, ChosenUrl(chosenUrl) 
	, Projects() {

	// get some urls to prepopulate the list
	std::vector<mvceditor::UrlTagClass> allUrlTags = GetFilteredUrls(wxT("http://"));
	FillUrlList(allUrlTags);
	
	ProjectChoice->Clear();
	ProjectChoice->Append(_("All Enabled Projects"), (void*)NULL);
	for (size_t i = 0; i < projects.size(); ++i) {
		if (projects[i].IsEnabled) {
			Projects.push_back(projects[i]);
		}
	}

	// now put the enabled projects in the choice
	for (size_t i = 0; i < Projects.size(); ++i) {
		
		// should be ok to reference this vector since it wont change because this is a 
		// modal dialog
		ProjectChoice->Append(Projects[i].Label, &Projects[i]);
	}
	ProjectChoice->Select(0);

	TransferDataToWindow();
	Filter->SetFocus();
}

void mvceditor::ChooseUrlDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		int index = UrlList->GetSelection();
		if (index >= 0) {
			wxURI selection(UrlList->GetString(index));
			std::vector<wxFileName> sourceDirs = ActiveSourceDirectories();
			UrlTagFinder.FindByUrl(selection, sourceDirs, ChosenUrl);

			// add the extra string also
			wxURI entireUri(ChosenUrl.Url.BuildURI() + ExtraText->GetValue());
			ChosenUrl.Url = entireUri;
		}
		EndModal(wxOK);
	}
}

void mvceditor::ChooseUrlDialogClass::OnListItemDoubleClicked(wxCommandEvent& event) {
	OnOkButton(event);	
}

void mvceditor::ChooseUrlDialogClass::OnListItemSelected(wxCommandEvent& event) {
	wxString finalUrl = UrlList->GetStringSelection() + ExtraText->GetValue();
	FinalUrlLabel->SetLabel(finalUrl);
	event.Skip();
}

void mvceditor::ChooseUrlDialogClass::OnFilterText(wxCommandEvent& event) {
	wxString filter = Filter->GetValue();
	if (filter.IsEmpty()) {

		// filter string cannot be empty
		// empty string =  no filter show all
		filter = wxT("http://");
	}

	std::vector<mvceditor::UrlTagClass> filteredUrls;
	
	// project 0 is the "all enabled projects"
	int sel = ProjectChoice->GetSelection();
	if (sel >= 1 && sel < (int)ProjectChoice->GetCount()) {
		mvceditor::ProjectClass* project = (mvceditor::ProjectClass*)ProjectChoice->GetClientData(sel);
		filteredUrls = GetFilteredUrlsByProject(filter, *project);
	}
	else {
		filteredUrls = GetFilteredUrls(filter);
	}
	FillUrlList(filteredUrls);

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

void mvceditor::ChooseUrlDialogClass::OnProjectChoice(wxCommandEvent& event) {
	int sel = event.GetSelection();
	wxString filter = Filter->GetValue();

	std::vector<mvceditor::UrlTagClass> filteredUrls;

	// project 0 is the "all enabled projects"
	if (sel >= 1 && sel < (int)ProjectChoice->GetCount()) {
		mvceditor::ProjectClass* project = (mvceditor::ProjectClass*)ProjectChoice->GetClientData(sel);
		filteredUrls = GetFilteredUrlsByProject(filter, *project);
	}
	else {
		filteredUrls = GetFilteredUrls(filter);
	}
	FillUrlList(filteredUrls);

	wxString finalUrl = UrlList->GetStringSelection() + ExtraText->GetValue();
	FinalUrlLabel->SetLabel(finalUrl);
}

std::vector<mvceditor::UrlTagClass> mvceditor::ChooseUrlDialogClass::GetFilteredUrls(const wxString& filter) {
	std::vector<mvceditor::UrlTagClass> filteredUrls;
	std::vector<wxFileName> sourceDirs = ActiveSourceDirectories();
	UrlTagFinder.FilterUrls(filter, sourceDirs, filteredUrls);
	return filteredUrls;
}


std::vector<mvceditor::UrlTagClass> mvceditor::ChooseUrlDialogClass::GetFilteredUrlsByProject(const wxString& filter, const mvceditor::ProjectClass& project) {
	std::vector<wxFileName> dirs;
	std::vector<mvceditor::SourceClass>::const_iterator src;
	for (src = project.Sources.begin(); src != project.Sources.end(); ++src) {
		dirs.push_back(src->RootDirectory);
	}
	
	std::vector<mvceditor::UrlTagClass> filteredUrls;
	UrlTagFinder.FilterUrls(filter, dirs, filteredUrls);

	// check that the controller is part of the project's sources
	std::vector<mvceditor::UrlTagClass>::iterator url = filteredUrls.begin();
	while (url != filteredUrls.end()) {
		wxFileName fileName = url->FileName;
		wxString fullPath = fileName.GetFullPath();
		if (!project.IsAPhpSourceFile(fullPath)) {
			url =  filteredUrls.erase(url);
		}
		else {
			url++;
		}
	}
	return filteredUrls;
}

void mvceditor::ChooseUrlDialogClass::FillUrlList(const std::vector<mvceditor::UrlTagClass>& urls) {
	UrlList->Freeze();
	UrlList->Clear();
	for (size_t i = 0; i < urls.size(); ++i) {
		UrlList->Append(urls[i].Url.BuildURI());
	}
	if (UrlList->GetCount() > 0) {
		UrlList->Select(0);
	}
	UrlList->Thaw();
}

std::vector<wxFileName> mvceditor::ChooseUrlDialogClass::ActiveSourceDirectories() {
	std::vector<wxFileName> dirs;
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	std::vector<mvceditor::SourceClass>::const_iterator src;
	
	// project 0 is the "all enabled projects"
	int sel = ProjectChoice->GetSelection();
	if (sel >= 1 && sel < (int)ProjectChoice->GetCount()) {
		mvceditor::ProjectClass* project = (mvceditor::ProjectClass*)ProjectChoice->GetClientData(sel);
		for (src = project->Sources.begin(); src != project->Sources.end(); ++src) {
			dirs.push_back(src->RootDirectory);
		}
	}
	else {
		for (project = Projects.begin(); project != Projects.end(); ++project) {
			for (src = project->Sources.begin(); src != project->Sources.end(); ++src) {
				dirs.push_back(src->RootDirectory);
			}
		}
	}
	return dirs;
}
