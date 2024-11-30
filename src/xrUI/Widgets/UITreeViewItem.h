//=============================================================================
//  Filename:   UITreeViewItem.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  TreeView Item class
//=============================================================================
#pragma once

#include "UIListBox.h"
#include "UIListBoxItem.h"
#include "UIStatic.h"

class UI_API CUITreeViewItem:
	public CUIListBox
{
	typedef CUIListBox inherited;
	// Являемся ли мы началом подыерархии
	bool			isRoot;
	// Если мы рут, то этот флаг показывает открыта наша подыерархия или нет
	bool			isOpened;
	// Смещение в пробелах
	int				iTextShift;
	// Кому мы пренадлежим
	CUITreeViewItem *pOwner;
	//произвольное число, приписанное объекту
	int m_iValue;
public:
	void			SetRoot(bool set);
	bool			IsRoot() const						{ return isRoot; }

	// Устанавливаем смещение текста
	void			SetTextShift(int delta)				{ iTextShift += delta; }

	int GetValue() { return m_iValue; }
	void SetValue(int value) { m_iValue = value; }

	// Владелец
	CUITreeViewItem * GetOwner() const					{ return pOwner; }
	void			SetOwner(CUITreeViewItem *owner)	{ pOwner = owner; }
protected:
	// Функция вызываемая при изменении свойства рута
	// для изменения визуального представления себя
	virtual void	OnRootChanged();
public:
	// Раксрыть/свернуть локальнцю иерархию
	void			Open();
	void			Close();
	bool			IsOpened() const					{ return isOpened; }
protected:
	// Функция вызываемая при изменении cостояния открыто/закрыто 
	// для изменения визуального представления себя
	virtual void	OnOpenClose();
public:
    
	// Список элементов, которые уровнем ниже нас
	typedef			xr_vector<CUITreeViewItem*>		SubItems;
	typedef			SubItems::iterator				SubItems_it;
	SubItems		vSubItems;

	CUIStatic		UIBkg;

	// Добавить элемент
	void AddItem(CUITreeViewItem *pItem);
	// Удалить все
	void DeleteAllSubItems();
	// Найти элемент с заданным именем
	// Return:	указатель на элемент, если нашли, либо NULL в противном случае
	CUITreeViewItem * Find(LPCSTR text) const;
	// Найти элемент с заданным значением
	// Return:	указатель на элемент, если нашли, либо NULL в противном случае
	CUITreeViewItem * Find(int value) const;
	// Найти заданный элемент
	// Return:	указатель на элемент, если нашли, либо NULL в противном случае
	CUITreeViewItem * Find(CUITreeViewItem *pItem) const;
	// Вернуть иерархию от верха до текущего элемента в виде строки-пути
	// Рутовые элементы заканчиваются символом "/"
	xr_string GetHierarchyAsText();

	// Redefine some stuff
	// ATTENTION! Для корректного функционирования значков [+-] вызов SetText
	// Должен предшествовать SetRoot
	virtual void SetText(LPCSTR str);
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);

	// Ctor and Dtor
	CUITreeViewItem();
	virtual		~CUITreeViewItem();

	// Устанавливаем цвет текста в зависимости от того, прочитан ли артикл
	void	MarkArticleAsRead(bool value);
	bool	IsArticleReaded() const;
	// Цвет текста когда артикл не прочитан и не прочитан
	void	SetReadedColor(u32 cl)		{ m_uReadedColor = cl;		}
	void	SetUnreadedColor(u32 cl)	{ m_uUnreadedColor = cl;	}
	void	SetManualSetColor(bool val)	{ m_bManualSetColor = val;	}
	// Устанавливаем цвет в зависимости от состояния элемента
	void	SetItemColor()
	{
		m_bArticleRead ? SetTextColor(m_uReadedColor) :SetTextColor(m_uUnreadedColor);
	}

private:
	friend class CUITreeViewItem;

	// Применить состояние вверх по иерархии
	void	CheckParentMark(CUITreeViewItem *pOwner);
	// Цвет текста когда артикл не прочитан
	u32		m_uUnreadedColor;
	// Цвет текста когда артикл не прочитан
	u32		m_uReadedColor;
	// Флажек состояния прочитки
	bool	m_bArticleRead;
	// Если true, то MarkArticleAsRead не будет вызывать
	// SetItemColor()
	bool	m_bManualSetColor;
};

//////////////////////////////////////////////////////////////////////////
//  Function for automatic tree hierarchy creation
//////////////////////////////////////////////////////////////////////////

using GroupTree = xr_vector<shared_str>;
using GroupTree_it = GroupTree::iterator;

//////////////////////////////////////////////////////////////////////////

UI_API void CreateTreeBranch(shared_str nestingTree, shared_str leafName, CUIListBox *pListToAdd, int leafProperty,
					  CGameFont *pRootFont, u32 rootColor, CGameFont *pLeafFont, u32 leafColor, bool markRead);
