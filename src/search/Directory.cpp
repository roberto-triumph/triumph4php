
#include <search/Directory.h>
#include <wx/dir.h>
#include <wx/utils.h>
#include <wx/filename.h>

bool mvceditor::RecursiveRmDir(wxString path) {
	wxDir dir(path);
	wxString name;
	bool next = dir.GetFirst(&name, wxEmptyString, wxDIR_FILES | wxDIR_DIRS | wxDIR_HIDDEN);

	// if dir is already empty then we want to return true
	bool good = !next;
	wxFileName dirName;
	dirName.AssignDir(path);
	while (next) {
		wxString nextFile = dirName.GetPathWithSep() + name;
		if (wxDirExists(nextFile)) {
			RecursiveRmDir(nextFile);
		}
		else {
			good = wxRemoveFile(nextFile);
			wxASSERT_MSG(good, wxT("could not remove file:") + nextFile);
		}
		next = dir.GetNext(&name);
	}

	// remove this directory
	good = wxRmdir(path);
	wxASSERT_MSG(good, wxT("could not remove directory:") + path);
	return good;
}