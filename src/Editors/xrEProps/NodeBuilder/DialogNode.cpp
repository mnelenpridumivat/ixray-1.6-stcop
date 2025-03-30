#include "stdafx.h"
#include "DialogNode.h"

CDialogNode::CDialogNode(const xr_string Name) :
	INodeUnknown(Name.data())
{
	AddContactLink("Out", true);
	AddContactLink("In");
};

void CDialogNode::Draw()
{
	INodeUnknown::Draw();

	DrawHeader();

	auto RenderItemString = [this](const char* RawName, const char* Name, shared_str& Data, XML_NODE*& Node, size_t Size)
	{
		ImGui::Text(Name);
		ImGui::SameLine();
		ImGui::PushItemWidth(Size);
		string256 Value1 = {};

		if (Data.size() > 0)
		{
			strcpy(Value1, *Data);
		}

		if (ImGui::InputText((xr_string("##") + NodeName + Name).c_str(), (char*)&Value1, sizeof(Value1)))
		{
			Data = Value1;

			if (Node == nullptr)
			{
				Node = ParentNode->ToElement()->InsertNewChildElement(RawName);
			}

			Node->ToElement()->SetText(Value1);
		}
		ImGui::PopItemWidth();
	};

	RenderItemString("has_info", "Has Info:", HasInfo, HasInfoNode, 154);
	RenderItemString("dont_has_info", "Don't Has Info:", DontHasInfo, DontHasInfoNode, 120);
	RenderItemString("precondition", "Precondition:", Precondition, PreconditionNode, 131);

	ImGui::Separator();

	RenderItemString("text", "Text:", Text, TextNode, 175);
	ImGui::Separator();

	RenderItemString("action", "Action:", Action, ActionNode, 164);
	RenderItemString("give_info", "Give Info:", GiveInfo, GiveInfoNode, 146);
	ImGui::Checkbox((xr_string("Is Final##") + NodeName).c_str(), &IsFinal);

	DrawEnd();
}