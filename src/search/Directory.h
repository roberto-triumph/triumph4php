
#ifndef __MVCEDITOR_DIRECTORY_H_
#define __MVCEDITOR_DIRECTORY_H_

#include <wx/string.h>

namespace mvceditor {

/**
 * Delete a directory and all of its sub-directories
 *
 * @param path the directory to delete
 * @return boolean TRUE if the directory was deleted
 */
bool RecursiveRmDir(wxString path);
}

#endif