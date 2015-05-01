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
#include <globals/Number.h>
#include <globals/Errors.h>
#include <Triumph.h>
#include <wx/ffile.h>
#include <wx/textfile.h>
#include <algorithm>

t4p::FindInFilesHitClass::FindInFilesHitClass()
	: wxObject()
	, FileName()
	, Preview()
	, LineNumber() 
	, LineOffset(0)
	, FileOffset(0) 
	, MatchLength(0) {

}

t4p::FindInFilesHitClass::FindInFilesHitClass(const wxString& fileName, const wxString& preview, 
	int lineNumber, int lineOffset, int fileOffset, int matchLength)
	: wxObject()
	// use c_str() to deep copy
	, FileName(fileName.c_str())
	, Preview(preview.c_str())
	, LineNumber(lineNumber) 
	, LineOffset(lineOffset) 
	, FileOffset(fileOffset) 
	, MatchLength(matchLength) {

}

t4p::FindInFilesHitClass::FindInFilesHitClass(const t4p::FindInFilesHitClass& hit) 
	: wxObject()
	, FileName()
	, Preview() 
	, LineNumber(1) 
	, LineOffset(0)
	, FileOffset(0) 
	, MatchLength(0) {
	Copy(hit);
}

t4p::FindInFilesHitClass& t4p::FindInFilesHitClass::operator=(const t4p::FindInFilesHitClass& hit) {
	Copy(hit);
	return *this;
}

bool t4p::FindInFilesHitClass::operator==(const t4p::FindInFilesHitClass& hit) {
	return FileName == hit.FileName 
		&& LineNumber == hit.LineNumber
		&& LineOffset == hit.LineOffset
		&& FileOffset == hit.FileOffset;
}

void t4p::FindInFilesHitClass::Copy(const t4p::FindInFilesHitClass& hit) {
	FileName = hit.FileName.c_str();
	Preview = hit.Preview.c_str();
	LineNumber = hit.LineNumber;
	LineOffset = hit.LineOffset;
	FileOffset = hit.FileOffset;
	MatchLength = hit.MatchLength;
}

IMPLEMENT_DYNAMIC_CLASS(t4p::FindInFilesHitClass, wxObject)
namespace t4p {

	IMPLEMENT_VARIANT_OBJECT(FindInFilesHitClass)

}


t4p::FindInFilesHitEventClass::FindInFilesHitEventClass(int eventId, const std::vector<t4p::FindInFilesHitClass> &hits)
	: wxEvent(eventId, t4p::EVENT_FIND_IN_FILES_FILE_HIT)
	, Hits(hits) {
}

wxEvent* t4p::FindInFilesHitEventClass::Clone() const {
	wxEvent* newEvt = new t4p::FindInFilesHitEventClass(GetId(), Hits);
	return newEvt;
}

std::vector<t4p::FindInFilesHitClass> t4p::FindInFilesHitEventClass::GetHits() const {
	return Hits;
}

t4p::FindInFilesBackgroundReaderClass::FindInFilesBackgroundReaderClass(t4p::RunningThreadsClass& runningThreads, int eventId) 
	: BackgroundFileReaderClass(runningThreads, eventId) 
	, FindInFiles()
	, SkipFiles() {

}

bool t4p::FindInFilesBackgroundReaderClass::InitForFind( t4p::FindInFilesClass findInFiles, 
															  bool doHiddenFiles,
															  std::vector<wxString> skipFiles) {

	// find in files needs to be a copy; just to be sure
	// its thread safe
	FindInFiles = findInFiles;
	SkipFiles = skipFiles;

	std::vector<t4p::SourceClass> sources;
	sources.push_back(FindInFiles.Source);
	return Init(sources, t4p::DirectorySearchClass::RECURSIVE, doHiddenFiles) && FindInFiles.Prepare();
}

bool t4p::FindInFilesBackgroundReaderClass::InitForReplace(t4p::FindInFilesClass findInFiles,
																 const std::vector<wxString>& replaceFiles,
																 std::vector<wxString> skipFiles) {
	FindInFiles = findInFiles;
	SkipFiles = skipFiles;
	return FindInFiles.Prepare() && InitMatched(replaceFiles);
}

bool t4p::FindInFilesBackgroundReaderClass::BackgroundFileRead(DirectorySearchClass& search) {
	bool found = false;
	found = search.Walk(FindInFiles);
	if (found) {
		wxString fileName = search.GetMatchedFiles().back();
		
		// if this match is for one of the skip files then we want to ignore it
		// DirectorySearch doesn't have a GetCurrentFile() so the one way to know the
		// file that was searched is to do the search
		std::vector<wxString>::iterator it = find(SkipFiles.begin(), SkipFiles.end(), fileName);
		std::vector<t4p::FindInFilesHitClass> hits;
		if (it == SkipFiles.end()) {
			bool destroy = IsCancelled();
			do {
				if (destroy) {
					break;
				}
				t4p::FindInFilesHitClass hit(fileName, 
					t4p::IcuToWx(FindInFiles.GetCurrentLine()), 
					FindInFiles.GetCurrentLineNumber(),
					FindInFiles.GetLineOffset(),
					FindInFiles.GetFileOffset(),
					FindInFiles.GetMatchLength()
				);
				hits.push_back(hit);

			}
			while (!destroy && FindInFiles.FindNext());
			if (!destroy && !hits.empty()) {

				// PostEvent will change the ID of the event to the correct
				// one
				t4p::FindInFilesHitEventClass hitEvent(wxID_ANY, hits);
				PostEvent(hitEvent);
			}
		}
	}
	return found;
}

bool t4p::FindInFilesBackgroundReaderClass::BackgroundFileMatch(const wxString& file) {
	wxString fileToReplace = file;
	int matches = 0;

	// don't do replace for open files
	std::vector<wxString>::iterator it = find(SkipFiles.begin(), SkipFiles.end(), fileToReplace);
	if (it == SkipFiles.end()) {
		matches += FindInFiles.ReplaceAllMatchesInFile(fileToReplace);
	}
	return matches > 0;
}

wxString t4p::FindInFilesBackgroundReaderClass::GetLabel() const {
	return wxT("Find In Files");
}

t4p::FindInFilesFeatureClass::FindInFilesFeatureClass(t4p::AppClass& app)
	: FeatureClass(app)
	, PreviousFindInFiles()
	, DoHiddenFiles(false) {
}
