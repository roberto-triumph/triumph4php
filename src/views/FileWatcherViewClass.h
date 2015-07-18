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
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef T4P_FILEWATCHERVIEWCLASS_H
#define T4P_FILEWATCHERVIEWCLASS_H

#include <views/FeatureViewClass.h>
#include <views/wxformbuilder/FileWatcherFeatureForms.h>
#include <features/FileWatcherFeatureClass.h>
#include <globals/Events.h>

namespace t4p {

class FileWatcherViewClass : public t4p::FeatureViewClass {

public:

	FileWatcherViewClass(t4p::FileWatcherFeatureClass& feature);

	void AddPreferenceWindow(wxBookCtrlBase* parent);

private:

	t4p::FileWatcherFeatureClass& Feature;

	// to track opened files, since the watcher needs to
	// see which files are currently opened
	void OnFileClosed(t4p::CodeControlEventClass& event);
	void OnFileOpened(t4p::CodeControlEventClass& event);

	DECLARE_EVENT_TABLE()
};

class FileWatcherPreferencesPanelClass : public FileWatcherPreferencesPanelGeneratedClass {

public:

	FileWatcherPreferencesPanelClass(wxWindow* parent, t4p::FileWatcherFeatureClass& feature);

private:

	t4p::FileWatcherFeatureClass& Feature;
};

}

#endif

