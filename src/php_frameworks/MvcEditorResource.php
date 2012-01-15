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
 * This is a dynamic resource artifact.  The MVC Editor will use a list of resources to aid
 * in code completion.  The PHP code will only augment what the MVC Editor does; MVC Editor will already
 * gather resources the source code (classes, methods, functions) and will gather resource types
 * from the PHPDoc comments; but sometimes some resources may not be are created at run-time and the
 * editor cannot gather them. In this case, the PHP code can tell the editor the what those dynamic resources
 * are.
 */
class MvcEditorResource {

	/**
	 * This is the fully qualified name of a resource. This means that it includes a class name followed by
	 * the method name, with a "::" in the middle. For example 'MY_Email::read'
	 */
	public $resource;
	
	/**
	 * The lexeme of the resource; the identifier name only. For example, if the resource was 'MY_Email::read'
	 * then the identifer would be 'read'
	 */
	public $identifier;
	
	/**
	 * The fully qualified name of the type of this resource. This means that it includes a class name followed by
	 * the method name, with a "::" in the middle. For example 'MY_Email::read'
	 */
	public $returnType;
	
	/**
	 * For a class property; signature will just be the same as resource. For a method  
	 */
	public $signature;
	
	/**
	 * This will be shown in the tool tip when a user hovers on one of the instances of this resource.
	 */
	public $comment;
	
	/**
	 * Create a property resource(a class member). For now this property is assumed to have public access.
	 * @return MvcEditorResource
	 */
	public static function MakeProperty($className, $propertyName, $propertyType, $comment) {
		$resource = $className . '::' . $propertyName;
		$ret = new MvcEditorResource($resource, $propertyName, $propertyType, $resource, $comment);
		return $ret;
	}
	
	/**
	 * Create a class method. For now this method is assumed to have public access.
	 * @return MvcEditorResource
	 */
	public static function MakeMethod($className, $methodName, $methodArgs, $methodReturnType, $comment) {
		$resource = $className . '::' . $methodName;
		$signature = 'public function ' . $methodName . '(' . $methodArgs . ')';
		$ret = new MvcEditorResource($resource, $methodName, $methodReturnType, $signature, $comment);
		return $ret;
	}
	
	private function __construct($resource, $identifier, $returnType, $signature, $comment) {
		$this->resource = $resource;
		$this->identifier = $identifier;
		$this->returnType = $returnType;
		$this->signature = $signature;
		$this->comment = $comment;
	}
}