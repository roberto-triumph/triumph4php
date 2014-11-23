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
#include <globals/Number.h>

t4p::ChooseUrlDialogClass::ChooseUrlDialogClass(wxWindow* parent, 
													  t4p::UrlTagFinderClass& urls,
													  const std::vector<t4p::ProjectClass>& projects,
													  t4p::UrlTagClass& chosenUrl)
	: ChooseUrlDialogGeneratedClass(parent, wxID_ANY)
	, UrlTagFinder(urls)
	, ChosenUrl(chosenUrl) 
	, Projects() {

	// get some urls to prepopulate the list
	std::vector<t4p::UrlTagClass> allUrlTags = GetFilteredUrls(wxT("http://"));
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

void t4p::ChooseUrlDialogClass::OnOkButton(wxCommandEvent& event) {
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

void t4p::ChooseUrlDialogClass::OnListItemDoubleClicked(wxCommandEvent& event) {
	OnOkButton(event);	
}

void t4p::ChooseUrlDialogClass::OnListItemSelected(wxCommandEvent& event) {
	wxString finalUrl = UrlList->GetStringSelection() + ExtraText->GetValue();
	FinalUrlLabel->SetLabel(finalUrl);
	event.Skip();
}

void t4p::ChooseUrlDialogClass::OnFilterText(wxCommandEvent& event) {
	wxString filter = Filter->GetValue();
	if (filter.IsEmpty()) {

		// filter string cannot be empty
		// empty string =  no filter show all
		filter = wxT("http://");
	}

	std::vector<t4p::UrlTagClass> filteredUrls;
	
	// project 0 is the "all enabled projects"
	int sel = ProjectChoice->GetSelection();
	if (sel >= 1 && t4p::NumberLessThan(sel, ProjectChoice->GetCount())) {
		t4p::ProjectClass* project = (t4p::ProjectClass*)ProjectChoice->GetClientData(sel);
		filteredUrls = GetFilteredUrlsByProject(filter, *project);
	}
	else {
		filteredUrls = GetFilteredUrls(filter);
	}
	FillUrlList(filteredUrls);

	wxString finalUrl = UrlList->GetStringSelection() + ExtraText->GetValue();
	FinalUrlLabel->SetLabel(finalUrl);
}

void t4p::ChooseUrlDialogClass::OnFilterTextEnter(wxCommandEvent& event) {
	OnOkButton(event);	
}

void t4p::ChooseUrlDialogClass::OnFilterKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_DOWN && event.GetModifiers() == wxMOD_NONE) {
		int selection = UrlList->GetSelection();
		if (selection >= 0 && t4p::NumberLessThan(selection, UrlList->GetCount() - 1)) {
			UrlList->SetSelection(selection  + 1);
		}
		Filter->SetFocus();
	}
	else if (event.GetKeyCode() == WXK_UP && event.GetModifiers() == wxMOD_NONE) {
		int selection = UrlList->GetSelection();
		if (selection > 0 && t4p::NumberLessThan(selection, UrlList->GetCount())) {
			UrlList->SetSelection(selection  - 1);
		}
		Filter->SetFocus();
	}
	else {
		event.Skip();
	}
}

void t4p::ChooseUrlDialogClass::OnExtraText(wxCommandEvent& event) {
	wxString finalUrl = UrlList->GetStringSelection() + ExtraText->GetValue();
	FinalUrlLabel->SetLabel(finalUrl);
	event.Skip();
}

void t4p::ChooseUrlDialogClass::OnExtraChar(wxKeyEvent& event) {
	wxString finalUrl = UrlList->GetStringSelection() + ExtraText->GetValue();
	FinalUrlLabel->SetLabel(finalUrl);
	event.Skip();
}

void t4p::ChooseUrlDialogClass::OnProjectChoice(wxCommandEvent& event) {
	int sel = event.GetSelection();
	wxString filter = Filter->GetValue();

	std::vector<t4p::UrlTagClass> filteredUrls;

	// project 0 is the "all enabled projects"
	if (sel >= 1 && t4p::NumberLessThan(sel, ProjectChoice->GetCount())) {
		t4p::ProjectClass* project = (t4p::ProjectClass*)ProjectChoice->GetClientData(sel);
		filteredUrls = GetFilteredUrlsByProject(filter, *project);
	}
	else {
		filteredUrls = GetFilteredUrls(filter);
	}
	FillUrlList(filteredUrls);

	wxString finalUrl = UrlList->GetStringSelection() + ExtraText->GetValue();
	FinalUrlLabel->SetLabel(finalUrl);
}

std::vector<t4p::UrlTagClass> t4p::ChooseUrlDialogClass::GetFilteredUrls(const wxString& filter) {
	std::vector<t4p::UrlTagClass> filteredUrls;
	std::vector<wxFileName> sourceDirs = ActiveSourceDirectories();
	UrlTagFinder.FilterUrls(filter, sourceDirs, filteredUrls);
	return filteredUrls;
}


std::vector<t4p::UrlTagClass> t4p::ChooseUrlDialogClass::GetFilteredUrlsByProject(const wxString& filter, const t4p::ProjectClass& project) {
	std::vector<wxFileName> dirs;
	std::vector<t4p::SourceClass>::const_iterator src;
	for (src = project.Sources.begin(); src != project.Sources.end(); ++src) {
		dirs.push_back(src->RootDirectory);
	}
	
	std::vector<t4p::UrlTagClass> filteredUrls;
	UrlTagFinder.FilterUrls(filter, dirs, filteredUrls);

	// check that the controller is part of the project's sources
	std::vector<t4p::UrlTagClass>::iterator url = filteredUrls.begin();
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

void t4p::ChooseUrlDialogClass::FillUrlList(const std::vector<t4p::UrlTagClass>& urls) {
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

std::vector<wxFileName> t4p::ChooseUrlDialogClass::ActiveSourceDirectories() {
	std::vector<wxFileName> dirs;
	std::vector<t4p::ProjectClass>::const_iterator project;
	std::vector<t4p::SourceClass>::const_iterator src;
	
	// project 0 is the "all enabled projects"
	int sel = ProjectChoice->GetSelection();
	if (sel >= 1 && t4p::NumberLessThan(sel, ProjectChoice->GetCount())) {
		t4p::ProjectClass* project = (t4p::ProjectClass*)ProjectChoice->GetClientData(sel);
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
