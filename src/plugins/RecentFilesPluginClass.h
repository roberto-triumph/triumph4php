
#include <PluginClass.h>
#include <wx/docview.h>

namespace mvceditor {

class RecentFilesPluginClass : public PluginClass {

public:

	RecentFilesPluginClass(mvceditor::AppClass& app);

	void AddFileMenuItems(wxMenu* fileMenu);

	void LoadPreferences(wxConfigBase* config);

	void SavePreferences();

private:

	/**
	 * sub-menu to hold the recent files
	 */
	wxMenu* RecentFilesMenu;

	/**
	 * class that encapsulates the logic
	 */
	wxFileHistory FileHistory;

	/**
	 * handler for the file menu event
	 */
	void OnRecentFileMenu(wxCommandEvent& event);

	/**
	 * when a file has been opened, add it to the recent
	 * list.
	 */
	void OnAppFileOpened(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

}