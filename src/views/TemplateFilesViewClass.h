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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __TEMPLATEFILES_VIEWCLASS_H__
#define __TEMPLATEFILES_VIEWCLASS_H__

#include <views/FeatureViewClass.h>
#include <features/TemplateFilesFeatureClass.h>
#include <views/wxformbuilder/TemplateFilesFeatureForms.h>
#include <language_php/TemplateFileTagClass.h>
#include <actions/ActionClass.h>
#include <wx/imaglist.h>

namespace t4p {

/**
 * This is a class that will show the user any view files that are
 * associated with the current URL (the URL that the user chose in
 * the toolbar).
 * 
 * The class will do the following:
 * (1) generate the call stack file in a background thread
 * (2) call the PHP view file detector (in an external process)
 * 
 * After all of this, it will populate a window with the template files
 * for the URL that the user has selected.
 */
class TemplateFilesViewClass : public t4p::FeatureViewClass {
	
public:
	
	TemplateFilesViewClass(t4p::TemplateFilesFeatureClass& feature);

	void SetCurrentUrl(t4p::UrlTagClass url);
	
	void AddViewMenuItems(wxMenu* viewMenu);


	/**
	 * @return wxString the currently opened file. Note that this may not be
	 *         a valid file (new, untitled files).
	 */
	wxString CurrentFile();

	/**
	 * Opens the given file. file must be a full path.
	 */
	void OpenFile(wxString file);
	
private:
	
	/**
	 * show (or create) the view files window and start the calculations if needed
	 */
	void OnTemplateFilesMenu(wxCommandEvent& event);

	/**
	 * when the template file detection process completes update the variable tree
	 */
	void OnTemplateDetectionComplete(t4p::ActionEventClass& event);
		
	void ShowPanel();
	
	t4p::TemplateFilesFeatureClass& Feature;
	
	DECLARE_EVENT_TABLE()
};

class TemplateFilesPanelClass : public TemplateFilesPanelGeneratedClass {
	
public:

	TemplateFilesFeatureClass& Feature;
	
	TemplateFilesViewClass& View;
	
	TemplateFilesPanelClass(wxWindow* parent, int id, 
		TemplateFilesFeatureClass& feature,
		TemplateFilesViewClass& view);

	void UpdateResults();

	void ClearResults();

	void UpdateControllers();
	
protected:

	void OnHelpButton(wxCommandEvent& event);

	void OnCurrentButton(wxCommandEvent& event);

	void OnControllerChoice(wxCommandEvent& event);

	void OnActionChoice(wxCommandEvent& event);

	void OnTreeItemActivated(wxTreeEvent& event);

private:

	enum {
		IMAGE_TEMPLATE_FOLDER = 0,
		IMAGE_TEMPLATE_FOLDER_OPEN,
		IMAGE_TEMPLATE_FILE,
		IMAGE_TEMPLATE_VARIABLE
	};

	wxImageList ImageList;
};

}

#endif