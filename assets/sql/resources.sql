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
-- This database is used by Triumph to hold the results of PHP source code
-- parsing. The database is also given to the various PHP detector scripts.
--

-- this table store all "source" directories. a source directory is just a 
-- directory that contains source code files
-- each source corresponds to a source directory that the user has added to a project.
CREATE TABLE IF NOT EXISTS sources (
	source_id INTEGER NOT NULL PRIMARY KEY,
	
	-- the full path has OS-dependant file separators
	-- the full path is the entire path to a source directory
	-- don't do it case-insensitive because different file systems handle case differently
	directory TEXT NOT NULL 
);

-- this table stores all of the files that have been seen by Triumph
CREATE TABLE IF NOT EXISTS file_items (
	file_item_id INTEGER NOT NULL PRIMARY KEY, 
		
	-- the foreign key to the source directory
	source_id INTEGER NOT NULL,

	-- the full path has OS-dependant file separators
	-- the full path is the entire path to a file
	-- don't do it case-insensitive because different file systems handle case differently
	-- this may not be an actual file path when is_new = 1
	full_path TEXT NOT NULL, 
	
	-- the name (and extension) of the file
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	name TEXT NOT NULL COLLATE NOCASE,
	
	-- this is up to the second precision. this time is used to check to see if
	-- the file needs to be parsed
	last_modified DATETIME NOT NULL, 
	
	-- 1 when the file has been parsed for resources
	is_parsed INTEGER NOT NULL, 
	
	-- 1 if this is a 'new' file; a file that the user has created but has not yet
	-- been saved.  We still store it here so that the editor is aware of the file,
	-- the editor can then jump to the file or use it in auto completion.
	is_new INTEGER NOT NULL
);

-- this table stores all of the resources (tags) for all files that
-- have been seen by Triumph
-- note that this table will have "duplicate" entries for methods, properties,
-- functions and classes.  One entry will be as it was found in the source
-- and one will be the "fully qualified" entry.  This will make it easy to
-- perform all lookups using a single index (this is the purpose of the key
-- column).
CREATE TABLE IF NOT EXISTS resources (

	id INTEGER NOT NULL PRIMARY KEY,
	
	-- the file that the resource is located in
	file_item_id INTEGER NOT NULL, 
	
	-- the foreign key to the source directory
	source_id INTEGER NOT NULL,
	
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
	-- Note that the key is not necessarily unique; 2 different files may declare the same
	-- class / methods / functions.
	key TEXT NOT NULL COLLATE NOCASE,
	
	-- the identifier is the lexeme of the resource. 
	-- For classes, defines, or functions, this is the class / define function name
	-- For methods, members, class constants, this is the method / member / constant name only
	-- For namespaces, the key will always be the fully qualified namespace
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	identifier TEXT NOT NULL COLLATE NOCASE,  
	
	-- this will only be set for methods, members, or class constants. This is the
	-- name of the class that holds the method / member / constant
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	class_name TEXT NOT NULL COLLATE NOCASE,
	
	-- a number that signifies the type of resource
	-- 0 = class
	-- 1 = method
	-- 2 = function
	-- 3 = member (property)
	-- 4 = define
	-- 5 = class constant
	-- 6 = namespace
	type INTEGER NOT NULL, 
	
	-- this is the namespace that holds the class or function.
	-- In the case where no namespace was declared, this will be the root namespace "\"
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	namespace_name TEXT NOT NULL COLLATE NOCASE, 
	
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
	-- The signature *may* contain the return type, if Triumph is able to determine
	-- it from the PHPDoc comment.
	signature TEXT NOT NULL, 
	
	-- This is the entire comment that preceded the resource. Only PHPDoc comments are 
	-- captured. This is the comment
	-- as was found in the sources, complete with the original newlines, spaces, and comment
	-- delimiters ("/**" and "*/")
	comment TEXT NOT NULL,
	
	-- if Triumph is able to determine it from the PHPDoc comment, then this column
	-- contains the function / method return type. In the case of members, this is the
	-- member class name.
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	return_type TEXT NOT NULL COLLATE NOCASE, 
	
	-- 1 if this is a method / member / class constant and it has been labeled as protected
	-- if is_protected = 0 and is_private = 0, then this resource is a public resource
	is_protected INTEGER NOT NULL, 
	
	-- 1 if this is a method / member / class constant and it has been labeled as private
	-- if is_protected = 0 and is_private = 0, then this resource is a public resource
	is_private INTEGER NOT NULL,
	
	-- 1 if this is a method / member / class constant and it has been labeled as static
	-- this is important, as triumph is smart enough to show only static members / methods
	-- when a static call is being made
	is_static INTEGER NOT NULL, 
	
	-- 1 if this resource is a 'magic' resource.
	is_dynamic INTEGER NOT NULL, 
	
	-- 1 if this resource is a "native" function; ie a resource that is in the standard 
	-- PHP libraries
	is_native INTEGER NOT NULL,
	
	-- 1 if this is a function / method that has variable arguments. variable argument
	-- detection works based off whether the body of the function / method
	-- has a call to func_get_arg or its friends
	has_variable_args INTEGER NOT NULL
);

-- This table stores all of the trait relationships that have been found by Triumph.
-- note that this table will have "duplicate" entries for each trait.
-- One entry will be as it was found in the source
-- and one will be the "fully qualified" entry.  This will make it easy to
-- perform all lookups using a single index (this is the purpose of the key
-- column).
CREATE TABLE IF NOT EXISTS trait_resources (

	-- the file that class_name is located in (the class that uses the trait)
	file_item_id INTEGER NOT NULL, 
	
	-- the foreign key to the source directory
	source_id INTEGER NOT NULL,

	-- the key is used to perform lookups into this table. The key will be either
	-- 1. The name of the class that uses a trait (same as class_name column)
	-- 2. The fully qualified name of the class that uses the trait (concatenation of namespace_name and class_name)
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	key TEXT NOT NULL COLLATE NOCASE,
	
	-- the name of the class that uses a trait. This is the name of the class only
	-- (no namespace)
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	class_name TEXT NOT NULL COLLATE NOCASE,
	
	-- the namespace of the class that uses the trait. This will be "\" if the class is
	-- in the root namespace
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	namespace_name TEXT NOT NULL COLLATE NOCASE,
	
	-- the name of the class of the trait that is being used. This is the name of the class only
	-- (no namespace)
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	trait_name TEXT NOT NULL COLLATE NOCASE,
	
	-- the namespace of the trait being used. This will be "\" if the trait is
	-- in the root namespace
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	trait_namespace_name TEXT NOT NULL COLLATE NOCASE,
	
	-- a comma-separated list of all of the aliased methods from the trait
	aliases TEXT NOT NULL,
	
	-- a comma-separated list of all of the "insteadof" traits
	instead_ofs TEXT NOT NULL
);

--
-- This table will store any table names that we
-- detect from the configured database connections
--
CREATE TABLE IF NOT EXISTS db_tables (

	table_id INTEGER NOT NULL PRIMARY KEY,
	
	-- this is the name of the connection that was used to extract the
	-- tables from
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	connection_label TEXT NOT NULL COLLATE NOCASE,
	
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	table_name TEXT NOT NULL COLLATE NOCASE
);

-- this table stores full paths to files or directories that
-- the user wants quick access to. These are separate from
-- projects all together.
CREATE TABLE IF NOT EXISTS file_cabinet_items (
	
	file_cabinet_item_id INTEGER NOT NULL PRIMARY KEY,
	
	-- the full name of the file (with the extesion) or if this is a directory
	-- then name is the name of the last sub-directory
	name TEXT NOT NULL COLLATE NOCASE,
	
	-- this full_path has OS-dependant directory separators
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	-- if this item is a directory, it will end with the directory
	-- separator
	full_path TEXT NOT NULL COLLATE NOCASE
);

--
-- This table will store any column names that we
-- detect from the configured database connections
--
CREATE TABLE IF NOT EXISTS db_columns (

	-- this is the name of the connection that was used to extract the
	-- tables from
	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	connection_label TEXT NOT NULL COLLATE NOCASE,

	-- storing it as case-insensitive because we always want to do case-insensitive lookups
	column_name TEXT NOT NULL COLLATE NOCASE
);

--
-- Triumph will check the version number in this table and compare it against the
-- version the code expects.  Triumph will recreate the database if the
-- versions do not match
--
CREATE TABLE schema_version (
	version_number INT NOT NULL
);

-- to enable fast lookups for file paths
CREATE UNIQUE INDEX IF NOT EXISTS idxFullPath ON file_items(full_path);

-- to enable fast lookups for resources.
-- Note that the key is not necessarily unique; 2 different files may declare the same
-- class / methods / functions.
CREATE INDEX IF NOT EXISTS idxResourceKey ON resources(key, type);

-- to enable fast lookups for resources from a specific file.
CREATE INDEX IF NOT EXISTS idxResourceFileItem ON resources(file_item_id);

-- to enable fast deletions of entire resources from a specific source directory.
CREATE INDEX IF NOT EXISTS idxResourceSource ON resources(source_id);

-- to enable fast lookups for traits.
-- Note that the key is not necessarily unique; 2 different files may declare the same
-- class / methods / functions.
CREATE INDEX IF NOT EXISTS idxTraitKey ON trait_resources(key);

-- to enable fast lookups for file cabinet items.
CREATE INDEX IF NOT EXISTS idxFileCabinetName ON file_cabinet_items(name);

--
-- This number must match the version in CacheDbVersionActionClass.cpp
--
INSERT INTO schema_version (version_number) VALUES(10);

--
-- Write ahead logging to allow for concurrent reads and writes
--
PRAGMA journal_mode = WAL;