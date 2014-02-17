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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */

/**
 * This is a script to create an SQLite database of the PHP native functions.  This 
 * 
 * Requirements:
 * This script requires as input the php tags  file that is created from the PHP documentation.
 * The tag file can be created with the PhD program; available at git://github.com/robertop/phd.git

 */

// to use Zend Console
set_include_path(get_include_path() . PATH_SEPARATOR .  realpath(__DIR__ . '/../php_detectors/lib/'));
require_once 'Zend/Console/Getopt.php';

$rules = array(
	'tags_file|t=s' => 'The location of the tags file to read in',
	'phd_db_file|p=s' => 'The location of the SQLite DB file generated by PhD',
	'triumph_db_file|o=s' => 'The location of the SQLite DB file where resources will be written to',
	'help|h' => 'A help message'
);
$opts = new Zend_Console_Getopt($rules);
$tagsFile = $opts->getOption('tags_file');
$phdDbFile = $opts->getOption('phd_db_file');
$dbFile = $opts->getOption('triumph_db_file');
$help = $opts->getOption('help');

if ($help) {
	echo <<<EOF

This is a script to create an SQLite database of the PHP native functions.  Thie script
takes in a TAGS file and writes all tags to the SQLite database. Triumph then uses
the SQLite database for autocompletion, call tips for all PHP native functions.
 
Requirements:
This script requires as input the php TAGS file AND the SQLite files that are created 
from the PHP documentation. The TAGS file can be created with the PhD program; a local 
fork has been created that has the ability to output a TAGS file from the PHP 
documentation. The PhD fork is available at git://github.com/robertop/phd.git
The SQLite file can be created with the same PhD program, just need to specify
a different output format.

The reason Triumph does not use the PhD SQLite file is because it has a different
format (tables) than what is required. Triumph resource database can 
contain class members too, as well as info about access modifiers. We use the
PhD SQLite file to get the user-created documentation for the methods.

Program 
EOF;
	
	echo $opts->getUsageMessage();
	exit(0);
}
if (strlen(trim($tagsFile)) == 0) {
	echo "Missing argument: --tags_file.  See --help for more info";
	exit(-1);
}
if (strlen(trim($dbFile)) == 0) {
	echo "Missing argument: --triumph_db_file.  See --help for more info";
	exit(-1);
}
if (strlen(trim($phdDbFile)) == 0) {
	echo "Missing argument: --phd_db_file.  See --help for more info";
	exit(-1);
}


// these are analogous to the C++ source code
// see t4p::PhpTagClass::Types
define('RESOURCE_CLASS', 0);
define('RESOURCE_METHOD', 1);
define('RESOURCE_FUNCTION', 2);
define('RESOURCE_MEMBER', 3);
define('RESOURCE_DEFINE', 4);
define('RESOURCE_CLASS_CONSTANT', 5);
define('RESOURCE_NAMESPACE', 6);

$outputConnString = "sqlite:{$dbFile}";
$outputPdo = new PDO($outputConnString);

$phdConnString = "sqlite:{$phdDbFile}";
$phdPdo = new PDO($phdConnString);

initOutputPdo($outputPdo);
$outputPdo->beginTransaction();
$insertStmt = $outputPdo->prepare('INSERT INTO resources(' .
	'file_item_id, source_id, key, identifier, class_name, type, namespace_name, ' .
	'signature, comment, return_type, is_protected, is_private, is_static, ' .
	'is_dynamic, is_native) VALUES(' .
	'?, 0, ?, ?, ?, ?, ?, ' .
	'?, ?, ?, ?, ?, ?, ' .
	'?, ?)'
);
$commentStatements = prepStatements($phdPdo);
$if = fopen($tagsFile, 'rb');
if ($if) {
	$lineCount = 0;	
	while (!feof($if)) {
		$line = fgets($if);
		if (strlen($line) > 0 && $line[0] == '!') {
		
			// skip comment lines
			continue;
		}
		$lineCount++;
		if ($lineCount % 1000 == 0) {
			echo "processed $lineCount lines\n";
		}
		
		// summary of tag format
		// column 1 : identifier
		// column 2: file name  (location of tag) - currently not used because we are using tags files
		//           for native resources, and user will never be able to jump to the native resources
		// column 3: line number (+ VIM comment) -  currently not used because we are using tags files
		//           for native resources, and user will never be able to jump to the native resources
		// column 4-n: extension fields
		$columns = explode("\t", $line);
		$extensions = buildExtensions($columns);
		$identifier = $columns[0];
		
		$fileItemId = -1;
		$key = $identifier; 
		$className = ''; 
		$type = ''; 
		$namespaceName = '\\'; // the default namespace
		$signature = ''; 
		$comment = findComment($phdPdo, $identifier, $extensions, $commentStatements); 
		$returnType = '';
		$isProtected = 0;
		$isPrivate = 0;
		$isStatic = 0; 
		$isDynamic = 0;
		$isNative = 1;
		
		$kind = strtolower($extensions['kind']);
		if ('c' == $kind) {
			$className = $identifier;
			$type = RESOURCE_CLASS;
		}
		else if ('d' == $kind) {
			$type = RESOURCE_DEFINE;
		}
		else if ('f' == $kind && strlen($extensions['class']) == 0) {
			$type = RESOURCE_FUNCTION;
			$signature = $extensions['signature'];
			
			// return type is in the signature before the function keyword
			$pos = strpos($signature, 'function');
			if ($pos !== FALSE) {
				$returnType = trim(substr($signature, 0, $pos));
			}
		}
		else if ('f' == $kind && strlen($extensions['class']) > 0) {
			$type = RESOURCE_METHOD;
			$className = $extensions['class'];
			$signature = $extensions['signature'];
			
			// return type is in the signature before the function keyword
			$pos = strpos($signature, 'function');
			if ($pos !== FALSE) {
				$returnType = trim(substr($signature, 0, $pos));
			}
		}
		else if ('o' == $kind) {
			$type = RESOURCE_CLASS_CONSTANT;
			$className = $extensions['class'];
		}
		else if ('p' == $kind) {
			$type = RESOURCE_MEMBER;
			$className = $extensions['class'];
		}
		// 'v' kind super global variables there is no type for them...
		
		$isProtected = stripos($extensions['access'], 'protected') !== FALSE ? 1 : 0;
		$isPrivate = stripos($extensions['access'], 'private') !== FALSE ? 1 : 0;		
		$isStatic = stripos($extensions['access'], 'static') !== FALSE ? 1 : 0;
		
		$insertStmt->execute(array(
			$fileItemId, $key, $identifier, $className, $type, $namespaceName,
			$signature, $comment, $returnType, $isProtected, $isPrivate, $isStatic, 
			$isDynamic, $isNative
		));
		if (RESOURCE_METHOD == $type || RESOURCE_MEMBER == $type || RESOURCE_CLASS_CONSTANT == $type) {
		
			// insert the 'fully qualified' form
			$key = $className . '::' . $identifier;
			$insertStmt->execute(array(
				$fileItemId, $key, $identifier, $className, $type, $namespaceName,
				$signature, $comment, $returnType, $isProtected, $isPrivate, $isStatic, 
				$isDynamic, $isNative
			));
		}
		if (RESOURCE_CLASS == $type || RESOURCE_FUNCTION == $type || RESOURCE_NAMESPACE == $type) {
		
			// insert the 'fully qualified' namespace form
			$key = $namespaceName . $identifier;
			$identifier = $key;
			$insertStmt->execute(array(
				$fileItemId, $key, $identifier, $className, $type, $namespaceName,
				$signature, $comment, $returnType, $isProtected, $isPrivate, $isStatic, 
				$isDynamic, $isNative
			));
		}
	}
	fclose($if);
	$outputPdo->commit();
}
else {
	echo "Could not open tags file:{$inputTags}\n";
}
echo "done\n";

/**
 * creates the triumph resource database and creates the tables if necessary.
 */
function initOutputPdo(PDO $outputPdo) {
	// TODO: this does not work; PDO cannot execute multiple queries
	// at once.
	$outputPdo->query(file_get_contents(__DIR__ . './sql/resources.sql'));
	
	// delete any existing resources, since the generated file will only 
	// contain the native functions it is safe to truncate the tables
	$outputPdo->query(
		'DELETE FROM file_items'
	);
	$outputPdo->query(
		'DELETE FROM resources'
	);
	
}

/**
 * parses a column of a single tags file row into a map that has
 * one key per extension. Returned array will always be the full
 * name of the extension.
 *
 * @param $columns exploded line of tags file
 * @return array associative array 
 */
function buildExtensions($columns) {
	$extensions = array(
		'kind' => '',
		'class' => '',
		'access' => '',
		'inheritance' => '',
		'implementation' => '',
		'signature' => ''
	);
	for ($i = 3; $i < count($columns); $i++) {
		$ext = $columns[$i];
		$arrExt = array();
		
		// each extension is key:value
		// but make sure that any ':' in the value is not used
		$colonPos = strpos($ext, ':');
		if ($colonPos !== FALSE) {
			$arrExt[0] = substr($ext, 0, $colonPos);
			$arrExt[1] = substr($ext, $colonPos + 1);
		}
		else {
			$arrExt[0] = $ext;
		}
		if (count($arrExt) == 2) {
			$extensionKey = $arrExt[0];
			switch($extensionKey) {
				case 'k':
					$extensionKey = 'kind';
					break;
				case 'a':
					$extensionKey = 'access';
					break;
				case 'k':
					$extensionKey = 'kind';
					break;
				case 'i':
					$extensionKey = 'inheritance';
					break;
				case 'm':
					$extensionKey = 'implementation';
					break;
				case 'S':
					$extensionKey = 'signature';
					break;
				default:
					$extensionKey = trim($arrExt[0]);
			}
			$extensions[$extensionKey] = trim($arrExt[1]); 
		}
		else if (count($arrExt) == 1) {
		
			// if no key, then default to 'kind' extension
			$extensions['kind'] = trim($arrExt[0]); 
		}
	}
	return $extensions;
}

/**
 * prepares the SQL statements needed to query for comments from
 * the PhD database.
 *
 * @parap PDO $phdPdo the connection to the PhD SQLite database
 * @return array of prepared statements that read the PhD SQLite file
 */
function prepStatements(PDO $phdPdo) {
	$changelogStmt = $phdPdo->prepare('SELECT version, change FROM changelogs WHERE function_name = ?');
	$functionStmt = $phdPdo->prepare('SELECT purpose, return_type, return_description FROM functions WHERE name = ?');
	$notesStmt = $phdPdo->prepare('SELECT type, description FROM notes WHERE function_name = ?');
	$paramsStmt = $phdPdo->prepare('SELECT name, type, description, optional, initializer FROM params WHERE function_name = ?');
	$seeAlsoStmt = $phdPdo->prepare('SELECT name, type, description FROM seealso WHERE function_name = ?');
	return array(
		'changelog' => $changelogStmt,
		'function' => $functionStmt,
		'notes' => $notesStmt,
		'params' => $paramsStmt,
		'seealso' => $seeAlsoStmt
	);
}

/**
 * Build the comment for the given tag.
 *
 * @param PDO $phdPdo the connection to the PhD database
 * @param string $identifier the function name to look for
 * @param array $extensions the tag extension; to get the class name if any
 * @param array $commentStatements the prepared statements
 * @return string the full comment
 */
function findComment(PDO $phdPdo, $identifier, $extensions, $commentStatements) {
	$name = $identifier;
	if ($extensions['class']) {
		$name = $extensions['class'] . '.' . $identifier;
	}
	//echo "function_name=$name\n";
	$changelogStmt = $commentStatements['changelog'];
	$functionStmt = $commentStatements['function'];
	$notesStmt = $commentStatements['notes'];
	$paramsStmt = $commentStatements['params'];
	$seeAlsoStmt = $commentStatements['seealso'];
	
	$comment = "";
	
	if ($functionStmt->execute(array($name))) {
		$function = $functionStmt->fetch();
		$functionBlock = '';
		if (strlen($function['purpose'])) {
			$functionBlock .= clean($function['purpose']);
		}
		if (strlen($function['return_type'])) {
			$functionBlock .= "\nReturns: " . clean($function['return_type']) . "\n";
		}
		if (strlen($function['return_description'])) {
			$functionBlock .= clean($function['return_description']);
			$functionBlock .= "\n";
		}		
		if (strlen($functionBlock)) {
			$comment .= $functionBlock . "\n";
		}
	}
	if ($paramsStmt->execute(array($name))) {
		$paramBlock = "";
		while ($param = $paramsStmt->fetch()) {
			$paramBlock .= clean($param['name']) . " [" . clean($param['type']) . "] " . clean($param['description']) . "\n";
		}
		if (strlen($paramBlock)) {
			$comment .= "\nParameters\n" . $paramBlock . "\n";
		}		
	}
	if ($notesStmt->execute(array($name))) {
		$notesBlock = '';
		while ($note = $notesStmt->fetch()) {
			$notesBlock.= clean($note['type']) . ": " . clean($note['description']);
			$notesBlock .= "\n";
		}
		if (strlen($notesBlock)) {
			$comment .= $notesBlock . "\n";
		}
	}
	if ($seeAlsoStmt->execute(array($name))) {
		$seeAlsoBlock = '';
		while ($see = $seeAlsoStmt->fetch()) {
			$seeAlsoBlock .= "@see " . clean($see['name']);
			if (strlen($see['type'])) {
				$seeAlsoBlock .= ' ' . clean($see['type']);
			}
			if (strlen($see['description'])) {
				$seeAlsoBlock .= ':' . clean($see['description']) . "\n";
			}
			else {
				$seeAlsoBlock .= "\n";
			}
		}
		if (strlen($seeAlsoBlock)) {
			$comment .= "See Also:\n" . $seeAlsoBlock . "\n";
		}
	}
	if ($changelogStmt->execute(array($name))) {
		$changelogBlock = '';
		while ($changelog = $changelogStmt->fetch()) {
			if (strlen($changelog['version'])) {
				$changelogBlock .= clean($changelog['version']) . "\n" ;
			}
			if (strlen($changelog['change'])) {
				$changelogBlock .= clean($changelog['change']) . "\n\n";
			}
		}
		if (strlen($changelogBlock)) {
			$comment .= "Changelog\n" . $changelogBlock . "\n";
		}
	}
	if (strlen($comment)) {
		$comment = "/**\n" . $comment . "*/";
	}
	return $comment;
}

/**
 * PHP documentation has some HTML. we want to ignore the tags for now.
 * @return the string, but with no HTML tags.
 */
function clean($str) {

	// after the tags are stripped, remove spaces too
	return trim(strip_tags($str));
}
