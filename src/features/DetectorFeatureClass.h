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
#ifndef SRC_FEATURES_DETECTORFEATURECLASS_H_
#define SRC_FEATURES_DETECTORFEATURECLASS_H_

#include <features/FeatureClass.h>

namespace t4p {
class DetectorClass {
	public:
	DetectorClass();

	virtual ~DetectorClass();

	virtual bool CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project) = 0;

	virtual wxString TestCommandLine(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project,
		const wxString& detectorScriptFullPath) = 0;

	virtual wxFileName LocalRootDir()  = 0;

	virtual wxFileName GlobalRootDir()  = 0;

	virtual wxFileName SkeletonFile() = 0;

	virtual wxString Label() = 0;
};

class UrlTagDetectorClass : public t4p::DetectorClass {
	public:
	UrlTagDetectorClass();

	bool CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project);

	wxString TestCommandLine(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project,
		const wxString& detectorScriptFullPath);

	wxFileName LocalRootDir();

	wxFileName GlobalRootDir();

	wxFileName SkeletonFile();

	wxString Label();
};

class TemplateFileTagsDetectorClass : public t4p::DetectorClass {
	public:
	TemplateFileTagsDetectorClass();

	bool CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project);

	wxString TestCommandLine(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project,
		const wxString& detectorScriptFullPath);

	wxFileName LocalRootDir();

	wxFileName GlobalRootDir();

	wxFileName SkeletonFile();

	wxString Label();
};

class TagDetectorClass : public t4p::DetectorClass {
	public:
	TagDetectorClass();

	bool CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project);

	wxString TestCommandLine(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project,
		const wxString& detectorScriptFullPath);

	wxFileName LocalRootDir();

	wxFileName GlobalRootDir();

	wxFileName SkeletonFile();

	wxString Label();
};

class DatabaseTagDetectorClass : public t4p::DetectorClass {
	public:
	DatabaseTagDetectorClass();

	bool CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project);

	wxString TestCommandLine(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project,
		const wxString& detectorScriptFullPath);

	wxFileName LocalRootDir();

	wxFileName GlobalRootDir();

	wxFileName SkeletonFile();

	wxString Label();
};

class ConfigTagDetectorClass : public t4p::DetectorClass {
	public:
	ConfigTagDetectorClass();

	bool CanTest(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project);

	wxString TestCommandLine(const t4p::GlobalsClass& globals, const t4p::ProjectClass& project,
		const wxString& detectorScriptFullPath);

	wxFileName LocalRootDir();

	wxFileName GlobalRootDir();

	wxFileName SkeletonFile();

	wxString Label();
};

class DetectorFeatureClass : public t4p::FeatureClass {
	public:
	DetectorFeatureClass(t4p::AppClass& app);

	void RunUrlDetectors();
	void RunTemplateFileDetectors();
	void RunTagDetectors();
	void RunDatabaseDetectors();
	void RunConfigDetectors();
};
}  // namespace t4p

#endif  // SRC_FEATURES_DETECTORFEATURECLASS_H_
