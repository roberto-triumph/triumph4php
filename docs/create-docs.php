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

/*
 * this is a script that will turn the markdown docs into HTML. It does
 * this by using a static site generator called sculpin.
 *
 * This script will:
 * 1. move all markdown docs into the sculpin source dir
 * 2. add the front-matter to each file so that sculpin
 *    uses the appropriate layout file
 * 3. run the sculping generate process
 */

$opts = getopt('r:h', array('root:', 'help'));
if (isset($opts['h']) || isset($opts['help'])) {
	echo <<<HELP
this is a script that will turn the markdown docs into HTML. It does
this by using a static site generator called sculpin.
 
This script will:
1. move all markdown docs into the sculpin source dir
2. add the front-matter to each file so that sculpin
   uses the appropriate layout file
3. run the sculping generate process


Program usage:

-r|--root          the location of the sculpin directory
                   this directory is where base of the
                   sulpin project
HELP;

}

$destDocRoot = '';
if (isset($opts['r'])) {
	$destDocRoot = $opts['r'];
}
if (isset($opts['root'])) {
	$destDocRoot = $opts['root'];
}
if (strlen(trim($destDocRoot)) == 0) {
	print("root argument must have a value. See --help for details.\n");
	exit(-1);
}

// create source dir if it does not already exist
$cmd = sprintf('mkdir -p "%s/source"', $destDocRoot);
print($cmd . "\n");
system($cmd);

// remove all md files from the source dir, that way if
// we delete a file it gets removed from the sources too
$cmd = sprintf('rm -rf "%s/source/*"', $destDocRoot);
print($cmd . "\n");
system($cmd);

foreach (glob(__DIR__ . "/*.md") as $strFile) {

	// loop though all files, on each file we need to
	// add the header that tels sculpin to use the
	// correct layout
	$header = "---\nlayout: default.html\n---\n";
	
	$helpFile = $destDocRoot . "/source" . DIRECTORY_SEPARATOR . basename($strFile);
	$contents = file_get_contents($strFile);
	$newContents = $header . $contents;
	
	$sourceFile = $destDocRoot . '/source/' . basename($strFile);
	file_put_contents($sourceFile, $newContents);
	print("wrote file " . $sourceFile . "\n");
}

$cmd = sprintf("cd %s && sculpin generate", $destDocRoot);
print($cmd . "\n");
system($cmd);

