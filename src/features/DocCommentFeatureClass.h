
#ifndef __T4P_CALLTIPFEATURECLASS_H__
#define __T4P_CALLTIPFEATURECLASS_H__

#include <features/FeatureClass.h>
#include <features/wxformbuilder/DocCommentFeatureForms.h>

namespace t4p {

// forward declaration, defined in another file
class CodeControClass;

/**
 * this feature will show a small panel with the PHP Doc
 * comment of the item that is currently under the 
 * mouse pointer or at the current cursor position. 
 */
class DocCommentFeatureClass : public t4p::FeatureClass {

public:

	DocCommentFeatureClass(t4p::AppClass& app);

	void AddEditMenuItems(wxMenu* editMenu);

private:

	void OnShowDocComment(wxCommandEvent& event);

	void OnMotionAlt(wxCommandEvent& event);

	void ShowDocComment(t4p::CodeControlClass* ctrl, int pos);

	DECLARE_EVENT_TABLE()
};

/**
 * the panel that shows a PHP doc comment.
 */
class DocCommentPanelClass : public DocCommentPanelGeneratedClass {

public:

	DocCommentPanelClass(wxWindow* parent);

	void SetText(const wxString& text);

protected:

	void OnClose(wxHyperlinkEvent& event);
	void OnKeyDown(wxKeyEvent& event);

	void DoDestroy();
};

}

#endif
