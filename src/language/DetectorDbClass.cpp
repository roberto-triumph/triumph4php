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
#include <language/DetectorDbClass.h>
#include <globals/Errors.h>
#include <globals/String.h>

t4p::DetectorDbClass::DetectorDbClass() 
: Session(NULL) {
	
}

void t4p::DetectorDbClass::Init(soci::session* session) {
	Session = session;
}

void t4p::DetectorDbClass::DeleteSource(const wxFileName& sourceDir) {
	wxASSERT_MSG(Session, wxT("session must be initialized"));
	if (!Session) {
		return;
	}
	try {
		int sourceId = 0;
		std::string stdSourceDir = t4p::WxToChar(sourceDir.GetPathWithSep());
		soci::statement stmt = (Session->prepare << "SELECT source_id FROM sources WHERE directory = ?",
			soci::into(sourceId), soci::use(stdSourceDir));
		stmt.execute(true);
		
		// delete from all tables
		Session->once << "DELETE FROM call_stacks WHERE source_id = ? ", soci::use(sourceId);
		Session->once << "DELETE FROM database_tags WHERE source_id = ? ", soci::use(sourceId);
		Session->once << "DELETE FROM template_file_tags WHERE source_id = ? ", soci::use(sourceId);
		Session->once << "DELETE FROM config_tags WHERE source_id = ? ", soci::use(sourceId);
		Session->once << "DELETE FROM detected_tags WHERE source_id = ? ", soci::use(sourceId);
		Session->once << "DELETE FROM url_tags WHERE source_id = ? ", soci::use(sourceId);
		Session->once << "DELETE FROM sources WHERE source_id = ? ", soci::use(sourceId);
		
	} catch (std::exception& e) {
		t4p::EditorLogWarning(t4p::WARNING_OTHER, wxString::FromAscii(e.what()));
	}
}

void t4p::DetectorDbClass::Wipe() {
	wxASSERT_MSG(Session, wxT("session must be initialized"));
	if (!Session) {
		return;
	}
	try {
		Session->once << "DELETE FROM call_stacks";
		Session->once << "DELETE FROM database_tags";
		Session->once << "DELETE FROM template_file_tags";
		Session->once << "DELETE FROM config_tags";
		Session->once << "DELETE FROM detected_tags";
		Session->once << "DELETE FROM url_tags";
		Session->once << "DELETE FROM sources";
	} catch (std::exception& e) {
		t4p::EditorLogWarning(t4p::WARNING_OTHER, wxString::FromAscii(e.what()));
	}
}

