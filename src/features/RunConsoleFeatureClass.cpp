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
#include "features/RunConsoleFeatureClass.h"
#include <wx/filename.h>
#include <wx/sstream.h>
#include "globals/Assets.h"
#include "globals/Errors.h"
#include "globals/Number.h"
#include "globals/String.h"
#include "Triumph.h"

static const int ID_PROCESS = wxNewId();

t4p::CliCommandClass::CliCommandClass()
	: Executable()
	, WorkingDirectory()
	, Arguments()
	, Description()
	, WaitForArguments(false)
	, ShowInToolbar(false) {
}

void t4p::CliCommandClass::Copy(const t4p::CliCommandClass& src) {
	Executable = src.Executable;
	WorkingDirectory = src.WorkingDirectory;
	Arguments = src.Arguments;
	Description = src.Description;
	WaitForArguments = src.WaitForArguments;
	ShowInToolbar = src.ShowInToolbar;
}

wxString t4p::CliCommandClass::CmdLine() const {
	wxString line;
	if (!Executable.IsEmpty()) {
		line += Executable;
	}
	if (!Arguments.IsEmpty()) {
		line += wxT(" ") + Arguments;
	}
	return line;
}


t4p::RunConsoleFeatureClass::RunConsoleFeatureClass(t4p::AppClass& app)
	: FeatureClass(app) {
}


void t4p::RunConsoleFeatureClass::LoadPreferences(wxConfigBase* config) {
	CliCommands.clear();
	long index;
	wxString groupName;
	bool found = config->GetFirstGroup(groupName, index);
	while (found) {
		if (groupName.Find(wxT("CliCommand_")) >= 0) {
			t4p::CliCommandClass newCommand;
			wxString key = groupName + wxT("/Executable");
			newCommand.Executable = config->Read(key);
			key = groupName + wxT("/WorkingDirectory");
			newCommand.WorkingDirectory.AssignDir(config->Read(key));
			key = groupName + wxT("/Arguments");
			newCommand.Arguments = config->Read(key);
			key = groupName + wxT("/Description");
			newCommand.Description = config->Read(key);
			key = groupName + wxT("/ShowInToolbar");
			config->Read(key, &newCommand.ShowInToolbar);
			key = groupName + wxT("/WaitForArguments");
			config->Read(key, &newCommand.WaitForArguments);

			CliCommands.push_back(newCommand);
		}
		found = config->GetNextGroup(groupName, index);
	}
}

void t4p::RunConsoleFeatureClass::PersistCommands() {
	wxConfigBase* config = wxConfig::Get();

	// delete any previous commands that are in the config
	wxString groupName;
	long index = 0;
	if (config->GetFirstGroup(groupName, index)) {
		do {
			if (groupName.Find(wxT("CliCommand_")) == 0) {
				config->DeleteGroup(groupName);
			}
		} while (config->GetNextGroup(groupName, index));
	}
	for (size_t i = 0; i < CliCommands.size(); ++i) {
		wxString key = wxString::Format(wxT("CliCommand_%ld/Executable"), i);
		config->Write(key, CliCommands[i].Executable);
		key = wxString::Format(wxT("CliCommand_%ld/WorkingDirectory"), i);
		config->Write(key, CliCommands[i].WorkingDirectory.GetPath());
		key = wxString::Format(wxT("CliCommand_%ld/Arguments"), i);
		config->Write(key, CliCommands[i].Arguments);
		key = wxString::Format(wxT("CliCommand_%ld/Description"), i);
		config->Write(key, CliCommands[i].Description);
		key = wxString::Format(wxT("CliCommand_%ld/ShowInToolbar"), i);
		config->Write(key, CliCommands[i].ShowInToolbar);
		key = wxString::Format(wxT("CliCommand_%ld/WaitForArguments"), i);
		config->Write(key, CliCommands[i].WaitForArguments);
	}
	config->Flush();

	// signal that this app has modified the config file, that way the external
	// modification check fails and the user will not be prompted to reload the config
	App.UpdateConfigModifiedTime();
}

void t4p::RunConsoleFeatureClass::AddCommand(const t4p::CliCommandClass& command) {
	CliCommands.push_back(command);
}


t4p::FileNameHitClass::FileNameHitClass()
	: StartIndex(0)
	, Length(0) {
}

