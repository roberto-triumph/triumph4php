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
#ifndef SRC_GLOBALS_PROJECTCLASS_H_
#define SRC_GLOBALS_PROJECTCLASS_H_

#include <wx/string.h>
#include <vector>
#include "globals/FileTypeClass.h"
#include "search/DirectorySearchClass.h"

namespace t4p {
/**
 * The Project class represents a single project.
 *
 * This project can hold files from multiple, separate directories.
 */
class ProjectClass {
 public:
    /**
     * A friendly label for this project. This is usually set by a user.
     */
    wxString Label;

    /**
     * The directories where source files are located in.
     */
    std::vector<t4p::SourceClass> Sources;

    /**
     * If TRUE, this project is enabled and is used by Triumph.
     */
    bool IsEnabled;

    /**
     * Construct a ProjectClass object from the given options
     *
     * @param ProjectOptionsClass options the new project's options
     */
    ProjectClass();

    ProjectClass(const t4p::ProjectClass& src);

    void operator=(const t4p::ProjectClass& src);

    /**
     * Add a source directory to this project.
     */
    void AddSource(const t4p::SourceClass& src);

    /**
     * Removes all of the sources from this project.
     */
    void ClearSources();

    /**
     * This method will return a list of sources suitable
     * for recursing into this project to find all of the PHP Source
     * code files in this project.
     *
     * @param fileTypes the configured file type extensions
     * @return all of this project's source directories
     * but with the PHP extensions added to each source.
     */
    std::vector<t4p::SourceClass> AllPhpSources(const t4p::FileTypeClass& fileType) const;

    /**
     * This method will return a list of sources suitable for recursing into
     * this project to find all relevant files; ie PHP, CSS, SQL,
     * and all misc file extensions.
     *
     * @param fileTypes the configured file type extensions
     * @return all of this project's sources with all of the wildcards (PHP, CSS, SQL, JS,
     * and misc file extensions) added to each sources.
     */
    std::vector<t4p::SourceClass> AllSources(const t4p::FileTypeClass& fileType) const;

    /**
     *
     * @params fullPath the full path to check
     * @param fileTypes the configured file type extensions
     * @return TRUE if given full path is a PHP file, as determined by
     * the sources directories and the file type php extensions.
     */
    bool IsAPhpSourceFile(const wxString& fullPath, const t4p::FileTypeClass& fileType) const;


    /**
     * @return TRUE if given full path is a "source" file; TRUE if
     *         the given file's extension is any of the fileType's
     *         extensions
     */
    bool IsASourceFile(const wxString& fullPath, const t4p::FileTypeClass& fileType) const;

    /**
     * @return bool TRUE if this project has AT LEAST 1 source
     */
    bool HasSources() const;

    /**
     * removes the source directory from the given full path.
     * Examples
     * source directory = /home/roberto/
     * fullPath = /home/roberto/workspace/now.php
     * Then this method returns "workspace/now.php"
     *
     * @param full path to a file
     * @return the part of the file without the source prefix
     * In the case that fullPath is not contained in any of this
     * project's sources, then this method returns nothing.
     */
    wxString RelativeFileName(const wxString& fullPath) const;

    /**
     *
     * return the CssFileExtensions + SqlFileExtensions + MiscFileExtensions for this project.
     * @return all extension wilcards except for PHP wildcards
     */
    std::vector<wxString> GetNonPhpExtensions(const t4p::FileTypeClass& fileType) const;

 private:
};
}  // namespace t4p

#endif  // SRC_GLOBALS_PROJECTCLASS_H_
