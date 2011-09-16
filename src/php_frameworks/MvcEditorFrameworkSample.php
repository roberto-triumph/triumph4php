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

class MvcEditorFrameworkSample extends MvcEditorFrameworkBaseClass {

	public function getName() {
		return 'sample framework';
	}
	
	public function getIdentifier() { 
		return 'Sample-Framework'; 
	}
	
	public function isUsedBy($dir) {
		return is_file($dir . DIRECTORY_SEPARATOR . 'library' . DIRECTORY_SEPARATOR . 'config.framework.php');
	}

	public function databaseInfo($dir) {
		$list = array();
		require_once($dir . DIRECTORY_SEPARATOR . 'library' . DIRECTORY_SEPARATOR . 'config.db.php');
		$info = new MvcEditorDatabaseInfo(MvcEditorDatabaseInfo::DRIVER_MYSQL, 'default', 'verifi master', MY_DB_MASTER_HOST, 3306, MY_DB_NAME, '', MY_DB_USER, MY_DB_PASSWORD);
		$list[] = $info;
		$info = new MvcEditorDatabaseInfo(MvcEditorDatabaseInfo::DRIVER_MYSQL, 'default', 'gateway master', GATEWAY_DB_MASTER_HOST, 3306, GATEWAY_DB_NAME, '', GATEWAY_DB_USER, GATEWAY_DB_PASSWORD);
		$list[] = $info;
		$info = new MvcEditorDatabaseInfo(MvcEditorDatabaseInfo::DRIVER_MYSQL, 'default', 'forex master', FOREX_DB_MASTER_HOST, 3306, FOREX_DB_NAME, '', FOREX_DB_USER, FOREX_DB_PASSWORD);
		$list[] = $info;
		return $list;
	}
}