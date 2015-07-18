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
#ifndef T4P_GLOBALACTIONCLASS_H
#define T4P_GLOBALACTIONCLASS_H

#include <actions/ActionClass.h>
#include <globals/GlobalsClass.h>

namespace t4p {

/**
 * An action is any short or long-lived process that reads Global data structures
 * and performs an action.
 * An action is an asynchronous operation. Take care when designing the class
 * so that any data structures are copied instead of referenced.
 *
 * Pay special attention to wxString variables, as the default wxString
 * assignment operator and copy constructors are NOT thread-safe (produce
 * shallow copies)
 */
class GlobalActionClass : public t4p::ActionClass {

	public:

	GlobalActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	virtual bool Init(t4p::GlobalsClass& globals) = 0;

	virtual bool DoAsync();
};

/**
 * A specialization of ActionClass; these kinds of actions are
 * small, quick actions that modify GlobalsClass in a specific
 * manner.
 */
class InitializerGlobalActionClass : public t4p::GlobalActionClass {

	public:

	InitializerGlobalActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	bool Init(t4p::GlobalsClass& globals);

	virtual void Work(t4p::GlobalsClass& globals) = 0;

	void BackgroundWork();

	bool DoAsync();

};

}

#endif
