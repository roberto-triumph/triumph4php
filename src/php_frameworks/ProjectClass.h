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
 * @author     $Author: robertop2004@gmail.com $
 * @date       $Date: 2011-06-10 17:19:31 -0700 (Fri, 10 Jun 2011) $
 * @version    $Rev: 555 $ 
 */
#ifndef PROJECTCLASS_H_
#define PROJECTCLASS_H_

#include <search/ResourceFinderClass.h>
#include <wx/wx.h>

namespace mvceditor {

/**
 * The frameworks MVC Editor supports
 */
enum Frameworks {
	GENERIC = 0,
	SYMFONY = 1
};

const wxString FrameworksDescriptions[2] = {wxT("Generic PHP Project"), 
	wxT("Symfony") };

/*
 * Data structure that holds project attributes.
 */
class ProjectOptionsClass {
	
public:
	
	/**
	 * Default constructor. Sets name & root path to empty string, and a 
	 * generic project.
	 */
	ProjectOptionsClass();
	
	/**
	 * Create a new project options object from another one. The new
	 * project options will have the same values as other.
	 * 
	 * @param ProjectOptionsClass other the options to copy FROM
	 */
	ProjectOptionsClass(const ProjectOptionsClass& other);
	
	/**
	 * The framework flag.
	 */
	Frameworks Framework;
	
	/**
	 * The location of the root directory of the project in
	 * the file system. 
	 */
	wxString RootPath;
};

/**
 * The Project class is a base class for all PHP projects
 */
class ProjectClass {
	
public:
	
	/**
	 * Construct a ProjectClass object from the given options. Memory management (delete) is left to the 
	 * caller of this method.
	 * 
	 * @param ProjectOptionsClass options the new project's options
	 */
	static ProjectClass* Factory(const ProjectOptionsClass& options);
	
	/**
	 * Creates a project for this framework on the file system.  The 
	 * project creation process usually involves creating a directory
	 * structure with various config files and such.
	 * Returns true if creation succeeded.  Any errors are populated
	 * into the errors out parameter.
	 * 
	 * @param wxString errors gets filled in with any errors that occur
	 * when creating this project.
	 */
	virtual bool Create(wxString& errors);
	
	/**
	 * Returns the root path of this project
	 */
	wxString GetRootPath() const;
	
	/**
	 * Returns the valid PHP file extensions for this project
	 * @return wxString file extensions
	 */
	wxString GetPhpFileExtensions() const;
	
	/**
	 * Returns the PHP keywords, according to the PHP version of this project.
	 */
	wxString GetPhpKeywords() const;
	
	/**
	 * returns the location of the PHP executable
	 */
	wxString GetPhpExecutable() const;
	
	ResourceFinderClass* GetResourceFinder();

protected:
	
	/**
	 * sanitize a string to be suitable as an argument to a command 
	 */
	wxString Sanitize(const wxString& arg) const;
	
	/*
	 * Protected so that construction can only be done
	 * through the factory method.
	 */
	ProjectClass(const ProjectOptionsClass& options);
	
	/*
	 * Holds project attributes.
	 */
	ProjectOptionsClass Options;

private:
	
	ResourceFinderClass ResourceFinder;
};


/*
 * Class that interfaces with the various symfony commands. 
 */
class SymfonyProjectClass : public ProjectClass {

public:
	
	/*
	 * constructor
	 */
	SymfonyProjectClass(const ProjectOptionsClass& options);
	
	/*
	 * Executes the command symfony generate:project
	 */
	virtual bool Create(const wxString& name, wxString& errors);
};

}
#endif /*PROJECTCLASS_H_*/
