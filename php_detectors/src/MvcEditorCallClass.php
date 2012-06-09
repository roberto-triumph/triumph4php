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
 * This is a class that will take care of deserializing a single CallClass from the call stack file.
 * Usage:
 * The user of this class will open the file using the PHP file functions, the call the fromFile method
 * giving it the opened file resource.  The fromFile method will then reset and fill in the public members
 * according to the contents of the file.
 */
class MvcEditorCallClass {

	const BEGIN_METHOD = 'BEGIN_METHOD';
	
	const BEGIN_FUNCTION = 'BEGIN_FUNCTION';
	
	const PARAM = 'PARAM';
	
	const T_ARRAY = 'ARRAY';
	
	const SCALAR = 'SCALAR';
	
	const OBJECT = 'OBJECT';
	
	const T_RETURN = 'RETURN';

	/** 
	 * @var string the type of call, one of the class constants
	 */
	public $type;

	/**
	 * @var string the fully qualified name of the resource being called; in the form ClassName::MethodName if this is a method
	 * or functionName if this is a function.
	 */
	public $resource;
	
	/**
	 * @var string, this is the argument to the function / method call that has begun in the
	 * previous line. The argument may be a string (ie. "news/view" without the quotes), a variable (ie. $data, or $this->data). In the
	 * case that one of the arguments is the result of another function / method call; then the argument will
	 * be the function name with a parenthesis pair; like this "myFunct()". The parentheses wil always be
	 * empty, even if that call had arguments.
	 */
	public $paramType;
	
	public $variableName;
	
	public $arrayKeys;
	
	public $scalar;
	
	/**
	 * sets all public members to the empty string / array.
	 */
	public function clear() {
		$this->type = '';
		$this->resource = '';
		$this->variableName = '';
		$this->arrayKeys = '';
		$this->scalar = '';
		$this->paramType = '';
	}

	/**
	 * Read a single instance of CallClass from the given file.
	
	 * @param resource $fileResource the file to read from
	 * @return boolean TRUE if the call instance was successfully read from the given file. If false, either
	 * file is not a call stack file or the file did not contain proper data.
	 */
	public function fromFile($fileResource) {
		$this->clear();
		$ret = false;
		if (!feof($fileResource)) {
			$line = trim(fgets($fileResource));
			if (!empty($line)) {
			
				// file format: a CSV file with variable columns. Each row may have a different
				// column count depending on the type. Format is as follows:
				//
				//
				// BEGIN_FUNCTION, function name,
				// BEGIN_METHOD, class name, method name
				// PARAM, SCALAR, expression
				// PARAM, OBJECT, variable name
				// PARAM, ARRAY, variable name, list of array keys
				// ARRAY, variable name, list of array keys
				// SCALAR, variable name,lexeme
				// OBJECT, variable name
				// RETURN
				//  
				// where
				// expression is the lexeme (string) of a constant (when argument is a string / number)
				// or it can be a variable name.
				$columns  = explode(',', $line);
				if (count($columns) >= 2 && $columns[0] == self::BEGIN_FUNCTION) {
					$this->type = $columns[0];
					$this->resource = $columns[1];
					$ret = true;
				}
				else if (count($columns) >= 3 && ($columns[0] == self::BEGIN_METHOD)) {
					$this->type = $columns[0];
					$this->resource = $columns[1] . '::' . $columns[2];
					$ret = true;
				}
				else if (count($columns) >= 3 && $columns[0] == self::PARAM) {
					$this->type = $columns[0];
					$this->paramType = $columns[1];
					if (self::T_ARRAY == $this->paramType) {
						$this->variableName = $columns[2];
						for ($i = 3; $i < count($columns); $i++) {
							$this->arrayKeys[] = $columns[$i];
						}
					}
					else if (self::SCALAR == $this->paramType) {
					
						// trim the ending newline that ends the line too
						// item may be sorrounded by quotes when it is a constant (ALWAYS DOUBLE QUOTES only)
						$this->scalar = trim($columns[2], "\"\n");
					}
					else if (self::OBJECT == $this->paramType) {
						$this->variableName = $columns[2];
					}
					$ret = true;
				}
				else if (count($columns) >= 2 && ($columns[0] == self::T_ARRAY)) {
					$this->type = $columns[0];
					$this->variableName = $columns[1];
					for ($i = 2; $i < count($columns); $i++) {
						$this->arrayKeys[] = $columns[$i];
					}
					$ret = true;
				}
				else if (count($columns) >= 2 && ($columns[0] == self::OBJECT)) {
					$this->type = $columns[0];
					$this->variableName = $columns[1];
					$ret = true;
				}
				else if (count($columns) >= 2 && ($columns[0] == self::SCALAR)) {
					$this->type = $columns[0];
					$this->scalar = $columns[1];
					// trim the ending newline that ends the line too
					// item may be sorrounded by quotes when it is a constant (ALWAYS DOUBLE QUOTES only)
					$this->scalar = trim($this->scalar , "\"\n");
					$ret = true;
				}
				else if (count($columns) >= 1 && ($columns[0] == self::T_RETURN)) {
					$this->type = $columns[0];
					$ret = true;
				}
			}
		}
		return $ret;
	}

}