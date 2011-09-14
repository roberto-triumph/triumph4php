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

require_once 'MvcEditorFrameworkBase.php';
require_once 'MvcEditorDatabaseInfo.php';

class MvcEditorFrameworkTest extends MvcEditorFrameworkBaseClass {

	public function getName() {
		return 'Test';
	}
	
	public function getIdentifier() { 
		return 'Test'; 
	}
	
	public function isUsedBy($dir) {
		return is_dir($dir);
	}

	public function databaseInfo($dir) {
		$list = array();
		$info = new MvcEditorDatabaseInfo(MvcEditorDatabaseInfo::DRIVER_MYSQL, 'dev', "local dev", '127.0.0.1', 3306, 'db_name', 'filename', 'root', '123');
		$list[] = $info;
		return $list;
	}
}