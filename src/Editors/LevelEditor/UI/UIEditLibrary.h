#pragma once

class UIEditLibrary :
    public XrUI
{
public:
    UIEditLibrary();
    virtual ~UIEditLibrary();

    static void Update();
    static void Show();
    static void Close();
    static void OnRender();

    ImTextureID m_RealTexture;

    void OnItemFocused(ListItem* item);

private:
    static UIEditLibrary* Form;

    virtual void Draw();
    void ImportClick();
    void DrawObjects();

    void DrawRightBar();
    void DrawObject(CCustomObject* obj, const char* name);
    void InitObjects();
    void OnPropertiesClick();
    void OnMakeThmClick();
    void OnPreviewClick();

    void MakeLOD(bool highQuality);
    void GenerateLOD(RStringVec& props, bool bHighQuality);

    void RefreshSelected();
    void ChangeReference(const RStringVec& items);
    bool SelectionToReference(ListItemsVec* props);
    void ShowProperty();
    void ExportOneOBJ(CEditableObject* EO);
    void ExportObj();

    UIItemListForm* m_ObjectList;
    UIPropertiesForm* m_Props;
    LPCSTR m_Current;
    bool m_Preview;
    ListItem* m_Selected;

    bool m_SelectLods;
    bool m_HighQualityLod;

    xr_vector<CSceneObject*> m_pEditObjects;
};