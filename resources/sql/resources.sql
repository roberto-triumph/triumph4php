-------------------------------------------------------------------
-- This software is released under the terms of the MIT License
-- 
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
-- 
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
-- 
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.
--
-- @copyright  2009-2011 Roberto Perpuly
-- @license    http://www.opensource.org/licenses/mit-license.php The MIT License
-------------------------------------------------------------------

--
-- This database is used by MVC Editor to hold the results of PHP source code
-- parsing. The database is also given to the various PHP detector scripts.
--

-- this table stores all of the files that have been seen by MVC Editor
CREATE TABLE IF NOT EXISTS file_items (
	file_item_id INTEGER PRIMARY KEY, 

	-- the full path has OS-dependant file separators
	-- the full path is the entire path to a file
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	-- this may not be an actual file path when is_new = 1
	full_path TEXT COLLATE NOCASE, 
	
	-- this is up to the second precision. this time is used to check to see if
	-- the file needs to be parsed
	last_modified DATETIME, 
	
	-- 1 when the file has been parsed for resources
	is_parsed INTEGER, 
	
	-- 1 if this is a 'new' file; a file that the user has created but has not yet
	-- been saved.  We still store it here so that the editor is aware of the file,
	-- the editor can then jump to the file or use it in auto completion.
	is_new INTEGER
);

-- this table stores all of the resources (tags) for all files that
-- have been seen by MVC Editor
-- note that this table will have "duplicate" entries for methods, properties,
-- functions and classes.  One entry will be as it was found in the source
-- and one will be the "fully qualified" entry.  This will make it easy to
-- perform all lookups using a single index (this is the purpose of the key
-- column).
CREATE TABLE IF NOT EXISTS resources (
	file_item_id INTEGER, 
	
	-- the key is used to perform lookups into this table.  The key will have
	-- multiple formats:
	-- 1. the key will be the identifier
	-- 2. the key will be the fully qualified namespace name in the case of classes, defines, 
	--    or functions. The fully qualified name is the namespace name plus the
	--    identifier. For example, "\Exception" is the fully qualified key
	--    for the exception class.
	-- 3. the key will be the fully qualified method / member name in the case
	--    of methods and members. The fully qualified name is the namespace name plus the
	--    identifierFor example, "Exception::getMessage" is the 
	--    fully qualified key for the getMessage() method of the Exception class.
	-- 4. for namespaces, the key will always be the fully qualified namespace
	--
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	key TEXT COLLATE NOCASE,
	
	-- the identifier is the lexeme of the resource. 
	-- For classes, defines, or functions, this is the class / define function name
	-- For methods, members, class constants, this is the method / member / constant name only
	-- For namespaces, the key will always be the fully qualified namespace
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	identifier TEXT COLLATE NOCASE,  
	
	-- this will only be set for methods, members, or class constants. This is the
	-- name of the class that holds the method / member / constant
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	class_name TEXT COLLATE NOCASE,
	
	-- a number that signifies the type of resource
	-- 0 = class
	-- 1 = method
	-- 2 = function
	-- 3 = member (property)
	-- 4 = define
	-- 5 = class constant
	-- 6 = namespace
	type INTEGER, 
	
	-- this is the namespace that holds the class or function.
	-- In the case where no namespace was declared, this will be the root namespace "\"
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	namespace_name TEXT COLLATE NOCASE, 
	
	-- this is the function / method signature. for classes, this is the
	-- class declaration 
	-- examples
	-- 1. function
	--    string strpos($haystack, $needle, $offset)
	-- 2. method
	--    public function __construct()
	-- 3. class
	--    class News extends CI_Controller
	--
	-- The signatures are re-constituted from an AST representation, they will not
	-- contain newlines.
	-- The signature *may* contain the return type, if MVC Editor is able to determine
	-- it from the PHPDoc comment.
	signature TEXT, 
	
	-- This is the entire comment that preceded the resource. Only PHPDoc comments are 
	-- captured. This is the comment
	-- as was found in the sources, complete with the original newlines, spaces, and comment
	-- delimiters ("/**" and "*/")
	comment TEXT,
	
	-- if MVC Editor is able to determine it from the PHPDoc comment, then this column
	-- contains the function / method return type. In the case of members, this is the
	-- member class name.
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	return_type TEXT COLLATE NOCASE, 
	
	-- 1 if this is a method / member / class constant and it has been labeled as protected
	-- if is_protected = 0 and is_private = 0, then this resource is a public resource
	is_protected INTEGER, 
	
	-- 1 if this is a method / member / class constant and it has been labeled as private
	-- if is_protected = 0 and is_private = 0, then this resource is a public resource
	is_private INTEGER,
	
	-- 1 if this is a method / member / class constant and it has been labeled as static
	is_static INTEGER, 
	
	-- 1 if this resource is a 'magic' resource.
	is_dynamic INTEGER, 
	
	-- 1 if this resource is a "native" function; ie a resource that is in the standard 
	-- PHP libraries
	is_native INTEGER
);

-- to enable fast lookups for file paths
CREATE UNIQUE INDEX IF NOT EXISTS idxFullPath ON file_items(full_path);

-- to enable fast lookups for resources
CREATE INDEX IF NOT EXISTS idxResourceKey ON resources(key, type);