#pragma once

class CScriptGameObject;

struct SCutsceneObjectElement
{
    SCutsceneObjectElement(LPCSTR ObjectName);
    ~SCutsceneObjectElement();
    void SetAllBonesVisibility(bool Visibility);
    void SetBoneVisibility(u16 BoneID, bool Visibility);
    void SetParent(SCutsceneObjectElement* Parent, u16 BoneID);
    void SetAnimToPlay(LPCSTR AnimName);
    void SetOnFinishFunc(LPCSTR Name);

    void Activate();
    void Update(Fvector Deviation);
    
#ifndef MASTER_GOLD
    void StopAnimation();
    void ForwardAnimation();
    void BackwardAnimation();
#endif
    
private:
#ifndef MASTER_GOLD
    CBlend* m_pBlend = nullptr;
#endif
    IKinematicsAnimated* HudModelKinematicsAnimated = nullptr;
    IKinematics* HudModelKinematics = nullptr;
    IRenderVisual* HudModel = nullptr;
    
    SCutsceneObjectElement* parent = nullptr;
    u16 AttachBoneID = u16(-1);

    shared_str AnimName;
    shared_str OnFinishFuncName = "nil";

    static void OnFinishFunc(CBlend* P);
};

class CCutsceneItem
{
    CObject* PivotObject = nullptr;
    shared_str NameSect;
    xr_vector<SCutsceneObjectElement*> CutsceneElements;
public:
    ~CCutsceneItem();
    
    void Construct(LPCSTR Section);
    void Activate();
    void Update();
    LPCSTR GetName();
    SCutsceneObjectElement* CreateObjectElement(LPCSTR ObjectName);
    void SetPivotObject(CScriptGameObject* PivotObject);
    CObject* GetPivotObject() const {return PivotObject;}
    
#ifndef MASTER_GOLD
    void StopAnimation();
    void ForwardAnimation();
    void BackwardAnimation();
#endif
};
