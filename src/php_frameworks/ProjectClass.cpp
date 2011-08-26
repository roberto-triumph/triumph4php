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
 * @author     $Author: robertop2004@gmail.com $
 * @date       $Date: 2011-06-10 17:19:31 -0700 (Fri, 10 Jun 2011) $
 * @version    $Rev: 555 $ 
 */
#include <php_frameworks/ProjectClass.h>

mvceditor::ProjectOptionsClass::ProjectOptionsClass()  
	: Framework(GENERIC)
	, RootPath() { 
}
	
mvceditor::ProjectOptionsClass::ProjectOptionsClass(const ProjectOptionsClass& other) { 
	Framework = other.Framework; 
	RootPath = other.RootPath; 
}

mvceditor::ProjectClass* mvceditor::ProjectClass::Factory(const ProjectOptionsClass& options) {
	mvceditor::ProjectClass* project = NULL;
	switch(options.Framework) {
		case SYMFONY:
			project = new mvceditor::SymfonyProjectClass(options);
			break;
		case GENERIC:
			project = new mvceditor::ProjectClass(options);
			break;
	}
	return project;
}

wxString mvceditor::ProjectClass::GetPhpKeywords() const {
	return wxT("php if else elseif do while for foreach switch ")
	  wxT("case break continue default function return public private protected ")
	  wxT("class interface extends implements static final const true false ")
	  wxT("NULL global array echo empty eval exit isset list print unset __LINE__ ")
	  wxT("__FILE__ __DIR__ __FUNCTION__ __CLASS__ __METHOD__ __NAMESPACE__ ")
	  wxT("require require_once include include_once stdClass parent self abstract ")
	  wxT("clone namespace use as new bool boolean float double real string int ")
	  wxT("integer");
}

bool mvceditor::ProjectClass::Create(wxString& errors) {
	
	//folder has been created by FolderDialog, so nothing to do here 
	return true;
}

mvceditor::ProjectClass::ProjectClass(const mvceditor::ProjectOptionsClass& options)
	: Options(options)
	, ResourceFinder() {
	ResourceFinder.FilesFilter = GetPhpFileExtensions();
}

wxString  mvceditor::ProjectClass::GetRootPath() const { 
	return Options.RootPath; 
}

wxString mvceditor::ProjectClass::Sanitize(const wxString& arg) const {
	wxString ret = arg;
	ret.Replace(wxT("'"), wxT("\\'"), true);
	return wxT("'") + ret + wxT("'");
}

wxString mvceditor::ProjectClass::GetPhpFileExtensions() const {
	return wxT("*.php");
}

wxString mvceditor::ProjectClass::GetPhpExecutable() const {
	return wxT("php");
}

mvceditor::ResourceFinderClass* mvceditor::ProjectClass::GetResourceFinder() {
	return &ResourceFinder;
}

mvceditor::SymfonyProjectClass::SymfonyProjectClass(const mvceditor::ProjectOptionsClass& options)
	: ProjectClass(options) {
	
}

bool mvceditor::SymfonyProjectClass::Create(const wxString& name, wxString& errors) {
	wxString cmd = wxT("symfony generate:project ") + Sanitize(name);
	wxSetWorkingDirectory(Options.RootPath);
	wxArrayString output, execErrors;
	int returnCode = wxExecute(cmd, output, execErrors);
	for (size_t i = 0; i < execErrors.Count(); ++i) {
		errors += wxT("\n") + execErrors[i]; 
	}
	return 0 == returnCode;
}
