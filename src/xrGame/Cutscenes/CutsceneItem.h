#pragma once

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
private:
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
	Fvector Deviation;
public:
    ~CCutsceneItem();
    
    void Construct(LPCSTR Section);
    void Activate();
    LPCSTR GetName();
    SCutsceneObjectElement* CreateObjectElement(LPCSTR ObjectName);
    void SetPivotObject(CObject* PivotObject);
    CObject* GetPivotObject(){return PivotObject;}
};
