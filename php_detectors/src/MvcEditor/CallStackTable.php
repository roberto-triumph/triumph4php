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
class MvcEditor_CallStackTable extends Zend_Db_Table_Abstract {

	protected $_name = 'call_stacks';

	/**
	 * loads the call stack. call stack is sorted in the order that it is executed.
	 */
	public function load() {
		$select = $this->select();
		$select->order('step_number');
		$stmt = $select->query(Zend_Db::FETCH_ASSOC);
		$callStacks = array();
		while ($row = $stmt->fetch()) {
			$call = new MvcEditor_CallStack();
			if ($call->fromLine($row['step_type'], $row['expression'])) {
				$callStacks [] = $call;
			}
		}
		return $callStacks;
	}
	
	function getMethodCalls($callStacks) {
		$methodCalls = array();
		foreach ($callStacks as $call) {
			if (MvcEditor_CallStack::METHOD_CALL == $call->type) {
				$methodCalls[$call->destinationVariable] = $call;
			}
		}
		return $methodCalls;
	}
	
	function getPropertyCalls($callStacks) {
		$propertyCalls = array();
		foreach ($callStacks as $call) {
			if (MvcEditor_CallStack::PROPERTY == $call->type) {
				$propertyCalls[$call->destinationVariable] = $call;
			}
		}
		return $propertyCalls;
	}
	
	function getVariables($callStacks) {
		$variableCalls = array();
		$arrayKeys = array();
		foreach ($callStacks as $call) {
			if (MvcEditor_CallStack::T_ARRAY == $call->type 
				|| MvcEditor_CallStack::SCALAR == $call->type
				|| MvcEditor_CallStack::NEW_OBJECT == $call->type
				|| MvcEditor_CallStack::ASSIGN == $call->type) {
				$variableCalls[$call->destinationVariable] = $call;
			}
		}
		return $variableCalls;
	}
	
	function getArrayKeys($callStacks, $variableName) {
		$keys = array();
		foreach ($callStacks as $call) {
			if (MvcEditor_CallStack::ARRAY_KEY == $call->type && \opstring\compare($variableName, $call->destinationVariable) == 0) {
				$keys[$call->arrayKey] = 1;
			}
		}
		return array_keys($keys);
	}
	
	function printScope($callStacks) {
		foreach ($callStacks as $call) {
			if (MvcEditor_Callstack::T_ARRAY == $call->type) {
				echo $call->type, "\t\t\t\t", $call->destinationVariable, "\n";
			}
			if (MvcEditor_Callstack::METHOD_CALL == $call->type) {
				echo $call->type, "\t\t\t\t", $call->destinationVariable, "=", $call->objectName . "->" . $call->methodName . "(" . join(',', $call->functionArguments). ")" , "\n";
			}
			else if (MvcEditor_Callstack::PROPERTY == $call->type) {
				echo $call->type, "\t\t\t\t", $call->destinationVariable, "=", $call->objectName . "->" . $call->propertyName, "\n";
			}
			else if (MvcEditor_Callstack::FUNCTION_CALL == $call->type) {
				echo $call->type, "\t\t\t\t", $call->destinationVariable, "=", $call->functioName . "(" . join(',', $call->functionArguments). ")" , "\n";
			}
			else if (MvcEditor_Callstack::SCALAR == $call->type) {
				echo $call->type, "\t\t\t\t", $call->destinationVariable, ",", $call->scalarValue , "\n";
			}
			else if (MvcEditor_Callstack::ASSIGN == $call->type) {
				echo $call->type, "\t\t\t\t", $call->destinationVariable . "=" . $call->sourceVariable, "\n";
			}
			else if (MvcEditor_Callstack::ARRAY_KEY == $call->type) {
				echo $call->type, "\t\t\t\t", $call->destinationVariable . "[" . $call->arrayKey . "]", "\n";
			}
			else if (MvcEditor_Callstack::NEW_OBJECT == $call->type) {
				echo $call->type, "\t\t\t\t", $call->destinationVariable . "=new " . $call->className . "()", "\n";
			}
		}
	}
}