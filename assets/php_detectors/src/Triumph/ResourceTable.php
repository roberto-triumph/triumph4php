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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */

class Triumph_ResourceTable extends Zend_Db_Table_Abstract {

	protected $_name = 'resources';
	
	protected $_primary = array('file_item_id', 'key');
	
	/**
	 * Retrieves all of the methods from the given files. Only resources that 
	 * are methods will be returned. Only public methods are returned.
	 *
	 * @param Zend_Db_Adapter_Abstract $db the DB to query
	 * @param Triumph_FileItem[] $fileItems the file item IDs to query
	 * @return Triumph_Resource[] the resources that were found in any of the given files.
	 */
	public function PublicMethodsFromFiles($fileItems) {
		if (empty($fileItems)) {
			return array();
		}
		$fileItemIds = array();
		foreach ($fileItems as $fileItem) {
			$fileItemIds []= $fileItem->file_item_id;
		}
				
		// key = identifier ==> don't get the fully qualified matches
		$select = $this->_db->select();
		$select->from(array('r' => 'resources'), array('class_name', 'identifier', 'signature', 'return_type', 'comment'))
			->join(array('f' => 'file_items'), 'r.file_item_id = f.file_item_id', array('full_path'));
		$select->where("r.file_item_id IN(?)", $fileItemIds)
			->where('key = identifier')
			->where('type = ?', Triumph_Resource::TYPE_METHOD)
			->where('is_private = 0')
			->where('is_protected = 0');			
		$stmt = $select->query();
		$methods = array();
		while ($row = $stmt->fetch(Zend_Db::FETCH_ASSOC)) {
			$method = new Triumph_Resource();
			$method->MakeMethod($row['class_name'], $row['identifier'], $row['signature'], $row['return_type'], $row['comment']);
			$method->fullPath = $row['full_path'];
			$methods[] = $method;
		}
		return $methods;
 	}
	
}
