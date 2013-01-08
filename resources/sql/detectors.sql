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
-- This database is used by MVC Editor to communicate between the 
-- C++ source code and the PHP detectors. The various detector scripts
-- will write into this database and MVC Editor will read from the tables.
--


-- this table will store all of the detected URLs. A URL Resource
-- stores a URL along with the location of the entry point; ie. what
-- class / method is invoked when the URL is requested.
-- 
-- in the case that a single function is invoked, ClassName will be 
-- empty and MethodName will contain the name of the function being called.
CREATE TABLE IF NOT EXISTS url_resources (

	-- full URL; including protocol, host, file, and query args
	-- http://localhost/file.php?x=1&c=2
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	url TEXT NOT NULL PRIMARY KEY COLLATE NOCASE,
	
	-- full path to the file 
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	full_path TEXT NOT NULL COLLATE NOCASE,
	
	-- fully qualified class name of the class that is invoked when the URL
	-- is requested.
	-- this will always contain namespaces, including
	-- the root namespace
	-- \MyController
	-- \ns\My\Controller
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	class_name TEXT NOT NULL COLLATE NOCASE,
	
	-- name of the method of the class that is invoked when the URL
	-- is requested.
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	method_name TEXT NOT NULL COLLATE NOCASE
);

CREATE TABLE IF NOT EXISTS call_stacks (
	
	-- zero-based index of the 
	-- where zero (0) is the first step: the entry point. all queries for a
	-- complete call stack should be sorted by this column so that the call stack 
	-- list is in order of program execution.
	step_number INTEGER NOT NULL PRIMARY KEY,
	
	-- the type of step
	-- BEGIN_FUNCTION, BEGIN_METHOD,
	-- ARRAY, SCALAR, OBJECT, PARAM, or RETURN
	step_type VARCHAR(50) NOT NULL,
	
	-- this is  a string of text that is dependant on the step type
	-- the possibilities:
	--
	-- BEGIN_FUNCTION expression is the function name
	-- BEGIN_METHOD expression is the class and method name (class::method)
	-- ARRAY expression is the array variable name followed by a comma separated list of the array keys
	-- SCALAR expression is the lexeme (string contents)
	-- OBJECT expression is the variable name 
	-- PARAM expression is the param type (ARRAY, SCALAR, OBJECT, ot VARIABLE)
	--       followed by the serialized type
	-- RETURN expression is empty
	expression TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS template_files (

	-- 
	-- needed for Zend_Db_Table_Abstract 
	--
	id INTEGER PRIMARY KEY AUTOINCREMENT, 
	
	-- this is the full path to the template file. 
	-- It contains OS-dependant directoy separators
	full_path TEXT NOT NULL,

	-- This is the comma separated list of templates variables
	-- Each variable will have th siguil ('$')
	variables TEXT NOT NULL
);


--
-- this table will store all of the "dynamic" tags 
-- (methods and properties) that MVC Editor could not detect because
-- they are added during runtime.  MVC Editor will look at this table
-- during the code completion process.
--
CREATE TABLE IF NOT EXISTS detected_tags (

	-- 
	-- needed for Zend_Db_Table_Abstract 
	--
	id INTEGER PRIMARY KEY AUTOINCREMENT, 
	
	--
	-- The key is the string that MVC Editor will use to query
	-- The key is either the method / property / function name, or
	-- the fully qualified name (class::method, class::property)
	-- this is NOT unique because there can be two properties that
	-- have the same name in different classes.
	--
	key TEXT NOT NULL COLLATE NOCASE,
	
	-- 
	-- type of tag
	-- 0 = CLASS
	-- 1 = METHOD
	-- 2 = FUNCTION
	-- 3 = MEMBER 
	-- 4 = DEFINE
	-- 5 = CLASS CONSTANT
	--
	type INTEGER NOT NULL,
	
	--
	-- the name of the class of this tag (only the class name)
	--
	class_name TEXT NOT NULL COLLATE NOCASE,
	
	--
	-- the name of the method or property of this tag 
	-- (only the property or method name)
	--
	method_name TEXT NOT NULL COLLATE NOCASE,
	
	--
	-- the class name of the member; or the return type
	-- if this tag is a method. This is the fully qualified name 
	-- with namespaces
	--
	return_type TEXT NOT NULL COLLATE NOCASE,
	
	--
	-- the fully qualified namespace that this tag is located in.
	-- there is always a value here; if the tag is in the root
	-- namespace then this column will have the value '\'
	--
	namespace_name TEXT NOT NULL COLLATE NOCASE,
	
	--
	-- Description text that is shown to the user
	--
	comment TEXT NOT NULL
);

-- to enable fast lookups for detected tags.
-- Note that the key is not necessarily unique; 2 different files may declare the same
-- class / methods / functions.
CREATE INDEX IF NOT EXISTS idxDetectedTagKey ON detected_tags(key, type);

