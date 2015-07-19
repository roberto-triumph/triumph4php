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
#ifndef __RUNCONSOLEFEATURECLASS_H
#define __RUNCONSOLEFEATURECLASS_H

#include <features/FeatureClass.h>
#include <widgets/ProcessWithHeartbeatClass.h>
#include <vector>


namespace t4p {
// defined below
class RunConsoleFeatureClass;

/**
 * Class that represents a command to be run in the
 * 'console' (CLI).
 */
class CliCommandClass {
	public:
	/**
	 * Binary to be run. This may be a full path, or just the
	 * binary name if the binary is already in the PATH.
	 */
	wxString Executable;

	/**
	 * The full path to the directory where the command will
	 * run from.
	 */
	wxFileName WorkingDirectory;

	/**
	 * all of the arguments to give to the executable
	 */
	wxString Arguments;

	/**
	 * A short human-friendly description of the command. This is
	 * given by the user; it is not guaranteed to be unique.
	 */
	wxString Description;

	/**
	 * if TRUE, then when the console window is created for this command
	 * the command will NOT be run. This gives the user a chance to
	 * add runtime arguments.
	 */
	bool WaitForArguments;

	/**
	 * if TRUE, this item will show up in the toolbar. Note that the order
	 * is determined by the position in the list of commands.
	 */
	bool ShowInToolbar;

	CliCommandClass();

	void Copy(const CliCommandClass& src);

	/**
	 * @return the command to be executed
	 */
	wxString CmdLine() const;
};

/**
 * Keeps track of the position of a file name in the output.
 */
class FileNameHitClass {
	public:
	/**
	 * The 0-based index into output string. This is the start of the file name
	 */
	int32_t StartIndex;

	/**
	 * The number of characters in the file name
	 */
	int32_t Length;

	FileNameHitClass();
};

class RunConsoleFeatureClass : public FeatureClass {
	public:
	/**
	 * The list of commands to be persisted.
	 */
	std::vector<t4p::CliCommandClass> CliCommands;


	/**
	 * Constructor
 	 */
	RunConsoleFeatureClass(t4p::AppClass& app);

	void LoadPreferences(wxConfigBase* config);

	/**
	 * Add a new command to the list (but does not persist
	 * the list to disk)
	 */
	void AddCommand(const t4p::CliCommandClass& command);

	/**
	 * Saves the CLI commands to the [global] config
	 */
	void PersistCommands();

	/**
	 * Open a file.
	 */
	void LoadPage(const wxString& fileName);

	private:
	/**
	 * Handles the click of the saved command buttons
	 */
	void OnCommandButtonClick(wxCommandEvent& evt);
};
}
#endif // __RUNCONSOLEFEATURECLASS_H
