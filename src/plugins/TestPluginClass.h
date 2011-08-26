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
#ifndef TEST_PLUGIN_CLASS_H
#define TEST_PLUGIN_CLASS_H

#include <PluginClass.h>

namespace mvceditor {

/**
 * This is a plugin that is designed to test the plugin mechanism. 
 * This is also useful for prototyping of a feature or debugging (instead of
 * repeating steps just put the code in the Go() method )
 */
class TestPluginClass : public PluginClass {
public:
	TestPluginClass();
		
	/**
	 * This plugin will have no Edit menu items
	 */
	virtual void AddEditMenuItems(wxMenu* editMenu);
	
	/**
	 * This plugin will have no project menu items
	 */
	virtual void AddProjectMenuItems(wxMenu* projectMenu);

	/**
	 * This plugin will have one toolbar button, when clicked the Go() method will get called
	 */
	virtual void AddToolBarItems(wxAuiToolBar* toolBar);
	
	/**
	 * This plugin will have no extra windows.
	 */
	virtual void AddWindows();
	
	/**
	 * This plugin will have no context menu items
	 */
	virtual void AddCodeControlClassContextMenuItems(wxMenu* menu);
	
private:	

	/**
	 * the actual code that will get executed when the "GO" button is clicked
	 */
	void Go(wxCommandEvent& event);
	
	DECLARE_EVENT_TABLE()
};

}

#endif