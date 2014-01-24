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
#include <features/FindInFilesFeatureClass.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <widgets/UnicodeStringValidatorClass.h>
#include <widgets/RegularExpressionValidatorClass.h>
#include <globals/Errors.h>
#include <MvcEditor.h>
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

static std::vector<wxString> FilesFromHits(const std::vector<mvceditor::FindInFilesHitClass>& allHits) {
	std::vector<wxString>  files;
	std::vector<mvceditor::FindInFilesHitClass>::const_iterator hit;
	for (hit = allHits.begin(); hit != allHits.end(); ++hit) {
		files.push_back(hit->FileName);
	}
	if (!files.empty()) {
		std::vector<wxString>::iterator newEnd = std::unique(files.begin(), files.end());
		files.resize(newEnd - files.begin());
	}
	return files;
}

mvceditor::FindInFilesHitClass::FindInFilesHitClass()
	: FileName()
	, Preview()
	, LineNumber() 
	, LineOffset(0)
	, FileOffset(0) {

}

mvceditor::FindInFilesHitClass::FindInFilesHitClass(const wxString& fileName, const wxString& preview, 
	int lineNumber, int lineOffset, int fileOffset)

	// use c_str() to deep copy
	: FileName(fileName.c_str())
	, Preview(preview.c_str())
	, LineNumber(lineNumber) 
	, LineOffset(lineOffset) 
	, FileOffset(fileOffset) {

}

mvceditor::FindInFilesHitClass::FindInFilesHitClass(const mvceditor::FindInFilesHitClass& hit) 
	: FileName()
	, Preview() 
	, LineNumber(1) 
	, LineOffset(0)
	, FileOffset(0) {
	Copy(hit);
}

mvceditor::FindInFilesHitClass& mvceditor::FindInFilesHitClass::operator=(const mvceditor::FindInFilesHitClass& hit) {
	Copy(hit);
	return *this;
}


void mvceditor::FindInFilesHitClass::Copy(const mvceditor::FindInFilesHitClass& hit) {
	FileName = hit.FileName.c_str();
	Preview = hit.Preview.c_str();
	LineNumber = hit.LineNumber;
	LineOffset = hit.LineOffset;
	FileOffset = hit.FileOffset;
}

mvceditor::FindInFilesHitEventClass::FindInFilesHitEventClass(int eventId, const std::vector<mvceditor::FindInFilesHitClass> &hits)
	: wxEvent(eventId, mvceditor::EVENT_FIND_IN_FILES_FILE_HIT)
	, Hits(hits) {
}

wxEvent* mvceditor::FindInFilesHitEventClass::Clone() const {
	wxEvent* newEvt = new mvceditor::FindInFilesHitEventClass(GetId(), Hits);
	return newEvt;
}

std::vector<mvceditor::FindInFilesHitClass> mvceditor::FindInFilesHitEventClass::GetHits() const {
	return Hits;
}

mvceditor::FindInFilesBackgroundReaderClass::FindInFilesBackgroundReaderClass(mvceditor::RunningThreadsClass& runningThreads, int eventId) 
	: BackgroundFileReaderClass(runningThreads, eventId) 
	, FindInFiles()
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
																 const std::vector<wxString>& replaceFiles,
																 std::vector<wxString> skipFiles) {
	FindInFiles = findInFiles;
	SkipFiles = skipFiles;
	return FindInFiles.Prepare() && InitMatched(replaceFiles);
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
			bool destroy = IsCancelled();
			do {
				if (destroy) {
					break;
				}
				mvceditor::FindInFilesHitClass hit(fileName, 
					mvceditor::IcuToWx(FindInFiles.GetCurrentLine()), 
					FindInFiles.GetCurrentLineNumber(),
					FindInFiles.GetLineOffset(),
					FindInFiles.GetFileOffset()
				);
				hits.push_back(hit);

			}
			while (!destroy && FindInFiles.FindNext());
			if (!destroy && !hits.empty()) {

				// PostEvent will change the ID of the event to the correct
				// one
				mvceditor::FindInFilesHitEventClass hitEvent(wxID_ANY, hits);
				PostEvent(hitEvent);
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

wxString mvceditor::FindInFilesBackgroundReaderClass::GetLabel() const {
	return wxT("Find In Files");
}

mvceditor::FindInFilesResultsPanelClass::FindInFilesResultsPanelClass(wxWindow* parent, NotebookClass* notebook, 
		StatusBarWithGaugeClass* gauge, mvceditor::RunningThreadsClass& runningThreads)
	: FindInFilesResultsPanelGeneratedClass(parent)
	, FindInFiles()
	, RunningThreads(runningThreads)
	, Notebook(notebook)
	, Gauge(gauge)
	, MatchedFiles(0) 
	, RunningActionId(0) {
	FindInFilesGaugeId = wxNewId();
	RunningThreads.AddEventHandler(this);

	ReplaceButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("replace")));
	ReplaceAllInFileButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("replace-file")));
	ReplaceInAllFilesButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("replace-all")));
	PreviousHitButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("previous")));
	NextHitButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("next")));
	StopButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("stop")));
	CopySelectedButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("copy")));
	CopyAllButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("copy-all")));
	
	ResultsList->AppendTextColumn(_("File"), wxDATAVIEW_CELL_INERT, 
		wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	ResultsList->AppendTextColumn(_("Line Number"), wxDATAVIEW_CELL_INERT, 
		wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	ResultsList->AppendTextColumn(_("Preview"), wxDATAVIEW_CELL_INERT, 
		wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
}

mvceditor::FindInFilesResultsPanelClass::~FindInFilesResultsPanelClass() {

	// make sure we kill any running searches
	if (RunningActionId > 0) {
		RunningThreads.CancelAction(RunningActionId);
		
		// dont try to kill the gauge as the gauge window may not valid anymore
		// if the program is closed while the find is running
	}
	RunningThreads.RemoveEventHandler(this);
}

void mvceditor::FindInFilesResultsPanelClass::Find(const FindInFilesClass& findInFiles, bool doHiddenFiles) {
	FindInFiles.Copy(findInFiles);
	MatchedFiles = 0;
	ReplaceWithText->SetValue(mvceditor::IcuToWx(FindInFiles.ReplaceExpression));
	RegexReplaceWithHelpButton->Enable(mvceditor::FinderClass::REGULAR_EXPRESSION == FindInFiles.Mode);

	// for now disallow another find when one is already active
	if (RunningActionId > 0) {
		wxMessageBox(_("Find in files is already running. Please wait for it to finish."), _("Find In Files"));
		return;
	}
	std::vector<wxString> skipFiles = Notebook->GetOpenedFiles();
	mvceditor::FindInFilesBackgroundReaderClass* reader = 
		new mvceditor::FindInFilesBackgroundReaderClass(RunningThreads, FindInFilesGaugeId);
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
						mvceditor::FindInFilesHitClass hit(fileName, lineText, lineNumber,
							next - start, next);
						hits.push_back(hit);
						next = charPos + length;
					}
					else {
						break;
					}
				}
				if (!hits.empty()) {
					mvceditor::FindInFilesHitEventClass hitEvent(FindInFilesGaugeId, hits);
					wxPostEvent(this, hitEvent);
				}
			}
		}
	}
}

void mvceditor::FindInFilesResultsPanelClass::ShowNextMatch() {
	int selected = ResultsList->GetSelectedRow();
	int next = 0;
	if (selected > 0 && selected < ((int)ResultsList->GetItemCount() - 1)) {
		next = selected + 1;
	}
	else {
		// loop back to the beginning
		next = 0;
	}
	ShowMatch(next);
	ResultsList->SelectRow(next);
}

void mvceditor::FindInFilesResultsPanelClass::ShowPreviousMatch() {
	int selected = ResultsList->GetSelectedRow();
	int next = 0;
	if (selected > 0) {
		next = selected - 1;
	}
	else {
		
		// loop back to the end
		next = ResultsList->GetItemCount() - 1;
	}
	ShowMatch(next);
	ResultsList->SelectRow(next);
}

void mvceditor::FindInFilesResultsPanelClass::OnReplaceButton(wxCommandEvent& event) {
	if (ReplaceWithText->FindString(ReplaceWithText->GetValue()) == wxNOT_FOUND) {
		ReplaceWithText->AppendString(ReplaceWithText->GetValue());	
	}
	FindInFiles.ReplaceExpression = mvceditor::WxToIcu(ReplaceWithText->GetValue());
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
		wxString replaceWithText = mvceditor::IcuToWx(FindInFiles.ReplaceExpression);
		FindInFiles.CopyFinder(finder);
		if (finder.Prepare()) {
			
			// only replace when the cursor is at the hit (which would be the case when and only when the use
			// double clicked on the result list (and focused on the hit).
			if (finder.FindNext(text, position) && finder.GetLastMatch(position, length) && 
				position == codeControl->GetCurrentPos() &&
				finder.GetLastReplacementText(text, matchedText)) {
				codeControl->SetSelectionAndEnsureVisible(position, position + length);
				codeControl->ReplaceSelection(replaceWithText);
				codeControl->SetSelectionByCharacterPosition(position, position + replaceWithText.length());

				ShowNextMatch();
			}
		}
	}
}

void mvceditor::FindInFilesResultsPanelClass::OnReplaceAllInFileButton(wxCommandEvent& event) {
	if (ReplaceWithText->FindString(ReplaceWithText->GetValue()) == wxNOT_FOUND) {
		ReplaceWithText->AppendString(ReplaceWithText->GetValue());	
	}
	FindInFiles.ReplaceExpression = mvceditor::WxToIcu(ReplaceWithText->GetValue());
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

void mvceditor::FindInFilesResultsPanelClass::OnReplaceInAllFilesButton(wxCommandEvent& event) {

	// for now disallow another replace when one is already active
	if (RunningActionId > 0) {
		wxMessageBox(_("Find in files is already running. Please wait for it to finish."), _("Find In Files"));
		return;
	}
	if (ReplaceWithText->FindString(ReplaceWithText->GetValue()) == wxNOT_FOUND) {
		ReplaceWithText->AppendString(ReplaceWithText->GetValue());	
	}
	FindInFiles.ReplaceExpression = mvceditor::WxToIcu(ReplaceWithText->GetValue());
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
		mvceditor::FindInFilesBackgroundReaderClass* reader = 
			new mvceditor::FindInFilesBackgroundReaderClass(RunningThreads, FindInFilesGaugeId);
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

void mvceditor::FindInFilesResultsPanelClass::OnFindInFilesComplete(wxCommandEvent& event) {
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

void mvceditor::FindInFilesResultsPanelClass::OnActionComplete(mvceditor::ActionEventClass& event) {
	if (event.GetId() != FindInFilesGaugeId) {

		// event from another, concurrent find in files panel. let the other panel
		// handle it
		event.Skip();
		return;
	}
	RunningActionId = 0;
}

void mvceditor::FindInFilesResultsPanelClass::OnFileHit(mvceditor::FindInFilesHitEventClass& event) {
	std::vector<mvceditor::FindInFilesHitClass> hits = event.GetHits();
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
		std::vector<mvceditor::FindInFilesHitClass>::const_iterator hit;

		// in MSW the list control does not render the \t use another delimiter
		wxString format = wxT("%s\t:%d\t:%s");
		for (hit = hits.begin(); hit != hits.end(); ++hit) {
			wxString beforeHit = hit->Preview.Mid(0, hit->LineOffset);
			wxString afterHit = hit->Preview.Mid(hit->LineOffset + 10);
			wxString hitText = hit->Preview.Mid(hit->LineOffset, 10);
			
			
			wxString preview = beforeHit + hitText + afterHit;
			preview.Trim(true);
			preview.Trim(false);
			
			wxVector<wxVariant> data;
			data.push_back(hit->FileName);
			data.push_back(wxString::Format(wxT("%d"), hit->LineNumber));
			data.push_back(preview);
			ResultsList->AppendItem(data);
		}
		AllHits.insert(AllHits.end(), hits.begin(), hits.end());
	}
	else {
		Stop();
	}
}

void mvceditor::FindInFilesResultsPanelClass::OnStopButton(wxCommandEvent& event) {
	Stop();
}

void mvceditor::FindInFilesResultsPanelClass::Stop() {
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

void mvceditor::FindInFilesResultsPanelClass::OnRowActivated(wxDataViewEvent& event) {
	int sel = ResultsList->GetSelectedRow();
	ShowMatch(sel);
}

void mvceditor::FindInFilesResultsPanelClass::ShowMatch(int i) {
	if (i < 0 || i >= (int)AllHits.size()) {
		return;
	}
	wxString fileName = AllHits[i].FileName;
	int line = AllHits[i].LineNumber;
	--line; // line is 1-based but wxSTC lines start at zero
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

void mvceditor::FindInFilesResultsPanelClass::OnCopySelectedButton(wxCommandEvent& event) {
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

void mvceditor::FindInFilesResultsPanelClass::OnCopyAllButton(wxCommandEvent& event) {
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

void mvceditor::FindInFilesResultsPanelClass::OnPreviousHitButton(wxCommandEvent& event) {
	ShowPreviousMatch();
}

void mvceditor::FindInFilesResultsPanelClass::OnNextHitButton(wxCommandEvent& event) {
	ShowNextMatch();
}

void mvceditor::FindInFilesResultsPanelClass::OnRegExReplaceHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	mvceditor::PopulateRegExReplaceMenu(regExMenu, ID_REGEX_REPLACE_MENU_START);
	PopupMenu(&regExMenu);	
}

void mvceditor::FindInFilesResultsPanelClass::InsertReplaceRegExSymbol(wxCommandEvent& event) {
	int id = event.GetId() - ID_REGEX_REPLACE_MENU_START;
	mvceditor::AddSymbolToReplaceRegularExpression(ReplaceWithText, id, CurrentInsertionPointReplace);
}

void mvceditor::FindInFilesResultsPanelClass::OnKillFocusReplaceText(wxFocusEvent& event) {
	CurrentInsertionPointReplace = ReplaceWithText->GetInsertionPoint();
	event.Skip();
}

void mvceditor::FindInFilesResultsPanelClass::OnReplaceTextEnter(wxCommandEvent& event) {
	if (ReplaceWithText->FindString(ReplaceWithText->GetValue()) == wxNOT_FOUND) {
		ReplaceWithText->AppendString(ReplaceWithText->GetValue());	
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
	wxString msg;
	msg += _("Searched For: ");
	msg += mvceditor::IcuToWx(FindInFiles.Expression);
	if (mvceditor::FinderClass::REGULAR_EXPRESSION == FindInFiles.Mode) {
		msg += _(" (regex)");
	}
	else if (mvceditor::FinderClass::CASE_INSENSITIVE == FindInFiles.Mode) {
		msg += _(" (case)");
	}
	else if (mvceditor::FinderClass::EXACT == FindInFiles.Mode) {
		msg += _(" (exact)");
	}

	FindLabel->SetLabel(msg);
	ResultText->SetLabel(text);
	FindLabel->GetContainingSizer()->Layout();
	ResultText->GetContainingSizer()->Layout();
	this->Layout();
}

int mvceditor::FindInFilesResultsPanelClass::GetNumberOfMatchedFiles() {
	return MatchedFiles;
}

void mvceditor::FindInFilesResultsPanelClass::OnActionProgress(mvceditor::ActionProgressEventClass& event) {
	if (event.GetId() != FindInFilesGaugeId) {

		// event from another, concurrent find in files panel. let the other panel
		// handle it
		event.Skip();
		return;
	}
	Gauge->IncrementGauge(FindInFilesGaugeId, StatusBarWithGaugeClass::INDETERMINATE_MODE);	
}

mvceditor::FindInFilesDialogClass::FindInFilesDialogClass(wxWindow* parent, mvceditor::FindInFilesFeatureClass& feature)
	: FindInFilesDialogGeneratedClass(parent, wxID_ANY)
	, Feature(feature)
	, CurrentInsertionPointFind(0)
	, CurrentInsertionPointReplace(0) {
	Feature.FindHistory.Attach(FindText);
	Feature.ReplaceHistory.Attach(ReplaceWithText);
	Feature.DirectoriesHistory.Attach(Directory);
	Feature.FilesHistory.Attach(FilesFilter);
	
	// the first time showing this dialog populate the filter to have only PHP file extensions
	if (FilesFilter->GetCount() <= 0) {
		Feature.PreviousFindInFiles.Source.SetIncludeWildcards(feature.App.Globals.FileTypes.PhpFileExtensionsString);
	}
	std::vector<mvceditor::SourceClass> sources = feature.App.Globals.AllEnabledSources();
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
	wxString lastUsedPath = feature.PreviousFindInFiles.Source.RootDirectory.GetFullPath();
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
	mvceditor::RegularExpressionValidatorClass regExValidator(&Feature.PreviousFindInFiles.Expression, FinderMode);
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

void mvceditor::FindInFilesDialogClass::OnOkButton(wxCommandEvent& event) {
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
			Feature.FindHistory.Save();
			Feature.ReplaceHistory.Save();
			Feature.DirectoriesHistory.Save();
			Feature.FilesHistory.Save();

			Feature.FindHistory.Detach();
			Feature.ReplaceHistory.Detach();
			Feature.DirectoriesHistory.Detach();
			Feature.FilesHistory.Detach();
			EndModal(wxID_OK);
		}
	}
}

void mvceditor::FindInFilesDialogClass::OnCancelButton(wxCommandEvent& event) {

	// need to do this to prevent crash on app exit
	Feature.FindHistory.Detach();
	Feature.ReplaceHistory.Detach();
	Feature.DirectoriesHistory.Detach();
	Feature.FilesHistory.Detach();
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

void mvceditor::FindInFilesDialogClass::OnKillFocusFindText(wxFocusEvent& event) {
	CurrentInsertionPointFind = FindText->GetInsertionPoint();
	event.Skip();
}

void mvceditor::FindInFilesDialogClass::OnKillFocusReplaceText(wxFocusEvent& event) {
	CurrentInsertionPointReplace = ReplaceWithText->GetInsertionPoint();
	event.Skip();
}

mvceditor::FindInFilesFeatureClass::FindInFilesFeatureClass(mvceditor::AppClass& app)
	: FeatureClass(app)
	, FindHistory()
	, ReplaceHistory()
	, DirectoriesHistory()
	, FilesHistory()
	, PreviousFindInFiles()
	, DoHiddenFiles(false)
	, ResultsPanels() {
}

void mvceditor::FindInFilesFeatureClass::AddSearchMenuItems(wxMenu* searchMenu) {
	searchMenu->Append(mvceditor::MENU_FIND_IN_FILES + 0, _("Find In Files\tCTRL+SHIFT+F"), 
		_("Find an expression by searching entire directory contents"));
	searchMenu->Append(mvceditor::MENU_FIND_IN_FILES + 1, _("Next Find In Files Match\tALT+F3"), 
		_("Move the cursor to the next Find In Files Match"));
	searchMenu->Append(mvceditor::MENU_FIND_IN_FILES + 2, _("Previous Find In Files Match\tALT+SHIFT+F3"),
		_("Move the cursor to the previous Find In Files Match"));
}

void mvceditor::FindInFilesFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_FIND_IN_FILES + 0] = wxT("Find-Find In Files");
	menuItemIds[mvceditor::MENU_FIND_IN_FILES + 1] = wxT("Find-Find In Files Next Match");
	menuItemIds[mvceditor::MENU_FIND_IN_FILES + 2] = wxT("Find-Find In Files Previous Match");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::FindInFilesFeatureClass::OnEditFindInFiles(wxCommandEvent& event) {

	// prime finder with selected text
	wxString selectedText = GetSelectedText();
	if (!selectedText.empty()) {
		PreviousFindInFiles.Expression = mvceditor::WxToIcu(selectedText);
	}
	FindInFilesDialogClass dialog(GetMainWindow(), *this);
	if (dialog.ShowModal() == wxID_OK) {
		mvceditor::FindInFilesResultsPanelClass* panel = new mvceditor::FindInFilesResultsPanelClass(GetToolsNotebook(), 
			GetNotebook(), GetStatusBarWithGauge(), App.RunningThreads);
		wxBitmap findBitmap = wxNullBitmap;
		if (PreviousFindInFiles.ReplaceExpression.isEmpty()) {
			findBitmap = mvceditor::IconImageAsset(wxT("find-in-files"));
		}
		else {
			findBitmap = mvceditor::IconImageAsset(wxT("replace-in-files"));
		}
		
		if(AddToolsWindow(panel, _("Find In Files Results"), wxEmptyString, findBitmap)) {
			panel->Find(PreviousFindInFiles, DoHiddenFiles);
			ResultsPanels.push_back(panel);
		}
	}
}

void mvceditor::FindInFilesFeatureClass::OnEditFindInFilesNext(wxCommandEvent& event) {
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

void mvceditor::FindInFilesFeatureClass::OnEditFindInFilesPrevious(wxCommandEvent& event) {
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

void mvceditor::FindInFilesFeatureClass::OnToolsNotebookPageClosed(wxAuiNotebookEvent& event) {
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
	EVT_FIND_IN_FILES_HITS(wxID_ANY, mvceditor::FindInFilesResultsPanelClass::OnFileHit)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FILE_READ_COMPLETE, mvceditor::FindInFilesResultsPanelClass::OnFindInFilesComplete)
	EVT_ACTION_PROGRESS(wxID_ANY, mvceditor::FindInFilesResultsPanelClass::OnActionProgress)
	EVT_ACTION_COMPLETE(wxID_ANY, mvceditor::FindInFilesResultsPanelClass::OnActionComplete)

	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_ONE, mvceditor::FindInFilesResultsPanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_TWO, mvceditor::FindInFilesResultsPanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_THREE, mvceditor::FindInFilesResultsPanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_FOUR, mvceditor::FindInFilesResultsPanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_FIVE, mvceditor::FindInFilesResultsPanelClass::InsertReplaceRegExSymbol)
	
	EVT_DATAVIEW_ITEM_ACTIVATED(FindInFilesResultsPanelGeneratedClass::ID_RESULTS_LIST, mvceditor::FindInFilesResultsPanelClass::OnRowActivated)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::FindInFilesFeatureClass, wxEvtHandler)
	EVT_MENU(mvceditor::MENU_FIND_IN_FILES + 0, mvceditor::FindInFilesFeatureClass::OnEditFindInFiles)
	EVT_MENU(mvceditor::MENU_FIND_IN_FILES + 1, mvceditor::FindInFilesFeatureClass::OnEditFindInFilesNext)
	EVT_MENU(mvceditor::MENU_FIND_IN_FILES + 2, mvceditor::FindInFilesFeatureClass::OnEditFindInFilesPrevious)
	EVT_AUINOTEBOOK_PAGE_CLOSE(mvceditor::ID_TOOLS_NOTEBOOK, mvceditor::FindInFilesFeatureClass::OnToolsNotebookPageClosed)
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