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
#include <plugins/FindInFilesPluginClass.h>
#include <MvcEditorString.h>
#include <widgets/UnicodeStringValidatorClass.h>
#include <widgets/RegularExpressionValidatorClass.h>
#include <MvcEditorErrors.h>
#include <MvcEditor.h>
#include <wx/artprov.h>
#include <wx/clipbrd.h>
#include <wx/ffile.h>
#include <wx/textfile.h>
#include <wx/valgen.h>
#include <algorithm>

// these IDs are needed so that the IDs of the Regular expression help menu
// do not collide with the menu IDs of the FinderPlugin
static const int ID_REGEX_MENU_START = 9000;
static const int ID_REGEX_REPLACE_MENU_START = 10000;

mvceditor::FindInFilesHitClass::FindInFilesHitClass()
	: FileName()
	, Preview()
	, LineNumber() {

}

mvceditor::FindInFilesHitClass::FindInFilesHitClass(const wxString& fileName, const wxString& preview, int lineNumber)
	: FileName(fileName)
	, Preview(preview)
	, LineNumber(lineNumber) {

}

mvceditor::FindInFilesHitEventClass::FindInFilesHitEventClass(const std::vector<mvceditor::FindInFilesHitClass> &hits)
	: wxEvent(wxID_ANY, mvceditor::EVENT_FIND_IN_FILES_FILE_HIT)
	, Hits(hits) {
}

wxEvent* mvceditor::FindInFilesHitEventClass::Clone() const {
	wxEvent* newEvt = new mvceditor::FindInFilesHitEventClass(Hits);
	return newEvt;
}

mvceditor::FindInFilesBackgroundReaderClass::FindInFilesBackgroundReaderClass(wxEvtHandler& handler, mvceditor::RunningThreadsClass& runningThreads) 
	: BackgroundFileReaderClass(handler, runningThreads) 
	, FindInFiles()
	, Hits()
	, SkipFiles() {

}

bool mvceditor::FindInFilesBackgroundReaderClass::InitForFind( mvceditor::FindInFilesClass findInFiles, 
															  bool doHiddenFiles,
															  std::vector<wxString> skipFiles) {

	// find in files needs to be a copy; just to be sure
	// its thread safe
	FindInFiles = findInFiles;
	SkipFiles = skipFiles;

	std::vector<mvceditor::SourceClass> sources;
	sources.push_back(FindInFiles.Source);
	return Init(sources, mvceditor::DirectorySearchClass::RECURSIVE, doHiddenFiles) && FindInFiles.Prepare();
}

bool mvceditor::FindInFilesBackgroundReaderClass::InitForReplace(mvceditor::FindInFilesClass findInFiles,
																 const std::vector<mvceditor::FindInFilesHitClass>& hits,
																 std::vector<wxString> skipFiles) {
	FindInFiles = findInFiles;
	Hits = hits;
	SkipFiles = skipFiles;
	return InitMatched();
}

bool mvceditor::FindInFilesBackgroundReaderClass::BackgroundFileRead(DirectorySearchClass& search) {
	bool found = false;
	found = search.Walk(FindInFiles);
	if (found) {
		wxString fileName = search.GetMatchedFiles().back();
		
		// if this match is for one of the skip files then we want to ignore it
		// DirectorySearch doesn't have a GetCurrentFile() so the one way to know the
		// file that was searched is to do the search
		std::vector<wxString>::iterator it = find(SkipFiles.begin(), SkipFiles.end(), fileName);
		std::vector<mvceditor::FindInFilesHitClass> hits;
		if (it == SkipFiles.end()) {
			bool destroy = TestDestroy();
			do {			
				if (destroy) {
					break;
				}
				mvceditor::FindInFilesHitClass hit(fileName, 
					mvceditor::IcuToWx(FindInFiles.GetCurrentLine()), 
					FindInFiles.GetCurrentLineNumber());
				hits.push_back(hit);

			}
			while (!destroy && FindInFiles.FindNext());
			if (!destroy && !hits.empty()) {
				mvceditor::FindInFilesHitEventClass hitEvent(hits);
				wxPostEvent(&Handler, hitEvent);
			}
		}
	}
	return found;
}

bool mvceditor::FindInFilesBackgroundReaderClass::BackgroundFileMatch(const wxString& file) {
	wxString fileToReplace = file;
	int matches = 0;

	// don't do replace for open files
	std::vector<wxString>::iterator it = find(SkipFiles.begin(), SkipFiles.end(), fileToReplace);
	if (it == SkipFiles.end()) {
		matches += FindInFiles.ReplaceAllMatchesInFile(fileToReplace);
	}
	return matches > 0;
}
	
mvceditor::FindInFilesResultsPanelClass::FindInFilesResultsPanelClass(wxWindow* parent, NotebookClass* notebook, 
		StatusBarWithGaugeClass* gauge, mvceditor::RunningThreadsClass& runningThreads)
	: FindInFilesResultsPanelGeneratedClass(parent)
	, FindInFiles()
	, RunningThreads(runningThreads)
	, FindInFilesBackgroundFileReader(NULL)
	, Notebook(notebook)
	, Gauge(gauge)
	, MatchedFiles(0) {
	FindInFilesGaugeId = wxNewId();
}

mvceditor::FindInFilesResultsPanelClass::~FindInFilesResultsPanelClass() {

	// make sure we kill any running searches
	if (FindInFilesBackgroundFileReader) {
		FindInFilesBackgroundFileReader->StopReading();
		
		// dont try to kill the gauge as the gauge window may not valid anymore
		// if the program is closed while the find is running
	}
}

void mvceditor::FindInFilesResultsPanelClass::Find(const FindInFilesClass& findInFiles, bool doHiddenFiles) {
	FindInFiles = findInFiles;
	MatchedFiles = 0;

	// for now disallow another find when one is already active
	if (NULL != FindInFilesBackgroundFileReader) {
		wxMessageBox(_("Find in files is already running. Please wait for it to finish."), _("Find In Files"));
		return;
	}
	std::vector<wxString> skipFiles = Notebook->GetOpenedFiles();
	FindInFilesBackgroundFileReader = new mvceditor::FindInFilesBackgroundReaderClass(*this, RunningThreads);
	if (FindInFilesBackgroundFileReader->InitForFind(FindInFiles, doHiddenFiles, skipFiles)) {
		mvceditor::BackgroundFileReaderClass::StartError error;
		if (FindInFilesBackgroundFileReader->StartReading(error)) {
			EnableButtons(true, false, false);
			Gauge->AddGauge(_("Find In Files"), FindInFilesGaugeId, StatusBarWithGaugeClass::INDETERMINATE_MODE, 
				wxGA_HORIZONTAL);
			SetStatus(_("Searching"));

			// lets do the find in the opened files ourselves so that the hits are not stale
			FindInOpenedFiles();			
		}
		else if (error == mvceditor::BackgroundFileReaderClass::ALREADY_RUNNING)  {
			wxMessageBox(_("Find in files is already running. Please wait for it to finish."), _("Find In Files"));
			delete FindInFilesBackgroundFileReader;
			FindInFilesBackgroundFileReader = NULL;
		}
		else if (error == mvceditor::BackgroundFileReaderClass::NO_RESOURCES)  {
			mvceditor::EditorLogError(mvceditor::LOW_RESOURCES);
			delete FindInFilesBackgroundFileReader;
			FindInFilesBackgroundFileReader = NULL;
		}
	}
	else {
		wxMessageBox(_("Please enter a valid expression and path."));
		delete FindInFilesBackgroundFileReader;
		FindInFilesBackgroundFileReader = NULL;
	}
}

void mvceditor::FindInFilesResultsPanelClass::FindInOpenedFiles() {
	FinderClass finder;
	FindInFiles.CopyFinder(finder);
	if (FindInFiles.Prepare() && finder.Prepare()) {

		// search the opened files
		for (size_t i = 0; i < Notebook->GetPageCount(); ++i) {
			CodeControlClass* codeControl = Notebook->GetCodeControl(i);
			wxString fileName = codeControl->GetFileName();

			// make sure to respect the wildcard filter and the find path here too
			if (FindInFiles.Source.Contains(fileName)) {
				UnicodeString text = codeControl->GetSafeText();
				int32_t next = 0;
				int32_t charPos = 0,
					length = 0;
				std::vector<mvceditor::FindInFilesHitClass> hits;
				while (finder.FindNext(text, next)) {
					if (finder.GetLastMatch(charPos, length)) {
						int lineNumber = codeControl->LineFromCharacter(charPos);
						
						int start = codeControl->PositionFromLine(lineNumber);
						int end = codeControl->GetLineEndPosition(lineNumber);
						wxString lineText = codeControl->GetTextRange(start, end);

						// lineNumber is zero-based but we want to display it as 1-based
						lineNumber++;
						mvceditor::FindInFilesHitClass hit(fileName, lineText, lineNumber);
						hits.push_back(hit);
						next = charPos + length;
					}
					else {
						break;
					}
				}
				if (!hits.empty()) {
					mvceditor::FindInFilesHitEventClass hitEvent(hits);
					wxPostEvent(this, hitEvent);
				}
			}
		}
	}
}

void mvceditor::FindInFilesResultsPanelClass::ShowNextMatch() {
	wxArrayInt selections;
	ResultsList->GetSelections(selections);
	ResultsList->DeselectAll();
	int sel;
	if (selections.GetCount() > 0 && selections[0] < ((int)ResultsList->GetCount() - 1)) {
		sel = selections[0] + 1;
	}
	else {
		// loop back to the beginning
		sel = 0;
	}
	ShowMatch(sel);
	ResultsList->SetSelection(sel);
}

void mvceditor::FindInFilesResultsPanelClass::ShowPreviousMatch() {
	wxArrayInt selections;
	ResultsList->GetSelections(selections);
	ResultsList->DeselectAll();
	int sel;
	if (selections.GetCount() > 0 && selections[0] > 0) {
		sel = selections[0] - 1;
	}
	else {
		// loop back to the end
		sel = ResultsList->GetCount() - 1;
	}
	ShowMatch(sel);
	ResultsList->SetSelection(sel);
}

void mvceditor::FindInFilesResultsPanelClass::OnReplaceButton(wxCommandEvent& event) {
	CodeControlClass* codeControl = 
			Notebook->GetCodeControl(Notebook->GetSelection());	
			
	// if user changed tab, GetLastReplacementText would return false and nothing will be replaced
	UnicodeString text = codeControl->GetSafeText();
	if (codeControl) {		
		int32_t position = codeControl->GetCurrentPos(),
			length = 0;
		UnicodeString replaceText;
		FinderClass finder;
		FindInFiles.CopyFinder(finder);
		if (finder.Prepare()) {
			
			// only replace when the cursor is at the hit (which would be the case when and only when the use
			// double clicked on the result list (and focused on the hit).
			if (finder.FindNext(text, position) && finder.GetLastMatch(position, length) && 
				position == codeControl->GetCurrentPos() &&
				finder.GetLastReplacementText(text, replaceText)) {
				codeControl->SetSelectionAndEnsureVisible(position, position + length);
				codeControl->ReplaceSelection(mvceditor::IcuToWx(replaceText));
				codeControl->SetSelectionByCharacterPosition(position, position + replaceText.length());
			}
		}
	}
}

void mvceditor::FindInFilesResultsPanelClass::OnReplaceAllInFileButton(wxCommandEvent& event) {
	CodeControlClass* codeControl = 
			Notebook->GetCodeControl(Notebook->GetSelection());	
			
	// if user changed tab, the new tab would be modified; this is clear to the user
	 if (codeControl) {
		UnicodeString text = codeControl->GetSafeText();
		FinderClass finder;
		FindInFiles.CopyFinder(finder);
		if (finder.Prepare()) {
			int matches = finder.ReplaceAllMatches(text);
			codeControl->SetUnicodeText(text);
			SetStatus(wxString::Format(wxT("Status: Replaced %d matches"), matches));	
		}
	 }
}

void mvceditor::FindInFilesResultsPanelClass::OnReplaceInAllFilesButton(wxCommandEvent& event) {

	// for now disallow another replace when one is already active
	if (NULL != FindInFilesBackgroundFileReader) {
		wxMessageBox(_("Find in files is already running. Please wait for it to finish."), _("Find In Files"));
		return;
	}

	if (FindInFiles.Prepare()) {
		FinderClass finder;
		FindInFiles.CopyFinder(finder);
		if (finder.Prepare()) {

			// replace the open files
			for (size_t i = 0; i < Notebook->GetPageCount(); ++i) {
				CodeControlClass* codeControl = Notebook->GetCodeControl(i);
				UnicodeString text = codeControl->GetSafeText();
			
				// only update code control when there are replacements made
				if(finder.ReplaceAllMatches(text) > 0) {
					codeControl->SetUnicodeText(text);
				}
			}
		}
		
		// we've already searched, when replacing we should iterate through matched files hence we don't call DirectorySearch,.Init().
		FindInFilesBackgroundFileReader = new mvceditor::FindInFilesBackgroundReaderClass(*this, RunningThreads);
		FindInFilesBackgroundFileReader->InitForReplace(FindInFiles, AllHits, Notebook->GetOpenedFiles());
		mvceditor::BackgroundFileReaderClass::StartError error;
		if (FindInFilesBackgroundFileReader->StartReading(error)) {
			SetStatus(_("Find In Files In Progress"));
			Gauge->AddGauge(_("Find In Files"), FindInFilesGaugeId, StatusBarWithGaugeClass::INDETERMINATE_MODE, 
				wxGA_HORIZONTAL);
			EnableButtons(true, false, false);
			SetStatus(_("Replacing"));
		}
		else if (error == mvceditor::BackgroundFileReaderClass::ALREADY_RUNNING)  {
			wxMessageBox(_("Find in files is already running. Please wait for it to finish."), _("Find In Files"));
			delete FindInFilesBackgroundFileReader;
			FindInFilesBackgroundFileReader = NULL;
		}
		else if (error == mvceditor::BackgroundFileReaderClass::NO_RESOURCES)  {
			mvceditor::EditorLogError(mvceditor::LOW_RESOURCES);
			delete FindInFilesBackgroundFileReader;
			FindInFilesBackgroundFileReader = NULL;
		}
	}
	else {
		wxMessageBox(_("Please enter a valid expression and path."));
	}
}

void mvceditor::FindInFilesResultsPanelClass::OnFindInFilesComplete(wxCommandEvent& event) {
	int matchedFilesSize = GetNumberOfMatchedFiles();
	bool enableReplace = matchedFilesSize > 0 && !FindInFiles.ReplaceExpression.isEmpty();
	bool enableCopy = matchedFilesSize > 0;
	EnableButtons(false, enableReplace, enableCopy);
	Gauge->StopGauge(FindInFilesGaugeId);
	if (event.GetInt() == BackgroundFileReaderClass::WALK) {
		if (matchedFilesSize > 0) {
			SetStatus(wxString::Format(wxT("Found Matches in %d files"), matchedFilesSize));
		}
		else {
			SetStatus(_("Did not Find any Matches"));
		}
	}
	else if (event.GetEventType() == BackgroundFileReaderClass::MATCHED) {
		if (matchedFilesSize > 0) {
			SetStatus(wxString::Format(wxT("Replaced Matches in %d files"), matchedFilesSize));
		}
		else {
			SetStatus(_("Did not Replace. No Matches Were Found"));
		}
	}
}

void mvceditor::FindInFilesResultsPanelClass::OnWorkComplete(wxCommandEvent& event) {
	FindInFilesBackgroundFileReader = NULL;
}

void mvceditor::FindInFilesResultsPanelClass::OnFileHit(mvceditor::FindInFilesHitEventClass& event) {
	if (event.Hits.empty()) {
		return;
	}
	MatchedFiles++;

	// in case there are any hits
	ResultsList->Freeze();
	for (size_t i = 0; i < event.Hits.size(); ++i) {
		mvceditor::FindInFilesHitClass hit = event.Hits[i];

		// in MSW the list control does not render the \t use another delimiter
		wxString msg = hit.FileName + wxT("\t:") + 
			wxString::Format(wxT("%d"), hit.LineNumber) + wxT("\t:") +
			hit.Preview;
		msg.Trim();
		ResultsList->Append(msg.SubString(0, 200));
	}
	ResultsList->Thaw();
	AllHits.insert(AllHits.end(), event.Hits.begin(), event.Hits.end());
}

void mvceditor::FindInFilesResultsPanelClass::OnStopButton(wxCommandEvent& event) {
	Stop();
}

void mvceditor::FindInFilesResultsPanelClass::Stop() {
	if (NULL != FindInFilesBackgroundFileReader) {
		FindInFilesBackgroundFileReader->StopReading();
	}
	Gauge->StopGauge(FindInFilesGaugeId);
	SetStatus(_("Search stopped"));
	bool enableIterators = MatchedFiles > 0;
	EnableButtons(false, enableIterators, enableIterators);
}

void mvceditor::FindInFilesResultsPanelClass::OnDoubleClick(wxCommandEvent& event) {
	int sel = event.GetSelection();
	ShowMatch(sel);
}

void mvceditor::FindInFilesResultsPanelClass::ShowMatch(int i) {
	wxString result = ResultsList->GetString(i);
	int index = result.Find(wxT("\t:"));
	if (index > -1) {
		wxString fileName = result.substr(0, index);
		long line = 0;
		size_t indexAfterPos = result.find(wxT("\t:"), index + 2);
		if (indexAfterPos != std::string::npos) {
			wxString posString = result.substr(index + 2, indexAfterPos - index);
			posString.ToLong(&line);
			--line; // line is 1-based but wxSTC lines start at zero
		}
		Notebook->LoadPage(fileName);
		CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
		if (codeControl) {
			
			// search for the expression and highlight it. search from the start of the line.
			int32_t pos = codeControl->PositionFromLine(line);
			int32_t length = 0;
			FinderClass finder;
			FindInFiles.CopyFinder(finder);
			if (finder.Prepare()) {
				if (finder.FindNext(codeControl->GetSafeText(), pos) && finder.GetLastMatch(pos, length)) {
					
					//we want the cursor to be in the beginning of the hit hece pos is 2nd parameter
					codeControl->SetCurrentPos(pos);
					codeControl->SetAnchor(pos);
					codeControl->SetSelectionAndEnsureVisible(pos + length, pos);
				}
			}
		}
	}
}

void mvceditor::FindInFilesResultsPanelClass::OnCopySelectedButton(wxCommandEvent& event) {
	wxArrayInt selections;
	if (ResultsList->GetSelections(selections) > 0) {
		wxString selectedItems;
		for (size_t i = 0; i < selections.Count(); i++) {
			selectedItems += ResultsList->GetString(selections[i]);
			selectedItems += wxTextFile::GetEOL();
		}
		if (wxTheClipboard->Open()) {
			wxTheClipboard->SetData(new wxTextDataObject(selectedItems));
			wxTheClipboard->Close();
		}
	}
}

void mvceditor::FindInFilesResultsPanelClass::OnCopyAllButton(wxCommandEvent& event) {
	wxArrayString selectedItemsArray = ResultsList->GetStrings();
	wxString selectedItems;
	for (size_t i = 0; i < selectedItemsArray.Count(); i++) {
		selectedItems += selectedItemsArray[i];
		selectedItems += wxTextFile::GetEOL();
	}
	if (selectedItemsArray.Count() && wxTheClipboard->Open()) {
		wxTheClipboard->SetData(new wxTextDataObject(selectedItems));
		wxTheClipboard->Close();
	}
}

void mvceditor::FindInFilesResultsPanelClass::EnableButtons(bool enableStopButton, bool enableReplaceButtons, bool enableCopyButtons) {
	StopButton->Enable(enableStopButton);
	ReplaceButton->Enable(enableReplaceButtons);
	ReplaceAllInFileButton->Enable(enableReplaceButtons);
	ReplaceInAllFilesButton->Enable(enableReplaceButtons);
	CopySelectedButton->Enable(enableCopyButtons);
	CopyAllButton->Enable(enableCopyButtons);
}

void mvceditor::FindInFilesResultsPanelClass::SetStatus(const wxString& text) {
	
	// label might grow/shrink according to new text, must
	// tell the sizer to re-position the label correctly
	// we need this for the label to be right-aligned after
	// the text change
	ResultText->SetLabel(text);
	ResultText->GetContainingSizer()->Layout();
}

int mvceditor::FindInFilesResultsPanelClass::GetNumberOfMatchedFiles() {
	return MatchedFiles;
}

void mvceditor::FindInFilesResultsPanelClass::OnTimer(wxCommandEvent& event) {
	Gauge->IncrementGauge(FindInFilesGaugeId, StatusBarWithGaugeClass::INDETERMINATE_MODE);	
}

mvceditor::FindInFilesDialogClass::FindInFilesDialogClass(wxWindow* parent, mvceditor::FindInFilesPluginClass& plugin)
	: FindInFilesDialogGeneratedClass(parent, wxID_ANY)
	, Plugin(plugin)
	, CurrentInsertionPointFind(0)
	, CurrentInsertionPointReplace(0) {
	Plugin.FindHistory.Attach(FindText);
	Plugin.ReplaceHistory.Attach(ReplaceWithText);
	Plugin.DirectoriesHistory.Attach(Directory);
	Plugin.FilesHistory.Attach(FilesFilter);
	
	// the first time showing this dialog populate the filter to have only PHP file extensions
	if (FilesFilter->GetCount() <= 0) {
		Plugin.PreviousFindInFiles.Source.SetIncludeWildcards(plugin.App.Structs.PhpFileFiltersString);
	}
	std::vector<mvceditor::SourceClass> sources = plugin.App.Structs.AllEnabledSources();
	for (size_t i = 0; i < sources.size(); ++i) {
		mvceditor::SourceClass src = sources[i];
		wxString fullPath = src.RootDirectory.GetFullPath();
		if (Directory->FindString(fullPath, false) < 0) {
			Directory->AppendString(fullPath);
		}
	}

	// select the last used path by default
	bool isUsed = false;
	int foundIndex = -1;
	wxString lastUsedPath = plugin.PreviousFindInFiles.Source.RootDirectory.GetFullPath();
	if (!lastUsedPath.IsEmpty()) {
		int foundIndex = Directory->FindString(lastUsedPath, false);
		if (foundIndex >= 0) {
			isUsed = true;
		}
	}
	if (isUsed && foundIndex >= 0) {
		Directory->SetSelection(foundIndex);
	}
	else if (Directory->GetCount() > 0) {
		Directory->SetSelection(0);
	}
	mvceditor::RegularExpressionValidatorClass regExValidator(&Plugin.PreviousFindInFiles.Expression, FinderMode);
	FindText->SetValidator(regExValidator);
	UnicodeStringValidatorClass replaceExpressionValidator(&Plugin.PreviousFindInFiles.ReplaceExpression);
	wxGenericValidator modeValidator(&Plugin.PreviousFindInFiles.Mode);
	wxGenericValidator caseValidator(&Plugin.PreviousFindInFiles.CaseSensitive);
	ReplaceWithText->SetValidator(replaceExpressionValidator);
	FinderMode->SetValidator(modeValidator);
	CaseSensitive->SetValidator(caseValidator);
	wxGenericValidator doHiddenFilesValidator(&Plugin.DoHiddenFiles);
	DoHiddenFiles->SetValidator(doHiddenFilesValidator);

	FilesFilter->SetValue(Plugin.PreviousFindInFiles.Source.IncludeWildcardsString());

	FindText->SetFocus();

	// since this panel handles EVT_TEXT_ENTER, we need to handle the
	// tab traversal ourselves otherwise tab travesal wont work
	FindText->GetEventHandler()->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::FindInFilesDialogClass::OnKeyDown));
	ReplaceWithText->GetEventHandler()->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::FindInFilesDialogClass::OnKeyDown));
	FilesFilter->GetEventHandler()->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::FindInFilesDialogClass::OnKeyDown));
	
	// connect to the KILL_FOCUS events so that we can capture the insertion point
	// on Win32 GetInsertionPoint() returns 0 when the combo box is no
	// in focus; we must receive the position via an outside mechanism
	FindText->GetEventHandler()->Connect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(FindInFilesDialogClass::OnKillFocusFindText), NULL, this);
	ReplaceWithText->GetEventHandler()->Connect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(FindInFilesDialogClass::OnKillFocusReplaceText), NULL, this);
}

mvceditor::FindInFilesDialogClass::~FindInFilesDialogClass() {
	FindText->GetEventHandler()->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::FindInFilesDialogClass::OnKeyDown));
	ReplaceWithText->GetEventHandler()->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::FindInFilesDialogClass::OnKeyDown));
	FilesFilter->GetEventHandler()->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::FindInFilesDialogClass::OnKeyDown));

	FindText->GetEventHandler()->Disconnect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(FindInFilesDialogClass::OnKillFocusFindText), NULL, this);
	ReplaceWithText->GetEventHandler()->Disconnect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(FindInFilesDialogClass::OnKillFocusReplaceText), NULL, this);
}

void mvceditor::FindInFilesDialogClass::OnOkButton(wxCommandEvent& event) {
	if (TransferDataFromWindow()) {
		Plugin.PreviousFindInFiles.Source.SetIncludeWildcards(FilesFilter->GetValue());
		if (!Plugin.PreviousFindInFiles.Prepare()) {
			wxMessageBox(_("Expression is not valid."), _("Find In Files"), wxOK | wxCENTER, this);
		}
		else if (Directory->GetValue().IsEmpty()) {
			wxMessageBox(_("Find path must not be empty."), _("Find In Files"), wxOK | wxCENTER, this);
		}
		else {
			Plugin.PreviousFindInFiles.Source.RootDirectory.AssignDir(Directory->GetValue());
			Plugin.FindHistory.Save();
			Plugin.ReplaceHistory.Save();
			Plugin.DirectoriesHistory.Save();
			Plugin.FilesHistory.Save();

			Plugin.FindHistory.Detach();
			Plugin.ReplaceHistory.Detach();
			Plugin.DirectoriesHistory.Detach();
			Plugin.FilesHistory.Detach();
			EndModal(wxID_OK);
		}
	}
}

void mvceditor::FindInFilesDialogClass::OnCancelButton(wxCommandEvent& event) {

	// need to do this to prevent crash on app exit
	Plugin.FindHistory.Detach();
	Plugin.ReplaceHistory.Detach();
	Plugin.DirectoriesHistory.Detach();
	Plugin.FilesHistory.Detach();
	EndModal(wxID_CANCEL);
}

void mvceditor::FindInFilesDialogClass::OnDirChanged(wxFileDirPickerEvent& event) {
	wxString path = event.GetPath();

	// add the selected directory, but only if its not already in the list
	int index = Directory->FindString(path);
	if (wxNOT_FOUND != index && !path.IsEmpty()) {
		Directory->SetSelection(index);
	}
	else {
		int newIndex = Directory->Append(path);
		Directory->SetSelection(newIndex);
	}
}

void mvceditor::FindInFilesDialogClass::OnKeyDown(wxKeyEvent& event) {

	// warning: don't use "this"; look at the way this event has been connected 
	if (event.GetKeyCode() == WXK_TAB && event.ShiftDown()) {
		Navigate(wxNavigationKeyEvent::IsBackward);
	}
	else if (event.GetKeyCode() == WXK_TAB ) {
		Navigate(wxNavigationKeyEvent::IsForward);
	}
	else {
		event.Skip();
	}
}

void mvceditor::FindInFilesDialogClass::OnKillFocusFindText(wxFocusEvent& event) {
	CurrentInsertionPointFind = FindText->GetInsertionPoint();
	event.Skip();
}

void mvceditor::FindInFilesDialogClass::OnKillFocusReplaceText(wxFocusEvent& event) {
	CurrentInsertionPointReplace = ReplaceWithText->GetInsertionPoint();
	event.Skip();
}

mvceditor::FindInFilesPluginClass::FindInFilesPluginClass(mvceditor::AppClass& app)
	: PluginClass(app)
	, PreviousFindInFiles()
	, DoHiddenFiles(false)
	, ResultsPanels() {
}

void mvceditor::FindInFilesPluginClass::AddSearchMenuItems(wxMenu* searchMenu) {
	searchMenu->Append(mvceditor::MENU_FIND_IN_FILES + 0, _("Find In Files\tCTRL+SHIFT+F"), 
		_("Find an expression by searching entire directory contents"));
	searchMenu->Append(mvceditor::MENU_FIND_IN_FILES + 1, _("Next Find In Files Match\tALT+F3"), 
		_("Move the cursor to the next Find In Files Match"));
	searchMenu->Append(mvceditor::MENU_FIND_IN_FILES + 2, _("Previous Find In Files Match\tALT+SHIFT+F3"),
		_("Move the cursor to the previous Find In Files Match"));
}

void mvceditor::FindInFilesPluginClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_FIND_IN_FILES + 0] = wxT("Find-Find In Files");
	menuItemIds[mvceditor::MENU_FIND_IN_FILES + 1] = wxT("Find-Find In Files Next Match");
	menuItemIds[mvceditor::MENU_FIND_IN_FILES + 2] = wxT("Find-Find In Files Previous Match");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::FindInFilesPluginClass::OnEditFindInFiles(wxCommandEvent& event) {

	// prime finder with selected text
	wxString selectedText = GetSelectedText();
	if (!selectedText.empty()) {
		PreviousFindInFiles.Expression = mvceditor::WxToIcu(selectedText);
	}
	FindInFilesDialogClass dialog(NULL, *this);
	if (dialog.ShowModal() == wxID_OK) {
		mvceditor::FindInFilesResultsPanelClass* panel = new mvceditor::FindInFilesResultsPanelClass(GetToolsNotebook(), 
			GetNotebook(), GetStatusBarWithGauge(), App.RunningThreads);		
		if(AddToolsWindow(panel, _("Find In Files Results"))) {
			panel->Find(PreviousFindInFiles, DoHiddenFiles);
			ResultsPanels.push_back(panel);
		}
	}
}

void mvceditor::FindInFilesPluginClass::OnEditFindInFilesNext(wxCommandEvent& event) {
	wxAuiNotebook* notebook = GetToolsNotebook();
	int selection = notebook->GetSelection();
	wxWindow* window = notebook->GetPage(selection);
	std::vector<mvceditor::FindInFilesResultsPanelClass*>::iterator it = ResultsPanels.begin();
	while (it != ResultsPanels.end()) {
		if (*it == window) {
			(*it)->ShowNextMatch();
			break;
		}
		else {
			++it;
		}
	}
}

void mvceditor::FindInFilesPluginClass::OnEditFindInFilesPrevious(wxCommandEvent& event) {
	wxAuiNotebook* notebook = GetToolsNotebook();
	int selection = notebook->GetSelection();
	wxWindow* window = notebook->GetPage(selection);
	std::vector<mvceditor::FindInFilesResultsPanelClass*>::iterator it = ResultsPanels.begin();
	while (it != ResultsPanels.end()) {
		if (*it == window) {
			(*it)->ShowPreviousMatch();
			break;
		}
		else {
			++it;
		}
	}
}

void mvceditor::FindInFilesPluginClass::OnToolsNotebookPageClosed(wxAuiNotebookEvent& event) {
	int selection = event.GetSelection();
	wxAuiNotebook* notebook = GetToolsNotebook();
	wxWindow* window = notebook->GetPage(selection);
	std::vector<mvceditor::FindInFilesResultsPanelClass*>::iterator it = ResultsPanels.begin();
	while (it != ResultsPanels.end()) {
		if (*it == window) {
			(*it)->Stop();
			it = ResultsPanels.erase(it);
		}
		else {
			++it;
		}
	}
}

void mvceditor::FindInFilesDialogClass::OnRegExFindHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	mvceditor::PopulateRegExFindMenu(regExMenu, ID_REGEX_MENU_START);
	PopupMenu(&regExMenu);	
}

void mvceditor::FindInFilesDialogClass::OnRegExReplaceHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	mvceditor::PopulateRegExReplaceMenu(regExMenu, ID_REGEX_REPLACE_MENU_START);
	PopupMenu(&regExMenu);	
}

void mvceditor::FindInFilesDialogClass::InsertRegExSymbol(wxCommandEvent& event) {
	int id = event.GetId() - ID_REGEX_MENU_START;
	mvceditor::AddSymbolToRegularExpression(FindText, id, CurrentInsertionPointFind);
	FinderMode->SetSelection(FinderClass::REGULAR_EXPRESSION);
	event.Skip();
}

void mvceditor::FindInFilesDialogClass::InsertReplaceRegExSymbol(wxCommandEvent& event) {
	int id = event.GetId() - ID_REGEX_REPLACE_MENU_START;
	mvceditor::AddSymbolToReplaceRegularExpression(ReplaceWithText, id, CurrentInsertionPointReplace);
	FinderMode->SetSelection(FinderClass::REGULAR_EXPRESSION);
}

BEGIN_EVENT_TABLE(mvceditor::FindInFilesResultsPanelClass, FindInFilesResultsPanelGeneratedClass)
	
	// remove this handler; when searching many files the GUI is redrawn constantly and doesn't
	// look smooth
	//EVT_COMMAND(wxID_ANY, EVENT_FILE_READ, mvceditor::FindInFilesResultsPanelClass::OnFileSearched)
	EVT_FIND_IN_FILES_HITS(mvceditor::FindInFilesResultsPanelClass::OnFileHit)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FILE_READ_COMPLETE, mvceditor::FindInFilesResultsPanelClass::OnFindInFilesComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::FindInFilesResultsPanelClass::OnTimer)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::FindInFilesResultsPanelClass::OnWorkComplete)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::FindInFilesPluginClass, wxEvtHandler)
	EVT_MENU(mvceditor::MENU_FIND_IN_FILES + 0, mvceditor::FindInFilesPluginClass::OnEditFindInFiles)
	EVT_MENU(mvceditor::MENU_FIND_IN_FILES + 1, mvceditor::FindInFilesPluginClass::OnEditFindInFilesNext)
	EVT_MENU(mvceditor::MENU_FIND_IN_FILES + 2, mvceditor::FindInFilesPluginClass::OnEditFindInFilesPrevious)
	EVT_AUINOTEBOOK_PAGE_CLOSE(mvceditor::ID_TOOLS_NOTEBOOK, mvceditor::FindInFilesPluginClass::OnToolsNotebookPageClosed)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::FindInFilesDialogClass, FindInFilesDialogGeneratedClass)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_ONE, mvceditor::FindInFilesDialogClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_TWO, mvceditor::FindInFilesDialogClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_THREE, mvceditor::FindInFilesDialogClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_FOUR, mvceditor::FindInFilesDialogClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_FIVE, mvceditor::FindInFilesDialogClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_ONE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_ZERO_OR_ONE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_EXACT, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_AT_LEAST, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_BETWEEN, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_BEGIN_OF_LINE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_END_OF_LINE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_DIGIT, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_WHITE_SPACE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_ALPHANUMERIC, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_NOT_DECIMAL, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_NOT_WHITE_SPACE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_NOT_ALPHANUMERIC, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_CASE_SENSITIVE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_COMMENT, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_DOT_ALL, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_MULTI_LINE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_UWORD, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_STRING, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_VARIABLE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_NUMBER, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_WHITESPACE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
END_EVENT_TABLE()