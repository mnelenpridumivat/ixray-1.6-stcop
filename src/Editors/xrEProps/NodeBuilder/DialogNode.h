#pragma once
#include "Nodes.h"

#include "../../xrCore/FormatParsers/XML/xrXMLParser.h"

class XREPROPS_API CDialogNode final:
	public INodeUnknown
{
public:
	CDialogNode(const xr_string Name);

	virtual void Draw() override;
	virtual void AddContactLink(const xr_string& Name, bool IsOut = false) override;
	virtual void MakeOutNode(INodeUnknown* Node) override;

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