//=============================================================================
//  Filename:   UIEncyclopediaWnd.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Encyclopedia window
//=============================================================================

#include "stdafx.h"
#include "UIEncyclopediaWnd.h"
#include "../xrUI/UIXmlInit.h"
#include "../xrUI/Widgets/UIFrameWindow.h"
#include "../xrUI/Widgets/UIFrameLineWnd.h"
//#include "UIFrameLine.h"
#include "../xrUI/Widgets/UIAnimatedStatic.h"
#include "../xrUI/UIHelper.h"
#include "../xrUI/Widgets/UIStatic.h"
#include "../xrUI/Widgets/UIScrollView.h"
#include "../xrUI/Widgets/UIListBox.h"
#include "../xrUI/Widgets/UIListWnd.h"
#include "../xrUI/Widgets/UITreeViewItem.h"
#include "UIEncyclopediaArticleWnd.h"
#include "../encyclopedia_article.h"
#include "../alife_registry_wrappers.h"
#include "../actor.h"
#include "object_broker.h"
#include "../xrUI/Widgets/UITabControl.h"

#define				ENCYCLOPEDIA_DIALOG_XML		"pda_encyclopedia.xml"
#define				ENCYCLOPEDIA_DIALOG_ITEM_XML		"pda_encyclopedia_item.xml"

CUIEncyclopediaWnd::CUIEncyclopediaWnd()
{
	prevArticlesCount = 0;
}

CUIEncyclopediaWnd::~CUIEncyclopediaWnd()
{
	DeleteArticles();
}


void CUIEncyclopediaWnd::Init()
{
	CUIXml				uiXml;
	uiXml.Load(CONFIG_PATH, UI_PATH, ENCYCLOPEDIA_DIALOG_XML);

	CUIXmlInit			xml_init;

	xml_init.InitWindow(uiXml, "main_wnd", 0, this);

	// Load xml data

	if(uiXml.NavigateToNode("background", 0))
	{
		UIBackground = UIHelper::CreateFrameWindow(uiXml, "background", this);
	}

	if(uiXml.NavigateToNode("tab_background", 0))
	{
		UITabBackground = UIHelper::CreateFrameWindow(uiXml, "tab_background", this);
	}

	if(uiXml.NavigateToNode("tab", 0))
	{
		UITabControl					= new CUITabControl();
		UITabControl->SetAutoDelete		(true);
		AttachChild						(UITabControl);
		CUIXmlInit::InitTabControl		(uiXml, "tab", 0, UITabControl);
		UITabControl->SetMessageTarget	(this);
	}
	
	if(uiXml.NavigateToNode("left_background", 0))
	{
		m_left_background = UIHelper::CreateFrameWindow(uiXml, "left_background", this, false);
	}
	
	if(uiXml.NavigateToNode("right_background", 0))
	{
		m_right_background = UIHelper::CreateFrameWindow(uiXml, "right_background", this, false);
		if(uiXml.NavigateToNode("article_header_static", 0))
		{
			UIArticleHeader = UIHelper::CreateStatic(uiXml, "article_header_static", m_right_background);
		}
	}

	if(uiXml.NavigateToNode("left_background1", 0))
	{
		UIEncyclopediaIdxBkg = UIHelper::CreateFrameLine(uiXml, "left_background1", this);
		if(uiXml.NavigateToNode("left_caption", 0))
		{
			UIEncyclopediaIdxHeader = UIHelper::CreateStatic(uiXml, "left_caption", UIEncyclopediaIdxBkg);
		}
	}

	if(uiXml.NavigateToNode("right_background1", 0))
	{
		UIEncyclopediaInfoBkg = UIHelper::CreateFrameLine(uiXml, "right_background1", this);
		if(uiXml.NavigateToNode("right_caption", 0))
		{
			UIEncyclopediaInfoHeader = UIHelper::CreateStatic(uiXml, "right_caption", UIEncyclopediaInfoBkg);
		}
	}


	if(uiXml.NavigateToNode("idx_list", 0))
	{
		UIIdxList = new CUIListWnd(); UIIdxList->SetAutoDelete(true);
		if(m_left_background)
		{
			m_left_background->AttachChild(UIIdxList);
		}
		CUIXmlInit::InitListWnd(uiXml, "idx_list", 0, UIIdxList);
		UIIdxList->SetMessageTarget(this);
	}

	if(uiXml.NavigateToNode("info_list", 0))
	{
		UIInfoList = new CUIScrollView();
		UIInfoList->SetAutoDelete(true);
		m_right_background->AttachChild(UIInfoList);
		CUIXmlInit::InitScrollView(uiXml, "info_list", 0, UIInfoList);
	}

	if(uiXml.NavigateToNode("tree_item_font", 0))
	{
		CUIXmlInit::InitFont(uiXml, "tree_item_font", 0, m_uTreeItemColor, m_pTreeItemFont);
		R_ASSERT(m_pTreeItemFont);
	}
	if(uiXml.NavigateToNode("tree_root_font", 0))
	{
		CUIXmlInit::InitFont(uiXml, "tree_root_font", 0, m_uTreeRootColor, m_pTreeRootFont);
		R_ASSERT(m_pTreeRootFont);
	}
}

#include "../string_table.h"
void CUIEncyclopediaWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (UIIdxList == pWnd && LIST_ITEM_CLICKED == msg)
	{
		CUITreeViewItem* pTVItem = (CUITreeViewItem*)(pData);
		R_ASSERT(pTVItem);

		if (pTVItem->vSubItems.size())
		{
			VERIFY(m_ArticlesFull.contains(CurrentArticleType));
			auto& ArticlesDB = m_ArticlesFull[CurrentArticleType];
			
			CEncyclopediaArticle* A = ArticlesDB[pTVItem->vSubItems[0]->GetValue()].first;

			xr_string caption = "# ";
			std::string str(A->data()->group.c_str());

			std::vector<std::string>splitParts;
			if (!splitParts.empty())
				splitParts.clear();
			size_t pos = str.find("/");
			while (pos != std::string::npos)
			{
				splitParts.push_back(str.substr(0, pos));
				str.erase(0, pos + 1);
				pos = str.find("/");
			}
			splitParts.push_back(str);
			for (size_t i = 0; i < splitParts.size(); i++)
			{
				const std::string& part = splitParts[i];
				string4096 pp;
				xr_sprintf(pp, "%s", part.c_str());
				caption += CStringTable().translate(pp).c_str();

				if (i + 1 != splitParts.size())
					caption += "/";
			}

			//UIEncyclopediaInfoHeader->TextItemControl()->SetText(caption.c_str());
			//UIArticleHeader->SetText(caption.c_str());
			SetCurrentArtice(nullptr);
		}
		else
		{
			int idx = pTVItem->GetValue();
			if (idx == -1) return;
			
			VERIFY(m_ArticlesFull.contains(CurrentArticleType));
			auto& ArticlesDB = m_ArticlesFull[CurrentArticleType];
			
			CEncyclopediaArticle* A = ArticlesDB[idx].first;
			xr_string caption = "# ";
			std::string str(A->data()->group.c_str());

			std::vector<std::string>splitParts;
			if (!splitParts.empty())
				splitParts.clear();
			size_t pos = str.find("/");
			while (pos != std::string::npos)
			{
				splitParts.push_back(str.substr(0, pos));
				str.erase(0, pos + 1);
				pos = str.find("/");
			}
			splitParts.push_back(str);
			for (const std::string& part : splitParts)
			{
				string4096				pp;
				xr_sprintf(pp, "%s", part.c_str());
				caption += CStringTable().translate(pp).c_str();
				caption += "/";
			}

			caption += CStringTable().translate(A->data()->name).c_str();

			//UIEncyclopediaInfoHeader->TextItemControl()->SetText(caption.c_str());
			SetCurrentArtice(pTVItem);
			UIArticleHeader->TextItemControl()->SetText(CStringTable().translate(A->data()->name).c_str());
		}
	}

	if(pWnd == UITabControl && msg == TAB_CHANGED)
	{
		SetActiveSubdialog(UITabControl->GetActiveId());
	}

	inherited::SendMessage(pWnd, msg, pData);
}

void CUIEncyclopediaWnd::Draw()
{

	if (m_flags.test(eNeedReload))
	{
		if (Actor() && Actor()->encyclopedia_registry->registry().objects_ptr() && Actor()->encyclopedia_registry->registry().objects_ptr()->size() > prevArticlesCount)
		{
			ARTICLE_VECTOR::const_iterator it = Actor()->encyclopedia_registry->registry().objects_ptr()->begin();
			std::advance(it, prevArticlesCount);
			for (; it != Actor()->encyclopedia_registry->registry().objects_ptr()->end(); it++)
			{
				AddArticle(it->article_type, it->article_id, it->readed);
			}
			prevArticlesCount = Actor()->encyclopedia_registry->registry().objects_ptr()->size();
		}

		m_flags.set(eNeedReload, FALSE);
	}

	inherited::Draw();
}

void CUIEncyclopediaWnd::ReloadArticles()
{
	m_flags.set(eNeedReload, TRUE);
}


void CUIEncyclopediaWnd::Show(bool status)
{
	if (status)
		ReloadArticles();

	inherited::Show(status);
}


bool CUIEncyclopediaWnd::HasArticle(shared_str id)
{
	ReloadArticles();
	for(const auto& ArticleGroup : m_ArticlesFull)
	{
		for(const auto& Article : ArticleGroup.second)
		{
			if(Article.first->Id() == id)
			{
				return true;
			}
		}
	}
	return false;
}


void CUIEncyclopediaWnd::DeleteArticles()
{
	//UIIdxList->Clear();
	UIIdxList->RemoveAll();
	for(const auto& ArticleGroup : m_ArticlesFull)
	{
		for(const auto& Article : ArticleGroup.second)
		{
			auto ptr = Article.first;
			xr_delete(ptr);
		}
	}
	m_ArticlesFull.clear();
}

void CUIEncyclopediaWnd::SetCurrentArtice(CUITreeViewItem* pTVItem)
{
	UIInfoList->ScrollToBegin();
	UIInfoList->Clear();

	if (!pTVItem) return;

	// ��� ������ ��������, ��� ������� ������� �� �������
	if (!pTVItem->IsRoot())
	{
		VERIFY(m_ArticlesFull.contains(CurrentArticleType));
		auto& CurrentArticleDB = m_ArticlesFull[CurrentArticleType];
		
		CUIEncyclopediaArticleWnd* article_info = new CUIEncyclopediaArticleWnd();
		article_info->Init(ENCYCLOPEDIA_DIALOG_ITEM_XML, "encyclopedia_wnd:objective_item");
		article_info->SetArticle(CurrentArticleDB[pTVItem->GetValue()].first);
		UIInfoList->AddWindow(article_info, true);

		// ������� ��� �����������
		if (!pTVItem->IsArticleReaded())
		{
			if (Actor()->encyclopedia_registry->registry().objects_ptr())
			{
				for (ARTICLE_VECTOR::iterator it = Actor()->encyclopedia_registry->registry().objects().begin();
					it != Actor()->encyclopedia_registry->registry().objects().end(); it++)
				{
					if (CurrentArticleType == it->article_type &&
						CurrentArticleDB[pTVItem->GetValue()].first->Id() == it->article_id)
					{
						it->readed = true;
						CurrentArticleDB[pTVItem->GetValue()].second = true;
						break;
					}
				}
			}
		}
	}
}

void CUIEncyclopediaWnd::SetActiveSubdialog(const shared_str& section)
{
	if ( section == "tasks" )
	{
		CurrentArticleType = ARTICLE_DATA::eTaskArticle;
	}
	else if ( section == "journal" )
	{
		CurrentArticleType = ARTICLE_DATA::eJournalArticle;
	}
	else if (section == "info")
	{
		CurrentArticleType = ARTICLE_DATA::eInfoArticle;
	}
	else if ( section == "enc" )
	{
		CurrentArticleType = ARTICLE_DATA::eEncyclopediaArticle;
	}
	SetActiveArticlesType(CurrentArticleType);
}

void CUIEncyclopediaWnd::SetActiveArticlesType(ARTICLE_DATA::EArticleType articleType)
{
	UIIdxList->RemoveAll();
	auto ArticleDB = m_ArticlesFull.find(articleType);
	if (ArticleDB == m_ArticlesFull.end())
	{
		return;
	}
	for(u64 i = 0; i < ArticleDB->second.size(); i++)
	{
		const auto& articlePair = ArticleDB->second[i];
		const auto& article = articlePair.first;
		CreateTreeBranch(article->data()->group, article->data()->name, UIIdxList, i,
			m_pTreeRootFont, m_uTreeRootColor, m_pTreeItemFont, m_uTreeItemColor, articlePair.second);
	}
}

void CUIEncyclopediaWnd::AddArticle(ARTICLE_DATA::EArticleType articleType, shared_str article_id, bool bReaded)
{
	if(!m_ArticlesFull.contains(articleType))
	{
		m_ArticlesFull[articleType] = {};
	}
	auto& ArticlesDB = m_ArticlesFull[articleType];
	for (std::size_t i = 0; i < ArticlesDB.size(); i++)
	{
		if (ArticlesDB[i].first->Id() == article_id) return;
	}

	// ��������� �������
	ArticlesDB.resize(ArticlesDB.size() + 1);
	auto& NewArticle = ArticlesDB.back();
	CEncyclopediaArticle*& a = NewArticle.first;
	a = new CEncyclopediaArticle();
	a->Load(article_id);
	NewArticle.second = bReaded;

	// ������ ������� �������� ���� �� ��������� ����

	//CreateTreeBranch(a->data()->group, a->data()->name, UIIdxList, m_ArticlesDB.size() - 1,
	//	m_pTreeRootFont, m_uTreeRootColor, m_pTreeItemFont, m_uTreeItemColor, bReaded);
	
	SetActiveArticlesType(CurrentArticleType);
}

void CUIEncyclopediaWnd::ResetAll()
{
	inherited::ResetAll();
	ReloadArticles();
}
