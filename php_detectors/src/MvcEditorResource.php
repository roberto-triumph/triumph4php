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
require_once __DIR__ . '/../lib/opportunity/string.php';

set_include_path(get_include_path() . PATH_SEPARATOR .  realpath(__DIR__ . '/../lib/'));
require_once 'Zend/Db/Table.php';

/**
 * This is a resource artifact.  The MVC Editor will use a list of resources to aid
 * in code completion.  The PHP code will only augment what the MVC Editor does; MVC Editor will already
 * gather resources the source code (classes, methods, functions) and will gather resource types
 * from the PHPDoc comments; but sometimes some resources may not be are created at run-time and the
 * editor cannot gather them. In this case, the PHP code can tell the editor the what those dynamic resources
 * are.
 */
class MvcEditorResource {

	protected $_name = 'resources';
	
	protected $_sequence = FALSE;

	/**
	 * This is the fully qualified name of a resource. This means that it includes a class name followed by
	 * the method name, with a "::" in the middle. For example 'MY_Email::read'
	 * @var string
	 */
	public $resource;
	
	/**
	 * The lexeme of the resource; the identifier name only. For example, if the resource was 'MY_Email::read'
	 * then the identifer would be 'read'
	 * @var string
	 */
	public $identifier;
	
	/**
	 * The class of the resource; the class name only. For example, if the resource was 'MY_Email::read'
	 * then the identifer would be 'MY_Email'
	 * @var string
	 */
	public $className;
	
	/**
	 * The fully qualified name of the type of this resource. This means that it includes a class name followed by
	 * the method name, with a "::" in the middle. For example 'MY_Email::read'
	 * @var string
	 */
	public $returnType;
	
	/**
	 * For a class property; signature will just be the same as resource. For a method or functions, this will have the 
	 * function arguments.
	 * @var string
	 */
	public $signature;
	
	/**
	 * This will be shown in the tool tip when a user hovers on one of the instances of this resource.
	 * @var string
	 */
	public $comment;
	
	/**
	 * The full path to the file where this resource is located in. The path is OS-dependant (may contain
	 * forward or backslashes).
	 * @var string
	 */
	public $fullPath;
	
	/**
	 * The type of this resource. One of the type constants.
	 */
	public $type;
	
	const TYPE_CLASS = 0;
	const TYPE_METHOD = 1;
	const TYPE_FUNCTION = 2;
	const TYPE_MEMBER = 3;
	const TYPE_DEFINE = 4;
	const TYPE_CLASS_CONSTANT = 5;
	
	/**
	 * Create a property resource(a class member). For now this property is assumed to have public access.
	 */
	public static function CreateMember($className, $propertyName, $propertyType, $comment) {
		$member = new MvcEditorResource();
		$member->MakeMember($className, $propertyName, $propertyType, $comment);
		return $member;
	}
	
	/**
	 * Create a method resource(a class member). For now this method is assumed to have public access.
	 */
	public static function CreateMethod($className, $methodName, $methodArgs, $methodReturnType, $comment) {
		$method = new MvcEditorResource();
		$method->MakeMethod($className, $methodName, $methodArgs, $methodReturnType, $comment);
		return $method;
	}
	
	/**
	 * create a filled or empty instance. If creating an empty instance, the MakeXXX methods will be useful.
	 */
	public function __construct($resource = '', $identifier = '', $returnType = '', $signature = '', $comment = '', $type = '') {
		$this->resource = $resource;
		$this->identifier = $identifier;
		$this->returnType = $returnType;
		$this->signature = $signature;
		$this->comment = $comment;
		$this->type = $type;
		$this->fullPath = '';
		$this->className = '';
	}

	/**
	 * Create a property resource(a class member). For now this property is assumed to have public access.
	 */
	public function MakeMember($className, $propertyName, $propertyType, $comment) {
		$resource = $className . '::' . $propertyName;
		$this->className = $className;
		$this->resource = $resource;
		$this->identifier = $propertyName;
		$this->returnType = $propertyType;
		$this->signature = $resource;
		$this->comment = $comment;
		$this->type = self::TYPE_MEMBER;
	}
	
	/**
	 * Create a class method. For now this method is assumed to have public access.
	 */
	public function MakeMethod($className, $methodName, $methodArgs, $methodReturnType, $comment) {
		$resource = $className . '::' . $methodName;
		$signature = 'public function ' . $methodName . '(' . $methodArgs . ')';
		$this->resource = $resource;
		$this->className = $className;
		$this->identifier = $methodName;
		$this->returnType = $methodReturnType;
		$this->signature = $signature;
		$this->comment = $comment;
		$this->type = self::TYPE_METHOD;
	}
	
	/**
	 * Sets the properties so that this resource is a class.
	 * @return void
	 */
	public function MakeClass($className) {
		$resource = $className;
		$signature = 'class ' . $className;
 		$this->resource = $resource;
		$this->identifier = $className;
		$this->returnType = '';
		$this->signature = $resource;
		$this->comment = '';
		$this->type = self::TYPE_CLASS;
	}
	
	/**
	 * Sets the properties so that this resource is a function.
	 * @return void
	 */
	public function MakeFunction($functionName, $functionArgs) {
		$resource = $functionName;
		$signature = 'function ' . $functionName . '(' . $functionArgs . ')';
		$this->resource = $resource;
		$this->identifier = $functionName;
		$this->returnType = '';
 		$this->signature = $signature;
		$this->comment = '';
		$this->type = self::TYPE_FUNCTION;
	}
	
	public function Clear() {
		$this->resource = '';
		$this->identifier = '';
		$this->returnType = '';
		$this->signature = '';
		$this->comment = '';
		$this->type = '';
	}
	
	/**
	 * Retrieves all of the methods from the given files. Only resources that 
	 * are methods will be returned.
	 *
	 * @param Zend_Db_Adapter_Abstract $db the DB to query
	 * @param int[] $fileItemIds the file item IDs to query
	 * @return MvcEditorResource[] the resources that were found in any of the given files.
	 */
	public function MethodsFromFiles(Zend_Db_Adapter_Abstract $db, $fileItemIds) {
		if (empty($fileItemIds)) {
			return array();
		}
				
		// key = identifier ==> don't get the fully qualified matches
		$select = $db->select();
		$select->from(array('r' => 'resources'), array('class_name', 'identifier', 'signature', 'return_type', 'comment'))
			->join(array('f' => 'file_items'), 'r.file_item_id = f.file_item_id', array('full_path'));
		$select->where("r.file_item_id IN(?)", $fileItemIds)->where('key = identifier')->where('type = ?', self::TYPE_METHOD);
		$stmt = $db->query($select);
		$methods = array();
		while ($row = $stmt->fetch()) {
			$method = new MvcEditorResource();
			$method->MakeMethod($row['class_name'], $row['identifier'], $row['signature'], $row['return_type'], $row['comment']);
			$method->fullPath = $row['full_path'];
			$methods[] = $method;
		}
		return $methods;
 	}
	
	public static function typeString($type) {
		if (self::TYPE_CLASS == $type) {
			return "CLASS";
		}
		else if (self::TYPE_CLASS_CONSTANT == $type) {
			return "CLASS_CONSTANT";
		}
		else if (self::TYPE_DEFINE == $type) {
			return "DEFINE";
		}
		else if (self::TYPE_FUNCTION == $type) {
			return "FUNCTION";
		}
		else if (self::TYPE_MEMBER == $type) {
			return "MEMBER";
		}
		else if (self::TYPE_METHOD == $type) {
			return "METHOD";
		}
	}
}