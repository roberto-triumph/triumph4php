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
#include <MvcEditorString.h>
#include <unicode/ustring.h>
#include <unicode/ucnv.h>

bool mvceditor::UnicodeStringComparatorClass::operator()(const UnicodeString& str1, const UnicodeString& str2) const {
	return (str1.compare(str2) < (int8_t)0) ? true : false;
}

bool mvceditor::CaseStartsWith(UnicodeString haystack, UnicodeString needle) {
	haystack.toLower();
	needle.toLower();
	return haystack.startsWith(needle) ? true : false;
}

wxString mvceditor::IcuToWx(const UnicodeString& icu) {
	wxString wx;
	UErrorCode status = U_ZERO_ERROR;
	int32_t rawLength;
	int32_t length = icu.length();
	const UChar* src = icu.getBuffer();
	u_strToUTF8(NULL, 0, &rawLength, src, length, &status);
	status = U_ZERO_ERROR;
	char* dest = new char[rawLength + 1];
	int32_t written;
	u_strToUTF8(dest, rawLength + 1, &written, src, length, &status);
	if(U_SUCCESS(status)) {
		wx = wxString(dest, wxConvUTF8);
	}
	delete[] dest;
	return wx;
}

UnicodeString mvceditor::WxToIcu(wxString wx) {
	int charCount = wx.length();
	UErrorCode status = U_ZERO_ERROR;
	UnicodeString uni;
	int actualCount = 0;

	size_t rawLength;
	wxCharBuffer buf = wxConvUTF8.cWC2MB(wx.c_str(), wx.length() + 1, &rawLength);

	// 5th param is meant to be in bytes not chars
	// +1 = make room for the NULL terminator
	u_strFromUTF8(uni.getBuffer(charCount + 1), charCount + 1, &actualCount, buf.data(), rawLength, &status);
	if (U_SUCCESS(status)) {
		uni.releaseBuffer(actualCount);	
	}
	else {
		uni.releaseBuffer(0);
	}
	return uni;
}

UnicodeString mvceditor::CharToIcu(const char* source) {
	int charCount = strlen(source);
	UErrorCode status = U_ZERO_ERROR;
	UnicodeString uni;
	int actualCount = 0;
	
	// 5th param is meant to be in bytes
	// need to account for the null character, hence the +1
	u_strFromUTF8(uni.getBuffer(charCount + 1), charCount + 1, &actualCount, source, charCount, &status);
	if (U_SUCCESS(status)) {
		uni.releaseBuffer(actualCount);
	}
	else {
		uni.releaseBuffer(0);
	}
	return uni;
}

std::string mvceditor::IcuToChar(const UnicodeString& source) {
	std::string ret;
	UErrorCode status = U_ZERO_ERROR;
	int32_t rawLength;
	int32_t length = source.length();
	const UChar* src = source.getBuffer();
	u_strToUTF8(NULL, 0, &rawLength, src, length, &status);
	status = U_ZERO_ERROR;
	char* dest = new char[rawLength + 1];
	int32_t written;
	u_strToUTF8(dest, rawLength + 1, &written, src, length, &status);
	if(U_SUCCESS(status)) {
		ret = std::string(dest, written);
	}
	delete[] dest;
	return ret;
}

int mvceditor::Utf8PosToChar(const char* bytes, int bytesLength, int bytePos) {
	int charCount = 0;
	if (bytePos < bytesLength) {
		UChar c = 0;
		int i = 0;
		while(i < bytePos) {
			U8_NEXT(bytes, i, bytesLength, c);
			charCount++;
			if (c < 0) {
				charCount = -1;
				break;
			}
		}
	}
	else {
		charCount = -1;
	}
	return charCount;
}

int mvceditor::CharToUtf8Pos(const char* bytes, int bytesLength, int charPos) {
	int bytePos = 0;
	U8_FWD_N(bytes, bytePos, bytesLength, charPos);
	return bytePos;
}

int32_t mvceditor::FindPrevious(const UnicodeString& text, const UnicodeString& expression, int start) {
	int32_t textIndex = start < 0 ? text.length() : start;
	int32_t expressionIndex;
	int32_t expressionLength = expression.length();
	while (textIndex >= 0) {
		int32_t  j = 0;
		for (expressionIndex = expressionLength - 1; expressionIndex >= 0 && (textIndex - j) >= 0; --expressionIndex, ++j) {
			if  (expression[expressionIndex] != text[textIndex - j]) {
				break;
			}
		}
		if (expressionIndex == -1) {
			textIndex = textIndex - j + 1; // +1 because j is 1 past the beginning of the match(for loop)
			break;
		}
		--textIndex;
	}
	return textIndex;
}

/**
 * conversion from wxString to C++ string. This should only be used
 * when source is known to be an ascii string.
 */
std::string mvceditor::WxToChar(const wxString& source) {
	std::string s = std::string(source.ToUTF8());
	return s;
}

/**
 * conversion from wxString to C++ string
 */
wxString mvceditor::CharToWx(const char* source) {
	wxString wx(source, wxConvUTF8);
	return wx;
}