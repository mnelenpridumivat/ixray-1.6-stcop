#include "stdafx.h"
#include "CutsceneItem.h"

#include "ai_space.h"
#include "Kinematics.h"
#include "KinematicsAnimated.h"
#include "script_game_object.h"

SCutsceneObjectElement::SCutsceneObjectElement(LPCSTR ObjectName)
{
    HudModel = ::Render->model_Create(ObjectName);
    R_ASSERT3(HudModel, "Unable to find object", ObjectName);
    HudModelKinematics = smart_cast<IKinematics*>(HudModel);
    HudModelKinematicsAnimated = smart_cast<IKinematicsAnimated*>(HudModel);
}

void SCutsceneObjectElement::SetAllBonesVisibility(bool Visibility)
{
    R_ASSERT2(HudModelKinematics, "You need to create object first before set bones visibility");
	VisMask vis_mask;
    if (Visibility)
    {
        vis_mask.one();
    } else
    {
        vis_mask.zero();
    }
    HudModelKinematics->LL_SetBonesVisible(vis_mask);
}

void SCutsceneObjectElement::SetBoneVisibility(u16 BoneID, bool Visibility)
{
    R_ASSERT2(HudModelKinematics, "You need to create object first before set bones visibility");
    HudModelKinematics->LL_SetBoneVisible(BoneID, Visibility, true);
}

void SCutsceneObjectElement::SetParent(SCutsceneObjectElement* Parent, u16 BoneID)
{
    parent = Parent;
    AttachBoneID = BoneID;
}

void SCutsceneObjectElement::Activate()
{
    R_ASSERT2(HudModelKinematicsAnimated, "You need to create object first before set bones visibility");
    MotionID M2 = HudModelKinematicsAnimated->ID_Cycle_Safe(AnimName.c_str());
    if (bDebug) {
        Msg("playing item animation [%s]", AnimName.c_str());
    }
	R_ASSERT4(M2.valid(), "model has no motion", HudModel->getDebugName().c_str(), AnimName.c_str());
#ifndef MASTER_GOLD
    m_pBlend = HudModelKinematicsAnimated->PlayCycle(M2, true, &OnFinishFunc, this);
#else
    auto m_pBlend = HudModelKinematicsAnimated->PlayCycle(M2, true, &OnFinishFunc, this);
#endif
    m_pBlend->update_callback = false;
}

void SCutsceneObjectElement::Update(Fvector Deviation)
{
    Fmatrix m_transform;
    m_transform.identity();
    m_transform.c = Deviation;
    HudModelKinematics->CalculateBones(true);
    ::Render->set_Transform(&m_transform);
    ::Render->add_Visual(HudModel, true);
}

#ifndef MASTER_GOLD
void SCutsceneObjectElement::StopAnimation()
{
    m_pBlend->playing = false;
}

void SCutsceneObjectElement::ForwardAnimation()
{
    m_pBlend->playing = true;
    if(m_pBlend->speed < 0.f)
        m_pBlend->speed = -m_pBlend->speed;
}

void SCutsceneObjectElement::BackwardAnimation()
{
    m_pBlend->playing = true;
    if(m_pBlend->speed > 0.f)
        m_pBlend->speed = -m_pBlend->speed;
}
#endif

void SCutsceneObjectElement::SetAnimToPlay(LPCSTR AnimName)
{
    this->AnimName = AnimName;
}

void SCutsceneObjectElement::SetOnFinishFunc(LPCSTR Name)
{
    OnFinishFuncName = Name;
}

void SCutsceneObjectElement::OnFinishFunc(CBlend* P)
{
    VERIFY(P);
    auto Self = (SCutsceneObjectElement*)P->CallbackParam;
    VERIFY(Self);
    
    luabind::functor<void> funct;
    if (ai().script_engine().functor(pSettings->r_string(Self->OnFinishFuncName.c_str(), "construct_func"), funct))
    {
        funct(Self);
    }
}

CCutsceneItem::~CCutsceneItem()
{
    for(auto& elem : CutsceneElements)
    {
        xr_delete(elem);
    }
    CutsceneElements.clear();
}

void CCutsceneItem::Construct(LPCSTR Section)
{
    NameSect = Section;
    luabind::functor<void> funct;
    if (ai().script_engine().functor(pSettings->r_string(Section, "construct_func"), funct)) {
        funct(this);
    } else
    {
        R_ASSERT2(false, "Cutscene construct failed", Section);
    }
    
}

void CCutsceneItem::Activate()
{
    for(auto& elem : CutsceneElements)
    {
        elem->Activate();
    }
}

void CCutsceneItem::Update()
{
    R_ASSERT(GetPivotObject());
    auto pos = GetPivotObject() ? GetPivotObject()->Position() : Fvector(0, 0, 0);
    for (auto& elem : CutsceneElements)
    {
        elem->Update(pos);
    }
}

LPCSTR CCutsceneItem::GetName()
{
    return NameSect.c_str();
}

SCutsceneObjectElement* CCutsceneItem::CreateObjectElement(LPCSTR ObjectName)
{
    auto RetValue = new SCutsceneObjectElement(ObjectName);
    CutsceneElements.push_back(RetValue);
    return RetValue;
}

void CCutsceneItem::SetPivotObject(CScriptGameObject* PivotObject)
{
    this->PivotObject = &PivotObject->object();
}

#ifndef MASTER_GOLD
void CCutsceneItem::StopAnimation()
{
    for(auto& elem : CutsceneElements)
    {
        elem->StopAnimation();
    }
}

void CCutsceneItem::ForwardAnimation()
{
    for(auto& elem : CutsceneElements)
    {
        elem->ForwardAnimation();
    }
}

void CCutsceneItem::BackwardAnimation()
{
    for(auto& elem : CutsceneElements)
    {
        elem->BackwardAnimation();
    }
}
#endif

SCutsceneObjectElement::~SCutsceneObjectElement()
{
    ::Render->model_Delete(HudModel);
    HudModel = nullptr;
    HudModelKinematics = nullptr;
    HudModelKinematicsAnimated = nullptr;
}
