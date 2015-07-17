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
#ifndef T4P_FINDERFEATURECLASS_H_
#define T4P_FINDERFEATURECLASS_H_

#include <search/FinderClass.h>
#include <features/FeatureClass.h>
#include <actions/ActionClass.h>

namespace t4p {

/**
 * An action that will perform a find on an entire set of text
 * and POST FinderHitEventClas for each found hit. 
 */
class FinderActionClass : public t4p::ActionClass {
	
public:

	/**
	 * @param utf8buf this class will take ownership of this pointer
	 */
	FinderActionClass(t4p::RunningThreadsClass& runningThreads, int eventId,
		const UnicodeString& search, char* utf8Buf, int bufLength);
	
protected:
	
	void BackgroundWork();
	
	wxString GetLabel() const;
	
	private:
	
	/**
	 * to perform the search
	 */
	t4p::FinderClass Finder;

	/**
	 *  unicode representation of ut8buf
	 */
	UnicodeString Code;
	
	/**
	 * this class will own the pointer
	 */
	char* Utf8Buf;
	
	/**
	 * the number of characters in Utf8Buf
	 */
	int BufferLength;
};

/**
 * the event that is generated when a FinderActionClass finds an 
 * instance of the search text in the text being searched.
 * Note that positions are given as byte offsets and not character
 * counts so that the character-to-byte offsets are calculated in the
 * background thread.
 */
class FinderHitEventClass : public wxEvent {
	
public:

	/**
	 * this is the index into the utf8 buffer, not character pos
	 */
	int Start;
	
	/**
	 * this is the number of utf8 bytes, not number of characters
	 */
	int Length;
	
	FinderHitEventClass(int id, int start, int length);
	
	wxEvent* Clone() const;
	
};

extern const wxEventType EVENT_FINDER_ACTION;
extern const int ID_FINDER_ACTION;

typedef void (wxEvtHandler::*FinderHitEventClassFunction)(t4p::FinderHitEventClass&);

#define EVT_FINDER(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_FINDER_ACTION, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( FinderHitEventClassFunction, & fn ), (wxObject *) NULL ),


class FinderFeatureClass : public FeatureClass {

public:

	/**
	 * will perform the searching
	 */
	t4p::FinderClass Finder;
	t4p::FinderClass FinderReplace;

	/**
	 * Constructor
 	 */
	FinderFeatureClass(t4p::AppClass& app);
	
	void LoadPreferences(wxConfigBase* config);
	
	/**
	 * start a search in the background thread to look for word
	 * inside buf
	 * 
	 * @param word
	 * @param buf not owned by this class
	 * @param documentLength length of buf
	 */
	void StartFinderAction(UnicodeString word, char* buf, int documentLength);

};

}
#endif /*T4P_FINDERFEATURECLASS_H_*/
