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
#ifndef SRC_FEATURES_EDITORBEHAVIORFEATURECLASS_H_
#define SRC_FEATURES_EDITORBEHAVIORFEATURECLASS_H_

#include <vector>
#include "features/FeatureClass.h"
#include "globals/CodeControlOptionsClass.h"

namespace t4p {
/**
 * An EditorKeyboardCommandClass is a key binding that we can tell
 * scintilla to listen for.  Note that the Cmd, KeyCode and Modifiers
 * are SCINTILLA numbers and NOT wx Numbers; ie. wxSTC_SCMOD_* instead
 * of wxACCEL_* and so on.
 *
 * See http://www.scintilla.org/ScintillaDoc.html
 * under "Key bindings"
 */
class EditorKeyboardCommandClass {
 public:
    /**
     * human-friendly name of the command
     */
    wxString Name;

    /**
     * the scintilla command number. one of wxSTC_CMD_*
     */
    int CmdId;

    /**
     * bitwise mask of ALT, CTRL, SHIFT that will trigger
     * this command.
     * these are scintilla modifiers, not WX modifiers. ie.
     * wxSTC_SCMOD_SHIFT, wxSTC_SCMOD_CTRL, wxSTC_SCMOD_ALT
     * This can be zero; if zero it means that we don't have
     * a shortcut for this command.
     */
    int Modifiers;

    /**
     * key code that will trigger this command.
     * this is a scintilla key code, not a WX key code
     * key codes > 300 have different values in wxWidgets and
     * scintilla. must map them correctly.
     * ie. wxSTC_KEY_*
     * This can be zero; if zero it means that we don't have
     * a shortcut for this command.
     */
    int KeyCode;

    EditorKeyboardCommandClass();

    EditorKeyboardCommandClass(const wxString& name, int cmdId, int modifiers, int keyCode);

    EditorKeyboardCommandClass(const t4p::EditorKeyboardCommandClass& src);

    t4p::EditorKeyboardCommandClass& operator=(const t4p::EditorKeyboardCommandClass& src);

    void Copy(const t4p::EditorKeyboardCommandClass& src);

    bool IsOk() const;

    void InitFromString(const wxString& str);

    wxString ToString() const;
};

/**
 * The EditorFeatureClass exposes scintilla features like
 * virtual space, multiple selection, and zoom
 */
class EditorBehaviorFeatureClass : public t4p::FeatureClass {
 public:
    /**
     * The keyboard commands (shortcuts) assigned to the Scintilla
     * control.
     */
    std::vector<t4p::EditorKeyboardCommandClass> KeyboardCommands;

    EditorBehaviorFeatureClass(t4p::AppClass& app);

    void LoadPreferences(wxConfigBase* config);

    void ToggleWordWrap();
    void ToggleIndentationGuides();
    void ToggleWhitespace();
    void ZoomIn();
    void ZoomOut();
    void ZoomReset();
    void EditConvertEols();

    // translations of scintilla commands to menu IDs
    // and vice versa
    int SciCommandToMenuId(int cmdId);
    int MenuIdToSciCommand(int menuId);

 private:
    /**
     * Handler to save the editor feature preferences.
     */
    void OnPreferencesSaved(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};
}  // namespace t4p

#endif  // SRC_FEATURES_EDITORBEHAVIORFEATURECLASS_H_
