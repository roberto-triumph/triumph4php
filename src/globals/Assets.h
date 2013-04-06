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
#include <wx/bitmap.h>

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
 * @return the loaded bitmap of the given auto complete image.  The image name must exist; if it
 * does not an assertion is triggered.
 */
wxBitmap AutoCompleteImageAsset(wxString imageName);

/**
 * @param the name of an image, minus the .png extension
 * @return the loaded bitmap of the given icon image.  The image name must exist; if it
 * does not an assertion is triggered.
 */
wxBitmap IconImageAsset(wxString imageName);

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
wxFileName UrlTagDetectorsGlobalAsset();

/**
 * @return the directory location of the PHP URL detector scripts.  The editor will use these scripts
 * to find out framework-specific URL routes. These are the URL detectors that the user
 * creates for their own projects.
 */
wxFileName UrlTagDetectorsLocalAsset();

/**
 * @return the directory location of the PHP template files detector scripts.  The editor will use these scripts
 * to find out framework-specific template files. These are the template files detectors that the user
 * creates for their own projects.
 */
wxFileName TemplateFileTagsDetectorsLocalAsset();

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
 * to find out framework-specific database settings. These are the database detectors that the user
 * creates for their own projects.
 */
wxFileName DatabaseTagDetectorsLocalAsset();

/**
 * @return the directory location of the PHP database detector scripts.  The editor will use these scripts
 * to find out framework-specific database settings. These are the database detectors that are provided
 * by MVC editor.
 */
wxFileName DatabaseTagDetectorsGlobalAsset();

/**
 * @return the directory location of the PHP Config detector scripts.  The editor will use these scripts
 * to find out framework-specific Config files. These are the Config files detectors that the user
 * creates for their own projects.
 */
wxFileName ConfigTagDetectorsLocalAsset();

/**
 * @return the directory location of the PHP Config detector scripts.  The editor will use these scripts
 * to find out framework-specific Config files. These are the Config files detectors that are provided
 * by MVC editor.
 */
wxFileName ConfigTagDetectorsGlobalAsset();

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

/**
 * The location of the tag cache db. The tag DB file contains all of
 * the parsed resources for all of defined projects' resources.
 *
 * The SQL schema for this cache can be found in resources/sql/resource.sql
 * Schema management will be done by TagCacheDbVersionClass. We will check the version
 * of the schema in the db against the schema the code expects and will re-create the schema
 * if the versions differ.  This check will be done at app start, so that in most of the code
 * we can assume that the schema is up-to-date.
 *
 * @see mvceditor::ParsedTagFinderClass
 * @see mvceditor::TagCacheDbVersionActionClass
 *
 * @return the full path to the tag database that stores tags for all defined projects
 */
wxFileName TagCacheAsset();

/**
 * A separate db file that contains the same schema as TagCache, but this DB is 
 * used to store tags for opened files only.  The samve version checking mechanism
 * that applies to TagCache also applies to WorkingCache
 *
 * @return the full path to the tag database that stores tags for all opened files
 * in a session
 */
wxFileName TagCacheWorkingAsset();

/**
 * The location of the detectors cache for all defined projects. 
 *
 * The SQL schema for this cache can be found in resources/sql/detectors.sql
 * Schema management will be done by DetectorCacheDbVersionClass. We will check the version
 * of the schema in the db against the schema the code expects and will re-create the schema
 * if the versions differ.  This check will be done at app start, so that in most of the code
 * we can assume that the schema is up-to-date.
 *
 * @see mvceditor::DetectorCacheDbVersionClass
 *
 * @return the full path to the tag database that stores detected tags for all defined projects
 */
wxFileName DetectorCacheAsset();

}

#endif