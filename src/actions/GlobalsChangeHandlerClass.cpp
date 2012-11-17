/**
 * The MIT License
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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <actions/GlobalsChangeHandlerClass.h>
#include <actions/ActionClass.h>
#include <globals/Errors.h>

mvceditor::GlobalsChangeHandlerClass::GlobalsChangeHandlerClass(mvceditor::GlobalsClass& globals) 
	: wxEvtHandler()
	, Globals(globals) {
}

void mvceditor::GlobalsChangeHandlerClass::OnFrameworkFound(mvceditor::FrameworkFoundEventClass& event) {
	Globals.Frameworks.push_back(event.GetFramework());
}

void mvceditor::GlobalsChangeHandlerClass::OnSqlMetaDataComplete(mvceditor::SqlMetaDataEventClass& event) {
	Globals.SqlResourceFinder.Copy(event.NewResources);
	std::vector<UnicodeString> errors = event.Errors;
	for (size_t i = 0; i < errors.size(); ++i) {
		wxString wxError = mvceditor::IcuToWx(errors[i]);
		mvceditor::EditorLogError(mvceditor::BAD_SQL, wxError);
	}
}

void mvceditor::GlobalsChangeHandlerClass::OnGlobalCacheComplete(mvceditor::GlobalCacheCompleteEventClass& event) {
	mvceditor::GlobalCacheClass* globalCache = event.GlobalCache;
	if (Globals.ResourceCache.IsInitGlobal(globalCache->ResourceDbFileName)) {
		Globals.ResourceCache.RemoveGlobal(globalCache->ResourceDbFileName);
	}
	Globals.ResourceCache.RegisterGlobal(globalCache);
}

BEGIN_EVENT_TABLE(mvceditor::GlobalsChangeHandlerClass, wxEvtHandler)
	EVT_FRAMEWORK_FOUND(mvceditor::GlobalsChangeHandlerClass::OnFrameworkFound)
	EVT_SQL_META_DATA_COMPLETE(mvceditor::ID_EVENT_ACTION_SQL_METADATA, mvceditor::GlobalsChangeHandlerClass::OnSqlMetaDataComplete)
	EVT_GLOBAL_CACHE_COMPLETE(mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE, mvceditor::GlobalsChangeHandlerClass::OnGlobalCacheComplete)
END_EVENT_TABLE()


