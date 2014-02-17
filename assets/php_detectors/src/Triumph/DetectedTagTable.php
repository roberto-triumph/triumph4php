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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */

/**
 * This class will take care of inserting and deleting from the
 * detected_tags table.
 */
class Triumph_DetectedTagTable extends Zend_Db_Table_Abstract {

	protected $_name = 'detected_tags';
	
	/**
	 * Saves all of the tags into the database
	 *
	 * @param Triumph_DetectedTag[] $allTags the tags to save
	 */
	public function saveTags($allTags, $sourceDir) {
		
		// remove all tags from previous detection. since there is no
		// easy way to tell of duplicates
		$sourceDbTable = new Triumph_SourceTable($this->getAdapter());
		$sourceId = $sourceDbTable->getOrSave($sourceDir);
		$strWhere = $this->getAdapter()->quoteInto("source_id = ?", $sourceId);
		$this->delete($strWhere);
		
		if (empty($allTags)) {
			return ;
		}
				
		// sqlite optimizes transactions really well; use transaction so that the inserts are faster
		$this->getAdapter()->beginTransaction();
		foreach ($allTags as $tag) {
			
			// insert twice; once fully qualified and once
			// just the method name; that way qualified lookups
			// work 
			$this->insert(array(
				'source_id' => $sourceId,
				'key' => $tag->className . '::' . $tag->identifier,
				'type' => $tag->type,
				'class_name' => $tag->className,
				'method_name' => $tag->identifier,
				'return_type' => $tag->returnType,
				'namespace_name' => $tag->namespaceName,
				'comment' => $tag->comment
			));
			
			$this->insert(array(
				'source_id' => $sourceId,
				'key' => $tag->identifier,
				'type' => $tag->type,
				'class_name' => $tag->className,
				'method_name' => $tag->identifier,
				'return_type' => $tag->returnType,
				'namespace_name' => $tag->namespaceName,
				'comment' => $tag->comment
			));
		}
		$this->getAdapter()->commit();
 	}
}
