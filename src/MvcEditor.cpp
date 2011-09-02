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
#include <wx/wx.h>
#include <wx/cmdline.h>
#include <windows/AppFrameClass.h>

class MvcEditorAppClass : public wxApp {

public:
	/** 
	 * Initialize the application 
	 */
	virtual bool OnInit();
};

IMPLEMENT_APP(MvcEditorAppClass)

/**
 * when app starts, create the new app frame
 */
bool MvcEditorAppClass::OnInit() {
	mvceditor::AppFrameClass* frame = new mvceditor::AppFrameClass(NULL);
	wxCmdLineEntryDesc description[3];
	description[0].description = wxT("File name to open on startup");
	description[0].flags =  wxCMD_LINE_PARAM_OPTIONAL;
	description[0].kind = wxCMD_LINE_OPTION;
	description[0].longName = wxT("file");
	description[0].shortName = wxT("f");
	description[0].type = wxCMD_LINE_VAL_STRING;
	description[1].description = wxT("Project to open on startup");
	description[1].flags =  wxCMD_LINE_PARAM_OPTIONAL;
	description[1].kind = wxCMD_LINE_OPTION;
	description[1].longName = wxT("project");
	description[1].shortName = wxT("p");
	description[1].type = wxCMD_LINE_VAL_STRING;
	description[2].description = wxT("");
	description[2].flags =  0;
	description[2].kind = wxCMD_LINE_NONE;
	description[2].longName = NULL;
	description[2].shortName = NULL;
	description[2].type = wxCMD_LINE_VAL_NONE;
	wxCmdLineParser parser(description, argc, argv);
	parser.SetLogo(wxT("MVC Editor"));
	int result = parser.Parse(true);
	if (0 == result) {
		wxString filename,
			projectDirectory;
		if (parser.Found(wxT("file"), &filename)) {
			std::vector<wxString> filenames;
			filenames.push_back(filename);
			frame->FileOpen(filenames);
		}
		if (parser.Found(wxT("project"), &projectDirectory)) {
			frame->ProjectOpen(projectDirectory);
		}	
		SetTopWindow(frame);
		frame->Maximize(true);
		frame->Show(true);
		return true;
	}
	return false;
}