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

class Triumph_ConfigTagTable extends Zend_Db_Table_Abstract {

	protected $_name = 'config_tags';
	

	/**
	 * Saves all of the given config tags. 
	 *
	 * @param Triumph_DatabaseTag[] the database tags to save
	 * @param string $sourceDir the source directory for this database tag
	 */
	public function saveConfigTags($configTags, $sourceDir) {
		
		// delete all old config tags
		$sourceDbTable = new Triumph_SourceTable($this->getAdapter());
		$sourceId = $sourceDbTable->getOrSave($sourceDir);
		$strWhere = $this->getAdapter()->quoteInto("source_id = ?", $sourceId);
		$this->delete($strWhere);
		
		// sqlite optimizes transactions really well; use transaction so that the inserts are faster
		$this->getAdapter()->beginTransaction();
		foreach ($configTags as $configTag) {
			$this->insert(array(
				'source_id' => $sourceId,
				'label' => $configTag->label,
				'full_path' => $configTag->fullPath
			));
		}
		$this->getAdapter()->commit();
 	}
	
}
