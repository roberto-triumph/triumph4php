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

set_include_path(get_include_path() . PATH_SEPARATOR .  realpath(__DIR__ . '/../lib/'));
require_once 'Zend/Db/Table.php';
 
/**
 * MvcEditorFileItemClass represents a single file name (full path) that was
 * found by MVC Editor.  
 *
 * MVC Editor creates a database of resources; this source code file can be 
 * used to get items from it.
 */
class MvcEditorFileItemClass extends Zend_Db_Table_Abstract {

	protected $_name = 'file_items';

	/**
	 * primary key for this file
	 * /
	public $fileItemId;
	
	/**
	 * The full path that this item represents.  This always has the file separators
	 * that the host operating system has; in Windows full path is separated by
	 * backslash and on linux it is separated by forward slash.
	 * /
	public $fullPath;
	
	public function __construct($fileItemId = 0, $fullPath = '') {
		$this->fileItemId = $fileItemId;
		$this->fullPath = $fullPath;
	}
	*/
	
	/**
	 * @param $pdo the connection to the resource database
	 * @param $startingPath the path to query
	 * @return MvcEditorFileItemClass[] all files whose full path starts with $startingPath 
	 */
	public function MatchingFiles($startingPath) {
		$startingPath .= '%';
		
		$select = $this->select();
		$select->where('full_path LIKE ?', $startingPath);
		$stmt = $select->query();
		
		return $stmt->fetchAll();
	}
}