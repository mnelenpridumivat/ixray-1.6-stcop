#pragma once
#include "../../xrEProps/NodeBuilder/DialogNode.h"
#include "../../xrEProps/NodeBuilder/Builder.h"

#include "../../../xrCore/FormatParsers/XML/xrXMLParser.h"

class CUIDialogView:
	public CNodeViewport
{
private:
	CUIDialogView();
	~CUIDialogView();

	virtual void Draw() override;
	void Show(bool State);
	void OpenDialog(const shared_str& Str, XML_NODE* Node);

	void SelectNodeEvent(INodeUnknown* Node);
public:
	static void OpenFile(const xr_path& Path);

private:
	bool IsOpenList = true;
	xr_vector<std::pair<shared_str, XML_NODE*>> Dialogs;
	CXml File;

	shared_str HasInfo;
	shared_str DontHasInfo;
	shared_str Precondition;
};