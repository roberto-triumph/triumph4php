[Table Of Contents](/#toc) | [Next - Debugger](/debugger/)


# PHP Code Completion #
triumph4php boasts a powerful PHP code completion system.  It is capable of completing 

  * PHP keywords
  * PHP built-in functions, classes, methods, and defines
  * User-created functions, classes, methods, and defines
  * Namespaces

triumph4php code completion is a bit different than most IDEs in that it does not 
attempt to hide its internals; when code completion is triggered triumph4php 
will show you any errors in the bottom status bar. You can then see why 
code completion failed to show you any suggestions.

##Dynamic Code Completion##
triumph4php code completion is also a bit different than most IDEs in that it 
will not bail out if it could not determine a variable's type; triumph4php will 
instead show you any matching methods from all classes. It was decided to do 
this because most of the time, PHP code is not always properly documented and 
it hinders code completion in other IDEs. This behavior is called 
"Dynamic Code completion" and can be toggled on or off in the Preferences -> Editor 
Behavior dialog.

**Note: Dynamic code completion is off by default.**

##Help! PHP code completion fails to give me any suggestions!##
  * PHP code completion for user-created classes, methods, and functions only 
    work on when you create and enable a [project](/projects/). 
  * Class, method, function names will be parsed out of the code.  This means that 
	if a file has PHP sytax errors, the class, method, or function might not
	be able to be parsed out
  * When code completion fails, look at the status bar, it will contain
    a small message describing why there are no suggestions

## Type Resolution##
triumph4php is able to determine type (and have 'proper' code completion) in the following cases:

###Code completion on local variables###

	$var = new PDO('localhost');
	$var-> // autocompletion works
	// ...
	$var = new PDO('localhost');
	$cpy = $var;
	$cpy-> // autocompletion works

###Code completion on function type hints###

	function myFunct(PDO $pdo) {
		$pdo-> // autocompletion works
	}

###Code completion on functions documented with PHPDoc###
	
	/** @return PDO */
	function myFunct() { }
	// ...
	$pdo = myFunct();
	$pdo-> // autocompletion works

###Code completion on class members documented with PHPDoc###

	class MyClass {
		/** @var PDO */
		$pdo;
	}
	// ...
	$my = new MyClass();
	$my->pdo-> // autocompletion works

###Code completion with proper static checks###
	
	class MyClass {
		static $MAX;
		const MIN = 1;
		public $value;
	}
	// ...
	$my = new MyClass();
	$my-> // autocompletion will show MAX, MIN, and $value
	MyClass:: // autocompletion will NOT show $value since 
	          // it cannot be accessed statically

###Code completion with proper access (public vs. private) checks###

	class MyClass {
		private $MAX;
		function __construct() {
			$this-> // autocompletion will show MAX since MAX can be accessed inside of class
		}
	}
	// ...
	$my = new MyClass();
	$my-> // autocompletion won't show MAX since MAX is private

###Code completion with method chains###
	
	class MyClass {
		/** @return MyClass */
		function parent() { /* */}
	}
	// ... 
	$my = new MyClass();
	$my->parent()-> // autocompletion will show methods of MyClass since parent is documented

###Code completion with dynamic autocompletion enabled###

	class MyClass {
		function parent() { /* \*/}
	}
	// ... 
	$my = new MyClass();
	$my->parent()->pare // autocompletion will show methods that start with "pare" since dynamic autocompletion is enabled

###Code completion on variables documented with PHPDoc###
	
	/** @var my MyClass */
	$my->// autocompletion will show methods of MyClass since $my is documented


[Table Of Contents](/#toc) | [Next - Debugger](/debugger/)