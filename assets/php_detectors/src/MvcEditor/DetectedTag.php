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
 * A detected tag is a tag artifact.  The MVC Editor will use the list of detected tags to aid
 * in code completion.  The tag detectors will only augment what the MVC Editor does; MVC Editor will already
 * gather tags the source code (classes, methods, functions) and will gather tag types
 * from the PHPDoc comments; but sometimes some tags may not be caught because they are created 
 * at run-time and with variable names. In this case, the PHP tag detectors can tell the editor 
 * what those dynamic resources are.
 */
class MvcEditor_DetectedTag {

	/**
	 * The lexeme of the tag; the identifier name only. For example, if the resource was 'MY_Email::read'
	 * then the identifer would be 'read'
	 * @var string
	 */
	public $identifier;
	
	/**
	 * The class of the tag; the class name only. For example, if the resource was 'MY_Email::read'
	 * then the identifer would be 'MY_Email'
	 * @var string
	 */
	public $className;
	
	/**
	 * The fully qualified namespace name of the tag; For example, if the resource was '\Zend\Email::read'
	 * then the identifer would be '\Zend'
	 * @var string
	 */
	public $namespaceName;
	
	/**
	 * The fully qualified name of the return type of this tag. If the return type is a class
	 * it should contain the fully qualified namespace name. 
	 * @var string
	 */
	public $returnType;
	
	/**
	 * A friendly message that is displayed to the user when this tag is shown.
	 * @var string
	 */
	public $comment;
		
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
	public static function CreateMember($className, $propertyName, $propertyType, $namespaceName = '\\', $comment = '') {
		$member = new MvcEditor_DetectedTag();
		$member->MakeMember($className, $propertyName, $propertyType, $namespaceName, $comment);
		return $member;
	}
	
	/**
	 * Create a method resource(a class member). For now this method is assumed to have public access.
	 */
	public static function CreateMethod($className, $methodName, $methodReturnType, $namespaceName = '\\', $comment = '') {
		$method = new MvcEditor_DetectedTag();
		$method->MakeMethod($className, $methodName, $methodReturnType, $namespaceName, $comment);
		return $method;
	}
	
	/**
	 * create a filled or empty instance. If creating an empty instance, the MakeXXX methods will be useful.
	 */
	public function __construct($resource = '', $identifier = '', $returnType = '', $type = '', $namespaceName = '\\', $comment = '') {
		$this->resource = $resource;
		$this->identifier = $identifier;
		$this->returnType = $returnType;
		$this->type = $type;
		$this->className = '';
		$this->namespaceName = $namespaceName;
		$this->comment = $comment;
	}

	/**
	 * Create a property resource(a class member). For now this property is assumed to have public access.
	 */
	public function MakeMember($className, $propertyName, $propertyType, $namespaceName, $comment) {
		$this->className = $className;
		$this->identifier = $propertyName;
		$this->returnType = $propertyType;
		$this->namespaceName = $namespaceName;
		$this->comment = $comment;
		$this->type = self::TYPE_MEMBER;
	}
	
	/**
	 * Create a class method. For now this method is assumed to have public access.
	 */
	public function MakeMethod($className, $methodName, $methodReturnType, $namespaceName, $comment) {
		$this->className = $className;
		$this->identifier = $methodName;
		$this->returnType = $methodReturnType;
		$this->namespaceName = $namespaceName;
		$this->comment = $comment;
		$this->type = self::TYPE_METHOD;
	}
	
	/**
	 * Sets the properties so that this resource is a function.
	 * @return void
	 */
	public function MakeFunction($functionName, $returnType, $namespaceName, $comment) {
		$resource = $functionName;
		$this->identifier = $functionName;
		$this->returnType = $returnType;
		$this->namespaceName = $namespaceName;
		$this->type = self::TYPE_FUNCTION;
	}
	
	public function Clear() {
		$this->identifier = '';
		$this->returnType = '';
		$this->comment = '';
		$this->type = '';
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