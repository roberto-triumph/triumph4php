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
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <widgets/AuiManager.h>

int t4p::AuiLayerCount(wxAuiManager& auiManager) {
	int layer = 0;
	wxAuiPaneInfoArray infos = auiManager.GetAllPanes();
	for (size_t i = 0; i < infos.size(); i++) {
		if (infos[i].dock_direction == wxAUI_DOCK_BOTTOM && infos[i].dock_layer > layer) {
			layer = infos[i].dock_layer;
		}
	}
	return layer;
}

int t4p::AuiRowCount(wxAuiManager& auiManager) {
	int row = 0;
	wxAuiPaneInfoArray infos = auiManager.GetAllPanes();
	for (size_t i = 0; i < infos.size(); i++) {
		if (infos[i].dock_direction == wxAUI_DOCK_BOTTOM && infos[i].dock_row > row) {
			row = infos[i].dock_row;
		}
	}
	return row;
}
