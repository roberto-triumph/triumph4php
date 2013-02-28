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
 */
class MvcEditor_CallStack {

	/** 
	 * variable is assigned a scalar (string or number) 
	 */
	const SCALAR = 'SCALAR';
	
	/** 
	 * variable is assigned an array. Note that there will be one ARRAY_KEY 
	 *  assignment for each array key
	 */
	const T_ARRAY = 'ARRAY';
	
	/** 
	 * a new key being assigned to an array.
	 */
	const ARRAY_KEY = 'ARRAY_KEY';
	
	/** 
	 * variable is created using the new operator
	 */
	const NEW_OBJECT = 'NEW_OBJECT';
	
	/** 
	 * variable is assigned another variable
	 */
	const ASSIGN = 'ASSIGN';
	
	/** 
	 * variable is assigned an object property
	 */
	const PROPERTY = 'PROPERTY';
	
	/** 
	 * variable is assigned the return value of an object method
	 */
	const METHOD_CALL = 'METHOD_CALL';
	
	/** 
	 * variable is assigned the return value of a function
	 */
	const FUNCTION_CALL = 'FUNCTION_CALL';
	
	/** 
	 * not a variable; this symbol declares that a new scope
	 * has started.  All of the symbols that follow this symbol belong
	 * in the scope of this function.
	 * FunctionName will contain the function name 
	 * of the scope
	 */
	const BEGIN_FUNCTION = 'BEGIN_FUNCTION';
	
	/** 
	 * not a variable; this symbol declares that a new scope
	 * has started.  All of the symbols that follow this symbol belong
	 * in the scope of this method.
	 * ClassName and MethodName will contain the  class and method
	 * of the scope
	 */
	const BEGIN_METHOD = 'BEGIN_METHOD';

	/**
	 * the symbol type. one of the constants.
	 */
	public $type; 
	
	/**
	 *  the variable being assigned (the left side of 
	 *  $a = $b). Will contain the siguil ('$')
	 */
	public $destinationVariable;
	
	/**
	 *  if type = SCALAR, then this will be the scalar's lexeme (the contents)
	 *  for the code:
	 *    $s = 'hello';
	 *  ScalarValue will be 'hello' (no quotes)
	 */
	public $scalarValue;
	
	/**
	 * if type = ARRAY_KEY this is the key that was assigned to the array
	 *  for the code:
	 *    $s['greeting'] = 'hello';
	 * ArrayKey will be 'greeting' (no quotes)
	 */
	public $arrayKey;
	
	/**
	 *  the name of the variable to assign (the right side of 
	 *  $a = $b)
	 */
	public $sourceVariable;
	
	/**
	 * if type = METHOD or type = PROPERTY, this is the name of the
	 * variable used
	 *  for the code:
	 *    $name =  $person->firstName
	 * ObjectName will be $person
	 */
	public $objectName;
	
	/**
	 * if type = PROPERTY, this is the name of the property
	 * .  this never contains a siguil
	 * for the code:
	 *    $name = $person->firstName
	 * PropertyName will be firstName
	 */
	public $propertyName;
	
	/**
	 * if type = METHOD, this is the name of the method being 
	 * called.
	 *  for the code:
	 *    $name = $person->getName()
	 * MethodName will be getName
	 * 
	 * if type = BEGIN_METHOD, this is the name of the 
	 * method where all of the subsquent variables are located in
	 */
	public $methodName;
	
	/**
	 * if type = FUNCTION, this is the name of the function being 
	 * called.
	 *  for the code:
	 *    $name = getName()
	 * FunctionName will be getName
	 * 
	 * if type = BEGIN_FUNCTION, this is the name of the 
	 * function where all of the subsquent variables are located in
	 */
	public $functionName;
	
	/**
	 * if type = NEW_OBJECT, this is the name of the class being 
	 * instatiated.
	 *  for the code:
	 *    $user = new UserClass;
	 * ClassName will be UserClass
	 
	 * if type = BEGIN_METHOD, this is the name of the 
	 * class where all of the subsquent variables are located in
	 */
	public $className;
	
	/**
	 * if type = METHOD or type = FUNCTION this is the list of
	 * variables that were passed into the function / method. These are the
	 * "simple" variables; ie the temporary variables that are
	 * assigned for example the code:
	 * 
	 * $name = buildName(getFirstName($a), getLastName($a));
	 * 
	 * then FunctionArguments are [ $@tmp1, $@tmp2 ]
	 * because the resul of getFirstName and getLastName get assigned to
	 * a temp variable.
	 * 
	 */
	public $functionArguments;
	
	/**
	 * sets all public members to the empty string / array.
	 */
	public function clear() {
		$this->type = self::T_ARRAY;
		$this->arrayKey = '';
		$this->className = '';
		$this->destinationVariable = '';
		$this->functionArguments = array();
		$this->functionName = '';
		$this->methodName = '';
		$this->objectName = '';
		$this->scalarValue = '';
		$this->sourceVariable = '';
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
		// BEGIN_FUNCTION: function name,
		// BEGIN_METHOD: class name, method name
		// SCALAR: destination variable name, scalar value
		// NEW_OBJECT: destination variable name, class name
		// ARRAY: destination variable name
		// ARRAY_KEY: destination variable name, array key
		// ASSIGN: destination variable name, source variable
		// PROPERTY: destination variable name, object name, property name
		// METHOD_CALL: destination variable name, object name, method name, argument1, argument2,...
		// FUNCTION_CALL: destination variable name, function name, argument1, argument2,...
		//
		$columns  = explode(',', $line);
		if (count($columns) >= 1 && $stepType == self::BEGIN_FUNCTION) {
			$this->type = $stepType;
			$this->functionName = $columns[0];
			$ret = true;
		}
		else if (count($columns) >= 2 && ($stepType == self::BEGIN_METHOD)) {
			$this->type = $stepType;
			$this->className = $columns[0];
			$this->methodName = $columns[1];
			$ret = true;
		}
		else if (count($columns) >= 2 && $stepType == self::SCALAR) {
			$this->type = $stepType;
			$this->destinationVariable = $columns[0];
			$this->scalarValue = $columns[1];
			$ret = true;
		}
		else if (count($columns) >= 2 &&  $stepType == self::NEW_OBJECT) {
			$this->type = $stepType;
			$this->destinationVariable = $columns[0];
			$this->className = $columns[1];
			$ret = true;
		}
		else if (count($columns) >= 1 && $stepType == self::T_ARRAY) {
			$this->type = $stepType;
			$this->destinationVariable = $columns[0];
			$ret = true;
		}
		else if (count($columns) >= 2 && $stepType == self::ARRAY_KEY) {
			$this->type = $stepType;
			$this->destinationVariable = $columns[0];
			$this->arrayKey = $columns[1];
			$ret = true;
		}
		else if (count($columns) >= 2 && $stepType == self::ASSIGN) {
			$this->type = $stepType;
			$this->destinationVariable = $columns[0];
			$this->sourceVariable = $columns[1];
			$ret = true;
		}
		else if (count($columns) >= 3 && $stepType == self::PROPERTY) {
			$this->type = $stepType;
			$this->destinationVariable = $columns[0];
			$this->objectName = $columns[1];
			$this->propertyName = $columns[2];
			$ret = true;
		}
		else if (count($columns) >= 3 && $stepType == self::METHOD_CALL) {
			$this->type = $stepType;
			$this->destinationVariable = $columns[0];
			$this->objectName = $columns[1];
			$this->methodName = $columns[2];
			for ($i = 3; $i < count($columns); $i++) {
				$this->functionArguments[] = $columns[$i];
			}
			$ret = true;
		}
		else if (count($columns) >= 2 && $stepType == self::FUNCTION_CALL) {
			$this->type = $stepType;
			$this->destinationVariable = $columns[0];
			$this->functionName = $columns[1];
			for ($i = 2; $i < count($columns); $i++) {
				$this->functionArguments[] = $columns[$i];
			}
			$ret = true;
		}
		return $ret;
	}
}
