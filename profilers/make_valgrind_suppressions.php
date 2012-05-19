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
 * This file will create a suppressions file to use when profiling the various
 * profiler apps.  Since these apps use wxWidgets, some of the underlying libraries
 * like pango, gtk, ans such are not clean. The resulting suppression file can then
 * be used when running valgrind; the end goal is that it becomes MUCH easier to
 * spot errors in the MVC Editor source code.
 */

// get the nice args parsing library
set_include_path(get_include_path() . PATH_SEPARATOR .  realpath(__DIR__ . '/../php_detectors/lib/'));
require_once 'Zend/Console/Getopt.php';

$rules = array(
	'help|h' => 'A help message'
);
$opts = new Zend_Console_Getopt($rules);
$help = $opts->getOption('help');

if ($help) {
	echo <<<EOF
This file will create a suppressions file to use when profiling the various
profiler apps.  Since these apps use wxWidgets, some of the underlying libraries
like pango, gtk, and such are not clean. The resulting suppression file can then
be used when running valgrind; the end goal is that it becomes MUCH easier to
spot errors in the MVC Editor source code.

This program will output the resulting suppression file to STDOUT, user can then
redirect the output to any file on the file system.
EOF;
	exit(0);
}

// the valgrind suppression types
// from http://valgrind.org/docs/manual/mc-manual.html#mc-manual.suppfiles
$types = array(
	"Value1",
	"Value2",
	"Value4",
	"Value8",
	"Value16",
	"Cond",
	"Addr1",
	"Addr2",
	"Addr4",
	"Addr8",
	"Addr16",
	"Jump",
	"Param",
	"Free",
	"Overlap",
	"Leak"
);

// the files to suppress errors from.  These were parsed out of a run of one of the
// profiler app runs. They might differ on different versions of linux.
$files = array(
	"/lib/libdbus-1.so.3.4.0",
	"/lib/libexpat.so.1.5.2",
	"/lib/libglib-2.0.so.0.2400.1",
	"/lib/libselinux.so.1",
	"/usr/lib/gio/modules/libgvfsdbus.so",
	"/usr/lib/gtk-2.0/2.10.0/engines/libmurrine.so",
	"/usr/lib/gtk-2.0/2.10.0/immodules/im-ibus.so",
	"/usr/lib/gtk-2.0/modules/libcanberra-gtk-module.so",
	"/usr/lib/libatk-1.0.so.0.3009.1",
	"/usr/lib/libcairo.so.2.10800.10",
	"/usr/lib/libfontconfig.so.1.4.4",
	"/usr/lib/libfreetype.so.6.3.22",
	"/usr/lib/libgdk_pixbuf-2.0.so.0.2000.1",
	"/usr/lib/libgdk-x11-2.0.so.0.2000.1",
	"/usr/lib/libgio-2.0.so.0.2400.1",
	"/usr/lib/libgmodule-2.0.so.0.2400.1",
	"/usr/lib/libgobject-2.0.so.0.2400.1",
	"/usr/lib/libgthread-2.0.so.0.2400.1",
	"/usr/lib/libgtk-x11-2.0.so.0.2000.1",
	"/usr/lib/libgvfscommon.so.0.0.0",
	"/usr/lib/libibus.so.1.0.0",
	"/usr/lib/libpango-1.0.so.0.2800.0",
	"/usr/lib/libpangocairo-1.0.so.0.2800.0",
	"/usr/lib/libpangoft2-1.0.so.0.2800.0",
	"/usr/lib/libpixman-1.so.0.16.4",
	"/usr/lib/libX11.so.6.3.0",
	"/usr/lib/libxcb.so.1.1.0",
	"/usr/lib/libXcomposite.so.1.0.0",
	"/usr/lib/libXcursor.so.1.0.2",
	"/usr/lib/libXdamage.so.1.1.0",
	"/usr/lib/libXext.so.6.4.0",
	"/usr/lib/libXfixes.so.3.1.0",
	"/usr/lib/libXinerama.so.1.0.0",
	"/usr/lib/libXi.so.6.1.0",
	"/usr/lib/libXrandr.so.2.2.0",
	"/usr/lib/libXrender.so.1.3.0",
	"/usr/lib/pango/1.6.0/modules/pango-basic-fc.so"
);

$i = 0;
foreach ($types as $type) {
	foreach ($files as $file) {
		$i++;
		if ($type == 'Free') {
			echo "{\n";
			echo "suppression $i \n";
			echo "Memcheck:{$type} \n";
			echo "...\n";
			echo "obj:{$file}\n";
			echo "}\n\n";
		}
		else if ($type == 'Param') {
			// don't suppress for now
		}
		else {		
			echo "{\n";
			echo "suppression $i \n";
			echo "Memcheck:{$type} \n";
			echo "...\n";
			echo "obj:{$file}\n";
			echo "}\n\n";
		}
	}
}

$i++;
echo "{\n";
echo "suppression $i \n";
echo "Memcheck:Free \n";
echo "...\n";
echo "fun:_dl_catch_error\n";
echo "...\n";
echo "}\n\n";
