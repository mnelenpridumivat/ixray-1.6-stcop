#pragma once

class ESceneSoundSrcTool: public ESceneCustomOTool
{
	typedef ESceneCustomOTool inherited;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						ESceneSoundSrcTool		():ESceneCustomOTool(OBJCLASS_SOUND_SRC){;}
	// definition
    IC LPCSTR			ClassName				(){return "sound_src";}
    IC LPCSTR			ClassDesc				(){return "Sound Source";}
    IC int				RenderPriority			(){return 10;}

    virtual void		Clear					(bool bSpecific=false){inherited::Clear(bSpecific);}
    // IO
    virtual bool   		IsNeedSave				(){return inherited::IsNeedSave();}
    virtual bool   		LoadStream            	(IReader&);
	virtual bool   		LoadLTX            		(CInifile&);
	virtual bool   		LoadJSON            		(nlohmann::json&);
    virtual void   		SaveStream            	(IWriter&);
    virtual void   		SaveLTX            		(CInifile&, int id);
    virtual void   		SaveJSON            		(nlohmann::json&, int id);
    virtual bool		LoadSelection      		(IReader&);
    virtual void		SaveSelection      		(IWriter&);

    virtual CCustomObject* CreateObject			(LPVOID data, LPCSTR name);
};