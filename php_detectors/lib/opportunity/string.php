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
 * This namespace will contain all generic string functions.
 */
namespace opstring;

/**
 * Makes sure that haystack always  ends with needle.  If haystack already
 * ends with needle, then haystack is returned. If haystack does not end
 * with needle, then needle is appended to haystack. Comparison is case
 * sensitive.
 *
 * @param $haystack the string to search in
 * @param $needle the character to search for
 * @return a string that will always have $needle at the end
 */
function ensure_ends_with($haystack, $needle) {
	
	if (substr($haystack, - 1) != $needle) {
		$haystack.= $needle;
	}
	return $haystack;
}

/**
 * Convenience function that checks whether haystack starts with needle. Comparison
 * is case-sensitive.
 *
 * @param $haystack the string to search in
 * @param $needle the character to search for
 * @return boolean TRUE if haystack begins with needle Examples:
 *         haystack: "combo box" needle: "com"
 *         haystack: "combo box" needle: "combo"
 */
function begins_with($haystack, $needle) {
	return stripos($haystack, $needle) === 0;
}

/**
 * Gets the contents of a string that are after an initial string. This is *almost*
 * like the strstr function, the difference being tht this function will NOT return
 * the needle. Example:
 * haystack: hello@gmail.com needle: @  return: gmail.com
 *
 * @param $haystack the string to search in
 * @param $needle the character to search for
 * @return the part of haystack that is AFTER needle.  If needle is not in haystack,
 *         then the function returns FALSE
 */
function after($haystack, $needle) {
	$after = strstr($haystack, $needle);
	if ($after !== FALSE) {
		$after = substr($after, strlen($needle));
	}
	return $after;
}

/**
 * Get the contents of a string that are before an trailing string.  This is
 * like the strstr function.
 *
 * @param $haystack the string to search in
 * @param $needle the character to search for
 * @return the part of haystack that is BEFORE needle. If needle is not in haystack,
 *          this function returns FALSE
 */
function before($haystack, $needle) {
	$before = strstr($haystack, $needle, TRUE);
	return $before;
}