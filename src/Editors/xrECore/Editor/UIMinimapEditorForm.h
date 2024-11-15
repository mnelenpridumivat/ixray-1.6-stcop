//---------------------------------------------------------------------------
#pragma once

class UIMinimapEditorForm:
	public IEditorWnd
{
private:
	struct Element {
		xr_string name;
		ImVec2 position;
		ImVec2 RenderSize;
		ImVec2 FileSize;
		ImTextureID	Texture;
		xr_string TexturePath;
		bool selected = false;
		bool locked = false;
		bool hidden = false;
	};

	template<typename T>
	T Clamp(T value, T min, T max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}
public:
	UIMinimapEditorForm();
	virtual ~UIMinimapEditorForm();
	virtual void Draw();

public:
	static void Update();
	static void Show();

private:
	static UIMinimapEditorForm* Form;

private:
	void LoadBGClick(const xr_string = "");
	int LoadTexture(Element&, const xr_string = "");

	void ShowMenu();
	void RenderCanvas();
	void ShowElementList();
	void ShowPreview();
	void UnselectAllElements();
	void SelectElement(Element&);

	void CreateElementPopup();

	void SaveFile(bool);
	void OpenFile();

	void ReloadMapInfo(const xr_string& fn);
	void ReloadLevelsList();

	bool GetTextureFromLevelLtx(const xr_string, xr_string&);

	xr_vector<Element> elements;
	xr_vector<xr_string> levels;

	bool m_AlwaysDrawBorder = false;
	bool isDragging = false;
	bool PreviewMode = false;

	float m_Zoom = 1.0f;
	int m_ItemsOpacity = 255;

	ImVec2 initial_bg_position;
	ImVec2 initial_mouse_pos;
	
	Element* selectedElement=nullptr;

	xr_string ActiveFile;
	xr_string ActiveFileShort;
	bool isEdited;

	Element CreatingData;
	bool showMPLevels = false;
private:
	ImTextureID					m_BackgroundTexture;
	ImTextureID					m_TextureRemove;
	//
	ImVec2		m_BackgroundPosition;
	ImVec2		m_BackgroundRenderSize;
	ImVec2		m_BackgroundSize;
	//
	U32Vec      m_ImageData;
	xr_string	m_BackgroundTexturePath;
};