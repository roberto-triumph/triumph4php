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
#include <environment/EnvironmentClass.h>

#include <wx/confbase.h>

mvceditor::EnvironmentClass::EnvironmentClass()
		: Apache()
		, Php() {
}

mvceditor::EnvironmentClass::~EnvironmentClass() {
}

mvceditor::PhpEnvironmentClass::PhpEnvironmentClass() 
	: PhpExecutablePath(wxT("php")) {
}

void mvceditor::EnvironmentClass::LoadFromConfig() {
	wxConfigBase* config = wxConfigBase::Get();
	wxString httpdPath;
	config->Read(wxT("Environment/PhpExecutablePath"), &Php.PhpExecutablePath);
	config->Read(wxT("Environment/ApacheHttpdPath"), &httpdPath);
	if(!httpdPath.IsEmpty()) {
		Apache.SetHttpdPath(httpdPath);
	}
}

void mvceditor::EnvironmentClass::SaveToConfig() {
	wxConfigBase* config = wxConfigBase::Get();
	config->Write(wxT("Environment/PhpExecutablePath"), Php.PhpExecutablePath);
	config->Write(wxT("Environment/ApacheHttpdPath"), Apache.GetHttpdPath());
	config->Flush();
}
