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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <features/TotalSearchFeatureClass.h>
#include <search/FindInFilesClass.h>
#include <globals/Assets.h>
#include <globals/Number.h>
#include <Triumph.h>

t4p::TotalSearchFeatureClass::TotalSearchFeatureClass(t4p::AppClass& app)
	: FeatureClass(app) {
}


void t4p::TotalSearchFeatureClass::OpenFileTag(const t4p::FileTagClass& fileTag, int lineNumber) {
	t4p::OpenFileCommandEventClass cmd(fileTag.FullPath, -1, -1, lineNumber);
	App.EventSink.Post(cmd);
}

void t4p::TotalSearchFeatureClass::OpenPhpTag(const t4p::PhpTagClass& tag) {
	UnicodeString content;
	bool hasSignature = false;
	wxString charset;
	t4p::FindInFilesClass::FileContents(tag.FullPath, content, charset, hasSignature);
	int32_t position,
			length;
	bool found = t4p::ParsedTagFinderClass::GetResourceMatchPosition(tag, content, position, length);

	if (found) {
		t4p::OpenFileCommandEventClass cmd(tag.FullPath, position, length);
		App.EventSink.Publish(cmd);
	} else {
		t4p::OpenFileCommandEventClass cmd(tag.FullPath);
		App.EventSink.Publish(cmd);
	}
}

void t4p::TotalSearchFeatureClass::OpenDbTable(const t4p::DatabaseTableTagClass& tableTag) {
	t4p::OpenDbTableCommandEventClass cmd(
		t4p::EVENT_CMD_DB_TABLE_DEFINITION_OPEN, tableTag.TableName, tableTag.ConnectionHash
	);
	App.EventSink.Publish(cmd);
}

void t4p::TotalSearchFeatureClass::OpenDbData(const t4p::DatabaseTableTagClass& tableTag) {
	t4p::OpenDbTableCommandEventClass cmd(
		t4p::EVENT_CMD_DB_TABLE_DATA_OPEN, tableTag.TableName, tableTag.ConnectionHash
	);
	App.EventSink.Publish(cmd);
}

