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
 * This is a class that will take care of deserializing a single CallClass from the call stack file.
 * Usage:
 * The user of this class will open the file using the PHP file functions, the call the fromFile method
 * giving it the opened file resource.  The fromFile method will then reset and fill in the public members
 * according to the contents of the file.
 */
class MvcEditor_CallStack {

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
	 * Read a single instance of MvcEditor_CallStack from the line.
	 *
	 * @param string $stepType one of the constants above
	 * @param string line from 
	 * @return boolean TRUE if the call instance was successfully read from the given line. If false, 
	 * fthe line did not contain proper data.
	 */
	public function fromLine($stepType, $line) {
		$this->clear();
		$ret = false;
		$line = trim($line);
		if (empty($line)) {
			return $ret;
		}
			
		// line format: a CSV line with variable columns. Each row may have a different
		// column count depending on the type. Format is as follows:
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
		if (count($columns) >= 1 && $stepType == self::BEGIN_FUNCTION) {
			$this->type = $stepType;
			$this->resource = $columns[0];
			$ret = true;
		}
		else if (count($columns) >= 2 && ($stepType == self::BEGIN_METHOD)) {
			$this->type = $stepType;
			$this->resource = $columns[0] . '::' . $columns[1];
			$ret = true;
		}
		else if (count($columns) >= 2 && $stepType == self::PARAM) {
			$this->type = $stepType;
			$this->paramType = $columns[0];
			if (self::T_ARRAY == $this->paramType) {
				$this->variableName = $columns[1];
				for ($i = 2; $i < count($columns); $i++) {
					$this->arrayKeys[] = $columns[$i];
				}
			}
			else if (self::SCALAR == $this->paramType) {
			
				// trim the ending newline that ends the line too
				// item may be sorrounded by quotes when it is a constant (ALWAYS DOUBLE QUOTES only)
				$this->scalar = trim($columns[1], "\"\n");
			}
			else if (self::OBJECT == $this->paramType) {
				$this->variableName = $columns[1];
			}
			$ret = true;
		}
		else if (count($columns) >= 1 && ($stepType == self::T_ARRAY)) {
			$this->type = $stepType;
			$this->variableName = $columns[0];
			for ($i = 1; $i < count($columns); $i++) {
				$this->arrayKeys[] = $columns[$i];
			}
			$ret = true;
		}
		else if (count($columns) >= 1 && ($stepType == self::OBJECT)) {
			$this->type = $stepType;
			$this->variableName = $columns[0];
			$ret = true;
		}
		else if (count($columns) >= 1 && ($stepType == self::SCALAR)) {
			$this->type = $stepType;
			$this->scalar = $columns[0];
			// trim the ending newline that ends the line too
			// item may be sorrounded by quotes when it is a constant (ALWAYS DOUBLE QUOTES only)
			$this->scalar = trim($this->scalar , "\"\n");
			$ret = true;
		}
		else if (count($columns) >= 1 && ($stepType == self::T_RETURN)) {
			$this->type = $stepType;
			$ret = true;
		}
		return $ret;
	}
}
