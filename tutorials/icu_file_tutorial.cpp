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
#include <unicode/ucsdet.h>
#include <unicode/unistr.h>
#include <unicode/ustdio.h>
#include <unicode/ustring.h>

/**
 * This tutorial uses ICU to open a file, detect the encoding, grab the contents of the file,
 * and output the file contents to the console.
 * Also, the file contents are UTF-8 encoded then output to the console.
 */

// disable MSW warning
#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

int main(int argc, char **argv) {
    // THIS THE FILE THAT WILL BE OPENED
#ifdef _WIN32
    char* fileName = "C:\\Users\\Roberto\\Documents\\triumph4php\\php_samples\\sample_unicode.php";
#else
    const char* fileName = "/home/roberto/workspace/triumph4php/php_samples/sample_unicode.php";
#endif

    FILE* rawFile = fopen(fileName, "r");
    UnicodeString fileContents;
    if (rawFile) {
        // obtain file size and read the raw data
        fseek(rawFile , 0 , SEEK_END);
        int32_t size = ftell(rawFile);
        rewind(rawFile);
        char* tmp = new char[size];
        fread(tmp, 1, size, rawFile);
        fclose(rawFile);

        // what encoding is it??
        UErrorCode status = U_ZERO_ERROR;
        const char* name = 0;
        UCharsetDetector* csd = ucsdet_open(&status);
        const UCharsetMatch *ucm;
        ucsdet_setText(csd, tmp, size, &status);
        if (U_SUCCESS(status)) {
            ucm = ucsdet_detect(csd, &status);
            if (U_SUCCESS(status) && ucm != NULL) {
                name = ucsdet_getName(ucm, &status);
                if (U_SUCCESS(status)) {
                    int32_t cc = ucsdet_getConfidence(ucm, &status);
                    printf("File encoding=%s confidence=%d\n", name, cc);
                } else {
                    puts("could not get detected charset name\n");
                }
            } else {
                puts("could not detect charset\n");
            }
            ucsdet_close(csd);
        } else {
            puts("could init detector\n");
        }

        delete[] tmp;

        // encode to string
        UFILE* file = u_fopen(fileName, "r", NULL, NULL);
        if (name != 0) {
            int error = u_fsetcodepage(name, file);
            if (0 == error) {
                int32_t read = u_file_read(fileContents.getBuffer(size + 1), size, file);
                fileContents.releaseBuffer(read);
            }
        }
        u_fclose(file);

        UFILE *out = u_finit(stdout, NULL, NULL);
        u_fprintf(out, "***********\nFILE\n***************\n%S\n", fileContents.getTerminatedBuffer());
        u_fclose(out);

        // print UTF-8 encoded String
        int32_t utf8Length = 0;
        status = U_ZERO_ERROR;
        u_strToUTF8(NULL, 0, &utf8Length, fileContents.getTerminatedBuffer(), fileContents.length(), &status);
        printf("error:%s\n", u_errorName(status));
        char* utf8Buf = new char[utf8Length + 1];
        int32_t written = 0;
        status = U_ZERO_ERROR;
        u_strToUTF8(utf8Buf, utf8Length + 1, &written, fileContents.getTerminatedBuffer(), fileContents.length(), &status);
        printf("error:%s\n", u_errorName(status));
        printf("***********\nUTF-8 FILE\n***************\n%s\n", utf8Buf);
        delete[] utf8Buf;
    } else {
        printf("Could not open file %s\n", fileName);
    }
    return 0;
}
