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
#include "features/FileModifiedCheckFeatureClass.h"
#include <vector>
#include "actions/FileModifiedCheckActionClass.h"
#include "Triumph.h"

const int t4p::ID_FILE_MODIFIED_ACTION = wxNewId();

t4p::FileModifiedCheckFeatureClass::FileModifiedCheckFeatureClass(t4p::AppClass& app)
    : FeatureClass(app) {
}


void t4p::FileModifiedCheckFeatureClass::StartFilePoll(std::vector<t4p::FileModifiedTimeClass> filesToPoll) {
    if (!filesToPoll.empty()) {
        t4p::FileModifiedCheckActionClass* action = new t4p::FileModifiedCheckActionClass(App.RunningThreads,
                t4p::ID_FILE_MODIFIED_ACTION);
        action->SetFiles(filesToPoll);
        App.RunningThreads.Queue(action);
    }
}
