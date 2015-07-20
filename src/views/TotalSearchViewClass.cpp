/**
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#include <views/TotalSearchViewClass.h>
#include <search/FindInFilesClass.h>
#include <globals/Assets.h>
#include <globals/Number.h>
#include <Triumph.h>
#include <wx/imaglist.h>
#include <wx/wupdlock.h>
#include <map>
#include <vector>

static int ID_DIALOG_TIMER = wxNewId();
static int ID_TAG_SEARCH = wxNewId();

t4p::TotalSearchViewClass::TotalSearchViewClass(t4p::TotalSearchFeatureClass& feature)
	: FeatureViewClass()
	, Feature(feature) {
}


void t4p::TotalSearchViewClass::AddSearchMenuItems(wxMenu* searchMenu) {
	searchMenu->Append(t4p::MENU_TOTAL_SEARCH + 0,
		_("Total Search\tCTRL+R"), _("Search for project resources"), wxITEM_NORMAL);
}

void t4p::TotalSearchViewClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[t4p::MENU_TOTAL_SEARCH + 0] = wxT("Resource-Search For Resource ...");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void t4p::TotalSearchViewClass::OnTotalSearch(wxCommandEvent& event) {
	std::vector<t4p::TotalTagResultClass> selectedTags;
	int lineNumber = 0;
	t4p::TotalSearchDialogClass dialog(GetMainWindow(), Feature, selectedTags, lineNumber);
	if (wxOK == dialog.ShowModal() && !selectedTags.empty()) {
		for (size_t i = 0; i < selectedTags.size(); ++i) {
			t4p::TotalTagResultClass result = selectedTags[i];
			switch (result.Type) {
			case t4p::TotalTagResultClass::FILE_TAG:
				Feature.OpenFileTag(result.FileTag, lineNumber);
				break;
			case t4p::TotalTagResultClass::TABLE_DATA_TAG:
				Feature.OpenDbData(result.TableTag);
				break;
			case t4p::TotalTagResultClass::TABLE_DEFINITION_TAG:
				Feature.OpenDbTable(result.TableTag);
				break;
			case t4p::TotalTagResultClass::CLASS_TAG:
			case t4p::TotalTagResultClass::FUNCTION_TAG:
			case t4p::TotalTagResultClass::METHOD_TAG:
				Feature.OpenPhpTag(result.PhpTag);
				break;
			}
		}
	}
}

t4p::TotalSearchDialogClass::TotalSearchDialogClass(wxWindow* parent, t4p::TotalSearchFeatureClass& feature,
	std::vector<t4p::TotalTagResultClass>& selectedTags, int& lineNumber)
: TotalSearchDialogGeneratedClass(parent, wxID_ANY)
, Feature(feature)
, LastSearch()
, Timer(this, ID_DIALOG_TIMER)
, RunningThreads()
, Results()
, SelectedTags(selectedTags)
, LineNumber(lineNumber)
, IsCacheBeingBuilt(false)
, IsCacheEmpty(false) {
	RunningThreads.SetMaxThreads(1);
	RunningThreads.AddEventHandler(this);
	Timer.Start(300, wxTIMER_CONTINUOUS);

	MatchesList->Clear();
	CacheStatusLabel->SetLabel(wxT("Cache Status: OK"));

	UpdateCacheStatus();
}

void t4p::TotalSearchDialogClass::OnCancelButton(wxCommandEvent& event) {
	RunningThreads.Shutdown();
	RunningThreads.RemoveEventHandler(this);
	EndModal(wxID_CANCEL);
}

void t4p::TotalSearchDialogClass::OnHelpButton(wxCommandEvent& event) {
	TotalSearchHelpDialogGeneratedClass dialog(this);
	dialog.ShowModal();
}

void t4p::TotalSearchDialogClass::OnMatchesListKeyDown(wxKeyEvent& event) {
	int nextIndex = MatchesList->GetSelection();
	bool skip = true;
	if (event.GetKeyCode() == WXK_RETURN) {
		skip = false;
	} else if (event.GetKeyCode() == WXK_UP) {
		skip = true;
	} else if (event.GetKeyCode() == WXK_DOWN) {
		skip = true;
	}
	if (!skip && t4p::NumberLessThan(nextIndex, MatchesList->GetCount())) {
		ChooseSelectedAndEnd(nextIndex);
	}
	if (skip) {
		event.Skip();
	}
}

void t4p::TotalSearchDialogClass::OnMatchesListDoubleClick(wxCommandEvent& event) {
	int selected = event.GetSelection();
	if (selected >= 0) {
		ChooseSelectedAndEnd(selected);
	}
}

void t4p::TotalSearchDialogClass::OnOkButton(wxCommandEvent& event) {
	// open all checked tags
	for (unsigned int i = 0; i < MatchesList->GetCount(); ++i) {
		if (MatchesList->IsChecked(i)) {
			t4p::TotalTagResultClass tag = Results[i];
			SelectedTags.push_back(tag);
		}
	}
	RunningThreads.Shutdown();
	RunningThreads.RemoveEventHandler(this);
	EndModal(wxOK);
}

void t4p::TotalSearchDialogClass::OnSearchEnter(wxCommandEvent& event) {
	int selected = MatchesList->GetSelection();
	if (selected >= 0) {
		ChooseSelectedAndEnd(selected);
	}
}

void t4p::TotalSearchDialogClass::OnSearchKeyDown(wxKeyEvent& event) {
	int currentIndex = MatchesList->GetSelection();
	if (currentIndex == wxNOT_FOUND) {
		event.Skip();
		return;
	}

	// at this point currentIndex is guaranteed to be >= 0
	unsigned int nextIndex = currentIndex;
	bool skip = true;
	if (event.GetKeyCode() == WXK_DOWN) {
		nextIndex++;
		if (nextIndex >= MatchesList->GetCount()) {
			nextIndex = 0;
		}
		skip = false;
	} else if (event.GetKeyCode() == WXK_UP) {
		if (nextIndex == 0) {
			nextIndex = MatchesList->GetCount() - 1;
		} else {
			nextIndex--;
		}
		skip = false;
	}
	if (nextIndex < MatchesList->GetCount()) {
		MatchesList->SetSelection(nextIndex);

		// in Mac os x the item is not scrolled to the
		// viewable port when it is selected, we must do
		// it ourselves
		wxPlatformInfo info;
		if (info.GetOperatingSystemId() == wxOS_MAC_OSX_DARWIN) {
			MatchesList->EnsureVisible(nextIndex);
		}
	}
	if (skip) {
		event.Skip();
	}
}

void t4p::TotalSearchDialogClass::OnTimer(wxTimerEvent& event) {
	UpdateCacheStatus();

	// don't want to query the cache while the cache is being
	// built. this is because the tag cache is built using
	// sqlite and sqlite cannot handle simultaneous reads and
	// writes.
	if (IsCacheBeingBuilt) {
		return;
	}

	wxString text = SearchText->GetValue();

	// trim spaces from the ends
	text.Trim(false).Trim(true);
	if (text != LastSearch && text.length() > 2) {
		Timer.Stop();
		LastSearch = text;

		t4p::TotalTagSearchActionClass* action =
			new t4p::TotalTagSearchActionClass(RunningThreads, ID_TAG_SEARCH);
		action->SetSearch(Feature.App.Globals, text, Feature.App.Globals.AllEnabledSourceDirectories());
		RunningThreads.Queue(action);
	}
}

void t4p::TotalSearchDialogClass::OnSearchComplete(t4p::TotalTagSearchCompleteEventClass& event) {
	MatchesList->Clear();
	Results = event.Tags;
	LineNumber = event.LineNumber;

	std::vector<t4p::TotalTagResultClass>::const_iterator tag;
	for (tag = Results.begin(); tag != Results.end(); ++tag) {
		wxString value;
		wxString desc;
		if (t4p::TotalTagResultClass::CLASS_TAG == tag->Type) {
			value = t4p::IcuToWx(tag->PhpTag.ClassName);
			desc = tag->PhpTag.FullPath;
		} else if (t4p::TotalTagResultClass::FILE_TAG == tag->Type) {
			value = tag->FileTag.Name();
			desc = tag->FileTag.FullPath;
		} else if (t4p::TotalTagResultClass::FUNCTION_TAG == tag->Type) {
			value = t4p::IcuToWx(tag->PhpTag.Identifier);
			desc = tag->PhpTag.FullPath;
		} else if (t4p::TotalTagResultClass::METHOD_TAG == tag->Type) {
			value = t4p::IcuToWx(tag->PhpTag.ClassName) +
				wxT("::") + t4p::IcuToWx(tag->PhpTag.Identifier);
			desc = tag->PhpTag.FullPath;
		} else if (t4p::TotalTagResultClass::TABLE_DATA_TAG == tag->Type) {
			t4p::DatabaseTagClass dbTag;
			Feature.App.Globals.FindDatabaseTagByHash(tag->TableTag.ConnectionHash, dbTag);
			value = tag->TableTag.TableName;
			desc = _("Database Table Data for ")
				+ tag->TableTag.TableName
				+ _(" in connection ")
				+ t4p::IcuToWx(dbTag.Label);
		} else if (t4p::TotalTagResultClass::TABLE_DEFINITION_TAG == tag->Type) {
			t4p::DatabaseTagClass dbTag;
			Feature.App.Globals.FindDatabaseTagByHash(tag->TableTag.ConnectionHash, dbTag);
			value = tag->TableTag.TableName;
			desc = _("Database Table Definition for ")
				+ tag->TableTag.TableName
				+ _(" in connection ")
				+ t4p::IcuToWx(dbTag.Label);
		}

		MatchesList->Append(value + wxT(" - ") + desc);
	}
	if (MatchesList->GetCount() > 0) {
		MatchesList->SetSelection(0);
	}
	if (MatchesList->GetCount() == 0) {
		MatchesLabel->SetLabel(_("No matches found"));
	} else if (MatchesList->GetCount() == 1) {
		MatchesLabel->SetLabel(_("1 match found"));
	} else {
		MatchesLabel->SetLabel(wxString::Format("%ld matches found", event.Tags.size()));
	}
	Timer.Start(300, wxTIMER_CONTINUOUS);
}

void t4p::TotalSearchDialogClass::ChooseSelectedAndEnd(size_t selected) {
	if (selected >= Results.size()) {
		return;
	}
	t4p::TotalTagResultClass tag = Results[selected];
	SelectedTags.push_back(tag);

	RunningThreads.Shutdown();
	RunningThreads.RemoveEventHandler(this);
	EndModal(wxOK);
}

void t4p::TotalSearchDialogClass::UpdateCacheStatus() {
	if (Feature.App.Sequences.Running()) {
		// only update the label when there is a change in
		// indexing status, that way we eliminate flicker
		if (!IsCacheBeingBuilt) {
			wxWindowUpdateLocker locker(this);
			CacheStatusLabel->SetLabel(wxT("Cache Status: Indexing"));
			this->Layout();
		}
		IsCacheBeingBuilt = true;
		return;
	}

	if (Feature.App.Globals.TagCache.IsFileCacheEmpty()) {
		if (!IsCacheEmpty) {
			wxWindowUpdateLocker locker(this);
			CacheStatusLabel->SetLabel(wxT("Cache Status: Empty"));
			this->Layout();
		}
		IsCacheEmpty = true;
		return;
	}
	if (IsCacheBeingBuilt && !Feature.App.Sequences.Running()) {
		// here the tag parsing has finished
		IsCacheBeingBuilt = false;
		wxWindowUpdateLocker locker(this);
		CacheStatusLabel->SetLabel(wxT("Cache Status: OK"));
		this->Layout();
	}
}

BEGIN_EVENT_TABLE(t4p::TotalSearchViewClass, t4p::FeatureViewClass)
	EVT_MENU(t4p::MENU_TOTAL_SEARCH + 0, t4p::TotalSearchViewClass::OnTotalSearch)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::TotalSearchDialogClass, TotalSearchDialogGeneratedClass)
	EVT_TIMER(ID_DIALOG_TIMER, t4p::TotalSearchDialogClass::OnTimer)
	EVENT_TOTAL_TAG_SEARCH_COMPLETE(ID_TAG_SEARCH, t4p::TotalSearchDialogClass::OnSearchComplete)
END_EVENT_TABLE()
