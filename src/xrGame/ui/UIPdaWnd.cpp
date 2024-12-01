#include "stdafx.h"
#include "UIPdaWnd.h"
#include "../PDA.h"

#include "../../xrUI/xrUIXmlParser.h"
#include "../../xrUI/UIXmlInit.h"
#include "UIInventoryUtilities.h"

#include "../Level.h"
#include "UIGameCustom.h"

#include "../../xrUI/Widgets/UIStatic.h"
#include "../../xrUI/Widgets/UIFrameWindow.h"
#include "../../xrUI/Widgets/UITabControl.h"
#include "UIMapWnd.h"
#include "../../xrUI/Widgets/UIFrameLineWnd.h"
#include "object_broker.h"
#include "UIMessagesWindow.h"
#include "UIMainIngameWnd.h"
#include "../../xrUI/Widgets/UITabButton.h"
#include "../../xrUI/Widgets/UIAnimatedStatic.h"

#include "../../xrUI/UIHelper.h"
#include "../../xrUI/Widgets/UIHint.h"
#include "../../xrUI/Widgets/UIBtnHint.h"
#include "UITaskWnd.h"
#include "UIRankingWnd.h"
#include "UILogsWnd.h"
#include "UIEncyclopediaWnd.h"
#include <Actor.h>
#include <Inventory.h>
#include <UIScriptWnd.h>
#include "player_hud.h"

#include "../xrEngine/XR_IOConsole.h"

#define PDA_XML		"pda.xml"

u32 g_pda_info_state = 0;

void RearrangeTabButtons(CUITabControl* pTab);
CDialogHolder* CurrentDialogHolder();

CUIPdaWnd::CUIPdaWnd()
{
	pUITaskWnd       = nullptr;
//-	pUIFactionWarWnd = nullptr;
	pUIRankingWnd    = nullptr;
	pUIEncyclopediaWnd = nullptr;
	pUILogsWnd       = nullptr;
	m_hint_wnd       = nullptr;
	m_battery_bar = nullptr;
	m_power = 0.f;
	last_cursor_pos.set(UI_BASE_WIDTH / 2.f, UI_BASE_HEIGHT / 2.f);
	m_cursor_box.set(117.f, 39.f, UI_BASE_WIDTH - 121.f, UI_BASE_HEIGHT - 37.f);
	Init();
}

CUIPdaWnd::~CUIPdaWnd()
{
	delete_data( pUITaskWnd );
//-	delete_data( pUIFactionWarWnd );
	delete_data( pUIRankingWnd );
	delete_data( pUILogsWnd );
	delete_data(pUIEncyclopediaWnd);
	delete_data( m_hint_wnd );
	delete_data( UINoice );
}

void CUIPdaWnd::Init()
{
	CUIXml					uiXml;
	uiXml.Load				(CONFIG_PATH, UI_PATH, PDA_XML);

	m_pActiveDialog			= nullptr;
	m_sActiveSection		= "";

	CUIXmlInit::InitWindow	(uiXml, "main", 0, this);

	UIMainPdaFrame			= UIHelper::CreateStatic	( uiXml, "background_static", this );
	m_caption				= UIHelper::CreateTextWnd	( uiXml, "caption_static", this );
	m_caption_const			= ( m_caption->GetText() );
	m_clock					= UIHelper::CreateTextWnd	( uiXml, "clock_wnd", this );
/*
	m_anim_static			= new CUIAnimatedStatic();
	AttachChild				(m_anim_static);
	m_anim_static->SetAutoDelete(true);
	CUIXmlInit::InitAnimatedStatic(uiXml, "anim_static", 0, m_anim_static);
*/
	m_btn_close				= UIHelper::Create3tButton( uiXml, "close_button", this );
	m_hint_wnd				= UIHelper::CreateHint( uiXml, "hint_wnd" );


	m_battery_bar = new CUIProgressBar();
	m_battery_bar->SetAutoDelete(true);
	AttachChild(m_battery_bar);
	CUIXmlInit::InitProgressBar(uiXml, "battery_bar", 0, m_battery_bar);
	m_battery_bar->Show(true);

	pUITaskWnd					= new CUITaskWnd();
	pUITaskWnd->hint_wnd		= m_hint_wnd;
	pUITaskWnd->Init			();

//-		pUIFactionWarWnd				= new CUIFactionWarWnd();
//-		pUIFactionWarWnd->hint_wnd		= m_hint_wnd;
//-		pUIFactionWarWnd->Init			();

	pUIRankingWnd					= new CUIRankingWnd();
	pUIRankingWnd->Init				();

	pUILogsWnd						= new CUILogsWnd();
	pUILogsWnd->Init				();

	pUIEncyclopediaWnd				= new CUIEncyclopediaWnd();
	pUIEncyclopediaWnd->Init		();


	UITabControl					= new CUITabControl();
	UITabControl->SetAutoDelete		(true);
	AttachChild						(UITabControl);
	CUIXmlInit::InitTabControl		(uiXml, "tab", 0, UITabControl);
	UITabControl->SetMessageTarget	(this);

	UINoice					= new CUIStatic();
	UINoice->SetAutoDelete	( true );
	CUIXmlInit::InitStatic	( uiXml, "noice_static", 0, UINoice );

//	RearrangeTabButtons		(UITabControl);
}

void CUIPdaWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	switch ( msg )
	{
	case TAB_CHANGED:
		{
			if ( pWnd == UITabControl )
			{
				SetActiveSubdialog			(UITabControl->GetActiveId());
			}
			break;
		}
	case BUTTON_CLICKED:
		{
			if ( pWnd == m_btn_close )
			{
				if (Actor()->inventory().GetActiveSlot() == PDA_SLOT)
					Actor()->inventory().Activate(NO_ACTIVE_SLOT);
				//HideDialog();
			}
			break;
		}
	default:
		{
		if (m_pActiveDialog) {
			m_pActiveDialog->SendMessage(pWnd, msg, pData);
		}
		}
	};
}

void CUIPdaWnd::Show(bool status)
{
	inherited::Show						(status);
	if(status)
	{
		InventoryUtilities::SendInfoToActor	("ui_pda");

		if (!m_sActiveSection || strcmp(m_sActiveSection.c_str(), "") == 0)
		{
			SetActiveSubdialog("eptTasks");
			UITabControl->SetActiveTab("eptTasks");
		}
		else
			SetActiveSubdialog(m_sActiveSection);

		CurrentGameUI()->HideActorMenu();
	}else
	{
		InventoryUtilities::SendInfoToActor("ui_pda_hide");

		//if (GameConstants::GetPDA_FlashingIconsQuestsEnabled())
		//	CurrentGameUI()->UIMainIngameWnd->SetFlashIconState_(CUIMainIngameWnd::efiPdaTask, false);

		CurrentGameUI()->UIMainIngameWnd->SetFlashIconState_(CUIMainIngameWnd::efiEncyclopedia, false);
		//CurrentGameUI()->UIMainIngameWnd->SetFlashIconState_(CUIMainIngameWnd::efiJournal, false);

		if (m_pActiveDialog)
		{
			m_pActiveDialog->Show(false);
			m_pActiveDialog = pUITaskWnd; //hack for script window
		}
		g_btnHint->Discard();
		g_statHint->Discard();
	}
}

void CUIPdaWnd::Update()
{
	inherited::Update();
	if (m_pActiveDialog)
		m_pActiveDialog->Update();

	m_clock->TextItemControl().SetText(InventoryUtilities::GetGameTimeAsString(InventoryUtilities::etpTimeToMinutes).c_str());

	m_battery_bar->SetProgressPos(m_power);

	Device.seqParallel.push_back(xr_make_delegate(pUILogsWnd, &CUILogsWnd::PerformWork));
}

void CUIPdaWnd::SetActiveSubdialog(const shared_str& section)
{
	if ( m_sActiveSection == section ) return;

	if ( m_pActiveDialog )
	{
		if (UIMainPdaFrame->IsChild(m_pActiveDialog))
			UIMainPdaFrame->DetachChild(m_pActiveDialog);
		m_pActiveDialog->Show( false );
	}

	if ( section == "eptTasks" )
	{
		m_pActiveDialog = pUITaskWnd;
	}
	else if (section == "eptRanking")
	{
		if (IsGameTypeSingle()) {
			m_pActiveDialog = pUIRankingWnd;
		}
	}
	else if ( section == "eptLogs" )
	{
		m_pActiveDialog = pUILogsWnd;
	}
	else if (section == "eptEnc")
	{
		m_pActiveDialog = pUIEncyclopediaWnd;
	}

	luabind::functor<CUIDialogWndEx*> functor;
	if (ai().script_engine().functor("pda.set_active_subdialog", functor))
	{
		CUIDialogWndEx* scriptWnd = functor(section.c_str());
		if (scriptWnd)
		{
			scriptWnd->SetHolder(CurrentDialogHolder());
			m_pActiveDialog = scriptWnd;
		}
	}

	if (m_pActiveDialog)
	{
		if (!UIMainPdaFrame->IsChild(m_pActiveDialog))
			UIMainPdaFrame->AttachChild(m_pActiveDialog);
		m_pActiveDialog->Show(true);
		m_sActiveSection = section;
		SetActiveCaption();
	}
	else
	{
		m_sActiveSection = "";
	}

	/*R_ASSERT2(m_pActiveDialog, "active dialog is not initialized");
	UIMainPdaFrame->AttachChild		(m_pActiveDialog);
	m_pActiveDialog->Show			(true);

	if ( UITabControl->GetActiveId() != section )
	{
		UITabControl->SetActiveTab( section );
	}
	m_sActiveSection = section;
	SetActiveCaption();*/
}

void CUIPdaWnd::SetActiveCaption()
{
	TABS_VECTOR*	btn_vec		= UITabControl->GetButtonsVector();
	TABS_VECTOR::iterator it_b	= btn_vec->begin();
	TABS_VECTOR::iterator it_e	= btn_vec->end();
	for ( ; it_b != it_e; ++it_b )
	{
		if ( (*it_b)->m_btn_id == m_sActiveSection )
		{
			LPCSTR cur = (*it_b)->TextItemControl()->GetText();
			string256 buf;
			xr_strconcat(buf, m_caption_const.c_str(), cur );
			SetCaption( buf );
			UITabControl->Show(true);
			m_clock->Show(true);
			m_caption->Show(true);
			m_battery_bar->Show(true);
			return;
		}
	}

	UITabControl->Show(false);
	m_clock->Show(false);
	m_caption->Show(false);
	m_battery_bar->Show(false);
}

void CUIPdaWnd::Show_SecondTaskWnd( bool status )
{
	if ( status )
	{
		SetActiveSubdialog( "eptTasks" );
	}
	pUITaskWnd->Show_TaskListWnd( status );
}

void CUIPdaWnd::Show_MapLegendWnd( bool status )
{
	if ( status )
	{
		SetActiveSubdialog( "eptTasks" );
	}
	pUITaskWnd->ShowMapLegend( status );
}

void CUIPdaWnd::Draw()
{
	if (Device.dwFrame == dwPDAFrame)
		return;

	dwPDAFrame = Device.dwFrame;

	inherited::Draw();
//.	DrawUpdatedSections();
	DrawHint();
	UINoice->Draw(); // over all
}

void CUIPdaWnd::DrawHint()
{
	if ( m_pActiveDialog == pUITaskWnd )
	{
		pUITaskWnd->DrawHint();
	}
//-	else if ( m_pActiveDialog == pUIFactionWarWnd )
//-	{
//		m_hint_wnd->Draw();
//-	}
	else if ( m_pActiveDialog == pUIRankingWnd )
	{
		pUIRankingWnd->DrawHint();
	}
	else if ( m_pActiveDialog == pUILogsWnd )
	{

	}
	m_hint_wnd->Draw();
}

void CUIPdaWnd::UpdatePda()
{
	pUILogsWnd->UpdateNews();

	if ( m_pActiveDialog == pUITaskWnd )
	{
		pUITaskWnd->ReloadTaskInfo();
	}
}

void CUIPdaWnd::UpdateRankingWnd()
{
	pUIRankingWnd->Update();
}

void CUIPdaWnd::Reset()
{
	inherited::ResetAll		();

	if ( pUITaskWnd )		pUITaskWnd->ResetAll();
//-	if ( pUIFactionWarWnd )	pUITaskWnd->ResetAll();
	if ( pUIRankingWnd )	pUIRankingWnd->ResetAll();
	if ( pUILogsWnd )		pUILogsWnd->ResetAll();
	if (pUIEncyclopediaWnd)		pUIEncyclopediaWnd->ResetAll();
}

void CUIPdaWnd::SetCaption( LPCSTR text )
{
	m_caption->SetText( text );
}

void RearrangeTabButtons(CUITabControl* pTab)
{
	TABS_VECTOR *	btn_vec		= pTab->GetButtonsVector();
	TABS_VECTOR::iterator it	= btn_vec->begin();
	TABS_VECTOR::iterator it_e	= btn_vec->end();

	Fvector2					pos;
	pos.set						((*it)->GetWndPos());
	float						size_x;

	for ( ; it != it_e; ++it )
	{
		(*it)->SetWndPos		(pos);
		(*it)->AdjustWidthToText();
		size_x					= (*it)->GetWndSize().x + 30.0f;
		(*it)->SetWidth			(size_x);
		pos.x					+= size_x - 6.0f;
	}
	
	pTab->SetWidth( pos.x + 5.0f );
	pos.x = pTab->GetWndPos().x - pos.x;
	pos.y = pTab->GetWndPos().y;
	pTab->SetWndPos( pos );
}

void CUIPdaWnd::Enable(bool status)
{
	if (status)
		ResetCursor();
	else
	{
		g_player_hud->reset_thumb(false);
		ResetJoystick(false);
		bButtonL = false;
		bButtonR = false;
	}

	inherited::Enable(status);
}

bool CUIPdaWnd::OnKeyboardAction(int dik, EUIMessages keyboard_action)
{
	if (WINDOW_KEY_PRESSED == keyboard_action && IsShown())
	{
		if (!psActorFlags.test(AF_3D_PDA))
		{
			EGameActions action = get_binded_action(dik);

			if (action == kQUIT || action == kINVENTORY || action == kACTIVE_JOBS)
			{
				HideDialog();
				return true;
			}
		}
		else
		{
			CPda* pda = Actor()->GetPDA();
			if (pda)
			{
				EGameActions action = get_binded_action(dik);

				if (action == kQUIT) // "Hack" to make Esc key open main menu instead of simply hiding the PDA UI
				{
					if (pda->GetState() == CPda::eHiding || pda->GetState() == CPda::eHidden)
					{
						HideDialog();
						Console->Execute("main_menu");
					}
					else if (pda->m_bZoomed)
					{
						pda->m_bZoomed = false;
						CurrentGameUI()->SetMainInputReceiver(nullptr, false);
					}
					else
						Actor()->inventory().Activate(NO_ACTIVE_SLOT);

					return true;
				}

				if (action == kUSE || action == kACTIVE_JOBS || action == kINVENTORY || (action > kCAM_ZOOM_OUT && action < kWPN_NEXT)) // Since UI no longer passes non-movement inputs to the actor input receiver this is needed now.
				{
					CObject* obj = (GameID() == eGameIDSingle) ? Level().CurrentEntity() : Level().CurrentControlEntity();
					{
						IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(obj));
						if (IR) IR->IR_OnKeyboardPress(action);
					}
					return true;
				}

				// Don't allow zoom in while draw/holster animation plays, freelook is enabled or a hand animation plays
				if (pda->IsPending())
					return false;

				// Simple PDA input mode - only allow input if PDA is zoomed in. Both left and right mouse button will zoom in instead of only right mouse button
				if (psActorFlags.test(AF_SIMPLE_PDA))
				{
					if (action == kWPN_ZOOM)
					{
						if (!pda->m_bZoomed)
						{
							Actor()->StopSprint();
						}
						else
						{
							Enable(false);
							CurrentGameUI()->SetMainInputReceiver(nullptr, false);
						}

						pda->m_bZoomed = !pda->m_bZoomed;
						return true;
					}
				}
				// "Normal" input mode, PDA input can be toggled without having to be zoomed in
				else
				{
					if (action == kWPN_ZOOM)
					{
						if (!pda->m_bZoomed)
						{
							Actor()->StopSprint();
						}
						else
							CurrentGameUI()->SetMainInputReceiver(nullptr, false);
						pda->m_bZoomed = !pda->m_bZoomed;
						return true;
					}
				}
			}
		}
	}
	return inherited::OnKeyboardAction(dik, keyboard_action);

	/*if (is_binded(kACTIVE_JOBS, dik))
	{
		if ( WINDOW_KEY_PRESSED == keyboard_action )
			HideDialog();

		return true;
	}	

	return inherited::OnKeyboardAction(dik,keyboard_action);*/
}

void CUIPdaWnd::PdaContentsChanged(pda_section::part type)
{
	if (type == pda_section::encyclopedia)
	{
		pUIEncyclopediaWnd->ReloadArticles();
		CurrentGameUI()->UIMainIngameWnd->SetFlashIconState_(CUIMainIngameWnd::efiEncyclopedia, true);

	}
	/*else if (type == pda_section::quests && GameConstants::GetPDA_FlashingIconsQuestsEnabled())
	{
		CurrentGameUI()->UIMainIngameWnd->SetFlashIconState_(CUIMainIngameWnd::efiPdaTask, true);
	}*/
}

#include "../xrUI/UICursor.h"

void CUIPdaWnd::ResetCursor()
{
	if (!last_cursor_pos.similar({ 0.f, 0.f }))
		GetUICursor().SetUICursorPosition(last_cursor_pos);
}
