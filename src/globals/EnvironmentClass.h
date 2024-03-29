/**
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#ifndef SRC_GLOBALS_ENVIRONMENTCLASS_H_
#define SRC_GLOBALS_ENVIRONMENTCLASS_H_

#include <pelet/TokenClass.h>
#include <wx/config.h>
#include <vector>
#include "globals/ApacheClass.h"

namespace t4p {
/**
 * Holds the PHP binary location
 */
class PhpEnvironmentClass {
 public:
    /**
     * Full path to the PHP executable
     */
    wxString PhpExecutablePath;

    /**
     * The version to use in Source code parsing.
     */
    pelet::Versions Version;

    /**
     * If TRUE, we will get the version of use from the PHP executable
     * itself by reading the output of "php -v"
     */
    bool IsAuto;

    /**
     * if TRUE, PHP is installed in the system. If false, Triumph will not
     * attempt to run any scripts and should not show the user any "PHP not found"
     * errors.
     */
    bool Installed;

    PhpEnvironmentClass();

    /**
     * completely copy an instance
     * @param src item to copy from
     */
    PhpEnvironmentClass(const t4p::PhpEnvironmentClass& src);

    /**
     * completely copy an instance
     * @param src item to copy from
     */
    t4p::PhpEnvironmentClass& operator=(const t4p::PhpEnvironmentClass& src);

    /**
     * completely copy an instance
     * @param src item to copy from
     */
    void Copy(const t4p::PhpEnvironmentClass& src);

    /**
     * assign defaults based on the current operating system
     */
    void Init();

    /**
     * Use the PHP executable to determine the PHP verion and
     * then fills in the  Version property
     */
    void AutoDetermine();

    /**
     * @return bool TRUE is php is not installed or binary location
     *         is invalid
     */
    bool NotInstalled() const;
};

/**
 * Holds the full path to the web browser binary (google chrome, IE, firefox,...)
 * and a 'friendly' name that the user can change.
 */
class WebBrowserClass {
 public:
    /**
     * should be unique, but that is not enforced here
     */
    wxString Name;

    wxFileName FullPath;

    WebBrowserClass();

    /**
     * completely copy an instance
     * @param src item to copy from
     */
    WebBrowserClass(const t4p::WebBrowserClass& src);

    WebBrowserClass(wxString name, wxFileName fullPath);

    /**
     * completely copy an instance
     * @param src item to copy from
     */
    t4p::WebBrowserClass& operator=(const t4p::WebBrowserClass& src);

    /**
     * completely copy an instance
     * @param src item to copy from
     */
    void Copy(const t4p::WebBrowserClass& src);
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

    /**
     * completely copy an instance
     * @param src item to copy from
     */
    EnvironmentClass(const t4p::EnvironmentClass& src);

    /**
     * completely copy an instance
     * @param src item to copy from
     */
    t4p::EnvironmentClass& operator=(const t4p::EnvironmentClass& src);

    /**
     *  assigns default values based on the current operating system
     */
    void Init();

    /**
     * completely copy an instance
     * @param src item to copy from
     */
    void Copy(const t4p::EnvironmentClass& src);

    ApacheClass Apache;

    PhpEnvironmentClass Php;

    /**
     * The list of web browsers used to launch when the user click Run On Web Browser
     */
    std::vector<WebBrowserClass> WebBrowsers;

    /**
     * Save the environment settings to the given config
     */
    void SaveToConfig(wxConfigBase* config) const;

    /**
     * Get the environment settings from the given config
     */
    void LoadFromConfig(wxConfigBase* config);

    /**
     * @return vector of all WebBrowsers' names
     */
    std::vector<wxString> BrowserNames() const;

    /**
     * @param name the browser name to look up. lookup is case sensitive.
     * @param fullPath will get filled with the full path of the corresponding browser
     * @return TRUE if name was found.
     */
    bool FindBrowserByName(const wxString& name, wxFileName& fileName) const;

 private:
    void AddDefaults();
};
}  // namespace t4p

#endif  // SRC_GLOBALS_ENVIRONMENTCLASS_H_
