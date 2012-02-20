<?php
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

/**
 * This class encapsulates the logic for each distinct PHP framework.  The methods of this class are questions that a framework
 * needs to be able to answer so that MVC Editor knows where certain artifacts are located; like database connnections,
 * routes, logs, etc...
 *
 * Each different framework will have its own class that inherits from this class and will answer the questions appropriately.
 */
abstract class MvcEditorFrameworkBaseClass {

	/**
	 * @return string a friendly name for this framework. This name
	 * will be displayed in various dialogs of the Editor.
	 * Any newlines or tabs will be converted to single spaces.
	 * Examples: 'symfony', 'code igniter', 'cakephp', 'lithium 0.10'
	 */
	abstract public function getName();
	
	/**
	 * @return string a string that uniquely identifies this framework (case insensitive). The identifier should
	 * be unique so that MVC Editor can handle separate versions of a framework gracefully.
	 * Examples: 'symfony_1_4', 'CI-0.10'
	 * This string is the one used for the input into the PHP program; it is used to determine which 
	 * specific instance of MvcEditorFrameworkBaseClass to use
	 */
	abstract public function getIdentifier();
	
	/**
	 * The framework detection method. This method should detect whether the given
	 * directory contains files that use this framework. Most of the time this will
	 * consist of looking for specific framework files. By returning TRUE, the editor will 
	 * relate the project at $dir to this framework; this triggers various 
	 * the enabling of various dialogs and functionality.
	 *
	 * @param string the directory in question; absolute path. 
	 * @return bool TRUE if the directory houses a project that uses this framework.
	 */
	abstract public function isUsedBy($dir);
	
	/**
	 * The database connection detection method. Sub classes should read the database connection
	 * info from their respective files and create the necessary MvcEditorDatabaseInfo objects.
	 *
	 * @param $dir the base directory of the project in question; absolute path.
	 * @return array of MvcEditorDatabaseInfo object
	 */
	abstract public function databaseInfo($dir);
	
	/**
	 * Sub-classes will implement this method to list all of their configuration files; MVC Editor will
	 * use these to populate menus.
	 *
	 * @param $dir the base directory of the project in question; absolute path.
	 * @return array associative (name => full path) string full path to the project's configuration files. The MVC Editor will
	 * use the returned files to give the user 'quick' access to the framework's configuration files.
	 * The returned files MUST HAVE OS-dependant file separators.
	 */
	abstract public function configFiles($dir);
	
	/**
	 * Sub-classes will implement this method to aid MVC Editor in code completion. The sub classes will return 
	 * any resources that it dynamically adds to its base classes; they could also be the result of
	 * magic methods.
	 *
	 * @param $dir the base directory of the project in question; absolute path.
	 * @return array of MvcEditorResource objects
	 */
	abstract public function resources($dir);
	
	/**
	 * Sub-classes will implement this method to help MVC Editor create URLs from files;
	 * sub-clases will use the routing rules specific to its framework.
	 *
	 * @param $dir the base directory of the project in question; absolute path.
	 * @param $fileName the file to make the URL
	 * @return array of MvcEditorUrlClass objects for the given file
	 */
	abstract public function makeUrls($dir, $fileName);
	
	/**
	 * Sub-classes will implement this method to help MVC Editor determine which template files 
	 * are executed by a specific URL. Frameworks usually use a specific function / method to invoke
	 * template files; others may just have a specific convention (view files are named the same as the
	 * invoked action. 
	 * In the first case above, subclasses will usually read the call stack file that MVC Editor
	 * generates, iterate through the function calls, and look for invocations of the framework's template
	 * function / method.
	 * For the second case, a subclass may be able to determine the view by just looking at the URL.
	 *
	 * @param string $dir the base directory of the project in question; absolute path.
	 * @param string $url full URL to the action in question
	 * @param string $callStackFile the location of the call stack file, use the CallStackClass to read 
	 *        iterate through the action's function / method calls.
	 * @return array of full paths, each item will be the full path to the view files that the given action
	 *         uses.
	 */
	abstract public function viewFiles($dir, $url, $callStackFile);

}