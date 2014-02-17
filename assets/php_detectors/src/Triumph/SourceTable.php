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


class Triumph_SourceTable extends Zend_Db_Table_Abstract {

	protected $_name = 'sources';
	

	/**
	 * @return the primary key of the source directory
	 */
	public function getOrSave($sourceDir) {
		
		/// make sure that sourceDir ends with the separator to make sure
		// only the correct entries are selected / saved
		$sourceDir = \opstring\ensure_ends_with($sourceDir, DIRECTORY_SEPARATOR);
		
		$select = $this->select()->where('directory = ?', $sourceDir);
		$stmt = $select->query(Zend_Db::FETCH_ASSOC);
		if ($row = $stmt->fetch()) {
			return $row['source_id'];
		}
		return $this->insert(array(
			'directory' => $sourceDir
		));
		
	}

}
