/**
 * @copyright  2014 Roberto Perpuly
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
#include <features/FileCabinetFeatureClass.h>
#include <Triumph.h>
#include <globals/Assets.h>

t4p::FileCabinetFeatureClass::FileCabinetFeatureClass(t4p::AppClass& app)
: FeatureClass(app)
, Store() {
}

t4p::FileCabinetItemClass t4p::FileCabinetFeatureClass::AddDirectoryToCabinet(const wxString& dir) {
	t4p::FileCabinetItemClass fileCabinetItem;
	fileCabinetItem.FileName.AssignDir(dir);
	Store.Store(App.Globals.ResourceCacheSession, fileCabinetItem);
	return fileCabinetItem;
}

t4p::FileCabinetItemClass t4p::FileCabinetFeatureClass::AddFileToCabinet(const wxString& file) {
	t4p::FileCabinetItemClass fileCabinetItem;
	fileCabinetItem.FileName.Assign(file);
	Store.Store(App.Globals.ResourceCacheSession, fileCabinetItem);
	return fileCabinetItem;
}

void t4p::FileCabinetFeatureClass::DeleteCabinetItem(int id) {
	Store.Delete(App.Globals.ResourceCacheSession, id);
}
