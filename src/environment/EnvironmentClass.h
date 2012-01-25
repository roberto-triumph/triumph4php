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
#ifndef __environmentclass__
#define __environmentclass__

#include <environment/ApacheClass.h>
#include <map>

namespace mvceditor {

/**
 * Holds the PHP binary location
 */
class PhpEnvironmentClass {

public:
	PhpEnvironmentClass();
	
	wxString PhpExecutablePath;
};

/**
 * The Environment class is a simple container to the configured options of the currently running development stack
 * (LAMP / WAMP / MAMP enviroment). Each composite class will perform all of the underlying logic.
 * 
 */
class EnvironmentClass {

public:
	EnvironmentClass();
	~EnvironmentClass();
	
	ApacheClass Apache;
	
	PhpEnvironmentClass Php;
	
	/**
	 * The list of web browsers used to launch when the user click Run On Web
	 * The key is a human-friendly name, the value is the location to the web
	 * browser executable.
	 */
	std::map<wxString, wxFileName> WebBrowsers;
	
	/**
	 * Save the environment settings to the global config (wxConfigBase::Get())
	 */
	void SaveToConfig() const;

	/**
	 * Get the environment settings from the global config (wxConfigBase::Get())
	 */
	void LoadFromConfig();
};

}
#endif // __environmentclass__
