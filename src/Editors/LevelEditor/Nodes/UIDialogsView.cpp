#include "stdafx.h"
#include "UIDialogsView.h"
#include "../Editor/UI_LevelTools.h"
#include "../../../xrEngine/string_table.h"

CUIDialogView::CUIDialogView()
{
	NodeSelectCallback = xr_make_delegate(this, &CUIDialogView::SelectNodeEvent);
	bOpen = false;
}

CUIDialogView::~CUIDialogView()
{
	for (auto Node : Nodes)
	{
		xr_delete(Node);
	}
}

void CUIDialogView::Draw()
{
	if (!bOpen)
		return;

	if (ImGui::Begin("Dialogs Editor", &bOpen))
	{
		if (ImGui::BeginChild("Dialogs in file", { (IsOpenList ? 300.f : 20.f), 0}))
		{
			if (IsOpenList)
			{
				ImGui::SetNextItemWidth(300);
				if (ImGui::BeginListBox("##ItemsInFile", { 0, ImGui::GetWindowSize().y - 30 }))
				{
					for (const auto& [ID, Node] : Dialogs)
					{
						bool bSelect = false;
						if (ImGui::Selectable(ID.c_str(), &bSelect))
						{
							OpenDialog(ID, Node);
							IsOpenList = false;
						}
					}
					ImGui::Separator();
					ImGui::EndListBox();
				}

				ImGui::Button("Save");
				ImGui::SameLine();

				ImGui::SetCursorPosX(282);

				if (ImGui::Button("<"))
				{
					IsOpenList = false;
				}
			}
			else if (ImGui::Button(">"))
			{
				IsOpenList = true;
			}
		}
		ImGui::EndChild();
		ImGui::SameLine();

		int HoveredNodeID = GetHoveredMode();

		if (ImGui::IsMouseReleased(1))
		{
			ImGui::OpenPopup("##nodesviewportcontextmenumacro");
		}

		if (ImGui::BeginPopup("##nodesviewportcontextmenumacro"))
		{
			if (ImGui::BeginMenu("Create Node"))
			{

				ImGui::EndMenu();
			}

			if (HoveredNodeID != -1 && ImGui::MenuItem("Remove"))
			{
				Nodes.erase
				(
					std::find_if(Nodes.begin(), Nodes.end(), [HoveredNodeID](INodeUnknown* Val)
					{
						return Val->NodeID == HoveredNodeID;
					})
				);
			}

			ImGui::EndPopup();
		}
		CNodeViewport::Draw();
	}

	ImGui::End();

	CNodeViewport::DrawEnd();
}

void CUIDialogView::Show(bool State)
{
	bOpen = State;
}

void CUIDialogView::OpenDialog(const shared_str& Str, XML_NODE* Node)
{
	for (auto Node : Nodes)
	{
		xr_delete(Node);
	}
	Nodes.clear();

	XML_NODE* RootNode = File.NavigateToNode(Node, "phrase_list");
	XML_NODE* PhraseNode = RootNode->FirstChildElement();

	xr_map<CDialogNode*, xr_vector<shared_str>> NodeGraph;

	auto MakeListStringFromNode = [](shared_str& Value, shared_str Text)
	{
		if (Value.size() > 0)
		{
			Value = make_string<shared_str>("%s, %s", *Value, *Text);
		}
		else
		{
			Value = Text;
		}
	};

	while (PhraseNode != nullptr)
	{
		xr_string UpperNodeName = PhraseNode->Value();

		if (UpperNodeName == "has_info")
		{
			shared_str NodeText = PhraseNode->ToElement()->GetText();
			MakeListStringFromNode(HasInfo, NodeText);
			PhraseNode = PhraseNode->NextSibling();
			continue;
		}
		else if (UpperNodeName == "dont_has_info")
		{
			shared_str NodeText = PhraseNode->ToElement()->GetText();
			MakeListStringFromNode(DontHasInfo, NodeText);
			PhraseNode = PhraseNode->NextSibling();
			continue;
		}
		else if (UpperNodeName == "precondition")
		{
			shared_str NodeText = PhraseNode->ToElement()->GetText();
			Precondition = NodeText;
			PhraseNode = PhraseNode->NextSibling();
			continue;
		}

		shared_str NodeID = PhraseNode->ToElement()->Attribute("id");
		if (NodeID.size() == 0)
		{
			PhraseNode = PhraseNode->NextSibling();
			continue;
		}

		CDialogNode* MacroNode = (CDialogNode*)Nodes.emplace_back(new CDialogNode(*NodeID));
		XML_NODE* ChildNode = PhraseNode->FirstChildElement();

		while (ChildNode != nullptr)
		{
			xr_string NodeName = ChildNode->Value();
			shared_str NodeText = ChildNode->ToElement()->GetText();

			if (NodeText.size() == 0)
				continue;

			if (NodeName == "text")
			{
				MacroNode->Text = NodeText;
			}
			else if (NodeName == "dont_has_info")
			{
				MakeListStringFromNode(MacroNode->DontHasInfo, NodeText);
			}
			else if (NodeName == "has_info")
			{
				MakeListStringFromNode(MacroNode->HasInfo, NodeText);
			}
			else if (NodeName == "is_final")
			{
				MacroNode->IsFinal = NodeText == "1";
			}
			else if (NodeName == "give_info")
			{
				MakeListStringFromNode(MacroNode->GiveInfo, NodeText);
			}
			else if (NodeName == "precondition")
			{
				MacroNode->Precondition = NodeText;
			}
			else if (NodeName == "action")
			{
				MacroNode->Action = NodeText;
			}
			else if (NodeName == "next")
			{
				NodeGraph[MacroNode].push_back(NodeText);
			}

			ChildNode = ChildNode->NextSibling();
		}
		PhraseNode = PhraseNode->NextSibling();
	}

	if (NodeGraph.empty())
		return;

	float NodeOffsetXIterator = 300;
	float NodeOffsetYIterator = 0;

	using GraphData = std::pair<CDialogNode*, xr_vector<shared_str>>;
	xr_vector<GraphData> vec(NodeGraph.begin(), NodeGraph.end());

	// Сортировка по убыванию NodeName
	std::sort(vec.begin(), vec.end(), [](GraphData L, GraphData R)
	{
		return L.first->NodeName < R.first->NodeName;
	});

	for (auto& [Node, ContactsList] : vec)
	{
		int ContackID = Node->GetContactLink(true);
		NodeOffsetYIterator = Node->StartPostion.y;
		NodeOffsetXIterator = Node->StartPostion.x + 300;

		for (const shared_str& NodeName : ContactsList)
		{
			for (INodeUnknown* TryNode : Nodes)
			{
				if (TryNode->NodeName == *NodeName)
				{
					TryNode->SetStartPos(NodeOffsetXIterator, NodeOffsetYIterator);

					int NextID = TryNode->GetContactLink();
					Node->CreateContactLink(ContackID, NextID);
					NodeOffsetYIterator += 230;
				}
			}
		}

		NodeOffsetXIterator += 300;
	}

	SelectNodeEvent(nullptr);
}

void CUIDialogView::SelectNodeEvent(INodeUnknown* Node)
{
	PropItemVec items;
	UIPropertiesForm* Properties = LTools->GetProperties();
	Properties->ClearProperties();

	if (Node == nullptr)
	{
		PHelper().CreateRText(items, "Preconditions\\Has Info", &HasInfo);
		PHelper().CreateRText(items, "Preconditions\\Don't Has Info", &DontHasInfo);
		PHelper().CreateRText(items, "Preconditions\\Lua Precondition", &Precondition);
		Properties->AssignItems(items);
		return;
	}

	CDialogNode* DialogNode = (CDialogNode*)Node;

	PHelper().CreateRText(items, "Preconditions\\Has Info", &DialogNode->HasInfo);
	PHelper().CreateRText(items, "Preconditions\\Don't Has Info", &DialogNode->DontHasInfo);
	PHelper().CreateRText(items, "Preconditions\\Lua Precondition", &DialogNode->Precondition);

	PHelper().CreateRText(items, "Actions\\Give Info", &DialogNode->GiveInfo);
	PHelper().CreateRText(items, "Actions\\Lua Action", &DialogNode->Action);

	PHelper().CreateRText(items, "Text\\String ID", &DialogNode->Text);

	static shared_str TranslateStr;
	TranslateStr = Platform::ANSI_TO_UTF8(*g_pStringTable->translate(*DialogNode->Text)).c_str();
	PHelper().CreateCaption(items, "Text\\Translated", TranslateStr);

	Properties->AssignItems(items);
}

void CUIDialogView::OpenFile(const xr_path& Path)
{
	static CUIDialogView Viewer;

	Viewer.File.Load(CONFIG_PATH, "gameplay", Path.xstring().c_str());

	XML_NODE* Node = Viewer.File.GetRoot();
	if (Node == nullptr)
		return;

	if (!Viewer.bOpen)
	{
		Viewer.Show(true);
		UI->Push(&Viewer, false);
	}

	XML_NODE* ChildNode = Node->FirstChildElement();

	while (ChildNode != nullptr)
	{
		if (ChildNode->ToElement() == nullptr)
		{
			ChildNode = ChildNode->NextSibling();
			continue;
		}

		shared_str NodeID = ChildNode->ToElement()->Attribute("id");
		if (NodeID.size() == 0)
			continue;

		Viewer.Dialogs[NodeID] = ChildNode;
		ChildNode = ChildNode->NextSibling();
	}
}
