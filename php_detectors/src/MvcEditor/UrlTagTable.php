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

class MvcEditor_UrlTagTable extends Zend_Db_Table_Abstract {

	protected $_name = 'url_tags';

	/**
	 * saves the given MvcEditor_Urls into the database.
	 * 
	 * @param MvcEditor_Url[] $arrUrls the urls to insert 
	 */
	function saveUrls($arrUrls, $sourceDir) {
		if (!is_array($arrUrls)) {
			return;
		}
		
		// delete all old urls
		/// make sure that sourceDir ends with the separator to make sure
		// only the correct entries are deleted
		// also  escape a value so that it is suitable for using in a LIKE SQL clause
		// ie. so that an underscore is treated literally
		$sourceDir = \opstring\ensure_ends_with($sourceDir, DIRECTORY_SEPARATOR);
		$sourceDir = \opstring\replace($sourceDir, '_', '^_');
		$strWhere = $this->getAdapter()->quoteInto("full_path LIKE ? ESCAPE '^'", $sourceDir . '%');
		$this->delete($strWhere);
		
		// sqlite optimizes transactions really well; use transaction so that the inserts are faster
		$this->getAdapter()->beginTransaction();
		foreach ($arrUrls as $url) {
			
			$this->insert(array(
				'url' => $url->url,
				'full_path' => $url->fileName,
				'class_name' => $url->className,
				'method_name' => $url->methodName
			));
		}
		$this->getAdapter()->commit();
	}
}