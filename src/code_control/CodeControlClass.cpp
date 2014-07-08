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
#include <code_control/CodeControlClass.h>
#include <code_control/DocumentClass.h>
#include <globals/String.h>
#include <globals/GlobalsClass.h>
#include <globals/Errors.h>
#include <globals/Events.h>
#include <widgets/StatusBarWithGaugeClass.h>
#include <search/FindInFilesClass.h>
#include <wx/filename.h>
#include <wx/ffile.h>
#include <wx/stc/stc.h>
#include <wx/utils.h>
#include <wx/tokenzr.h>
#include <unicode/ustring.h>
#include <sys/stat.h>

// IMPLEMENTATION NOTE:
// Take care when using positions given by Scintilla.  Scintilla gives positions in bytes while wxString and UnicodeString
// use character positions. Take caution when using methods like GetCurrentPos(), WordStartPosition(), WordEndPosition()
// This causes problems when Scintilla is handling UTF-8 documents.
// There is a method called GetSafeSubString() that will help you in this regard.

const int t4p::CODE_CONTROL_LINT_RESULT_MARKER = 1;
const int t4p::CODE_CONTROL_SEARCH_HIT_GOOD_MARKER = 2;
const int t4p::CODE_CONTROL_SEARCH_HIT_BAD_MARKER = 3;
const int t4p::CODE_CONTROL_BOOKMARK_MARKER = 4;
const int t4p::CODE_CONTROL_EXECUTION_MARKER = 5;
const int t4p::CODE_CONTROL_BREAKPOINT_MARKER = 6;

// the indicator to show squiggly lines for lint errors
const int t4p::CODE_CONTROL_INDICATOR_PHP_LINT = 0;

// the indicator to show boxes around found words when user double clicks
// on a word
const int t4p::CODE_CONTROL_INDICATOR_FIND = 1;

// start stealing styles from "asp javascript" we will never use those styles
const int t4p::CODE_CONTROL_STYLE_PHP_LINT_ANNOTATION = wxSTC_HJA_START;

static bool SaveFileWithCharset(const wxString& fullPath, const wxString& contents, 
	const wxString& charset, bool hasSignature) {
	bool ret = false;
	
	wxFFile file(fullPath, wxT("wb"));
	wxCSConv conv(charset);
    if (file.IsOpened() && conv.IsOk()) {
		if (hasSignature) {
			wxString sig = wxT("\xfeff");
			file.Write(sig, conv);
		}
		
		ret = file.Write(contents, conv);
	}
	return ret;
		
}

t4p::CodeControlClass::CodeControlClass(wxWindow* parent, CodeControlOptionsClass& options,
											  t4p::GlobalsClass* globals, t4p::EventSinkClass& eventSink,
			wxWindowID id, const wxPoint& position, const wxSize& size, long style,
			const wxString& name)
		: wxStyledTextCtrl(parent, id, position, size, style, name)
		, CodeControlOptions(options)
		, CurrentFilename()
		, CurrentDbTag()
		, HotspotTimer(this)
		, Globals(globals)
		, EventSink(eventSink)
		, WordHighlightIsWordHighlighted(false)
		, DocumentMode(TEXT) 
		, IsHidden(false) 
		, IsTouched(false) 
		, HasSearchMarkers(false) 
		, HasFileSignature(false) 
		, Charset() {
	Document = NULL;
	
	// we will handle right-click menu ourselves
	UsePopUp(false);
	SetYCaretPolicy(wxSTC_CARET_EVEN, 0);
	ApplyPreferences();
}

t4p::CodeControlClass::~CodeControlClass() {
	Document->DetachFromControl(this);
	delete Document;
}
void t4p::CodeControlClass::TrackFile(const wxString& filename, UnicodeString& contents, 
		const wxString& charset, bool hasSignature) {
	SetUnicodeText(contents);
	HasFileSignature = hasSignature;
	Charset = charset;
	EmptyUndoBuffer();
	SetSavePoint();
	CurrentFilename = filename;
	wxFileName file(filename);
	if (file.IsOk()) {
		FileOpenedDateTime = file.GetModificationTime();
	}
	AutoDetectDocumentMode();

	// order is important; calling FileOpened after document type is detected so that
	// the proper callback is used
	Document->FileOpened(filename);
}

void t4p::CodeControlClass::SetUnicodeText(UnicodeString& contents) {

	// lets avoid the IcuToWx to prevent going from 
	// UnicodeString -> UTF8 -> wxString  -> UTF8 -> Scintilla
	// cost of translation (computation and memoory) could be big for big sized files
	// because of the double encoding due to all three libraries using
	// different internal encodings for their strings
	// code below just goes
	// UnicodeString -> UTF8 -> Scintilla
	UErrorCode status = U_ZERO_ERROR;
	int32_t rawLength;
	int32_t length = contents.length();
	const UChar* src = contents.getBuffer();
	u_strToUTF8(NULL, 0, &rawLength, src, length, &status);
	status = U_ZERO_ERROR;
	char* dest = new char[rawLength + 1];
	int32_t written;
	u_strToUTF8(dest, rawLength + 1, &written, src, length, &status);
	if(U_SUCCESS(status)) {
	
		// SetText message
		SendMsg(2181, 0, (long)(const char*)dest);
	}
	delete[] dest;
}

void t4p::CodeControlClass::Revert() {
	if (!IsNew()) {
		LoadAndTrackFile(CurrentFilename);
	}
}

void t4p::CodeControlClass::LoadAndTrackFile(const wxString& fileName) {
	UnicodeString contents;

	// not using wxStyledTextCtrl::LoadFile() because it does not correctly handle files with high ascii characters
	bool hasSignature = false;
	wxString charset;
	t4p::FindInFilesClass::OpenErrors error = FindInFilesClass::FileContents(fileName, contents, 
		charset, hasSignature);
	if (error == t4p::FindInFilesClass::NONE) {
		TrackFile(fileName, contents, charset, hasSignature);
	}
	else if (error == t4p::FindInFilesClass::FILE_NOT_FOUND) {
		t4p::EditorLogError(t4p::ERR_INVALID_FILE, fileName);
	}
	else if (t4p::FindInFilesClass::CHARSET_DETECTION == error) {
		t4p::EditorLogError(t4p::ERR_CHARSET_DETECTION, fileName);
	}
}

void t4p::CodeControlClass::TreatAsNew() {
	CurrentFilename = wxT("");
	FileOpenedDateTime = wxDateTime::Now();
}

void t4p::CodeControlClass::UpdateOpenedDateTime(wxDateTime openedDateTime) {
	FileOpenedDateTime = openedDateTime;
}

bool t4p::CodeControlClass::IsNew() const {
	return CurrentFilename.empty();
}

bool t4p::CodeControlClass::SaveAndTrackFile(wxString newFilename) {
	bool saved = false;
	if (CodeControlOptions.TrimTrailingSpaceBeforeSave) {
		TrimTrailingSpaces();
	}
	if (CodeControlOptions.RemoveTrailingBlankLinesBeforeSave) {
		RemoveTrailingBlankLines();
	}

	if (!CurrentFilename.empty() || CurrentFilename == newFilename) {
		saved = SaveFileWithCharset(CurrentFilename, GetValue(), Charset, HasFileSignature);

		// if file is not changing name then its not changing extension
		// no need to auto detect the document mode
	}
	else if (SaveFileWithCharset(newFilename, GetValue(), Charset, HasFileSignature)) {
		CurrentFilename = newFilename;
		saved = true;

		// if the file extension changed let's update the code control appropriate
		// for example if a .txt file was saved as a .sql file
		AutoDetectDocumentMode();

		// need to notify the document of the new name
		Document->FileOpened(newFilename);
	}
	if (saved) {
		SetSavePoint();

		// when saving, update the internal timestamp so that the external mod check logic works correctly
		// using stat() function instead of wxFileName::GetModificationTime()
		// it seems that GetFileTimes() win32 function and stat() function
		// do different things; GetFileTimes() seems to have some caching
		struct stat buff;
		const wxCharBuffer cname = CurrentFilename.c_str();
		if (stat(cname.data(), &buff) >= 0) {
			FileOpenedDateTime.Set(buff.st_mtime);
		}
		else {
			FileOpenedDateTime = wxDateTime::Now();
		}
	}
	return saved;
}

wxString t4p::CodeControlClass::GetFileName() const {
	return CurrentFilename;
}

wxDateTime t4p::CodeControlClass::GetFileOpenedDateTime() const {
	return FileOpenedDateTime;
}

void t4p::CodeControlClass::SetSelectionAndEnsureVisible(int start, int end) {
	
	// make sure that selection ends up in the middle of the screen, hence the new caret policy
	SetYCaretPolicy(wxSTC_CARET_JUMPS | wxSTC_CARET_EVEN, 0);
	SetSelectionByCharacterPosition(start, end, true);
	EnsureCaretVisible();
	SetYCaretPolicy(wxSTC_CARET_EVEN, 0);
}

void t4p::CodeControlClass::SetSelectionByCharacterPosition(int start, int end, bool setPos) {
	int documentLength = GetTextLength();
	char* buf = new char[documentLength];
	
	// GET_TEXT  message
	SendMsg(2182, documentLength, (long)buf);
	
	int byteStart = t4p::CharToUtf8Pos(buf, documentLength, start);
	int byteEnd = t4p::CharToUtf8Pos(buf, documentLength, end);
	SetSelection(byteStart, byteEnd);

	if (setPos) {
		GotoPos(byteStart);
	}
	delete[] buf;
}

void t4p::CodeControlClass::OnCharAdded(wxStyledTextEvent &event) {
	if (event.GetId() != GetId()) {
		event.Skip();
		return;
	}

	// clear the auto complete message
	wxWindow* window = GetGrandParent();
	wxFrame* frame = wxDynamicCast(window, wxFrame);
	if (frame) {
		t4p::StatusBarWithGaugeClass* gauge = (t4p::StatusBarWithGaugeClass*)frame->GetStatusBar();
		gauge->SetColumn0Text(wxT(""));
	}

	char ch = (char)event.GetKey();
	if (CodeControlOptions.EnableAutomaticLineIndentation) {
		HandleAutomaticIndentation(ch);
	}
	if (CodeControlOptions.EnableAutoCompletion) {

		// attempt to show code completion on method operator / scope operator
		// since this event happens after currentPos is advanced; so
		// current char is now at currentPos - 1, so previous char is at currentPos - 2
		char prevChar = GetCharAt(GetCurrentPos() - 2);
		if (('-' == prevChar && '>' == ch) || (':' == prevChar && ':' == ch)) {
			HandleAutoCompletion();	
		}	
		HandleCallTip(ch);
	}

	// expand the line if currently folded. adding +1 to the current line so that even if enter is pressed,
	// the folded line is expanded
	int currentLine = GetCurrentLine();
	EnsureVisible(currentLine + 1);
	
	event.Skip();
}

void t4p::CodeControlClass::HandleAutomaticIndentation(char chr) {
	int currentLine = GetCurrentLine();
	
	// ATTN: Change this if support for mac files with \r is needed
	if ('\n' == chr) {
		int lineIndentation = 0;
		if (currentLine > 0) {
			lineIndentation = GetLineIndentation(currentLine - 1);
		}
		if (lineIndentation > 0) {

			// must check if the code uses tabs or spaces
			int indentSize = GetIndent() ? GetIndent() : GetTabWidth();
			int tabs = lineIndentation / indentSize;
			for (int i = 0; i < tabs; ++i) {
				CmdKeyExecute(wxSTC_CMD_TAB);
			}
		}
	}
}

std::vector<t4p::TagClass> t4p::CodeControlClass::GetTagsAtCurrentPosition() {
	wxString completeStatus;
	std::vector<t4p::TagClass> tags = Document->GetTagsAtCurrentPosition(completeStatus);
	if (!completeStatus.IsEmpty()) {
		wxWindow* window = GetGrandParent();

		// show the auto complete message
		wxFrame* frame = wxDynamicCast(window, wxFrame);
		if (frame) {
			t4p::StatusBarWithGaugeClass* gauge = (t4p::StatusBarWithGaugeClass*)frame->GetStatusBar();
			gauge->SetColumn0Text(completeStatus);
		}
	}
	return tags;
}

std::vector<t4p::TagClass> t4p::CodeControlClass::GetTagsAtPosition(int pos) {
	wxString completeStatus;
	std::vector<t4p::TagClass> tags = Document->GetTagsAtPosition(pos, completeStatus);
	if (!completeStatus.IsEmpty()) {
		wxWindow* window = GetGrandParent();

		// show the auto complete message
		wxFrame* frame = wxDynamicCast(window, wxFrame);
		if (frame) {
			t4p::StatusBarWithGaugeClass* gauge = (t4p::StatusBarWithGaugeClass*)frame->GetStatusBar();
			gauge->SetColumn0Text(completeStatus);
		}
	}
	return tags;
}

void t4p::CodeControlClass::HandleAutoCompletion() {
	wxString completeStatus;
	Document->HandleAutoCompletion(completeStatus);
	if (!completeStatus.IsEmpty()) {
		wxWindow* window = GetGrandParent();

		// show the auto complete message
		wxFrame* frame = wxDynamicCast(window, wxFrame);
		if (frame) {
			t4p::StatusBarWithGaugeClass* gauge = (t4p::StatusBarWithGaugeClass*)frame->GetStatusBar();
			gauge->SetColumn0Text(completeStatus);
		}
	}
}

void t4p::CodeControlClass::HandleCallTip(wxChar ch, bool force) {
	wxString completeStatus;
	Document->HandleCallTip(ch, force, completeStatus);
	if (!completeStatus.IsEmpty()) {
		wxWindow* window = GetGrandParent();

		// show the auto complete message
		wxFrame* frame = wxDynamicCast(window, wxFrame);
		if (frame) {
			t4p::StatusBarWithGaugeClass* gauge = (t4p::StatusBarWithGaugeClass*)frame->GetStatusBar();
			gauge->SetColumn0Text(completeStatus);
		}
	}
}

void t4p::CodeControlClass::OnUpdateUi(wxStyledTextEvent &event) {
	if (event.GetId() != GetId()) {
		event.Skip();
		return;
	}

	Document->MatchBraces(GetCurrentPos());
	HandleCallTip(0, false);
	event.Skip();
}

void t4p::CodeControlClass::OnMarginClick(wxStyledTextEvent& event) {
	if (event.GetId() != GetId()) {
		event.Skip();
		return;
	}
	if (event.GetMargin() == t4p::CodeControlOptionsClass::MARGIN_CODE_FOLDING) {
		int line = LineFromPosition(event.GetPosition());
		ToggleFold(line);
	}
	else {

		// features will not be interested in  the margin click of the code
		// folding markers
		EventSink.Publish(event);
	}
}

void t4p::CodeControlClass::AutoDetectDocumentMode() {
	wxString fileName = GetFileName();
	if (Globals->FileTypes.HasAPhpExtension(fileName)) {
		DocumentMode = PHP;
	}
	else if (Globals->FileTypes.HasASqlExtension(fileName)) {
		DocumentMode = SQL;
	}
	else if (Globals->FileTypes.HasACssExtension(fileName)) {
		DocumentMode = CSS;
	}
	else if (Globals->FileTypes.HasAJsExtension(fileName)) {
		DocumentMode = JS;
	}
	else if (Globals->FileTypes.HasAConfigExtension(fileName)) {
		DocumentMode = CONFIG;
	}
	else if (Globals->FileTypes.HasACrontabExtension(fileName)) {
		DocumentMode = CRONTAB;
	}
	else if (Globals->FileTypes.HasAYamlExtension(fileName)) {
		DocumentMode = YAML;
	}
	else if (Globals->FileTypes.HasAXmlExtension(fileName)) {
		DocumentMode = XML;
	}
	else if (Globals->FileTypes.HasARubyExtension(fileName)) {
		DocumentMode = RUBY;
	}
	else if (Globals->FileTypes.HasALuaExtension(fileName)) {
		DocumentMode = LUA;
	}
	else if (Globals->FileTypes.HasAMarkdownExtension(fileName)) {
		DocumentMode = MARKDOWN;
	}
	else if (Globals->FileTypes.HasABashExtension(fileName)) {
		DocumentMode = BASH;
	}
	else if (Globals->FileTypes.HasADiffExtension(fileName)) {
		DocumentMode = DIFF;
	}
	else {
		DocumentMode = TEXT;
	}
	ApplyPreferences();
}

void t4p::CodeControlClass::ApplyPreferences() {
	if (Document) {
		
		// need this here so that any events are not propagated while the object is
		// being destructed (valgrind found this error)
		Document->DetachFromControl(this);
		delete Document;
		Document = NULL;
	}
	if (t4p::CodeControlClass::SQL == DocumentMode) {
		Document = new t4p::SqlDocumentClass(Globals, CurrentDbTag);
		Document->SetControl(this);
	}
	else if (t4p::CodeControlClass::PHP == DocumentMode) {
		Document = new t4p::PhpDocumentClass(Globals);
		Document->SetControl(this);
	}
	else if (t4p::CodeControlClass::CSS == DocumentMode) {
		Document = new t4p::CssDocumentClass();
		Document->SetControl(this);
	}
	else if (t4p::CodeControlClass::JS == DocumentMode) {
		Document = new t4p::JsDocumentClass();
		Document->SetControl(this);
	}
	else {
		Document = new t4p::TextDocumentClass();
		Document->SetControl(this);
	}
}

UnicodeString t4p::CodeControlClass::WordAtCurrentPos() {
	int pos = WordStartPosition(GetCurrentPos(), true);
	int endPos = WordEndPosition(GetCurrentPos(), true);
	
	UnicodeString word = Document->GetSafeSubstring(pos, endPos);
	return word;
}

void  t4p::CodeControlClass::OnDoubleClick(wxStyledTextEvent& event) {
	if (event.GetId() != GetId()) {
		event.Skip();
		return;
	}

	// remove any previous match indicators
	SetIndicatorCurrent(CODE_CONTROL_INDICATOR_FIND);
	SetIndicatorValue(CODE_CONTROL_INDICATOR_FIND);
	IndicatorClearRange(0, GetTextLength());
		
	EventSink.Publish(event);
}

void t4p::CodeControlClass::OnContextMenu(wxContextMenuEvent& event) {
	wxWindow* frame = GetGrandParent();

	// Let the frame handle it because we want features to have menu items
	if (frame) {
		frame->GetEventHandler()->ProcessEvent(event);
	}
	else {
		event.Skip();
	}
}

UnicodeString t4p::CodeControlClass::GetSafeText() {
	return Document->GetSafeText();
}

void t4p::CodeControlClass::OnKeyDown(wxKeyEvent& event) {
	UndoHighlight();
	if (event.GetKeyCode() == WXK_ESCAPE) {
		CallTipCancel();
	}

	// set touched flag if the character was a normal or shifted character, symbol
	// or number
	int key = event.GetKeyCode();
	bool hasChanged = false;
	if (!event.HasModifiers() && key > WXK_SPACE && key < WXK_DELETE) {
		IsTouched = true;
		hasChanged = true;
	}
	else if (!event.HasModifiers() && WXK_BACK == key) {
		IsTouched = true;
		hasChanged = true;
	}
	if (!event.HasModifiers() && key >= WXK_NUMPAD0 && key < WXK_DIVIDE) {
		IsTouched = true;
		hasChanged = true;
	}
	
	// if there was a search marker, delete it
	if (hasChanged && HasSearchMarkers) {
		ClearSearchMarkers();
	}
	event.Skip();
}

void t4p::CodeControlClass::OnLeftDown(wxMouseEvent& event) {
	UndoHighlight();
	event.Skip();
}

void t4p::CodeControlClass::OnMotion(wxMouseEvent& event) {
	int pos = CharPositionFromPointClose(event.GetX(), event.GetY());
	if (pos < 0) {
		event.Skip();
		return;
	}
	
	int style = wxSTC_HPHP_DEFAULT;
	
	// enable doc text when user holds down **only** ALT
	// since CTRL+ALT enables multiple selection
	if ((GetStyleAt(pos) & wxSTC_HPHP_DEFAULT) && (event.GetModifiers() & wxMOD_ALT) 
		&& !(event.GetModifiers() & wxMOD_CONTROL)) {
		wxCommandEvent altEvt(t4p::EVT_MOTION_ALT);
		altEvt.SetInt(pos);
		altEvt.SetEventObject(this);
		EventSink.Publish(altEvt);
	}

	// enable clickable links on identifiers
	else if ((GetStyleAt(pos) & wxSTC_HPHP_DEFAULT) && (event.GetModifiers() & wxMOD_CMD) 
		&& !(event.GetModifiers() & wxMOD_ALT)) {
		StyleSetHotSpot(style, true);
		SetHotspotActiveForeground(true, *wxBLUE);
		SetHotspotActiveUnderline(true);
	}
	else {
		StyleSetHotSpot(style, false);
	}
	
	event.Skip();
}

void t4p::CodeControlClass::UndoHighlight() {
	if (WordHighlightIsWordHighlighted) {
	
		// kill any current highlight searches
		SetIndicatorCurrent(CODE_CONTROL_INDICATOR_FIND);
		SetIndicatorValue(CODE_CONTROL_INDICATOR_FIND);
		IndicatorClearRange(0, GetTextLength());

		WordHighlightIsWordHighlighted = false;
	}
}

void t4p::CodeControlClass::HighlightWord(int utf8Start, int utf8Length) {
	WordHighlightIsWordHighlighted = true;
	SetIndicatorCurrent(CODE_CONTROL_INDICATOR_FIND);
	SetIndicatorValue(CODE_CONTROL_INDICATOR_FIND);
	IndicatorFillRange(utf8Start, utf8Length);
}

void t4p::CodeControlClass::MarkLintError(const pelet::LintResultsClass& result) {
	
	// positions in scintilla are byte offsets. convert chars to bytes so we can mark
	// the squigglies properly
	int byteNumber = 0;
	if (result.CharacterPosition >= 0) {
		MarkerAdd(result.LineNumber - 1, CODE_CONTROL_LINT_RESULT_MARKER);

		int charNumber = result.CharacterPosition;

		int documentLength = GetTextLength();
		char* buf = new char[documentLength];
		
		// GET_TEXT  message
		SendMsg(2182, documentLength, (long)buf);
		byteNumber = t4p::CharToUtf8Pos(buf, documentLength, charNumber);
		
		SetIndicatorCurrent(CODE_CONTROL_INDICATOR_PHP_LINT);
		SetIndicatorValue(CODE_CONTROL_INDICATOR_PHP_LINT);

		// fill until the end of the word
		int end = WordEndPosition(byteNumber, true);
		IndicatorFillRange(byteNumber, end - byteNumber);

		delete[] buf;
	}
	Colourise(0, -1);

	wxString error = t4p::IcuToWx(result.Error);
	error += wxString::Format(wxT(" on line %d, offset %d"), result.LineNumber, result.CharacterPosition);
	AnnotationSetVisible(wxSTC_ANNOTATION_BOXED);
	AnnotationSetText(result.LineNumber, error);
	AnnotationSetStyle(result.LineNumber, CODE_CONTROL_STYLE_PHP_LINT_ANNOTATION);
}

void t4p::CodeControlClass::MarkLintErrorAndGoto(const pelet::LintResultsClass& result) {
	
	// positions in scintilla are byte offsets. convert chars to bytes so we can jump properly
	int byteNumber = 0;
	if (result.CharacterPosition >= 0) {
		MarkLintError(result);
		int charNumber = result.CharacterPosition;

		int documentLength = GetTextLength();
		char* buf = new char[documentLength];

		// GET_TEXT  message
		SendMsg(2182, documentLength, (long)buf);
		byteNumber = t4p::CharToUtf8Pos(buf, documentLength, charNumber);
		GotoPos(byteNumber);

		delete[] buf;
	}
}

void t4p::CodeControlClass::ClearLintErrors() {
	MarkerDeleteAll(CODE_CONTROL_LINT_RESULT_MARKER);
	SetIndicatorCurrent(CODE_CONTROL_INDICATOR_PHP_LINT);
	SetIndicatorValue(CODE_CONTROL_INDICATOR_PHP_LINT);
	IndicatorClearRange(0, GetTextLength());
	AnnotationClearAll();
}

void t4p::CodeControlClass::MarkSearchHit(int lineNumber, bool goodHit) {
	
	// line is 1-based but wxSTC lines start at zero
	if (goodHit) {
		MarkerAdd(lineNumber - 1, CODE_CONTROL_SEARCH_HIT_GOOD_MARKER);
	}
	else {
		MarkerAdd(lineNumber - 1, CODE_CONTROL_SEARCH_HIT_BAD_MARKER);
	}
	HasSearchMarkers = true;
}

void t4p::CodeControlClass::MarkSearchHitAndGoto(int lineNumber, int startPos, int endPos, bool goodHit) {
	MarkSearchHit(lineNumber, goodHit);
	SetSelectionAndEnsureVisible(startPos, endPos);
}

void t4p::CodeControlClass::ClearSearchMarkers() {
	MarkerDeleteAll(CODE_CONTROL_SEARCH_HIT_GOOD_MARKER);
	MarkerDeleteAll(CODE_CONTROL_SEARCH_HIT_BAD_MARKER);
	HasSearchMarkers = false;
}

bool t4p::CodeControlClass::BookmarkMarkCurrent(int& lineNumber, int& handle) {
	int currentLine = GetCurrentLine();
	int newHandle = MarkerAdd(currentLine, CODE_CONTROL_BOOKMARK_MARKER);
	if (newHandle != -1) {
		
		// we want to return 1-based line numbers, easier for the end user
		lineNumber = currentLine + 1;
		handle = newHandle;
	}
	return newHandle != -1;
}

bool t4p::CodeControlClass::BookmarkMarkAt(int lineNumber, int& handle) {
	
	// given line is 1-based, scintilla lines are 0-based
	int newHandle = MarkerAdd(lineNumber - 1, CODE_CONTROL_BOOKMARK_MARKER);
	if (newHandle != -1) {
		handle = newHandle;
	}
	return newHandle != -1;
}

int t4p::CodeControlClass::BookmarkGetLine(int handle) {
	int line = MarkerLineFromHandle(handle);
	if (line >= 0) {
		line++;
	}
	return line;
}

void t4p::CodeControlClass::BookmarkClearAll() {
	MarkerDeleteAll(CODE_CONTROL_BOOKMARK_MARKER);
}

void t4p::CodeControlClass::BookmarkClearAt(int lineNumber) {
	
	// given line is 1-based, scintilla lines are 0-based
	MarkerDelete(lineNumber - 1, CODE_CONTROL_BOOKMARK_MARKER);
}

bool t4p::CodeControlClass::ExecutionMarkAt(int lineNumber) {
	
	// given line is 1-based, scintilla lines are 0-based
	MarkerDeleteAll(CODE_CONTROL_EXECUTION_MARKER);
	int newHandle = MarkerAdd(lineNumber - 1, CODE_CONTROL_EXECUTION_MARKER);
	EnsureVisible(lineNumber - 1);
	return newHandle != -1;
}


void t4p::CodeControlClass::ExecutionMarkRemove() {
	MarkerDeleteAll(CODE_CONTROL_EXECUTION_MARKER);
}

bool t4p::CodeControlClass::BreakpointMarkAt(int lineNumber, int& handle) {
	
	// given line is 1-based, scintilla lines are 0-based
	int newHandle = MarkerAdd(lineNumber - 1, CODE_CONTROL_BREAKPOINT_MARKER);
	if (newHandle != -1) {
		handle = newHandle;
	}
	return newHandle != -1;
}

void t4p::CodeControlClass::BreakpointRemove(int lineNumber) {

	// given line is 1-based, scintilla lines are 0-based
	MarkerDelete(lineNumber - 1, CODE_CONTROL_BREAKPOINT_MARKER);
}

void t4p::CodeControlClass::BreakpointRemoveAll() {
	MarkerDeleteAll(CODE_CONTROL_BREAKPOINT_MARKER);
}

int t4p::CodeControlClass::BreakpointGetLine(int handle) {
	int line = MarkerLineFromHandle(handle);
	if (line >= 0) {
		line++;
	}
	return line;
}

void t4p::CodeControlClass::SetCurrentDbTag(const t4p::DatabaseTagClass& currentDbTag) {
	CurrentDbTag.Copy(currentDbTag);
	
	// if SQL document is active we need to change the currentInfo in that object
	// but since C++ does has poor RTTI we dont know what type Document pointer currently is
	// for now just refresh everything which will update CurrentDbTag
	ApplyPreferences();
}

void t4p::CodeControlClass::SetDocumentMode(Mode mode) {
	DocumentMode = mode;
	ApplyPreferences();
}

t4p::CodeControlClass::Mode t4p::CodeControlClass::GetDocumentMode() {
	return DocumentMode;
}

int t4p::CodeControlClass::LineFromCharacter(int charPos) {
	int documentLength = GetTextLength();
	char* buf = new char[documentLength];
	
	// GET_TEXT  message
	SendMsg(2182, documentLength, (long)buf);
	int pos = t4p::CharToUtf8Pos(buf, documentLength, charPos);
	delete[] buf;
	return LineFromPosition(pos);
}

void t4p::CodeControlClass::SetAsHidden(bool isHidden) {
	IsHidden = isHidden;

	// in case tool tip, auto complete lists that are currently active
	if (IsHidden && CallTipActive()) {
		CallTipCancel();
	}
	if (IsHidden && AutoCompActive()) {
		AutoCompCancel();
	}
}

void t4p::CodeControlClass::TrimTrailingSpaces() {
	int maxLines = GetLineCount();
	for (int line = 0; line < maxLines; line++) {
		int lineStart = PositionFromLine(line);
		int lineEnd = GetLineEndPosition(line);
		int i = lineEnd - 1;
		char ch = (char)GetCharAt(i);
		while ((i >= lineStart) && ((ch == ' ') || (ch == '\t'))) {
			i--;
			ch = GetCharAt(i);
		}
		if (i < (lineEnd - 1)) {
			SetTargetStart(i + 1);
			SetTargetEnd(lineEnd);
			ReplaceTarget(wxT(""));
		}
	}
}

void t4p::CodeControlClass::RemoveTrailingBlankLines() {
	if (DocumentMode != PHP) {
		return;
	}

	// search backwards from the end of the file
	// if the first non-whitespace characters are "?>" then
	// we remove all space characters
	int maxPos = GetTextLength();
	char c = 0, prev = 0;
	bool done = false;
	bool seenAngleBracket = false;
	bool seenQuestion = false;
	for (int i = maxPos - 1; i >= 0 && !done; i--) {
		if ('>' == prev && '?' == c) {

			// if we get here then the first non-space characters that were found were
			// the PHP end tags
			// 3 = don't remove the ending PHP tag
			SetTargetStart(i + 3);
			SetTargetEnd(maxPos);
			ReplaceTarget(wxT(""));
			done = true;
		}
		prev = c;
		c = GetCharAt(i);

		// once we see an angle bracket, we come to our decision
		// if we dont do this, then a single angle bracket will get removed
		// for example when the file ends like this:
		// "?>
		// \n
		// \n>"
		
		if ('>' == c && seenAngleBracket) {
			done = true;
		}
		else if ('>' == c && !seenAngleBracket) {
			seenAngleBracket = true;
		}
		if ('?' == c && seenQuestion) {
			done = true;
		}
		else if ('?' == c && !seenQuestion) {
			seenQuestion = true;
		}

		// if we hit anything other than a space character or the PHP end
		// tag, then don't modify anything
		if (!isspace(c) && c != '?' && c != '>') {
			done = true;
		}
	}
}

wxString t4p::CodeControlClass::GetIdString() const {

	// make sure string is unique across program instances
	long pid = wxGetProcessId();
	wxString idString = wxString::Format(wxT("File_%ld_%d"), pid, GetId());
	return idString;
}

void t4p::CodeControlClass::SetTouched(bool touched) {
	IsTouched = touched;
}

bool t4p::CodeControlClass::Touched() const {
	return IsTouched;
}

void t4p::CodeControlClass::OnHotspotClick(wxStyledTextEvent& event) {
	if (event.GetId() != GetId()) {
		event.Skip();
		return;
	}
	if (PHP != DocumentMode) {
		event.Skip();
		return;
	}
	int pos = event.GetPosition();
	if (pos < 0) {
		event.Skip();
		return;
	}
	if (!Globals) {
		event.Skip();
		return;
	}

	// process the event at some point later, otherwise scintilla will expand selection
	// when the tag is in the currently opened file.  
	// for example, when clicking on "$this->method()"
	HotspotTimer.Start(100, wxTIMER_ONE_SHOT);
}

void t4p::CodeControlClass::OnTimerComplete(wxTimerEvent& event) {
	wxStyledTextEvent evt(wxEVT_STC_HOTSPOT_CLICK);
	evt.SetId(GetId());
	evt.SetEventObject(this);
	evt.SetPosition(GetCurrentPos());
	EventSink.Publish(evt);
}

void t4p::CodeControlClass::OnModified(wxStyledTextEvent& event) {
	EventSink.Publish(event);
}

const wxEventType t4p::EVT_MOTION_ALT = wxNewEventType();

BEGIN_EVENT_TABLE(t4p::CodeControlClass, wxStyledTextCtrl)
	EVT_STC_MARGINCLICK(wxID_ANY, t4p::CodeControlClass::OnMarginClick)
	EVT_STC_DOUBLECLICK(wxID_ANY, t4p::CodeControlClass::OnDoubleClick)
	EVT_CONTEXT_MENU(t4p::CodeControlClass::OnContextMenu)

	EVT_STC_CHARADDED(wxID_ANY, t4p::CodeControlClass::OnCharAdded)
	EVT_STC_UPDATEUI(wxID_ANY, t4p::CodeControlClass::OnUpdateUi) 
	EVT_STC_MODIFIED(wxID_ANY, t4p::CodeControlClass::OnModified)

	EVT_LEFT_DOWN(t4p::CodeControlClass::OnLeftDown)
	EVT_KEY_DOWN(t4p::CodeControlClass::OnKeyDown)
	EVT_MOTION(t4p::CodeControlClass::OnMotion)
	EVT_STC_HOTSPOT_CLICK(wxID_ANY, t4p::CodeControlClass::OnHotspotClick)
	EVT_TIMER(wxID_ANY, t4p::CodeControlClass::OnTimerComplete)
END_EVENT_TABLE()