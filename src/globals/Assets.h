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
#ifndef __T4P_ASSETS_H__
#define __T4P_ASSETS_H__

#include <wx/filename.h>
#include <wx/bitmap.h>

namespace t4p {

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
wxBitmap BitmapImageAsset(wxString imageName);

/**
 * @param the name of an image, minus the .ico extension
 * @return the loaded bitmap of the given icon image.  The image name must exist; if it
 * does not an assertion is triggered.
 */
wxIcon IconImageAsset(wxString imageName);

/**
 * @return the base directory location of the PHP URL scripts.  The editor will use this as the
 * include path when calling all PHP detectors
 */
wxFileName PhpDetectorsBaseAsset();

/**
 * @return the directory location of the PHP URL detector scripts.  The editor will use these scripts
 * to find out framework-specific URL routes. These are the URL detectors that are provided
 * by Triumph.
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
 * by Triumph.
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
 * by Triumph.
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
 * by Triumph.
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
 * by Triumph.
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
 *         that has been created for Triumph. The directory is guaranteed to
 *         exist.
 */
wxFileName TempDirAsset();

/**
 * @return the full path to a directory inside of the system user data directory
 *         that has been created for Triumph. The directory is guaranteed to
 *         exist.
 */
wxFileName ConfigDirAsset();

/**
 * The bootstrap file contains the location where all settings are stored
 * The bootstrap file could be located in either the same directory
 * as the application executable (in the case of portable installations)
 * or in the user data directory in all other cases.
 *
 * @return the full path to the "bootstrap" file that contains the location
 *         of all user configurable settings. This file may not exist in the
 *         file system.
 */
wxFileName BootstrapConfigFileAsset();

/**
 * @return the full path to the settings directory. This setting is read from
 *         the bootstrap config file.  See  BootstrapConfigFileAsset() 
 *         to see where the bootstrap config file is stored.
 *         Note that this value is always read from the file system
 *         care should be taken when calling this function many times
 *         as it incurs a performance penalty
 *         Note 2: the returned path may not exist in the file system.
 *         It will not exist when the application is first run.
 */
wxFileName SettingsDirAsset();

/**
 * @param settingsDir the location of the settings directory (contents of this string 
 *        will be stored in the config file)
 *        If the settings directory is located in the same directory as the executable,
 *        the the settings directory will be stored in the "local" bootstrap config 
 *        file (located in the same directory
 *        as the executable). Otherwise, the settings directory will be saved to the
 *        global bootstrap config file (located in the
 *        user data directory)
 * Note that this function always writes to the file system
 * care should be taken when calling this function many times
 * as it incurs a performance penalty
 */
void SetSettingsDirLocation(const wxFileName& settingsDir);

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
 * @see t4p::ParsedTagFinderClass
 * @see t4p::TagCacheDbVersionActionClass
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
 * @see t4p::DetectorCacheDbVersionClass
 *
 * @return the full path to the tag database that stores detected tags for all defined projects
 */
wxFileName DetectorCacheAsset();

/**
 * @return wxFileName the full path to the file that stores the application's version
 *         number
 */
wxFileName VersionFileAsset();

/**
 * @return wxFileName the full path to the file that stores the application's 
 *         ChangeLog file
 */
wxFileName ChangeLogFileAsset();

}

#endif