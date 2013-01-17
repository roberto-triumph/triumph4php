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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __MVCEDITOR_TAGCLASS_H__
#define __MVCEDITOR_TAGCLASS_H__

#include <globals/String.h>
#include <unicode/unistr.h>
#include <wx/string.h>
#include <wx/datetime.h>
#include <wx/filename.h>
#include <vector>

namespace mvceditor {

/**
 * This class represents each tag we have found in the user's source code.  
 */
class TagClass {

public:

	/**
	 * All the resources we collect
	 */
	enum Types {
		CLASS,
		METHOD,
		FUNCTION,
		MEMBER,
		DEFINE,
		CLASS_CONSTANT,
		NAMESPACE
	};
	
	/**
	 * The identifer name of this tag. Members will not have a class name with it; ie. a Name method's Identifier will be Name
	 * @var UnicodeString
	 */
	UnicodeString Identifier;
	
	/**
	 * The name of the class that this tag belongs to; only members will have a class name with it: ie. User::Name
	 * The class name will NOT have the namespace
	 * @var UnicodeString
	 */
	UnicodeString ClassName;
	
	/**
	 * The namespace that this function / class is in.
	 */
	UnicodeString NamespaceName;
	
	/**
	 * The tag signature. For methods / functions; it is the entire argument list
	 * For classes; it is the class declaration ("class User extends Object implements ISerializable")
	 * @var UnicodeString
	 */
	UnicodeString Signature;
	
	/**
	 * If this tag item is a method / function / member, ReturnType is the function's return type
	 * @var UnicodeString
	 */
	UnicodeString ReturnType;
	
	/**
	 * The PHPDoc comment attached to the tag.
	 * @var UnicodeString
	 */
	UnicodeString Comment;

	/**
	 * Same as FileTagClass::IsNew ie TRUE if this tag was parsed from contents
	 * not yet written to disk
	 * @see FileTagClass::IsNew
	 */
	bool FileIsNew;
	
	/**
	 * The tag item type
	 * @var TagClass::Type
	 */
	TagClass::Types Type;

	/**
	 * TRUE if this is a protected member
	 */
	bool IsProtected;

	/**
	 * TRUE if this is a private member
	 */
	bool IsPrivate;

	/**
	 * TRUE if this is a static member
	 */
	bool IsStatic;

	/**
	 * TRUE if this is a tag is a 'dynamic' tag; it means that the tag
	 * is not actually in the source; it was either generated via a PHPDoc comment (@property, @method)
	 * or a a Feature object.
	 */
	bool IsDynamic;

	/**
	 * TRUE if this is a 'native' tag; one of the standard PHP functions / classes (str_*, array_*,
	 * DateTime).  This also includes any extensions (PDO, memcache, etc..) basically anything function
	 * that is documented in php.net.
	 */
	bool IsNative;

	/**
	 * This is the "key" that we will use for lookups. This is the string that will be used to index resources
	 * by so that we can use binary search.
	 * The key can be one of:
	 * - A single identifier (class name, function name, property / method name)
	 * - A full member name (Class::Method)
	 * - A fully namespaced name (\First\Sec\Class)
	 */
	UnicodeString Key;

	/**
	 * Full path to the file where this tag was found. Note that this may not be a valid file
	 * if a tag is a native or dynamic tag.
	 */
	wxString FullPath;

	/**
	 * The index to the file where this tag was found. 
	 */
	int FileTagId;
	
	TagClass();
	TagClass(const mvceditor::TagClass& src);

	static mvceditor::TagClass MakeNamespace(const UnicodeString& namespaceName);
	
	/**
	 * Clones a TagClass
	 */
	void operator=(const TagClass& src);
	void Copy(const mvceditor::TagClass& src);
	
	/**
	 * Defined a comparison function so that sorting algorithms work for tag containers. A tag is "less"
	 * than  another if Resource property is less than the other. (essentially containers are sorted by 
	 * Resource).
	 */
	bool operator<(const TagClass& a) const;

	/**
	 * Defined a comparison function so for find function. This will compare tag names in an 
	 * exact, case sensitive manner.
	 */	
	bool operator==(const TagClass& a) const;

	/**
	 * set all properties to empty string
	 */
	void Clear();
	
	/**
	 * @return TRUE if given key is the same as this tag's key (case insensitive)
	 */
	bool IsKeyEqualTo(const UnicodeString& key) const;

	/**
	 * @return the FileName that this tag is located in. This may be an
	 *         invalid FileName if this tag is a native or dynamic tag.
	 *         Note that this creates a new wxFileName, which may affect performance
	 */
	wxFileName FileName() const;

	/**
	 * @return the full path that this tag is located in.
	 *         if a tag is a native or dynamic tag.
	 */
	wxString GetFullPath() const;

	/**
	 * @param fullPath the full path where this tag is located
	 */
	void SetFullPath(const wxString& fullPath);

	/**
	 * @return TRUE if this is a function/method that has at least one parameter
	 */
	bool HasParameters() const;

};

class TraitTagClass {
	
public:

	/**
	 * the key is used to perform lookups into this table. The key will be either
	 * 1. The name of the class that uses a trait (same as ClassName property)
	 * 2. The fully qualified name of the class that uses the trait (concatenation of NamespaceName and ClassName)
	 */
	UnicodeString Key;

	/**
	 * the name of the class that uses a trait. This is the name of the class only
	 * (no namespace)
	 */
	UnicodeString ClassName;

	/** 
	 * the namespace of the class that uses the trait. This will be "\" if the class is
	 * in the root namespace
	 */
	UnicodeString NamespaceName;

	/** 
	 * the name of the class of the trait that is being used. This is the name of the class only
	 * (no namespace)
	 */
	UnicodeString TraitClassName;

	/**
	 * the namespace of the trait being used. This will be "\" if the trait is
	 * in the root namespace
	 */
	UnicodeString TraitNamespaceName;
	
	/**
	 * The names of any aliases
	 */
	std::vector<UnicodeString> Aliased;
	
	/**
	 * the names of any class names excluded from being used by the
	 * 'insteadof' operator
	 */
	std::vector<UnicodeString> InsteadOfs;
	
	TraitTagClass();
};

/**
 * This struct will be used to keep track of which files we have already cached.  The last modified timestamp
 * will be used so that we dont look at files that have not been modified since we last parsed them
 */
class FileTagClass {

public:
	
	/**
	 * The full path to the file where this tag was found
	 */
	wxString FullPath;
	
	/**
	 * The time that this tag was looked at.
	 */
	wxDateTime DateTime;

	/**
	 * unique identifier for this file. Guaranteed to be unique once this itemsd
	 * has been saved to the database.
	 */
	int FileId;
	
	/**
	 * whether or not file has been parsed, could be false if we only looked for files
	 */
	bool IsParsed;

	/**
	 * If TRUE, then this file is not yet written to disk (ie the tag only exists in memory
	 * ( but not yet in the filesystem). This is needed because the finder will do
	 * a sanity check to ensure that the file that contained a match still exists. Iif a file is deleted 
	 * after a file was cached then we want to eliminate that match. But, this sanity checks would kill
	 * matches that were a result of a manual call to BuildResourceCacheForFile. This flag ensures
	 * proper operation (resources that were parsed from code that the user has typed in but no yet
	 * saved are NOT removed).
	 */
	bool IsNew;

	FileTagClass();

	/**
	 * Check to see if this file needs to be parsed. A file needs to be parsed when
	 * 1. it is seen for the first time
	 * 2. has not been parsed yet (IsParsed is FALSE)
	 * 3. it has been modified since the last time we parsed it
	 */
	bool NeedsToBeParsed(const wxDateTime& fileLastModifiedTime) const;

	/**
	 * initialize the members of this file item for insertion into the database.
	 */
	void MakeNew(const wxFileName& fileName, bool isParsed);
};

}

#endif