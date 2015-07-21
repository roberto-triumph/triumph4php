/**
 * @copyright  2013 Roberto Perpuly
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
#ifndef SRC_FEATURES_CONFIGFILESFEATURECLASS_H_
#define SRC_FEATURES_CONFIGFILESFEATURECLASS_H_

#include <vector>
#include "actions/ActionClass.h"
#include "features/FeatureClass.h"
#include "globals/Events.h"
#include "language_php/ConfigTagClass.h"

namespace t4p {
/**
* pair together a project name and the config files for that
* project, that way we can build the submenus.
*/
class ConfigFilesFeaturePairClass {
 public:
    /**
     * the label of the project that the config files were
     * found in
     */
    wxString ProjectLabel;

    /**
     * the config files that were found in this project.
     */
    std::vector<t4p::ConfigTagClass> ConfigTags;

    ConfigFilesFeaturePairClass();

    ConfigFilesFeaturePairClass(const t4p::ConfigFilesFeaturePairClass& src);

    ConfigFilesFeaturePairClass& operator=(const t4p::ConfigFilesFeaturePairClass& src);

    void Copy(const t4p::ConfigFilesFeaturePairClass& src);
};

/**
 * This feature will take all of the detected config files and
 * will populate a menu with config files from each project.
 * The config files are detected with the help of ConfigTagDetectorActionClass
 * and are read with the help of ConfigTagFinderClass.
 */
class ConfigFilesFeatureClass : public t4p::FeatureClass {
 public:
    /**
     * max amount of menu items to show
     * the 100 is due to the menu ids allocated to each feature in
     * the FeatureClass MenuIds enum
     */
    static const size_t MAX_CONFIG_MENU_ITEMS = 100;

    ConfigFilesFeatureClass(t4p::AppClass& app);

    /**
     * organizes all of the detected config files by grouping
     * them into separate vectors, one vector per project.
     * This method is required for the menu handlers to work.
     */
    bool BuildConfigPairs(std::vector<t4p::ConfigFilesFeaturePairClass>& pairs);

    /**
    * When a menu item is selected; open the corresponding config
    * file
    *
    * @param index index into ConfigTags
    */
    void OpenConfigItem(size_t index);

 private:
    /**
     * Read all of the detected config tags into memory; that way we can
     * assign them a menu ID (the index into this vector will be used
     * as the menu ID).
     */
    std::vector<t4p::ConfigTagClass> ConfigTags;

    /**
     * when a file has been saved; check to see if it is one of the config
     * files that has been changed.  If, so the trigger database
     * detection
     */
    void OnFileSaved(t4p::CodeControlEventClass& event);

    DECLARE_EVENT_TABLE()
};
}  // namespace t4p

#endif  // SRC_FEATURES_CONFIGFILESFEATURECLASS_H_
