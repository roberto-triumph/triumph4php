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
#include <wx/stc/stc.h>
#include <wx/utils.h>
#include <wx/tokenzr.h>
#include <unicode/ustring.h>

// IMPLEMENTATION NOTE:
// Take care when using positions given by Scintilla.  Scintilla gives positions in bytes while wxString and UnicodeString
// use character positions. Take caution when using methods like GetCurrentPos(), WordStartPosition(), WordEndPosition()
// This causes problems when Scintilla is handling UTF-8 documents.
// There is a method called GetSafeSubString() that will help you in this regard.

const int mvceditor::CODE_CONTROL_LINT_RESULT_MARKER = 2;
const int mvceditor::CODE_CONTROL_LINT_RESULT_MARGIN = 2;

// the indicator to show squiggly lines for lint errors
const int mvceditor::CODE_CONTROL_INDICATOR_PHP_LINT = 0;

// the indicator to show boxes around found words when user double clicks
// on a word
const int mvceditor::CODE_CONTROL_INDICATOR_FIND = 1;

// start stealing styles from "asp javascript" we will never use those styles
const int mvceditor::CODE_CONTROL_STYLE_PHP_LINT_ANNOTATION = wxSTC_HJA_START;

mvceditor::CodeControlClass::CodeControlClass(wxWindow* parent, CodeControlOptionsClass& options,
											  mvceditor::GlobalsClass* globals, mvceditor::EventSinkClass& eventSink,
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
		, IsTouched(false) {
	Document = NULL;
	
	// we will handle right-click menu ourselves
	UsePopUp(false);
	SetYCaretPolicy(wxSTC_CARET_EVEN, 0);
	ApplyPreferences();
}

mvceditor::CodeControlClass::~CodeControlClass() {
	Document->DetachFromControl(this);
	delete Document;
}
void mvceditor::CodeControlClass::TrackFile(const wxString& filename, UnicodeString& contents) {
	SetUnicodeText(contents);
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

void mvceditor::CodeControlClass::SetUnicodeText(UnicodeString& contents) {

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

void mvceditor::CodeControlClass::Revert() {
	if (!IsNew()) {
		LoadAndTrackFile(CurrentFilename);
	}
}

void mvceditor::CodeControlClass::LoadAndTrackFile(const wxString& fileName) {
	UnicodeString contents;	

	// not using wxStyledTextCtrl::LoadFile() because it does not correctly handle files with high ascii characters
	mvceditor::FindInFilesClass::OpenErrors error = FindInFilesClass::FileContents(fileName, contents);
	if (error == mvceditor::FindInFilesClass::NONE) {
		TrackFile(fileName, contents);
	}
	else if (error == mvceditor::FindInFilesClass::FILE_NOT_FOUND) {
		mvceditor::EditorLogError(mvceditor::ERR_INVALID_FILE, fileName);
	}
	else if (mvceditor::FindInFilesClass::CHARSET_DETECTION == error) {
		mvceditor::EditorLogError(mvceditor::ERR_CHARSET_DETECTION, fileName);
	}
}

void mvceditor::CodeControlClass::TreatAsNew() {
	CurrentFilename = wxT("");
	FileOpenedDateTime = wxDateTime::Now();
}

void mvceditor::CodeControlClass::UpdateOpenedDateTime(wxDateTime openedDateTime) {
	FileOpenedDateTime = openedDateTime;
}

bool mvceditor::CodeControlClass::IsNew() const {
	return CurrentFilename.empty();
}

bool mvceditor::CodeControlClass::SaveAndTrackFile(wxString newFilename) {
	bool saved = false;
	if (CodeControlOptions.TrimTrailingSpaceBeforeSave) {
		TrimTrailingSpaces();
	}
	if (CodeControlOptions.RemoveTrailingBlankLinesBeforeSave) {
		RemoveTrailingBlankLines();
	}
	if (!CurrentFilename.empty() || CurrentFilename == newFilename) {
		saved = SaveFile(CurrentFilename);

		// if file is no changing name then its not changing extension
		// no need to auto detect the document mode
	}
	else if (SaveFile(newFilename)) {
		CurrentFilename = newFilename;
		saved = true;

		// if the file extension changed let's update the code control appropriate
		// for example if a .txt file was saved as a .sql file
		AutoDetectDocumentMode();

		// need to notify the document of the new name
		Document->FileOpened(newFilename);
	}
	if (saved) {

		// when saving, update the internal timestamp so that the external mod check logic works correctly
		wxFileName file(CurrentFilename);
		FileOpenedDateTime = file.GetModificationTime();
	}
	return saved;
}

wxString mvceditor::CodeControlClass::GetFileName() const {
	return CurrentFilename;
}

wxDateTime mvceditor::CodeControlClass::GetFileOpenedDateTime() const {
	return FileOpenedDateTime;
}

void mvceditor::CodeControlClass::SetSelectionAndEnsureVisible(int start, int end) {
	
	// make sure that selection ends up in the middle of the screen, hence the new caret policy
	SetYCaretPolicy(wxSTC_CARET_JUMPS | wxSTC_CARET_EVEN, 0);
	SetSelectionByCharacterPosition(start, end);
	EnsureCaretVisible();
	SetYCaretPolicy(wxSTC_CARET_EVEN, 0);
}

void mvceditor::CodeControlClass::SetSelectionByCharacterPosition(int start, int end) {
	int documentLength = GetTextLength();
	char* buf = new char[documentLength];
	
	// GET_TEXT  message
	SendMsg(2182, documentLength, (long)buf);
	
	int byteStart = mvceditor::CharToUtf8Pos(buf, documentLength, start);
	int byteEnd = mvceditor::CharToUtf8Pos(buf, documentLength, end);
	SetSelection(byteStart, byteEnd);
	delete[] buf;
}

void mvceditor::CodeControlClass::OnCharAdded(wxStyledTextEvent &event) {
	if (event.GetId() != GetId()) {
		event.Skip();
		return;
	}

	// clear the auto complete message
	wxWindow* window = GetGrandParent();
	wxFrame* frame = wxDynamicCast(window, wxFrame);
	if (frame) {
		mvceditor::StatusBarWithGaugeClass* gauge = (mvceditor::StatusBarWithGaugeClass*)frame->GetStatusBar();
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

void mvceditor::CodeControlClass::HandleAutomaticIndentation(char chr) {
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

std::vector<mvceditor::TagClass> mvceditor::CodeControlClass::GetTagsAtCurrentPosition() {
	return Document->GetTagsAtCurrentPosition();
}

std::vector<mvceditor::TagClass> mvceditor::CodeControlClass::GetTagsAtPosition(int pos) {
	return Document->GetTagsAtPosition(pos);
}

void mvceditor::CodeControlClass::HandleAutoCompletion() {
	wxString completeStatus;
	Document->HandleAutoCompletion(completeStatus);
	if (!completeStatus.IsEmpty()) {
		wxWindow* window = GetGrandParent();

		// show the auto complete message
		wxFrame* frame = wxDynamicCast(window, wxFrame);
		if (frame) {
			mvceditor::StatusBarWithGaugeClass* gauge = (mvceditor::StatusBarWithGaugeClass*)frame->GetStatusBar();
			gauge->SetColumn0Text(completeStatus);
		}
	}
}

void mvceditor::CodeControlClass::HandleCallTip(wxChar ch, bool force) {
	Document->HandleCallTip(ch, force);
}

void mvceditor::CodeControlClass::OnUpdateUi(wxStyledTextEvent &event) {
	if (event.GetId() != GetId()) {
		event.Skip();
		return;
	}
	Document->MatchBraces(GetCurrentPos());
	HandleCallTip(0, false);
	event.Skip();
}

void mvceditor::CodeControlClass::OnMarginClick(wxStyledTextEvent& event) {
	if (event.GetId() != GetId()) {
		event.Skip();
		return;
	}
	if (event.GetMargin() == CodeControlOptionsClass::MARGIN_CODE_FOLDING) {
		int line = LineFromPosition(event.GetPosition());
		ToggleFold(line);
	}
}

void mvceditor::CodeControlClass::AutoDetectDocumentMode() {
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
}

void mvceditor::CodeControlClass::ApplyPreferences() {
	if (Document) {
		
		// need this here so that any events are not propagated while the object is
		// being destructed (valgrind found this error)
		Document->DetachFromControl(this);
		delete Document;
		Document = NULL;
	}
	if (mvceditor::CodeControlClass::SQL == DocumentMode) {
		Document = new mvceditor::SqlDocumentClass(Globals, CurrentDbTag);
		Document->SetControl(this);
	}
	else if (mvceditor::CodeControlClass::PHP == DocumentMode) {
		Document = new mvceditor::PhpDocumentClass(Globals);
		Document->SetControl(this);
	}
	else if (mvceditor::CodeControlClass::CSS == DocumentMode) {
		Document = new mvceditor::CssDocumentClass();
		Document->SetControl(this);
	}
	else if (mvceditor::CodeControlClass::JS == DocumentMode) {
		Document = new mvceditor::JsDocumentClass();
		Document->SetControl(this);
	}
	else {
		Document = new mvceditor::TextDocumentClass();
		Document->SetControl(this);
	}
}

UnicodeString mvceditor::CodeControlClass::WordAtCurrentPos() {
	int pos = WordStartPosition(GetCurrentPos(), true);
	int endPos = WordEndPosition(GetCurrentPos(), true);
	
	UnicodeString word = Document->GetSafeSubstring(pos, endPos);
	return word;
}

void  mvceditor::CodeControlClass::OnDoubleClick(wxStyledTextEvent& event) {
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

void mvceditor::CodeControlClass::OnContextMenu(wxContextMenuEvent& event) {
	wxWindow* frame = GetGrandParent();

	// Let the frame handle it because we want features to have menu items
	if (frame) {
		frame->GetEventHandler()->ProcessEvent(event);
	}
	else {
		event.Skip();
	}
}

UnicodeString mvceditor::CodeControlClass::GetSafeText() {
	return Document->GetSafeText();
}

void mvceditor::CodeControlClass::OnKeyDown(wxKeyEvent& event) {
	UndoHighlight();
	if (event.GetKeyCode() == WXK_ESCAPE) {
		CallTipCancel();
	}

	// set touched flag if the character was a normal or shifted character, symbol
	// or number
	int key = event.GetKeyCode();
	if (!event.HasModifiers() && key > WXK_SPACE && key < WXK_DELETE) {
		IsTouched = true;
	}
	else if (!event.HasModifiers() && WXK_BACK == key) {
		IsTouched = true;
	}
	if (!event.HasModifiers() && key >= WXK_NUMPAD0 && key < WXK_DIVIDE) {
		IsTouched = true;
	}
	event.Skip();
}

void mvceditor::CodeControlClass::OnLeftDown(wxMouseEvent& event) {
	UndoHighlight();
	event.Skip();
}

void mvceditor::CodeControlClass::OnMotion(wxMouseEvent& event) {
	int pos = CharPositionFromPointClose(event.GetX(), event.GetY());
	if (pos < 0) {
		event.Skip();
		return;
	}
	
	int style = wxSTC_HPHP_DEFAULT;
	
	// enable doc text when user holds down ALT+SHIFT
	if ((GetStyleAt(pos) & wxSTC_HPHP_DEFAULT) && (event.GetModifiers() & wxMOD_ALT)) {
		wxCommandEvent altEvt(mvceditor::EVT_MOTION_ALT);
		altEvt.SetInt(pos);
		altEvt.SetEventObject(this);
		EventSink.Publish(altEvt);
	}

	// enable clickable links on identifiers
	else if ((GetStyleAt(pos) & wxSTC_HPHP_DEFAULT) && (event.GetModifiers() & wxMOD_CMD)) {
		StyleSetHotSpot(style, true);
		SetHotspotActiveForeground(true, *wxBLUE);
		SetHotspotActiveUnderline(true);
	}
	else {
		StyleSetHotSpot(style, false);
	}
	
	event.Skip();
}

void mvceditor::CodeControlClass::UndoHighlight() {
	if (WordHighlightIsWordHighlighted) {
	
		// kill any current highlight searches
		SetIndicatorCurrent(CODE_CONTROL_INDICATOR_FIND);
		SetIndicatorValue(CODE_CONTROL_INDICATOR_FIND);
		IndicatorClearRange(0, GetTextLength());

		WordHighlightIsWordHighlighted = false;
	}
}

void mvceditor::CodeControlClass::HighlightWord(int utf8Start, int utf8Length) {
	WordHighlightIsWordHighlighted = true;
	SetIndicatorCurrent(CODE_CONTROL_INDICATOR_FIND);
	SetIndicatorValue(CODE_CONTROL_INDICATOR_FIND);
	IndicatorFillRange(utf8Start, utf8Length);
}

void mvceditor::CodeControlClass::MarkLintError(const pelet::LintResultsClass& result) {
	
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
		byteNumber = mvceditor::CharToUtf8Pos(buf, documentLength, charNumber);
		
		SetIndicatorCurrent(CODE_CONTROL_INDICATOR_PHP_LINT);
		SetIndicatorValue(CODE_CONTROL_INDICATOR_PHP_LINT);
		IndicatorFillRange(byteNumber, 10);

		delete[] buf;
	}
	Colourise(0, -1);

	wxString error = mvceditor::IcuToWx(result.Error);
	error += wxString::Format(wxT(" on line %d, offset %d"), result.LineNumber, result.CharacterPosition);
	AnnotationSetVisible(wxSTC_ANNOTATION_BOXED);
	AnnotationSetText(result.LineNumber, error);
	AnnotationSetStyle(result.LineNumber, CODE_CONTROL_STYLE_PHP_LINT_ANNOTATION);
}

void mvceditor::CodeControlClass::MarkLintErrorAndGoto(const pelet::LintResultsClass& result) {
	
	// positions in scintilla are byte offsets. convert chars to bytes so we can jump properly
	int byteNumber = 0;
	if (result.CharacterPosition >= 0) {
		MarkLintError(result);
		int charNumber = result.CharacterPosition;

		int documentLength = GetTextLength();
		char* buf = new char[documentLength];

		// GET_TEXT  message
		SendMsg(2182, documentLength, (long)buf);
		byteNumber = mvceditor::CharToUtf8Pos(buf, documentLength, charNumber);
		GotoPos(byteNumber);

		delete[] buf;
	}
}

void mvceditor::CodeControlClass::ClearLintErrors() {
	MarkerDeleteAll(CODE_CONTROL_LINT_RESULT_MARKER);
	SetIndicatorCurrent(CODE_CONTROL_INDICATOR_PHP_LINT);
	SetIndicatorValue(CODE_CONTROL_INDICATOR_PHP_LINT);
	IndicatorClearRange(0, GetTextLength());
	AnnotationClearAll();
}

void mvceditor::CodeControlClass::SetCurrentDbTag(const mvceditor::DatabaseTagClass& currentDbTag) {
	CurrentDbTag.Copy(currentDbTag);
	
	// if SQL document is active we need to change the currentInfo in that object
	// but since C++ does has poor RTTI we dont know what type Document pointer currently is
	// for now just refresh everything which will update CurrentDbTag
	ApplyPreferences();
}

void mvceditor::CodeControlClass::SetDocumentMode(Mode mode) {
	DocumentMode = mode;
	ApplyPreferences();
}

mvceditor::CodeControlClass::Mode mvceditor::CodeControlClass::GetDocumentMode() {
	return DocumentMode;
}

int mvceditor::CodeControlClass::LineFromCharacter(int charPos) {
	int documentLength = GetTextLength();
	char* buf = new char[documentLength];
	
	// GET_TEXT  message
	SendMsg(2182, documentLength, (long)buf);
	int pos = mvceditor::CharToUtf8Pos(buf, documentLength, charPos);
	delete[] buf;
	return LineFromPosition(pos);
}

void mvceditor::CodeControlClass::SetAsHidden(bool isHidden) {
	IsHidden = isHidden;

	// in case tool tip, auto complete lists that are currently active
	if (IsHidden && CallTipActive()) {
		CallTipCancel();
	}
	if (IsHidden && AutoCompActive()) {
		AutoCompCancel();
	}
}

void mvceditor::CodeControlClass::TrimTrailingSpaces() {
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

void mvceditor::CodeControlClass::RemoveTrailingBlankLines() {
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

wxString mvceditor::CodeControlClass::GetIdString() const {

	// make sure string is unique across program instances
	long pid = wxGetProcessId();
	wxString idString = wxString::Format(wxT("File_%ld_%d"), pid, GetId());
	return idString;
}

void mvceditor::CodeControlClass::SetTouched(bool touched) {
	IsTouched = touched;
}

bool mvceditor::CodeControlClass::Touched() const {
	return IsTouched;
}

void mvceditor::CodeControlClass::OnHotspotClick(wxStyledTextEvent& event) {
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

void mvceditor::CodeControlClass::OnTimerComplete(wxTimerEvent& event) {
	wxStyledTextEvent evt(wxEVT_STC_HOTSPOT_CLICK);
	evt.SetId(GetId());
	evt.SetEventObject(this);
	evt.SetPosition(GetCurrentPos());
	EventSink.Publish(evt);
}

const wxEventType mvceditor::EVT_MOTION_ALT = wxNewEventType();

BEGIN_EVENT_TABLE(mvceditor::CodeControlClass, wxStyledTextCtrl)
	EVT_STC_MARGINCLICK(wxID_ANY, mvceditor::CodeControlClass::OnMarginClick)
	EVT_STC_DOUBLECLICK(wxID_ANY, mvceditor::CodeControlClass::OnDoubleClick)
	EVT_CONTEXT_MENU(mvceditor::CodeControlClass::OnContextMenu)

	EVT_STC_CHARADDED(wxID_ANY, mvceditor::CodeControlClass::OnCharAdded)
	EVT_STC_UPDATEUI(wxID_ANY, mvceditor::CodeControlClass::OnUpdateUi) 

	EVT_LEFT_DOWN(mvceditor::CodeControlClass::OnLeftDown)
	EVT_KEY_DOWN(mvceditor::CodeControlClass::OnKeyDown)
	EVT_MOTION(mvceditor::CodeControlClass::OnMotion)
	EVT_STC_HOTSPOT_CLICK(wxID_ANY, mvceditor::CodeControlClass::OnHotspotClick)
	EVT_TIMER(wxID_ANY, mvceditor::CodeControlClass::OnTimerComplete)
END_EVENT_TABLE()