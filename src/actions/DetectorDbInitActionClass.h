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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */

#ifndef __T4P_DETECTORDBINITACTIONCLASS_H__
#define __T4P_DETECTORDBINITACTIONCLASS_H__

#include <actions/GlobalActionClass.h>
 
namespace t4p {


/**
 * This class will prime the detectors sqlite db file.
 * enabled projects. The Config cache will be primed; although
 * it will be primed with the existing cache file which may be
 * stale. 
 */
class DetectorDbInitActionClass : public t4p::InitializerGlobalActionClass {

public:

	DetectorDbInitActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	void Work(t4p::GlobalsClass& globals);

	wxString GetLabel() const;
};

}

#endif