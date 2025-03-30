#pragma once
#include "Nodes.h"

#include "../../xrCore/FormatParsers/XML/xrXMLParser.h"

class XREPROPS_API CDialogNode :
	public INodeUnknown
{
public:
	CDialogNode(const xr_string Name);
	void Draw() override;

public:
	shared_str HasInfo;
	shared_str DontHasInfo;
	shared_str GiveInfo;
	shared_str Action;
	shared_str Precondition;
	shared_str Text;

	XML_NODE* HasInfoNode = nullptr;
	XML_NODE* DontHasInfoNode = nullptr;
	XML_NODE* GiveInfoNode = nullptr;
	XML_NODE* ActionNode = nullptr;
	XML_NODE* PreconditionNode = nullptr;
	XML_NODE* TextNode = nullptr;

	XML_NODE* ParentNode = nullptr;

	bool IsFinal = false;
};