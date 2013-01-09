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
#ifndef __MVCEDITOR_ASSETS_H__
#define __MVCEDITOR_ASSETS_H__

#include <wx/filename.h>

namespace mvceditor {

/**
 * @return the file location of the PHP native functions SQLite file. This file contains all of the
 *         classes, methods, functions, that are documented on the php.net site.  The file 
 *         was generated using the script resources/native_resources_convert.php
 */
wxFileName NativeFunctionsAsset();

/**
 * @return the file location of the SQL script to create the resources database.
 */
wxFileName ResourceSqlSchemaAsset();

/**
 * @param the name of an image, minus the .xpm extension
 * @return the file location of the given auto complete image.  The image name must exist; if it
 * does not an assertion is triggered.
 */
wxFileName AutoCompleteImageAsset(wxString imageName);

/**
 * @return the base directory location of the PHP URL scripts.  The editor will use this as the
 * include path when calling all PHP detectors
 */
wxFileName PhpDetectorsBaseAsset();

/**
 * @return the directory location of the PHP URL detector scripts.  The editor will use these scripts
 * to find out framework-specific URL routes. These are the URL detectors that are provided
 * by MVC editor.
 */
wxFileName UrlDetectorsGlobalAsset();

/**
 * @return the directory location of the PHP URL detector scripts.  The editor will use these scripts
 * to find out framework-specific URL routes. These are the URL detectors that the user
 * creates for their own projects.
 */
wxFileName UrlDetectorsLocalAsset();

/**
 * @return the directory location of the PHP template files detector scripts.  The editor will use these scripts
 * to find out framework-specific template files. These are the template files detectors that the user
 * creates for their own projects.
 */
wxFileName TemplateFilesDetectorsLocalAsset();

/**
 * @return the directory location of the PHP template files detector scripts.  The editor will use these scripts
 * to find out framework-specific template files. These are the template files detectors that are provided
 * by MVC editor.
 */
wxFileName TemplateFilesDetectorsGlobalAsset();

/**
 * @return the directory location of the PHP tag detector scripts.  The editor will use these scripts
 * to find out framework-specific template files. These are the template files detectors that the user
 * creates for their own projects.
 */
wxFileName TagDetectorsLocalAsset();

/**
 * @return the directory location of the PHP tag detector scripts.  The editor will use these scripts
 * to find out framework-specific template files. These are the template files detectors that are provided
 * by MVC editor.
 */
wxFileName TagDetectorsGlobalAsset();

/**
 * @return the directory location of the PHP database detector scripts.  The editor will use these scripts
 * to find out framework-specific template files. These are the template files detectors that the user
 * creates for their own projects.
 */
wxFileName DatabaseDetectorsLocalAsset();

/**
 * @return the directory location of the PHP database detector scripts.  The editor will use these scripts
 * to find out framework-specific template files. These are the template files detectors that are provided
 * by MVC editor.
 */
wxFileName DatabaseDetectorsGlobalAsset();

/**
 * @return the file location of the SQL script to create the detector database.
 */
wxFileName DetectorSqlSchemaAsset();

/**
 * @return the base directory location of the skeletons.  Skeletons are "templates"
 * that are used when the editor needs to paste source code in a (new) file; we 
 * don't hardcode the code in the executable.
 */
wxFileName SkeletonsBaseAsset();

/**
 * @return the full path to a directory inside of the system temp directory
 *         that has been created for MVC Editor. The directory is guaranteed to
 *         exist.
 */
wxFileName TempDirAsset();

/**
 * @return the full path to a directory inside of the system user data directory
 *         that has been created for MVC Editor. The directory is guaranteed to
 *         exist.
 */
wxFileName ConfigDirAsset();

}

#endif