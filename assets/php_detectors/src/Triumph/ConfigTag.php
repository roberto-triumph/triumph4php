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
 * A config file artifact. Triumph will turn config tags into menu items that 
 * can be easily accessed by the user; that way config files are easier to get at.
 */
class Triumph_ConfigTag {


	/**
	 * @var string a friendly name for this connection. Triumph will display this to the user.
	 */
	public $label;
	
	/**
	 * Triumph will open this file when the user clicks on a config menu item.
	 * @var string the full path to the config file.
	 */
	public $fullPath;
	

	/**
	 * @param $label string
	 * @param $fullPath string this must be the full path. must have OS-dependant directory separators
	 */
	public function __construct($label, $fullPath) {
		$this->label = $label;
		$this->fullPath = $fullPath;
	}

}