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
#ifndef __T4P_CONFIGTAGCLASS_H__
#define __T4P_CONFIGTAGCLASS_H__

#include <wx/string.h>
#include <wx/filename.h>
#include <globals/Sqlite.h>

namespace t4p {

/**
 * A config tag is a recognized config file for a php framework.
 * All config tags will be placed as menu items that are
 * easily accessible by the user. Config detector
 * scripts will populate this table based on a framework's config
 * files.
 */
class ConfigTagClass {

public:

	/**
	 * The label that will be showns as the menu item.
	 * Note that this should not be used to populate the label as
	 * it may contain ampersands. Use the MenuLabel method instead
	 */
	wxString Label;
	
	/**
	 * The file that will be opened when the menu item is activated
	 */
	wxFileName ConfigFileName;

	ConfigTagClass();

	ConfigTagClass(const t4p::ConfigTagClass& src);

	void Copy(const t4p::ConfigTagClass& src);

	t4p::ConfigTagClass& operator=(const t4p::ConfigTagClass& src);

	/**
	 * @return wxString menu item label with any ampersands already escaped.
	 */
	wxString MenuLabel() const;
};

/**
 * This class can be used to load all config tags from the
 * detectors sqlite file.  config detector PHP scripts will INSERT
 * into the db and this class will SELECT from the db.
 */
class ConfigTagFinderClass : public t4p::SqliteFinderClass {

public:

	ConfigTagFinderClass();

	/**
	 * @return all of the configs tags in all attached sqlite dbs
	 */
	std::vector<t4p::ConfigTagClass> All(const std::vector<wxFileName>& sourceDirectories);

};

}

#endif