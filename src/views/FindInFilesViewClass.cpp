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
 *f
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <views/FindInFilesViewClass.h>
#include <features/FindInFilesFeatureClass.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <globals/Number.h>
#include <widgets/UnicodeStringValidatorClass.h>
#include <widgets/RegularExpressionValidatorClass.h>
#include <widgets/DataViewGrid.h>
#include <globals/Errors.h>
#include <Triumph.h>
#include <wx/artprov.h>
#include <wx/clipbrd.h>
#include <wx/ffile.h>
#include <wx/textfile.h>
#include <wx/valgen.h>
#include <algorithm>

// these IDs are needed so that the IDs of the Regular expression help menu
// do not collide with the menu IDs of the FinderFeature
static const int ID_REGEX_MENU_START = 9000;
static const int ID_REGEX_REPLACE_MENU_START = 10000;

// maximuum number of find in files hits to be found; if there are more hits
// than this the search will stop.  This is done because the user cannot 
// possibly go through all hits; plus this program will allocate too much
// memory
static const size_t MAX_HITS = 1000;


class FindInFilesPreviewRenderer : public wxDataViewCustomRenderer {

public:
	
	t4p::FindInFilesHitClass Hit;
	
	FindInFilesPreviewRenderer()
	: wxDataViewCustomRenderer(wxT("t4p::FindInFilesHitClass")) {
		
	}
	
	bool SetValue(const wxVariant& value) {
		bool ret = true;
		wxAny any = value.GetAny();
		ret = any.GetAs(&Hit);
		return ret;
	}
	
	bool GetValue(wxVariant& value) const {
		bool ret = true;
		wxAny any = value.GetAny();
		any = Hit;
		return ret;
	}
	
	wxSize GetSize() const {
		wxString preview = Hit.Preview;
		preview.Trim(true);
		preview.Trim(false);
		return GetTextExtent(preview);
	}
	
	bool Render(wxRect rect, wxDC* dc, int state) {
		bool ret = true;
		wxString preview = Hit.Preview;
		
		// we want to trim the leading whitespace so that the
		// preview text is aligned left; however trimming the
		// leading spaces means that we need to account for the
		// number of spaces we trimmed so that we split
		// the before/after hit string correctly
		size_t spaceCount = 0;
		for (; spaceCount < preview.length(); ++spaceCount) {
			if (preview[spaceCount] != ' ' && preview[spaceCount] != '\t' 
				&& preview[spaceCount] != '\v'
				&& preview[spaceCount] != '\r' && preview[spaceCount] != '\n') {
				break;
			}
		}
		
		preview.Trim(false);
		
		// break the preview into before/during/after hit.
		// taking into account the spaces that were trimmed 
		// from the beginning of the string
		wxString beforeHit = preview.Mid(0, Hit.LineOffset - spaceCount);
		wxString hit = preview.Mid(Hit.LineOffset - spaceCount, Hit.MatchLength);
		wxString afterHit = preview.Mid(Hit.LineOffset + Hit.MatchLength - spaceCount);
		
		wxSize sizeBeforeHit = GetTextExtent(beforeHit);
		wxSize sizeHit = GetTextExtent(hit);
		
		wxPoint posBeforeHit = rect.GetTopLeft();
		wxPoint posHit = rect.GetTopLeft();
		posHit.x += sizeBeforeHit.GetWidth();
		wxPoint posAfterHit = rect.GetTopLeft();
		posAfterHit.x += sizeBeforeHit.GetWidth() + sizeHit.GetWidth();
		
		
		dc->DrawText(beforeHit, posBeforeHit);
		dc->SetBackgroundMode(wxSOLID);
		dc->SetTextBackground(*wxYELLOW);
		dc->DrawText(hit, posHit);
		dc->SetBackgroundMode(wxTRANSPARENT);
		dc->DrawText(afterHit, posAfterHit);
		
		return ret;
	}
};

static std::vector<wxString> FilesFromHits(const std::vector<t4p::FindInFilesHitClass>& allHits) {
	std::vector<wxString>  files;
	std::vector<t4p::FindInFilesHitClass>::const_iterator hit;
	for (hit = allHits.begin(); hit != allHits.end(); ++hit) {
		files.push_back(hit->FileName);
	}
	if (!files.empty()) {
		std::vector<wxString>::iterator newEnd = std::unique(files.begin(), files.end());
		files.resize(newEnd - files.begin());
	}
	return files;
}

t4p::FindInFilesResultsPanelClass::FindInFilesResultsPanelClass(wxWindow* parent, NotebookClass* notebook, 
		StatusBarWithGaugeClass* gauge, t4p::RunningThreadsClass& runningThreads)
	: FindInFilesResultsPanelGeneratedClass(parent)
	, FindInFiles()
	, RunningThreads(runningThreads)
	, Notebook(notebook)
	, Gauge(gauge)
	, MatchedFiles(0) 
	, RunningActionId(0) {
	FindInFilesGaugeId = wxNewId();
	RunningThreads.AddEventHandler(this);

	ReplaceButton->SetBitmapLabel(t4p::BitmapImageButtonPrepAsset(wxT("replace")));
	ReplaceAllInFileButton->SetBitmapLabel(t4p::BitmapImageButtonPrepAsset(wxT("replace-file")));
	ReplaceInAllFilesButton->SetBitmapLabel(t4p::BitmapImageButtonPrepAsset(wxT("replace-all")));
	PreviousHitButton->SetBitmapLabel(t4p::BitmapImageButtonPrepAsset(wxT("previous")));
	NextHitButton->SetBitmapLabel(t4p::BitmapImageButtonPrepAsset(wxT("next")));
	StopButton->SetBitmapLabel(t4p::BitmapImageButtonPrepAsset(wxT("stop")));
	CopySelectedButton->SetBitmapLabel(t4p::BitmapImageButtonPrepAsset(wxT("copy")));
	CopyAllButton->SetBitmapLabel(t4p::BitmapImageButtonPrepAsset(wxT("copy-all")));
	
	ResultsList->AppendTextColumn(_("File"), wxDATAVIEW_CELL_INERT, 
		wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	ResultsList->AppendTextColumn(_("Line Number"), wxDATAVIEW_CELL_INERT, 
		wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	
	FindInFilesPreviewRenderer* previewRenderer = new FindInFilesPreviewRenderer();
	wxDataViewColumn* previewColumn = new wxDataViewColumn(_("Preview"),
		previewRenderer, 2, wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	ResultsList->AppendColumn(previewColumn, wxT("t4p::FindInFilesHitClass"));
}

t4p::FindInFilesResultsPanelClass::~FindInFilesResultsPanelClass() {

	// make sure we kill any running searches
	if (RunningActionId > 0) {
		RunningThreads.CancelAction(RunningActionId);
		
		// dont try to kill the gauge as the gauge window may not valid anymore
		// if the program is closed while the find is running
	}
	RunningThreads.RemoveEventHandler(this);
}

void t4p::FindInFilesResultsPanelClass::Find(const FindInFilesClass& findInFiles, bool doHiddenFiles) {
	FindInFiles.Copy(findInFiles);
	MatchedFiles = 0;
	ReplaceWithText->SetValue(t4p::IcuToWx(FindInFiles.ReplaceExpression));
	RegexReplaceWithHelpButton->Enable(t4p::FinderClass::REGULAR_EXPRESSION == FindInFiles.Mode);

	// for now disallow another find when one is already active
	if (RunningActionId > 0) {
		wxMessageBox(_("Find in files is already running. Please wait for it to finish."), _("Find In Files"));
		return;
	}
	std::vector<wxString> skipFiles = Notebook->GetOpenedFiles();
	t4p::FindInFilesBackgroundReaderClass* reader = 
		new t4p::FindInFilesBackgroundReaderClass(RunningThreads, FindInFilesGaugeId);
	if (reader->InitForFind(FindInFiles, doHiddenFiles, skipFiles)) {
		RunningActionId = RunningThreads.Queue(reader);
		EnableButtons(true, false, false);
		Gauge->AddGauge(_("Find In Files"), FindInFilesGaugeId, StatusBarWithGaugeClass::INDETERMINATE_MODE, 
			wxGA_HORIZONTAL);
		SetStatus(_("Searching"));

		// lets do the find in the opened files ourselves so that the hits are not stale
		FindInOpenedFiles();
	}
	else {
		wxMessageBox(_("Please enter a valid expression and path."));
		delete reader;
	}
}

void t4p::FindInFilesResultsPanelClass::FindInOpenedFiles() {
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
				std::vector<t4p::FindInFilesHitClass> hits;
				while (finder.FindNext(text, next)) {
					if (finder.GetLastMatch(charPos, length)) {
						int lineNumber = codeControl->LineFromCharacter(charPos);
						
						int start = codeControl->PositionFromLine(lineNumber);
						int end = codeControl->GetLineEndPosition(lineNumber);
						wxString lineText = codeControl->GetTextRange(start, end);

						// lineNumber is zero-based but we want to display it as 1-based
						lineNumber++;
						t4p::FindInFilesHitClass hit(
							fileName, lineText, lineNumber,
							charPos - start, next, length);
						hits.push_back(hit);
						next = charPos + length;
					}
					else {
						break;
					}
				}
				if (!hits.empty()) {
					t4p::FindInFilesHitEventClass hitEvent(FindInFilesGaugeId, hits);
					wxPostEvent(this, hitEvent);
				}
			}
		}
	}
}

void t4p::FindInFilesResultsPanelClass::ShowNextMatch() {
	int selected = ResultsList->GetSelectedRow();
	int next = 0;
	if (selected >= 0 && t4p::NumberLessThan(selected, ResultsList->GetItemCount() - 1)) {
		next = selected + 1;
	}
	else {
		// loop back to the beginning
		next = 0;
	}
	ShowMatchAndEnsureVisible(next);
}

void t4p::FindInFilesResultsPanelClass::ShowPreviousMatch() {
	int selected = ResultsList->GetSelectedRow();
	int next = 0;
	if (selected > 0) {
		next = selected - 1;
	}
	else {
		
		// loop back to the end
		next = ResultsList->GetItemCount() - 1;
	}
	ShowMatchAndEnsureVisible(next);
}

void t4p::FindInFilesResultsPanelClass::OnReplaceButton(wxCommandEvent& event) {
	if (ReplaceWithText->FindString(ReplaceWithText->GetValue()) == wxNOT_FOUND) {
		ReplaceWithText->AppendString(ReplaceWithText->GetValue());	
	}
	FindInFiles.ReplaceExpression = t4p::WxToIcu(ReplaceWithText->GetValue());
	FindInFiles.Prepare();
	CodeControlClass* codeControl = 
			Notebook->GetCodeControl(Notebook->GetSelection());	
			
	// if user changed tab, GetLastReplacementText would return false and nothing will be replaced
	UnicodeString text = codeControl->GetSafeText();
	if (codeControl) {		
		int32_t position = codeControl->GetCurrentPos(),
			length = 0;
		FinderClass finder;
		UnicodeString matchedText;
		wxString replaceWithText = t4p::IcuToWx(FindInFiles.ReplaceExpression);
		FindInFiles.CopyFinder(finder);
		if (finder.Prepare()) {
			
			// only replace when the cursor is at the hit (which would be the case when and only when the use
			// double clicked on the result list (and focused on the hit).
			if (finder.FindNext(text, position) && finder.GetLastMatch(position, length) && 
				position == codeControl->GetCurrentPos() &&
				finder.GetLastReplacementText(text, matchedText)) {
				codeControl->SetSelectionAndEnsureVisible(position, position + length);
				codeControl->ReplaceSelection(replaceWithText);
				codeControl->SetSelectionByCharacterPosition(position, position + replaceWithText.length(), false);

				ShowNextMatch();
			}
		}
	}
}

void t4p::FindInFilesResultsPanelClass::OnReplaceAllInFileButton(wxCommandEvent& event) {
	if (ReplaceWithText->FindString(ReplaceWithText->GetValue()) == wxNOT_FOUND) {
		ReplaceWithText->AppendString(ReplaceWithText->GetValue());	
	}
	FindInFiles.ReplaceExpression = t4p::WxToIcu(ReplaceWithText->GetValue());
	FindInFiles.Prepare();
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

void t4p::FindInFilesResultsPanelClass::OnReplaceInAllFilesButton(wxCommandEvent& event) {

	// for now disallow another replace when one is already active
	if (RunningActionId > 0) {
		wxMessageBox(_("Find in files is already running. Please wait for it to finish."), _("Find In Files"));
		return;
	}
	if (ReplaceWithText->FindString(ReplaceWithText->GetValue()) == wxNOT_FOUND) {
		ReplaceWithText->AppendString(ReplaceWithText->GetValue());	
	}
	FindInFiles.ReplaceExpression = t4p::WxToIcu(ReplaceWithText->GetValue());
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
		t4p::FindInFilesBackgroundReaderClass* reader = 
			new t4p::FindInFilesBackgroundReaderClass(RunningThreads, FindInFilesGaugeId);
		reader->InitForReplace(FindInFiles, FilesFromHits(AllHits), Notebook->GetOpenedFiles());
		RunningActionId = RunningThreads.Queue(reader);
		SetStatus(_("Find In Files In Progress"));
		Gauge->AddGauge(_("Find In Files"), FindInFilesGaugeId, StatusBarWithGaugeClass::INDETERMINATE_MODE, 
			wxGA_HORIZONTAL);
		EnableButtons(true, false, false);
		SetStatus(_("Replacing"));
	}
	else {
		wxMessageBox(_("Please enter a valid expression and path."));
	}
}

void t4p::FindInFilesResultsPanelClass::OnFindInFilesComplete(wxCommandEvent& event) {
	int matchedFilesSize = GetNumberOfMatchedFiles();
	bool enableReplace = matchedFilesSize > 0;
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
		
		// now resize the columns so that the hits can be seen
		t4p::DataViewGridAutoSizeAllColumns(ResultsList);
	}
	else if (event.GetInt() == BackgroundFileReaderClass::MATCHED) {
		if (matchedFilesSize > 0) {
			SetStatus(wxString::Format(wxT("Replaced Matches in %d files"), matchedFilesSize));
		}
		else {
			SetStatus(_("Did not Replace. No Matches Were Found"));
		}
	}
}

void t4p::FindInFilesResultsPanelClass::OnActionComplete(t4p::ActionEventClass& event) {
	if (event.GetId() != FindInFilesGaugeId) {

		// event from another, concurrent find in files panel. let the other panel
		// handle it
		event.Skip();
		return;
	}
	RunningActionId = 0;
}

void t4p::FindInFilesResultsPanelClass::OnFileHit(t4p::FindInFilesHitEventClass& event) {
	std::vector<t4p::FindInFilesHitClass> hits = event.GetHits();
	if (hits.empty()) {
		return;
	}
	if (event.GetId() != FindInFilesGaugeId) {

		// event from another, concurrent find in files panel. let the other panel
		// handle it
		event.Skip();
		return;
	}

	// dont bother with more than this many hits, user cannot possibly do through them all
	if (AllHits.size() < MAX_HITS) {
		MatchedFiles++;

		wxArrayString hitList;
		std::vector<t4p::FindInFilesHitClass>::const_iterator hit;

		// in MSW the list control does not render the \t use another delimiter
		wxString format = wxT("%s\t:%d\t:%s");
		for (hit = hits.begin(); hit != hits.end(); ++hit) {
			wxVector<wxVariant> data;
			data.push_back(hit->FileName);
			data.push_back(wxString::Format(wxT("%d"), hit->LineNumber));
			wxAny any(*hit);
			data.push_back(any);
			
			ResultsList->AppendItem(data);
		}
		AllHits.insert(AllHits.end(), hits.begin(), hits.end());
	}
	else {
		Stop();
	}
}

void t4p::FindInFilesResultsPanelClass::OnStopButton(wxCommandEvent& event) {
	Stop();
}

void t4p::FindInFilesResultsPanelClass::Stop() {
	RunningThreads.CancelAction(RunningActionId);
	Gauge->StopGauge(FindInFilesGaugeId);
	if (AllHits.size() >= MAX_HITS) {
		SetStatus(_("Too many hits, Search stopped"));
	}
	else {
		SetStatus(_("Search stopped"));
	}
	bool enableIterators = MatchedFiles > 0;
	EnableButtons(false, enableIterators, enableIterators);
}

void t4p::FindInFilesResultsPanelClass::OnRowActivated(wxDataViewEvent& event) {
	int sel = ResultsList->GetSelectedRow();
	ShowMatch(sel);
}

void t4p::FindInFilesResultsPanelClass::ShowMatch(int i) {
	if (!t4p::NumberLessThan(i, AllHits.size())) {
		return;
	}
	wxString fileName = AllHits[i].FileName;
	int line = AllHits[i].LineNumber;
	Notebook->LoadPage(fileName);
	CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
	if (codeControl) {
			
		// search for the expression and highlight it. search from the start of the line.
		int32_t startPos = AllHits[i].FileOffset;
		int32_t length = 0;
		FinderClass finder;
		FindInFiles.CopyFinder(finder);
		if (finder.Prepare()) {
			if (finder.FindNext(codeControl->GetSafeText(), startPos) && finder.GetLastMatch(startPos, length)) {
				codeControl->MarkSearchHitAndGoto(line, startPos, startPos + length, true);
				codeControl->SetFocus();
			}
			else {
				
				// hit is no longer there, still go to the
				// line
				// it seems pretty weird for the editor to open the file
				// but not go to the line.
				codeControl->MarkSearchHitAndGoto(line, AllHits[i].FileOffset, AllHits[i].FileOffset, false);
				codeControl->SetFocus();
			}
		}
	}
}

void t4p::FindInFilesResultsPanelClass::ShowMatchAndEnsureVisible(int i) {
	ShowMatch(i);
	ResultsList->SelectRow(i);
	wxDataViewItem item = ResultsList->GetCurrentItem();
	ResultsList->EnsureVisible(item);
}

void t4p::FindInFilesResultsPanelClass::OnCopySelectedButton(wxCommandEvent& event) {
	int selected = ResultsList->GetSelectedRow();
	if (selected != wxNOT_FOUND) {
		wxString selectedItems = ResultsList->GetTextValue(selected, 0) 
			+ wxT(",")
			+ ResultsList->GetTextValue(selected, 1)
			+ wxT(",")
			+ ResultsList->GetTextValue(selected, 2);
		selectedItems += wxTextFile::GetEOL();
		
		if (wxTheClipboard->Open()) {
			wxTheClipboard->SetData(new wxTextDataObject(selectedItems));
			wxTheClipboard->Close();
		}
	}
}

void t4p::FindInFilesResultsPanelClass::OnCopyAllButton(wxCommandEvent& event) {
	if (ResultsList->GetItemCount() <= 0) {
		return;
	}
	wxString selectedItems;
	for (int i = 0; i < ResultsList->GetItemCount(); ++i) {
		selectedItems += ResultsList->GetTextValue(i, 0) 
			+ wxT(",")
			+ ResultsList->GetTextValue(i, 1)
			+ wxT(",")
			+ ResultsList->GetTextValue(i, 2);
		selectedItems += wxTextFile::GetEOL();
	}
		
	if (wxTheClipboard->Open()) {
		wxTheClipboard->SetData(new wxTextDataObject(selectedItems));
		wxTheClipboard->Close();
	}
}

void t4p::FindInFilesResultsPanelClass::OnPreviousHitButton(wxCommandEvent& event) {
	ShowPreviousMatch();
}

void t4p::FindInFilesResultsPanelClass::OnNextHitButton(wxCommandEvent& event) {
	ShowNextMatch();
}

void t4p::FindInFilesResultsPanelClass::OnRegExReplaceHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	t4p::PopulateRegExReplaceMenu(regExMenu, ID_REGEX_REPLACE_MENU_START);
	PopupMenu(&regExMenu);	
}

void t4p::FindInFilesResultsPanelClass::InsertReplaceRegExSymbol(wxCommandEvent& event) {
	int id = event.GetId() - ID_REGEX_REPLACE_MENU_START;
	t4p::AddSymbolToReplaceRegularExpression(ReplaceWithText, id, CurrentInsertionPointReplace);
}

void t4p::FindInFilesResultsPanelClass::OnKillFocusReplaceText(wxFocusEvent& event) {
	CurrentInsertionPointReplace = ReplaceWithText->GetInsertionPoint();
	event.Skip();
}

void t4p::FindInFilesResultsPanelClass::OnReplaceTextEnter(wxCommandEvent& event) {
	if (ReplaceWithText->FindString(ReplaceWithText->GetValue()) == wxNOT_FOUND) {
		ReplaceWithText->AppendString(ReplaceWithText->GetValue());	
	}
}

void t4p::FindInFilesResultsPanelClass::EnableButtons(bool enableStopButton, bool enableReplaceButtons, bool enableCopyButtons) {
	StopButton->Enable(enableStopButton);
	ReplaceButton->Enable(enableReplaceButtons);
	ReplaceAllInFileButton->Enable(enableReplaceButtons);
	ReplaceInAllFilesButton->Enable(enableReplaceButtons);
	CopySelectedButton->Enable(enableCopyButtons);
	CopyAllButton->Enable(enableCopyButtons);
}

void t4p::FindInFilesResultsPanelClass::SetStatus(const wxString& text) {
	
	// label might grow/shrink according to new text, must
	// tell the sizer to re-position the label correctly
	// we need this for the label to be right-aligned after
	// the text change
	wxString msg;
	msg += _("Searched For: ");
	msg += t4p::IcuToWx(FindInFiles.Expression);
	if (t4p::FinderClass::REGULAR_EXPRESSION == FindInFiles.Mode) {
		msg += _(" (regex)");
	}
	else if (t4p::FinderClass::CASE_INSENSITIVE == FindInFiles.Mode) {
		msg += _(" (case)");
	}
	else if (t4p::FinderClass::EXACT == FindInFiles.Mode) {
		msg += _(" (exact)");
	}

	FindLabel->SetLabel(msg);
	ResultText->SetLabel(text);
	FindLabel->GetContainingSizer()->Layout();
	ResultText->GetContainingSizer()->Layout();
	this->Layout();
}

int t4p::FindInFilesResultsPanelClass::GetNumberOfMatchedFiles() {
	return MatchedFiles;
}

void t4p::FindInFilesResultsPanelClass::OnActionProgress(t4p::ActionProgressEventClass& event) {
	if (event.GetId() != FindInFilesGaugeId) {

		// event from another, concurrent find in files panel. let the other panel
		// handle it
		event.Skip();
		return;
	}
	Gauge->IncrementGauge(FindInFilesGaugeId, StatusBarWithGaugeClass::INDETERMINATE_MODE);	
}

t4p::FindInFilesDialogClass::FindInFilesDialogClass(wxWindow* parent, t4p::FindInFilesFeatureClass& feature,
		t4p::FindInFilesViewClass& view)
	: FindInFilesDialogGeneratedClass(parent, wxID_ANY)
	, Feature(feature)
	, View(view)
	, CurrentInsertionPointFind(0)
	, CurrentInsertionPointReplace(0) {
	View.FindHistory.Attach(FindText);
	View.ReplaceHistory.Attach(ReplaceWithText);
	View.DirectoriesHistory.Attach(Directory);
	View.FilesHistory.Attach(FilesFilter);
	
	// the first time showing this dialog populate the filter to have only PHP file extensions
	if (FilesFilter->GetCount() <= 0) {
		Feature.PreviousFindInFiles.Source.SetIncludeWildcards(feature.App.Globals.FileTypes.PhpFileExtensionsString);
	}
	std::vector<t4p::SourceClass> sources = feature.App.Globals.AllEnabledSources();
	for (size_t i = 0; i < sources.size(); ++i) {
		t4p::SourceClass src = sources[i];
		wxString fullPath = src.RootDirectory.GetFullPath();
		if (Directory->FindString(fullPath, false) < 0) {
			Directory->AppendString(fullPath);
		}
	}

	// select the last used path by default
	bool isUsed = false;
	int foundIndex = -1;
	wxString lastUsedPath = feature.PreviousFindInFiles.Source.RootDirectory.GetFullPath();
	if (!lastUsedPath.IsEmpty()) {
		foundIndex = Directory->FindString(lastUsedPath, false);
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
	t4p::RegularExpressionValidatorClass regExValidator(&Feature.PreviousFindInFiles.Expression, FinderMode);
	FindText->SetValidator(regExValidator);
	UnicodeStringValidatorClass replaceExpressionValidator(&Feature.PreviousFindInFiles.ReplaceExpression, true);
	wxGenericValidator modeValidator(&Feature.PreviousFindInFiles.Mode);
	ReplaceWithText->SetValidator(replaceExpressionValidator);
	FinderMode->SetValidator(modeValidator);
	wxGenericValidator doHiddenFilesValidator(&Feature.DoHiddenFiles);
	DoHiddenFiles->SetValidator(doHiddenFilesValidator);

	FilesFilter->SetValue(Feature.PreviousFindInFiles.Source.IncludeWildcardsString());
	FindText->SetFocus();
}

void t4p::FindInFilesDialogClass::OnOkButton(wxCommandEvent& event) {
	if (TransferDataFromWindow()) {
		Feature.PreviousFindInFiles.Source.SetIncludeWildcards(FilesFilter->GetValue());
		if (!Feature.PreviousFindInFiles.Prepare()) {
			wxMessageBox(_("Expression is not valid."), _("Find In Files"), wxOK | wxCENTER, this);
		}
		else if (Directory->GetValue().IsEmpty()) {
			wxMessageBox(_("Find path must not be empty."), _("Find In Files"), wxOK | wxCENTER, this);
		}
		else {
			Feature.PreviousFindInFiles.Source.RootDirectory.AssignDir(Directory->GetValue());
			View.FindHistory.Save();
			View.ReplaceHistory.Save();
			View.DirectoriesHistory.Save();
			View.FilesHistory.Save();

			View.FindHistory.Detach();
			View.ReplaceHistory.Detach();
			View.DirectoriesHistory.Detach();
			View.FilesHistory.Detach();
			EndModal(wxID_OK);
		}
	}
}

void t4p::FindInFilesDialogClass::OnCancelButton(wxCommandEvent& event) {

	// need to do this to prevent crash on app exit
	View.FindHistory.Detach();
	View.ReplaceHistory.Detach();
	View.DirectoriesHistory.Detach();
	View.FilesHistory.Detach();
	EndModal(wxID_CANCEL);
}

void t4p::FindInFilesDialogClass::OnDirChanged(wxFileDirPickerEvent& event) {
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

void t4p::FindInFilesDialogClass::OnKeyDown(wxKeyEvent& event) {

	// the event object is the control that has focus
	if (event.GetKeyCode() == WXK_TAB && event.ShiftDown()) {
		((wxWindow*)event.GetEventObject())->Navigate(wxNavigationKeyEvent::IsBackward);
	}
	else if (event.GetKeyCode() == WXK_TAB) {
		((wxWindow*)event.GetEventObject())->Navigate(wxNavigationKeyEvent::IsForward);
	}
	else {
		event.Skip();
	}
}

void t4p::FindInFilesDialogClass::OnKillFocusFindText(wxFocusEvent& event) {
	CurrentInsertionPointFind = FindText->GetInsertionPoint();
	event.Skip();
}

void t4p::FindInFilesDialogClass::OnKillFocusReplaceText(wxFocusEvent& event) {
	CurrentInsertionPointReplace = ReplaceWithText->GetInsertionPoint();
	event.Skip();
}

t4p::FindInFilesViewClass::FindInFilesViewClass(t4p::FindInFilesFeatureClass& feature)
	: FeatureViewClass()
	, FindHistory()
	, ReplaceHistory()
	, DirectoriesHistory()
	, FilesHistory()
	, Feature(feature)
	, ResultsPanels() {
}

void t4p::FindInFilesViewClass::AddSearchMenuItems(wxMenu* searchMenu) {
	searchMenu->Append(t4p::MENU_FIND_IN_FILES + 0, _("Find In Files\tCTRL+SHIFT+F"), 
		_("Find an expression by searching entire directory contents"));
	searchMenu->Append(t4p::MENU_FIND_IN_FILES + 1, _("Next Find In Files Match\tALT+F3"), 
		_("Move the cursor to the next Find In Files Match"));
	searchMenu->Append(t4p::MENU_FIND_IN_FILES + 2, _("Previous Find In Files Match\tALT+SHIFT+F3"),
		_("Move the cursor to the previous Find In Files Match"));
}

void t4p::FindInFilesViewClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[t4p::MENU_FIND_IN_FILES + 0] = wxT("Find-Find In Files");
	menuItemIds[t4p::MENU_FIND_IN_FILES + 1] = wxT("Find-Find In Files Next Match");
	menuItemIds[t4p::MENU_FIND_IN_FILES + 2] = wxT("Find-Find In Files Previous Match");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void t4p::FindInFilesViewClass::OnEditFindInFiles(wxCommandEvent& event) {

	// prime finder with selected text
	wxString selectedText = GetSelectedText();
	if (!selectedText.empty()) {
		Feature.PreviousFindInFiles.Expression = t4p::WxToIcu(selectedText);
	}
	FindInFilesDialogClass dialog(GetMainWindow(), Feature, *this);
	if (dialog.ShowModal() == wxID_OK) {
		t4p::FindInFilesResultsPanelClass* panel = new t4p::FindInFilesResultsPanelClass(GetToolsNotebook(), 
			GetNotebook(), GetStatusBarWithGauge(), Feature.App.RunningThreads);
		wxBitmap findBitmap = wxNullBitmap;
		if (Feature.PreviousFindInFiles.ReplaceExpression.isEmpty()) {
			findBitmap = t4p::BitmapImageAsset(wxT("find-in-files"));
		}
		else {
			findBitmap = t4p::BitmapImageAsset(wxT("replace-in-files"));
		}
		
		if(AddToolsWindow(panel, _("Find In Files Results"), wxEmptyString, findBitmap)) {
			panel->Find(Feature.PreviousFindInFiles, Feature.DoHiddenFiles);
			ResultsPanels.push_back(panel);
		}
	}
}

void t4p::FindInFilesViewClass::OnEditFindInFilesNext(wxCommandEvent& event) {
	wxAuiNotebook* notebook = GetToolsNotebook();
	int selection = notebook->GetSelection();
	wxWindow* window = notebook->GetPage(selection);
	std::vector<t4p::FindInFilesResultsPanelClass*>::iterator it = ResultsPanels.begin();
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

void t4p::FindInFilesViewClass::OnEditFindInFilesPrevious(wxCommandEvent& event) {
	wxAuiNotebook* notebook = GetToolsNotebook();
	int selection = notebook->GetSelection();
	wxWindow* window = notebook->GetPage(selection);
	std::vector<t4p::FindInFilesResultsPanelClass*>::iterator it = ResultsPanels.begin();
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

void t4p::FindInFilesViewClass::OnToolsNotebookPageClosed(wxAuiNotebookEvent& event) {
	int selection = event.GetSelection();
	wxAuiNotebook* notebook = GetToolsNotebook();
	wxWindow* window = notebook->GetPage(selection);
	std::vector<t4p::FindInFilesResultsPanelClass*>::iterator it = ResultsPanels.begin();
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

void t4p::FindInFilesDialogClass::OnRegExFindHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	t4p::PopulateRegExFindMenu(regExMenu, ID_REGEX_MENU_START);
	PopupMenu(&regExMenu);	
}

void t4p::FindInFilesDialogClass::OnRegExReplaceHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	t4p::PopulateRegExReplaceMenu(regExMenu, ID_REGEX_REPLACE_MENU_START);
	PopupMenu(&regExMenu);	
}

void t4p::FindInFilesDialogClass::InsertRegExSymbol(wxCommandEvent& event) {
	int id = event.GetId() - ID_REGEX_MENU_START;
	t4p::AddSymbolToRegularExpression(FindText, id, CurrentInsertionPointFind);
	FinderMode->SetSelection(FinderClass::REGULAR_EXPRESSION);
	event.Skip();
}

void t4p::FindInFilesDialogClass::InsertReplaceRegExSymbol(wxCommandEvent& event) {
	int id = event.GetId() - ID_REGEX_REPLACE_MENU_START;
	t4p::AddSymbolToReplaceRegularExpression(ReplaceWithText, id, CurrentInsertionPointReplace);
	FinderMode->SetSelection(FinderClass::REGULAR_EXPRESSION);
}

BEGIN_EVENT_TABLE(t4p::FindInFilesResultsPanelClass, FindInFilesResultsPanelGeneratedClass)
	
	// remove this handler; when searching many files the GUI is redrawn constantly and doesn't
	// look smooth
	//EVT_COMMAND(wxID_ANY, EVENT_FILE_READ, t4p::FindInFilesResultsPanelClass::OnFileSearched)
	EVT_FIND_IN_FILES_HITS(wxID_ANY, t4p::FindInFilesResultsPanelClass::OnFileHit)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_FILE_READ_COMPLETE, t4p::FindInFilesResultsPanelClass::OnFindInFilesComplete)
	EVT_ACTION_PROGRESS(wxID_ANY, t4p::FindInFilesResultsPanelClass::OnActionProgress)
	EVT_ACTION_COMPLETE(wxID_ANY, t4p::FindInFilesResultsPanelClass::OnActionComplete)

	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_ONE, t4p::FindInFilesResultsPanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_TWO, t4p::FindInFilesResultsPanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_THREE, t4p::FindInFilesResultsPanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_FOUR, t4p::FindInFilesResultsPanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_FIVE, t4p::FindInFilesResultsPanelClass::InsertReplaceRegExSymbol)
	
	EVT_DATAVIEW_ITEM_ACTIVATED(FindInFilesResultsPanelGeneratedClass::ID_RESULTS_LIST, t4p::FindInFilesResultsPanelClass::OnRowActivated)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::FindInFilesViewClass, t4p::FeatureViewClass)
	EVT_MENU(t4p::MENU_FIND_IN_FILES + 0, t4p::FindInFilesViewClass::OnEditFindInFiles)
	EVT_MENU(t4p::MENU_FIND_IN_FILES + 1, t4p::FindInFilesViewClass::OnEditFindInFilesNext)
	EVT_MENU(t4p::MENU_FIND_IN_FILES + 2, t4p::FindInFilesViewClass::OnEditFindInFilesPrevious)
	EVT_AUINOTEBOOK_PAGE_CLOSE(t4p::ID_TOOLS_NOTEBOOK, t4p::FindInFilesViewClass::OnToolsNotebookPageClosed)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::FindInFilesDialogClass, FindInFilesDialogGeneratedClass)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_ONE, t4p::FindInFilesDialogClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_TWO, t4p::FindInFilesDialogClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_THREE, t4p::FindInFilesDialogClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_FOUR, t4p::FindInFilesDialogClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_FIVE, t4p::FindInFilesDialogClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_ONE, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_ZERO_OR_ONE, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_EXACT, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_AT_LEAST, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_BETWEEN, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_BEGIN_OF_LINE, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_END_OF_LINE, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_DIGIT, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_WHITE_SPACE, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_ALPHANUMERIC, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_NOT_DECIMAL, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_NOT_WHITE_SPACE, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_NOT_ALPHANUMERIC, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_CASE_SENSITIVE, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_COMMENT, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_DOT_ALL, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_MULTI_LINE, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_UWORD, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_STRING, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_VARIABLE, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_NUMBER, t4p::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_WHITESPACE, t4p::FindInFilesDialogClass::InsertRegExSymbol)
END_EVENT_TABLE()