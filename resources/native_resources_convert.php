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
	'db_file|d=s' => 'The location of the SQLite DB file where resources will be written to',
	'help|h' => 'A help message'
);
$opts = new Zend_Console_Getopt($rules);
$tagsFile = $opts->getOption('tags_file');
$dbFile = $opts->getOption('db_file');
$help = $opts->getOption('help');

if ($help) {
	echo <<<EOF

This is a script to create an SQLite database of the PHP native functions.  Thie script
takes in a TAGS file and writes all tags to the SQLite database. MVC Editor then uses
the SQLite database for autocompletion, call tips for all PHP native functions.
 
Requirements:
This script requires as input the php TAGS file that is created from the PHP documentation.
The TAGS file can be created with the PhD program; a local fork has been created that has 
the ability to output a TAGS file from the PHP documentation. The PhD fork is available 
at git://github.com/robertop/phd.git

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
	echo "Missing argument: --db _file.  See --help for more info";
	exit(-1);
}


// these are analogous to the C++ source code
// see mvceditor::ResourceClass::Types
define('RESOURCE_CLASS', 0);
define('RESOURCE_METHOD', 1);
define('RESOURCE_FUNCTION', 2);
define('RESOURCE_MEMBER', 3);
define('RESOURCE_DEFINE', 4);
define('RESOURCE_CLASS_CONSTANT', 5);
define('RESOURCE_NAMESPACE', 6);

$outputConnString = "sqlite:{$dbFile}";
$outputPdo = new PDO($outputConnString);

initOutputPdo($outputPdo);
$outputPdo->beginTransaction();
$insertStmt = $outputPdo->prepare('INSERT INTO resources(' .
	'file_item_id, key, identifier, class_name, type, namespace_name, ' .
	'signature, comment, return_type, is_protected, is_private, is_static, ' .
	'is_dynamic, is_native) VALUES(' .
	'?, ?, ?, ?, ?, ?, ' .
	'?, ?, ?, ?, ?, ?, ' .
	'?, ?)'
);

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
		$comment = '';  // TODO: fill this in with the comments from the PhD output
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
		$isProtected = stripos($extensions['access'], 'static') !== FALSE ? 1 : 0;
		
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

function initOutputPdo(PDO $outputPdo) {
	$outputPdo->query(
		'CREATE TABLE IF NOT EXISTS file_items ( ' .
		'  file_item_id INTEGER PRIMARY KEY, full_path TEXT, last_modified DATETIME, is_parsed INTEGER, is_new INTEGER ' .
		')'
	);
	$outputPdo->query(
		'CREATE TABLE IF NOT EXISTS resources ( ' .
		'  file_item_id INTEGER, key TEXT, identifier TEXT, class_name TEXT, ' .
		'  type INTEGER, namespace_name TEXT, signature TEXT, comment TEXT, ' .
		'  return_type TEXT, is_protected INTEGER, is_private INTEGER, ' .
		'  is_static INTEGER, is_dynamic INTEGER, is_native INTEGER ' .
		')'
	);
			
	$outputPdo->query(
		'CREATE UNIQUE INDEX IF NOT EXISTS idxFullPath ON file_items(full_path)'
	);
	$outputPdo->query(
		'CREATE INDEX IF NOT EXISTS idxResourceKey ON resources(key, type)'
	);
	$outputPdo->query(
		'DELETE FROM file_items'
	);
	$outputPdo->query(
		'DELETE FROM resources'
	);
	
}

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