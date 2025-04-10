#include "stdafx.h"
#include "CutsceneItem.h"

#include "ai_space.h"
#include "Kinematics.h"
#include "KinematicsAnimated.h"

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
    auto B = HudModelKinematicsAnimated->PlayCycle(M2, true, &OnFinishFunc, this);
    B->update_callback = false;
}

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

void CCutsceneItem::SetPivotObject(CObject* PivotObject)
{
    this->PivotObject = PivotObject;
}

SCutsceneObjectElement::~SCutsceneObjectElement()
{
    ::Render->model_Delete(HudModel);
    HudModel = nullptr;
    HudModelKinematics = nullptr;
    HudModelKinematicsAnimated = nullptr;
}
