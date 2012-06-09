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
#include <php_frameworks/ProjectClass.h>
#include <php_frameworks/FrameworkDetectorClass.h>
#include <widgets/ResourceCacheClass.h>
#include <windows/AppFrameClass.h>
#include <environment/AmpInfoClass.h>
#include <environment/UrlResourceClass.h>
#include <plugins/ProjectPluginClass.h>
#include <PluginClass.h>
#include <PreferencesClass.h>
#include <Events.h>
#include <wx/app.h>
#include <wx/event.h>
#include <wx/string.h>
#include <vector>

namespace mvceditor {

class AppClass : public wxApp {

public:

	/**
	 * The ampInfo stack.
	 * 
	 * @var AmpInfoClass
	 */
	AmpInfoClass AmpInfo;
	
	/**
	 * This object will be used to parse the resources of files that are currently open.
	 */
	ResourceCacheClass ResourceCache;
		
	/**
	 * This object will be used to detct the various PHP framework artifacts (resources,
	 * database connections, route URLs).
	 */
	PhpFrameworkDetectorClass PhpFrameworks;
	
	/**
	 * The URLs (entry points to the current project) that have been detected so far. Also holds the 
	 * "current" URL.
	 */
	UrlResourceFinderClass UrlResourceFinder;

	/**
	 * Any plugins should post any useful events to this event handler
	 * then all other plugins will get notified.  This is how
	 * inter-plugin communication is done in a way to reduce coupling
	 * Plugins need not push themselves onto this sink, the application will do 
	 * so at app start.
	 */
	EventSinkClass EventSink;

	/**
	 * The open project
	 * @var ProjectClass*
 	 */
	ProjectClass* Project;

	/** 
	 * Initialize the application 
	 */
	virtual bool OnInit();
	
	AppClass();

	~AppClass();

private:

	/**
	 * Parses any command line arguments.  Returns false if arguments are invalid.
	 */
	bool CommandLine();

	/**
	 * create plugins. only instantiates and nothing else
	 */
	void CreatePlugins();
	
	/**
	 * delete plugins from memory
	 */
	void DeletePlugins();

	/**
	 * asks plugins for any windows they want to create
	 */
	void PluginWindows();

	/**
	 * Additional functionality
	 */
	std::vector<PluginClass*> Plugins;
	
	/**
	 * The user preferences.  
	 * @var PreferencesClass;
	 */
	PreferencesClass Preferences;

	/**
	 * The main application frame.
	 */
	AppFrameClass* AppFrame;

	/**
	 * Will need to load preferences for the project before all others. This pointer just points to one of the Plugins
	 * in the Plugin vector; no need to delete.
	 */
	ProjectPluginClass* ProjectPlugin;

};

}