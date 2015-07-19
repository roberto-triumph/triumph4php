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
 * @copyright  2009 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __filetestfixtureclass__
#define __filetestfixtureclass__

#include <TriumphChecks.h>
#include <wx/filename.h>
#include <iostream>

/**
 * This test fixture is useful for testing classes that read from files.
 */
class FileTestFixtureClass {
	public:
	/**
	 * Construct a new test fixture that will use subdirectory tempDirectory within the OS temp
	 * @param wxString tempDirectory directory where files will be created.
	 */
	FileTestFixtureClass(const wxString& tempDirectory);

	virtual ~FileTestFixtureClass();

	void RecursiveRmDir(wxString path);

	/**
	 * Creates a file in the temp directory. The file will have the given contents.
	 *
	 * @param wxString fileName the name of the file.  The  name should be a relative path,
	 *			with tempDirectory (as given in the constructor) as root.
	 * @param wxString contents the file contents.
	 */
	void CreateFixtureFile(const wxString& fileName, const wxString& contents);

	/**
	 * "Hides" the given file by executing the appropriate OS command to set the file
	 * to be a hidden file.
	 * @param fileName full path of file to hide. Note: does NOT handle non-ascii file names
	 * @return wxString the name of the hidden file (ie. in case of linux the file is renamed to start
	 *        with a '.' )
	 */
	wxString HideFile(const wxString& fileName);

	/**
	 * Create a sub-directory within tempDirectory
	 *
	 * @param wxString subDirectory valid directory name
	 */
	void CreateSubDirectory(const wxString& subDirectory);

	/**
	 * Reads the file into a string and returns the string.
	 */
	wxString GetFileContents(const wxString& fileName);

	/**
	 * Creates the test dir if it does not already exist.
	 */
	void TouchTestDir();

	/**
	 * Use this to make wxFileNames for subdirectories of
	 * the test dir
	 *
	 * @return a wxFileName with TestProjectDir + dirName
	 */
	wxFileName AbsoluteDir(const wxString& dirName);

	/**
	 *
	 * @var wxString full path to the temp directory for this test
	 */
	wxString TestProjectDir;
};

#endif // __filetestfixtureclass__
