#! /usr/bin/php
<?php

/**
 * This script will download the PHP documentation, extract it, parse it, and save it into PHP source code format.
 *
 * sample HTML to parse
 *
 * <div id="function.strnatcasecmp" class="refentry"><hr />
 * <div class="refnamediv">
 *   <h1 class="refname">strnatcasecmp</h1>
 *   <p class="verinfo">(PHP 4, PHP 5)</p><p class="refpurpose"><span class="refname">strnatcasecmp</span> &mdash; <span class="dc-title">Case insensitive string comparisons using a &quot;natural order&quot; algorithm</span></p>
 * </div>
 * 
 *  <a name="function.strnatcasecmp.description"></a><div class="refsect1 description">
 *  <h3 class="title">Description</h3>
 *   <div class="methodsynopsis dc-description">
 *    <span class="type">int</span> <span class="methodname"><b>strnatcasecmp</b></span>
 *     ( <span class="methodparam"><span class="type">string</span> <tt class="parameter">$str1</tt></span>
 *    , <span class="methodparam"><span class="type">string</span> <tt class="parameter">$str2</tt></span>
 *    )</div>
 * 
 *   <p class="para rdfs-comment">
 *    This function implements a comparison algorithm that orders alphanumeric
 *    strings in the way a human being would.  The behaviour of this function is
 *    similar to <a href="#function.strnatcmp" class="function">strnatcmp()</a>, except that the comparison is
 *    not case sensitive.  For more information see: Martin Pool&#039;s <a href="http://sourcefrog.net/projects/natsort/" class="link external">&raquo; Natural Order String Comparison</a> page.
 *   </p>
 *  </div>
 */

/*
 * When CHECK_SYNTAX is TRUE, the output PHP file wil be run through a PHP lint check (php -l) so that we can assert that the generated file 
 * is correct.
 */
define('CHECK_SYNTAX', FALSE);

/*
 * name of the downloaded PHP doc file, after uncompressing
 */
define('PHP_MANUAL_FILE_NAME', 'php_manual_en.html');

/*
 * name of the downloaded PHP doc file
 */
define('PHP_MANUAL_COMPRESSED_FILE_NAME', PHP_MANUAL_FILE_NAME . '.gz');

/*
 * name of the file that the PHP code will be written to
 */
define('OUTPUT_FILE_NAME', 'native.php');

define('DO_PRINT', FALSE);

/*
 * in order to be able to run the generated code through a lint check, we need to generate proper PHP. oddly enough, some
 * extensions don't adhere to correct PHP and this function is meant to correct this.
 */
function identifier($name) {
	if (CHECK_SYNTAX) {
		if ('clone' == $name || 'isSet' == $name) {
			$name = 'y' . $name;
		}
		//some documentation has dashes in some class names (for example OCI-Collection, but they are not valid identifers
		$name = str_replace('-', '_', $name);
		$name = str_replace('$8bit', '$eightBit', $name);
	}
	return $name;
}

function getContents($file) {
	$nextFunction = FALSE;
	$contents = '';
	while (!$nextFunction) {
		$line = fgets($file);
		$contents .= trim($line) . ' ';
		$nextFunction = FALSE !== stripos($line, '</div><hr />');
	}
	$contents = str_replace('&nbsp;', ' ', $contents);
	$contents = html_entity_decode($contents);
	return $contents;
}

function parseName($contents) {
	$name = '';
	$matches = array();
	if (preg_match('/<h1 class="refname">([^<]+)<\/h1>/', $contents, $matches)) {
		$name = str_replace('\t', ' ', "{$matches[1]}");
		$name = str_replace('->', '::', $name);
		$name = identifier($name);
		
	}
	return $name;
}

function parseVersion($contents) {
	$matches = array();
	$version = '';
	if (preg_match('/<p class="verinfo">\(([^(]+)\)<\/p>/', $contents, $matches)) {
		$version = str_replace('\t', ' ', $matches[1]);
	}
	return $version;
}

function parseDescription($contents, $resourceName, $isMethod) {
	$indent = $isMethod ? "\t" : "";
	$matches = array();
	$description = '';
	if (preg_match('/<span class="dc-title">(.+)(<\/span><\/p>)?/', $contents, $matches)) {
		$description =  str_replace('\t', ' ', strip_tags($matches[1]));
	}
	$matches = array();
	if (preg_match('/<p class="para rdfs-comment">(.+)<\/p>(\s+)<\/div>/', $contents, $matches)) {
		$longDescription = str_replace('\t', ' ', strip_tags($matches[1]));
	}
	if (!$longDescription) {
		$longDescription = $description;
	}
	$longDescription = str_replace('*/', "* /", $longDescription);
	$longDescription = str_replace('      ', "\n", $longDescription);
	$longDescription = wordwrap($longDescription, 75, "\n");
	$longDescription = str_replace('      ', "\n", $longDescription);
	$longDescription = wordwrap($longDescription, 75, "\n");
	$longDescription = "/**\n{$longDescription}";
	$longDescription = str_replace("\n", "\n{$indent} * ",  $longDescription);
	$longDescription = "{$longDescription}\n{$indent} */";
	return $longDescription;
}

function parseSignature($contents, $resourceName, $isMethod) {
	$methodName = $isMethod ? substr($resourceName, stripos($resourceName, '::') + 2) : $resourceName;
	$methodName = identifier($methodName);
	
	$phpSignature = $methodName . "()";
	$matches = array();
	$start = stripos($contents, '<div class="methodsynopsis dc-description">');
	$end = stripos($contents, '</div>', $start);
	if (FALSE !== $start && FALSE !== $end) {
		$signature =  trim(strip_tags(substr($contents, $start, $end - $start + 1)));
		// remove the return value
		$phpSignature = substr($signature, strpos($signature, ' ') + 1);
		$className = substr($resourceName, 0, strpos($resourceName, ":"));
		$matches = array();
		preg_match_all('/(\$\w+)/', $phpSignature, $matches);
		$phpSignature = $methodName . "(" . join(", ", $matches[1]) . ")";
		$phpSignature = identifier($phpSignature);		
	}
	return $phpSignature; 
}


$status = 0;
if (!file_exists(PHP_MANUAL_COMPRESSED_FILE_NAME) && !file_exists(PHP_MANUAL_FILE_NAME)) {
	passthru('wget http://us.php.net/get/php_manual_en.html.gz/from/this/mirror', $status);
}
if (0 == $status && !file_exists(PHP_MANUAL_FILE_NAME)) {
	echo "Unizipping documentation\n";
	system('gzip -d ' . PHP_MANUAL_COMPRESSED_FILE_NAME, $status);
}
$file = fopen(PHP_MANUAL_FILE_NAME, "rb");
if ($file) {
	$matchCount = 0;
	$outputFile = fopen(OUTPUT_FILE_NAME, "wb+");
	if ($outputFile) {
		echo "Building documentation index\n";
		fwrite($outputFile, "<?php\n");
		$indent = "";
		while (!feof($file)) {
			$line = fgets($file);
			$match = preg_match('/<div id="function\.(\w+)(-\w+)*" class="refentry">/', $line);
			if ($match) {
				$contents = getContents($file);
				$name = parseName($contents);
				$isMethod = FALSE !== strpos($name, "::");
				$version = parseVersion($contents);
				$description = parseDescription($contents, $name, $isMethod);
				$signature = parseSignature($contents, $name, $isMethod);
				//skip the 'dummy' main function 
				if ('main' != $name && !$isMethod) {
					if (DO_PRINT) { echo "name="; var_dump($name); }
					if (DO_PRINT) { echo "description="; var_dump($description); }
					if (DO_PRINT) { echo "signature="; var_dump($signature); }
					$matchCount += $match;
					if (0 == fwrite($outputFile, "{$indent}{$description}\n{$indent}function {$signature} {}\n")) {
						echo "Writing to " . OUTPUT_FILE_NAME . " failed.\n";
						break;
					}
				}
			}
		}
		fseek($file, 0);
		echo "found {$matchCount} functions \n";
		$matchCount = 0;
		$indent = "\t";
		$lastClassName = '';
		
		/*
		 * find all the classes (they have a 'synopsis')
		 */
		fseek($file, 0);
		$definedClasses = '';
		while (!feof($file)) {
			$line = fgets($file);
			$matches = array();
			$match = preg_match('/<div id="(\w+).synopsis" class="section">/', $line, $matches);
			if ($match) {
				$definedClasses = $definedClasses . '|' . $matches[1];
			}
		}
		$definedClasses = substr($definedClasses, 1);
		fseek($file, 0);
		while (!feof($file)) {
			$line = fgets($file);
			
			// get PDO and Exception classes as well, they do not have 'function' prepended
			$match = preg_match("/<div id=\"(function|{$definedClasses})\.(\w+)(-\w+)*\" class=\"refentry\">/", $line);
			if ($match) {
				$contents = getContents($file);
				$name = parseName($contents);
				$isMethod = FALSE !== strpos($name, "::");
				if ($isMethod) {
					$matchCount += $match;
					$version = parseVersion($contents);
					$description = parseDescription($contents, $name, $isMethod);
					$signature = parseSignature($contents, $name, $isMethod);
					$className = identifier(substr($name, 0, strpos($name, ':')));
					if ($lastClassName != $className) {
						$classDeclaration = !$lastClassName ? "class {$className} {\n" : "}\nclass {$className} {\n";
						if (0 == fwrite($outputFile, $classDeclaration)) {
							echo "Writing to " . OUTPUT_FILE_NAME . " failed.\n";
							break;
						}	
						if (0 == fwrite($outputFile, "{$indent}{$description}\n{$indent}function {$signature} {}\n")) {
							echo "Writing to " . OUTPUT_FILE_NAME . " failed.\n";
							break;
						}
						$lastClassName = $className;
					}
					else {
						$lastClassName = $className;
						if (0 == fwrite($outputFile, "{$indent}{$description}\n{$indent}function {$signature} {}\n")) {
							echo "Writing to " . OUTPUT_FILE_NAME . " failed.\n";
							break;
						}
					}
					if (DO_PRINT) { echo "name="; var_dump($name); }
					if (DO_PRINT) { echo "description="; var_dump($description); }
					if (DO_PRINT) { echo "signature="; var_dump($signature); }
					
				}
			}
		}
		if (0 == fwrite($outputFile, "\n}\n")) {
			echo "Writing to " . OUTPUT_FILE_NAME . " failed.\n";
			break;
		}
		fclose($outputFile);
		echo "found {$matchCount} methods \n";
		if (CHECK_SYNTAX) {
			echo "checking syntax...\n";
			passthru("php -l " . OUTPUT_FILE_NAME);
		}
	}
	else {
		echo "Could Not create file for writing " . OUTPUT_FILE_NAME . "\n";
	}
}
else {
	echo "Could not get contents for PHP documentation file: " . PHP_MANUAL_FILE_NAME . "\n";
	echo "Before running this script, download the PHP documentation by performing the following commands:\n";
	echo "  wget http://us.php.net/get/php_manual_en.html.gz/from/this/mirror\n";
	echo "  gzip -d php_manual_en.html.gz\n";
}

?>