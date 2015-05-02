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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __T4P_JAVASCRIPTVIEWCLASS__
#define __T4P_JAVASCRIPTVIEWCLASS__

#include <features/views/FeatureViewClass.h>
#include <code_control/CodeControlClass.h>
#include <globals/events.h>

namespace t4p {

/**
 * The Javascript code compeltion class implements the code that 
 * code completes HTML keywords
 */
class JavascriptCodeCompletionProviderClass : public t4p::CodeCompletionProviderClass {
	
public:

	JavascriptCodeCompletionProviderClass();
	
	bool DoesSupport(t4p::FileType type);
	
	void Provide(t4p::CodeControlClass* ctrl, std::vector<t4p::CodeCompletionItemClass>& suggestions, wxString& completeStatus);
	
};

/**
 * this class highlights matching braces {}, [], and ()
 */
class JavascriptBraceMatchStylerClass : public t4p::BraceMatchStylerClass {
	
public:

	JavascriptBraceMatchStylerClass();
	
	bool DoesSupport(t4p::FileType type);
	
	void Style(t4p::CodeControlClass* ctrl, int postToCheck);
};


/**
 * The Javascript feature implements features that are useful for
 * coding Javascript, code completion, keyword highlighting
 */
class JavascriptViewClass : public t4p::FeatureViewClass {

public:

	JavascriptViewClass();
	
private:

	void OnAppFileOpened(t4p::CodeControlEventClass& event);
	
	/**
	 * to implement code completion of html tags and attributes
	 */
	t4p::JavascriptCodeCompletionProviderClass JavascriptCompletionProvider;
	
	/**
	 * to implement brace matching
	 */
	t4p::JavascriptBraceMatchStylerClass BraceStyler;
	
	DECLARE_EVENT_TABLE()
};

}

#endif // __T4P_JAVASCRIPTVIEWCLASS__
