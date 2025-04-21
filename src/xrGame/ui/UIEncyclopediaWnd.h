//=============================================================================
//  Filename:   UIEncyclopediaWnd.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Encyclopedia window
//=============================================================================

#pragma once

#include "../xrUI/Widgets/UIWindow.h"

#include "../encyclopedia_article_defs.h"

class CUITabControl;
class CUIListWnd;
class CEncyclopediaArticle;
class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIAnimatedStatic;
class CUIStatic;
class CUIListBox;
class CUIEncyclopediaCore;
class CUIScrollView;
class CUITreeViewItem;
class CUITabControl;
class CUIListWnd;

class CUIEncyclopediaWnd : public CUIWindow
{
private:
	typedef CUIWindow inherited;
	enum { eNeedReload = (1 << 0), };
	Flags16				m_flags;
public:
	CUIEncyclopediaWnd();
	~CUIEncyclopediaWnd() override;

	virtual void		Init();
	void		Show(bool status) override;
	void		SendMessage(CUIWindow* pWnd, s16 msg, void* pData = nullptr) override;
	void		Draw() override;

	void				AddArticle(ARTICLE_DATA::EArticleType articleType, shared_str, bool bReaded);
	void				DeleteArticles();
	bool				HasArticle(shared_str);

	void				ReloadArticles();
	virtual void		ResetAll();
protected:
	u32					prevArticlesCount;
	// �������� ������������ ����������

	CUIFrameWindow* UIBackground = nullptr;
	CUIFrameWindow* UITabBackground = nullptr;
	CUITabControl* UITabControl = nullptr;
	CUIFrameWindow*		m_left_background = nullptr;
	CUIFrameWindow*		m_right_background = nullptr;
	
	CUIFrameLineWnd* UIEncyclopediaIdxBkg = nullptr;
	CUIFrameLineWnd* UIEncyclopediaInfoBkg = nullptr;
	
	CUIStatic* UIEncyclopediaIdxHeader = nullptr;
	CUIStatic* UIEncyclopediaInfoHeader = nullptr;
	CUIAnimatedStatic* UIAnimation = nullptr;
	CUIStatic* UIArticleHeader = nullptr;

	// ��������� ������
	//typedef xr_vector<xr_pair<CEncyclopediaArticle*, bool>>			ArticlesDB;
	//typedef ArticlesDB::iterator						ArticlesDB_it;

	using ArticlesDB = xr_vector<xr_pair<CEncyclopediaArticle*, bool>>;
	using ArticlesDB_it = ArticlesDB::iterator;
	using ArticlesFullDB = xr_map<ARTICLE_DATA::EArticleType, ArticlesDB>;

	ArticlesFullDB			m_ArticlesFull;
	CGameFont* m_pTreeRootFont;
	u32						m_uTreeRootColor;
	CGameFont* m_pTreeItemFont;
	u32						m_uTreeItemColor;

	CUIListWnd* UIIdxList;
	CUIScrollView*			UIInfoList;

	ARTICLE_DATA::EArticleType CurrentArticleType = ARTICLE_DATA::eTaskArticle;

	void				SetCurrentArtice(CUITreeViewItem* pTVItem);
	void			SetActiveSubdialog	(const shared_str& section);
	void SetActiveArticlesType(ARTICLE_DATA::EArticleType articleType);
};
