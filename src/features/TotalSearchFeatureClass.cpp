/**
 * The MIT License
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
#include <features/TotalSearchFeatureClass.h>
#include <search/FindInFilesClass.h>
#include <globals/Assets.h>
#include <MvcEditor.h>
#include <wx/imaglist.h>

static int ID_DIALOG_TIMER = wxNewId();
static int ID_TAG_SEARCH = wxNewId();

mvceditor::TotalSearchFeatureClass::TotalSearchFeatureClass(mvceditor::AppClass& app)
	: FeatureClass(app) {

}


void mvceditor::TotalSearchFeatureClass::AddSearchMenuItems(wxMenu* searchMenu) {
	searchMenu->Append(mvceditor::MENU_TOTAL_SEARCH + 0,
		_("Total Search\tCTRL+R"), _("Search for project resources"), wxITEM_NORMAL);
}

void mvceditor::TotalSearchFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_TOTAL_SEARCH + 0] = wxT("Resource-Search For Resource ...");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::TotalSearchFeatureClass::OnTotalSearch(wxCommandEvent& event) {
	std::vector<mvceditor::TotalTagResultClass> selectedTags;
	int lineNumber = 0;
	mvceditor::TotalSearchDialogClass dialog(GetMainWindow(), *this, selectedTags, lineNumber);
	if (wxOK == dialog.ShowModal() && !selectedTags.empty()) {
		for (size_t i = 0; i < selectedTags.size(); ++i) {
			mvceditor::TotalTagResultClass result = selectedTags[i];
			switch (result.Type) {
			case mvceditor::TotalTagResultClass::FILE_TAG:
				OpenFileTag(result.FileTag, lineNumber);
				break;
			case mvceditor::TotalTagResultClass::TABLE_DATA_TAG:
				OpenDbData(result.TableTag);
				break;
			case mvceditor::TotalTagResultClass::TABLE_DEFINITION_TAG:
				OpenDbTable(result.TableTag);
				break;
			case mvceditor::TotalTagResultClass::CLASS_TAG:
			case mvceditor::TotalTagResultClass::FUNCTION_TAG:
			case mvceditor::TotalTagResultClass::METHOD_TAG:
				OpenPhpTag(result.PhpTag);
				break;
			}
		}
	}
}

void mvceditor::TotalSearchFeatureClass::OpenFileTag(const mvceditor::FileTagClass& fileTag, int lineNumber) {
	mvceditor::OpenFileCommandEventClass cmd(fileTag.FullPath, -1, -1, lineNumber);
	App.EventSink.Post(cmd);
}

void mvceditor::TotalSearchFeatureClass::OpenPhpTag(const mvceditor::TagClass& tag) {
	UnicodeString content;
	mvceditor::FindInFilesClass::FileContents(tag.FullPath, content);
	int32_t position, 
			length;
	bool found = mvceditor::ParsedTagFinderClass::GetResourceMatchPosition(tag, content, position, length);
	
	if (found) {
		mvceditor::OpenFileCommandEventClass cmd(tag.FullPath, position, length);
		App.EventSink.Publish(cmd);
	}
	else {
		mvceditor::OpenFileCommandEventClass cmd(tag.FullPath);
		App.EventSink.Publish(cmd);
	}
}

void mvceditor::TotalSearchFeatureClass::OpenDbTable(const mvceditor::DatabaseTableTagClass& tableTag) {
	mvceditor::OpenDbTableCommandEventClass cmd(
		mvceditor::EVENT_CMD_DB_TABLE_DEFINITION_OPEN, tableTag.TableName, tableTag.ConnectionHash
	);
	App.EventSink.Publish(cmd);
}

void mvceditor::TotalSearchFeatureClass::OpenDbData(const mvceditor::DatabaseTableTagClass& tableTag) {
	mvceditor::OpenDbTableCommandEventClass cmd(
		mvceditor::EVENT_CMD_DB_TABLE_DATA_OPEN, tableTag.TableName, tableTag.ConnectionHash
	);
	App.EventSink.Publish(cmd);
}

mvceditor::TotalSearchDialogClass::TotalSearchDialogClass(wxWindow* parent, mvceditor::TotalSearchFeatureClass& feature,
	std::vector<mvceditor::TotalTagResultClass>& selectedTags, int& lineNumber)
: TotalSearchDialogGeneratedClass(parent, wxID_ANY)
, Feature(feature)
, LastSearch()
, Timer(this, ID_DIALOG_TIMER)
, RunningThreads() 
, Results() 
, SelectedTags(selectedTags) 
, LineNumber(lineNumber) {
	RunningThreads.SetMaxThreads(1);
	RunningThreads.AddEventHandler(this);
	Timer.Start(300, wxTIMER_CONTINUOUS);
	
	MatchesList->Clear();
}

void mvceditor::TotalSearchDialogClass::OnCancelButton(wxCommandEvent& event) {
	RunningThreads.Shutdown();
	RunningThreads.RemoveEventHandler(this);
	EndModal(wxID_CANCEL);
}

void mvceditor::TotalSearchDialogClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = wxString::FromAscii("Type in a file name, file name:page number, "
		"class name,  or class name::method name. The resulting page will then be opened.\n\nExamples:\n\n"
		"user.php\n"
		"user.php:129\n"
		"User\n"
		"User::login\n\n"
		"You can search with partial files names\n"
		"ser.php (would match user.php)\n\n"
		"You can search with partial class names\n"
		"Use (would match Use, User, UserClass, ...)\n\n"
		"You can search entire class names\n"
		"User:: (would match all methods including inherited methods, from User class)\n\n"
		"You can search all methods\n"
		"::print (would match all methods in all classes that start with 'print' )\n\n"
		"You can also search for any database tables found in any of your configured connections\n\n"
		"use (would match on tables that start with \"use\": \"user\", \"user_roles\", ...)\n"
		"\n"
		"Cache Status:\n"
		"A search cannot be made while indexing is taking place. If indexing is taking place\n"
		"then you will see the status as \"indexing\". Please wait until indexing completes."
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Resource Search Help"), wxOK, this);
}

void mvceditor::TotalSearchDialogClass::OnMatchesListKeyDown(wxKeyEvent& event) {
	unsigned int nextIndex = MatchesList->GetSelection();
	bool skip = true;
	if (event.GetKeyCode() == WXK_RETURN) {
		skip = false;
	}
	else if (event.GetKeyCode() == WXK_UP) {
		skip = true;
	}
	else if (event.GetKeyCode() == WXK_DOWN) {
		skip = true;
	}
	if (!skip && nextIndex >= 0 && nextIndex < MatchesList->GetCount()) {
		ChooseSelectedAndEnd(nextIndex);
	}
	if (skip) {
		event.Skip();
	}
}

void mvceditor::TotalSearchDialogClass::OnMatchesListDoubleClick(wxCommandEvent& event) {
	unsigned int selected = event.GetSelection();
	if (selected >= 0) {
		ChooseSelectedAndEnd(selected);
	}
}

void mvceditor::TotalSearchDialogClass::OnOkButton(wxCommandEvent& event) {
	
	// open all checked tags
	for (unsigned int i = 0; i < MatchesList->GetCount(); ++i) {
		if (MatchesList->IsChecked(i)) {
			mvceditor::TotalTagResultClass tag = Results[i];
			SelectedTags.push_back(tag);
		}
	}
	RunningThreads.Shutdown();
	RunningThreads.RemoveEventHandler(this);
	EndModal(wxOK);
}

void mvceditor::TotalSearchDialogClass::OnSearchEnter(wxCommandEvent& event) {
	unsigned int selected = MatchesList->GetSelection();
	if (selected >= 0) {
		ChooseSelectedAndEnd(selected);
	}
}

void mvceditor::TotalSearchDialogClass::OnSearchKeyDown(wxKeyEvent& event) {
	unsigned int currentIndex = MatchesList->GetSelection();
	unsigned int nextIndex = currentIndex;
	bool skip = true;
	if (event.GetKeyCode() == WXK_DOWN) {
		nextIndex++;
		if (nextIndex >= MatchesList->GetCount()) {
			nextIndex = 0;
		}
		skip = false;
	}
	else if (event.GetKeyCode() == WXK_UP) {
		nextIndex--;
		if (nextIndex < 0) {
			nextIndex = MatchesList->GetCount() - 1;
		}
		skip = false;
	}
	if (nextIndex >= 0 && nextIndex < MatchesList->GetCount()) {
		MatchesList->SetSelection(nextIndex);
	}
	if (skip) {
		event.Skip();
	}
}

void mvceditor::TotalSearchDialogClass::OnTimer(wxTimerEvent& event) {
	wxString text = SearchText->GetValue();
	
	// trim spaces from the ends
	text.Trim(false).Trim(true);
	if (text != LastSearch && text.length() > 2) {
		Timer.Stop();
		LastSearch = text;
		
		mvceditor::TotalTagSearchActionClass* action =  
			new mvceditor::TotalTagSearchActionClass(RunningThreads, ID_TAG_SEARCH);
		action->SetSearch(Feature.App.Globals, text, Feature.App.Globals.AllEnabledSourceDirectories());
		RunningThreads.Queue(action);
	}
}

void mvceditor::TotalSearchDialogClass::OnSearchComplete(mvceditor::TotalTagSearchCompleteEventClass& event) {
	MatchesList->Clear();
	Results = event.Tags;
	LineNumber = event.LineNumber;
	
	std::vector<mvceditor::TotalTagResultClass>::const_iterator tag;
	for (tag = Results.begin(); tag != Results.end(); ++tag) {
		wxString value;
		wxString desc;
		if (mvceditor::TotalTagResultClass::CLASS_TAG == tag->Type) {
			value = mvceditor::IcuToWx(tag->PhpTag.ClassName);
			desc = tag->PhpTag.FullPath;
		}
		else if (mvceditor::TotalTagResultClass::FILE_TAG == tag->Type) {
			value = tag->FileTag.Name();
			desc = tag->FileTag.FullPath;
		}
		else if (mvceditor::TotalTagResultClass::FUNCTION_TAG == tag->Type) {
			value = mvceditor::IcuToWx(tag->PhpTag.Identifier);
			desc = tag->PhpTag.FullPath;
		}
		else if (mvceditor::TotalTagResultClass::METHOD_TAG == tag->Type) {
			value = mvceditor::IcuToWx(tag->PhpTag.ClassName) + 
				wxT("::") + mvceditor::IcuToWx(tag->PhpTag.Identifier);
			desc = tag->PhpTag.FullPath;
		}
		else if (mvceditor::TotalTagResultClass::TABLE_DATA_TAG == tag->Type) {
			value = tag->TableTag.TableName;
			desc = _("Database Table Data for ") + tag->TableTag.TableName;
		}
		else if (mvceditor::TotalTagResultClass::TABLE_DEFINITION_TAG == tag->Type) {
			value = tag->TableTag.TableName;
			desc = _("Database Table Definition for ") + tag->TableTag.TableName;
		}
		
		MatchesList->Append(value + wxT(" - ") + desc);
	}
	if (MatchesList->GetCount() > 0) {
		MatchesList->SetSelection(0);
	}
	if (MatchesList->GetCount() == 0) {
		MatchesLabel->SetLabel(_("No matches found"));
	}
	else if (MatchesList->GetCount() == 1) {
		MatchesLabel->SetLabel(_("1 match found"));
	}
	else {
		MatchesLabel->SetLabel(wxString::Format("%ld matches found", event.Tags.size()));
	}
	Timer.Start(300, wxTIMER_CONTINUOUS);
}

void mvceditor::TotalSearchDialogClass::ChooseSelectedAndEnd(size_t selected) {
	if (selected < 0 || selected >= Results.size()) {
		return;
	}
	mvceditor::TotalTagResultClass tag = Results[selected];
	SelectedTags.push_back(tag);
	
	RunningThreads.Shutdown();
	RunningThreads.RemoveEventHandler(this);
	EndModal(wxOK);
}

BEGIN_EVENT_TABLE(mvceditor::TotalSearchFeatureClass, mvceditor::FeatureClass)
	EVT_MENU(mvceditor::MENU_TOTAL_SEARCH + 0, mvceditor::TotalSearchFeatureClass::OnTotalSearch)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::TotalSearchDialogClass, TotalSearchDialogGeneratedClass)
	EVT_TIMER(ID_DIALOG_TIMER, mvceditor::TotalSearchDialogClass::OnTimer)
	EVENT_TOTAL_TAG_SEARCH_COMPLETE(ID_TAG_SEARCH, mvceditor::TotalSearchDialogClass::OnSearchComplete)
END_EVENT_TABLE()
