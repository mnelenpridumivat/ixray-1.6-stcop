
#include "stdafx.h"
#include "GameFont.h"
#include "string_table.h"

#include "../xrCore/API/xrAPI.h"
#include "../Include/xrRender/RenderFactory.h"
#include "../Include/xrRender/FontRender.h"
#include <freetype/freetype.h>

FT_Library FreetypeLib = nullptr;

bool CGameFont::bFreetypeInitialized = false;

void CGameFont::InitializeFreetype()
{
	FT_Error Error = FT_Init_FreeType(&FreetypeLib);
	R_ASSERT2(Error == 0, "Freetype2 initialize failed");
}

#define DI2PX(x) float(iFloor((x + 1) * float(Device.TargetWidth) * 0.5f))
#define DI2PY(y) float(iFloor((y + 1) * float(Device.TargetHeight) * 0.5f))

ENGINE_API Fvector2	g_current_font_scale = { 1.0f, 1.0f };

CGameFont::CGameFont(const char* section, u32 flags) : Name(section)
{
#ifdef DEBUG
	Msg("* Init font %s", section);
#endif

	uFlags = flags;

	// Read font name
	if (pSettings->line_exist(section, "name"))
		Data.Name = xr_strdup(xr_strlwr((char*)pSettings->r_string(section, "name")));

	if (!Data.Name || !xr_strlen(Data.Name))
		Data.Name = xr_strdup(section);

	// Set font shader and style
	Data.Shader = xr_strdup(pSettings->r_string(section, "shader"));

	Data.Style = nullptr;
	if (pSettings->line_exist(section, "style"))
		Data.Style = xr_strdup(pSettings->r_string(section, "style"));

	// Read font size
	Data.Size = 14;
	if (pSettings->line_exist(section, "size"))
		Data.Size = (u16)pSettings->r_u32(section, "size");
	
	if (pSettings->line_exist(section, "opentype"))
		Data.OpenType = pSettings->r_bool(section, "opentype");

	if (pSettings->line_exist(section, "letter_spacing"))
		LetterSpacing = pSettings->r_float(section, "letter_spacing");

	if (pSettings->line_exist(section, "line_spacing"))
		LineSpacing = pSettings->r_float(section, "line_spacing");

	// Init
	pFontRender = RenderFactory->CreateFontRender();
	Prepare(Data.Name, Data.Shader, Data.Style, Data.Size);
}

CGameFont::~CGameFont()
{
	// Shading
	RenderFactory->DestroyFontRender(pFontRender);
	pFontRender = nullptr;

	xr_free(Data.Shader);
	xr_free(Data.Style);
	xr_free(Data.Name);
}

void CGameFont::ReInit()
{
	Prepare(Data.Name, Data.Shader, Data.Style, Data.Size);
}

void CGameFont::Prepare(const char* name, const char* shader, const char* style, u32 size)
{
	Initialize2(name, shader, style, size);
}

wchar_t TranslateSymbolUsingCP1251(char Symbol);

xr_vector<xr_string> split(const xr_string& s, char delim)
{
	xr_vector<xr_string> elems;
	std::stringstream ss(s);
	xr_string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return std::move(elems);
}
#include <freetype/ftfntfmt.h>

constexpr u32 TextureDimension = 2048 * 2;
static u32 FontBitmap[TextureDimension * TextureDimension] = {};

void CGameFont::Initialize2(const char* name, const char* shader, const char* style, u32 size)
{
	if (!bFreetypeInitialized)
	{
		InitializeFreetype();
		bFreetypeInitialized = true;
	}

	ZeroMemory(&Style, sizeof(Style));
	Size = size;

	if (style != nullptr)
	{
		xr_string StyleDesc(style);
		xr_vector<xr_string> StyleTokens = split(StyleDesc, '|');
		for (const xr_string& token : StyleTokens)
		{
			if (token == "bold")
			{
				Style.bold = 1;
			}
			else if (token == "italic")
			{
				Style.italic = 1;
			}
			else if (token == "underline")
			{
				Style.underline = 1;
			}
			else if (token == "strike")
			{
				Style.strike = 1;
			}
		}
	}
	ZeroMemory(FontBitmap, sizeof(FontBitmap));

	// есть кучу способов высчитать размер шрифта для скейлинга
	// 1. основываясь на DPI(PPI), однако, как не вычисляй его он всегда считается исходя из разрешения моника(системы) и 23 дюймов(мб с дровами на моник - из реальных дюймов)
	// 2. основываясь на том, как ПЫС делают скейлинг из UI_BASE_HEIGHT/UI_BASE_WIDTH и тд...

	SetProcessDPIAware();
	HDC hDCScreen = GetDC(NULL);

	auto Hmm = (float)GetDeviceCaps(hDCScreen, VERTSIZE);
	auto Wmm = (float)GetDeviceCaps(hDCScreen, HORZSIZE);
	auto Hpx = (float)GetDeviceCaps(hDCScreen, VERTRES);
	auto Wpx = (float)GetDeviceCaps(hDCScreen, HORZRES);

	ReleaseDC(NULL, hDCScreen);

	auto is_res_depend = !!READ_IF_EXISTS(pSettings, r_bool, Name, "res_depend", TRUE);
	auto is_dpi_depend = !!READ_IF_EXISTS(pSettings, r_bool, Name, "dpi_depend", !is_res_depend);

	auto ppi = int(25.4f * sqrt(Hpx * Hpx + Wpx * Wpx) / sqrt(Hmm * Hmm + Wmm * Wmm));

	auto res_scale = is_res_depend ? float(Device.TargetHeight) / 900.0f : 1.0f;
	auto ppi_scale = is_dpi_depend ? float(ppi) / 92.0f : 1.0f;

	auto fHeight = float(size * res_scale * ppi_scale);

	CStringTable::LangName();
	xr_string NameWithExt = CStringTable::LangName() + "\\" + name + ".ttf";

	string_path FullPath;
	FS.update_path(FullPath, _game_fonts_, NameWithExt.c_str());

	IReader* FontFile = FS.r_open(FullPath);
	if (FontFile == nullptr)
	{
		Msg("! Can't open font file %s", name);

		string_path DefPath;
		NameWithExt = CStringTable::LangName() + "\\default.ttf";
		FS.update_path(DefPath, _game_fonts_, NameWithExt.c_str());
		FontFile = FS.r_open(DefPath);

		R_ASSERT3(FontFile != nullptr, "Can't find default font: %s", DefPath);
	}

	FT_Face OurFont;
	FT_Error FTError = FT_New_Memory_Face(FreetypeLib, (FT_Byte*)FontFile->pointer(), FontFile->length(), 0, &OurFont);
	R_ASSERT3(FTError == 0, "FT_New_Memory_Face return error", FullPath);

	u32 TargetX = 0;
	u32 TargetY = 0;
	u32 TargetX2 = 0;
	u32 TargetY2 = 0;

	FT_Size_RequestRec req;
	req.type = FT_SIZE_REQUEST_TYPE_CELL;
	req.width = 0;
	req.height = (uint32_t)fHeight * 64;
	req.horiResolution = 0;
	req.vertResolution = 0;
	FT_Request_Size(OurFont, &req);

#define FT_CEIL(X)  (((X + 63) & -64) / 64)

	float FontSizeInPixels = (float)FT_CEIL(OurFont->size->metrics.height);

	auto CopyGlyphImageToAtlas = [this, &TargetX, &TargetX2, &TargetY, &TargetY2, FontSizeInPixels](FT_Bitmap& GlyphBitmap)
		{
			TargetX2 = TargetX + GlyphBitmap.width;
			if (TargetX2 >= TextureDimension)
			{
				TargetX = 0;
				TargetX2 = GlyphBitmap.width;
				TargetY += (u32)FontSizeInPixels + 5;

				R_ASSERT2(TargetY <= TextureDimension, "Font too large, or dimension texture is too small");
			}

			u32 SourceX = 0;
			u32 SourceY = 0;

			TargetY2 = TargetY + (u32)FontSizeInPixels;
			u32 TargetYSaved = TargetY;
			TargetY = TargetY + u32(FontSizeInPixels - (float)GlyphBitmap.rows);

			switch (GlyphBitmap.pixel_mode)
			{
			case FT_PIXEL_MODE_GRAY:
			{
				u8* SourceImage = GlyphBitmap.buffer;
				for (u32 y = TargetY; y < TargetY2; y++, SourceY++)
				{
					for (u32 x = TargetX; x < TargetX2; x++, SourceX++)
					{
						u8 SourcePixel = SourceImage[(SourceY * GlyphBitmap.pitch) + SourceX];

						u32 FinalPixel = SourcePixel;
						FinalPixel |= (SourcePixel << 8);
						FinalPixel |= (SourcePixel << 16);
						FinalPixel |= (SourcePixel << 24);

						FontBitmap[(y * TextureDimension) + x] = FinalPixel;
					}
					SourceX = 0;
				}
			}
			break;
			case FT_PIXEL_MODE_BGRA:
			{
				u32* SourceImage = (u32*)GlyphBitmap.buffer;
				for (u32 y = TargetY; y < TargetY2; y++, SourceY++)
				{
					for (u32 x = TargetX; x < TargetX2; x++, SourceX++)
					{
						u32 SourcePixel = SourceImage[(SourceY * GlyphBitmap.pitch) + SourceX];

						u8 Alpha = (SourcePixel & 0x000000FF);
						u8 Red = (SourcePixel & 0x0000FF00) >> 8;
						u8 Green = (SourcePixel & 0x00FF0000) >> 16;
						u8 Blue = (SourcePixel & 0xFF000000) >> 24;

						u32 FinalPixel = Alpha;
						FinalPixel |= (u32(Blue) << 8);
						FinalPixel |= (u32(Green) << 16);
						FinalPixel |= (u32(Red) << 24);

						FontBitmap[(y * TextureDimension) + x] = FinalPixel;
					}
					SourceX = 0;
				}
			}
			break;
			default:
				break;
			}

			TargetY = TargetYSaved;
		};

	//const char* Format = FT_Get_Font_Format(OurFont);
	u32 index = 0;

	auto LoadGlyph = [&](FT_UInt glyphID)
	{
		u32 TrueGlyph = glyphID;
		if (Data.OpenType)
		{
			TrueGlyph = TranslateSymbolUsingCP1251((char)glyphID);
		}

		FT_UInt FreetypeCharacter = FT_Get_Char_Index(OurFont, TrueGlyph);

		FTError = FT_Load_Glyph(OurFont, FreetypeCharacter, FT_LOAD_RENDER);
		R_ASSERT3(FTError == 0, "FT_Load_Glyph return error", FullPath);
		FT_GlyphSlot Glyph = OurFont->glyph;
		FT_Glyph_Metrics& GlyphMetrics = Glyph->metrics;

		CopyGlyphImageToAtlas(Glyph->bitmap);

		RECT region;
		region.left = TargetX;
		region.right = TargetX + Glyph->bitmap.width;
		region.top = TargetY;
		region.bottom = long(TargetY + FontSizeInPixels);

		ABC widths;
		widths.abcA = FT_CEIL(GlyphMetrics.horiBearingX);
		widths.abcB = Glyph->bitmap.width;
		widths.abcC = FT_CEIL(GlyphMetrics.horiAdvance) - widths.abcB - widths.abcA;

		int GlyphTopScanlineOffset = int(FontSizeInPixels - Glyph->bitmap.rows);
		int yOffset = -Glyph->bitmap_top - GlyphTopScanlineOffset;
		yOffset += (int)FontSizeInPixels; // Return back to the center pos
		yOffset -= (int)(FontSizeInPixels / 4);

		GlyphData[glyphID] = { region, widths, yOffset };

		TargetX = TargetX2;
		TargetX += 4;
	};

	auto glyphID = FT_Get_First_Char(OurFont, &index);
	while(index != 0)
	{
		LoadGlyph(glyphID);
		glyphID = FT_Get_Next_Char(OurFont, glyphID, &index);
	}

	FT_Done_Face(OurFont);
	fCurrentHeight = FontSizeInPixels;

	string128 textureName;
	xr_sprintf(textureName, "$user$%s", Name); //#TODO optimize

	auto TargetDemensionY = 16u;

	while (TargetDemensionY < TargetY2) {
		TargetDemensionY *= 2u;
	}
#ifdef DEBUG
	Msg("* Font %s Y size [%d - %d]", Name, TargetDemensionY, TargetY2);
#endif
	R_ASSERT2(TargetDemensionY <= TextureDimension, "Font too large, or dimension texture is too small");

	pFontRender->CreateFontAtlas(TextureDimension, TargetDemensionY, textureName, FontBitmap);

	FS.r_close(FontFile);
	pFontRender->Initialize(shader, textureName);
}

void CGameFont::OutSet(float x, float y)
{
	fCurrentX = x;
	fCurrentY = y;
}

void CGameFont::OutSetI(float x, float y)
{
	OutSet(DI2PX(x), DI2PY(y));
}

void CGameFont::OnRender()
{
	PROF_EVENT("Render Font");

	pFontRender->OnRender(*this);
	if (!strings.empty())
		strings.resize(0);
}

u16 CGameFont::GetCutLengthPos(float fTargetWidth, const char* pszText)
{

	return 0;
}

u16 CGameFont::SplitByWidth(u16* puBuffer, u16 uBufferSize, float fTargetWidth, const char* pszText)
{

	return 0;
}

void CGameFont::MasterOut(
	BOOL bCheckDevice, BOOL bUseCoords, BOOL bScaleCoords, BOOL bUseSkip,
	float _x, float _y, float _skip, const char* fmt, va_list p)
{
	if (bCheckDevice && (!Device.b_is_Active))
		return;

	String rs;

	rs.x = (bUseCoords ? (bScaleCoords ? (DI2PX(_x)) : _x) : fCurrentX);
	rs.y = (bUseCoords ? (bScaleCoords ? (DI2PY(_y)) : _y) : fCurrentY);
	rs.c = dwCurrentColor;
	rs.height = fCurrentHeight;
	rs.align = eCurrentAlignment;
	int vs_sz = vsprintf(rs.string, fmt, p);

	if (!IsUTF8(rs.string))
	{
		rs.string_utf8 = Platform::ANSI_TO_UTF8(rs.string);
	}
	
	rs.string[sizeof(rs.string) - 1] = 0;
	if (vs_sz == -1)
	{
		return;
	}

	if (vs_sz)
		strings.push_back(rs);

	if (bUseSkip)
		OutSkip(_skip);
}

#define MASTER_OUT(CHECK_DEVICE,USE_COORDS,SCALE_COORDS,USE_SKIP,X,Y,SKIP,FMT) \
	{ va_list p; va_start ( p , fmt ); \
	  MasterOut( CHECK_DEVICE , USE_COORDS , SCALE_COORDS , USE_SKIP , X , Y , SKIP , FMT, p ); \
	  va_end( p ); }

void CGameFont::OutI(float _x, float _y, const char* fmt, ...)
{
	MASTER_OUT(FALSE, TRUE, TRUE, FALSE, _x, _y, 0.0f, fmt);
};

void CGameFont::Out(float _x, float _y, const char* fmt, ...)
{
	MASTER_OUT(TRUE, TRUE, FALSE, FALSE, _x, _y, 0.0f, fmt);
};

void CGameFont::OutNext(const char* fmt, ...)
{
	MASTER_OUT(TRUE, FALSE, FALSE, TRUE, 0.0f, 0.0f, 1.0f, fmt);
};

void CGameFont::OutSkip(float val)
{
	fCurrentY += val * CurrentHeight_();
}

float CGameFont::SizeOf_(int cChar)
{
	if (cChar < 0)
	{
		cChar = u8(cChar);
	}

	return static_cast<float>(WidthOf(cChar));
}

float CGameFont::SizeOf_(const char* s)
{
	return static_cast<float> (WidthOf(s));
}

float CGameFont::SizeOf_(const wide_char* wsStr)
{
	return 0;
}

float CGameFont::CurrentHeight_()
{
	return GetHeight();
}

void CGameFont::SetHeight(float S)
{
	if (uFlags & fsDeviceIndependent)
	{
		fCurrentHeight = S;
	}
}

const CGameFont::Glyph* CGameFont::GetGlyphInfo(int ch)
{
	auto symbolInfoIterator = GlyphData.find(ch);
	if (symbolInfoIterator == GlyphData.end())
	{
		return nullptr;
	}

	return &symbolInfoIterator->second;
}

int CGameFont::WidthOf(int ch)
{
	const Glyph* glyphInfo = GetGlyphInfo(ch);
	return glyphInfo ? (glyphInfo->Abc.abcA + glyphInfo->Abc.abcB + glyphInfo->Abc.abcC) : 5;
}

int CGameFont::WidthOf(const char* str)
{
	if (!str || !str[0])
	{
		return 0;
	}

	int size = 0;

	if (IsUTF8(str))
	{
		auto asda = Platform::ANSI_TO_TCHAR(str);
		int length = std::wcslen(asda);
		for (int i = 0; i < length; i++)
		{
			size += WidthOf(asda[i]);
		}

	}
	else
	{
		int length = xr_strlen(str);
		for (int i = 0; i < length; i++)
		{
			size += WidthOf((u8)str[i]);
		}
	}

	return size;
}

wchar_t CP1251ConvertationTable[] =
{
	0x0402, // Ђ
	0x0403, // Ѓ
	0x201A, // ‚
	0x0453, // ѓ
	0x201E, // „
	0x2026, // …
	0x2020, // †
	0x2021, // ‡
	0x20AC, // €
	0x2030, // ‰
	0x0409, // Љ
	0x2039, // ‹
	0x040A, // Њ
	0x040C, // Ќ
	0x040B, // Ћ
	0x040F, // Џ

	0x0452, // ђ
	0x2018, // ‘
	0x2019, // ’
	0x201C, // “
	0x201D, // ”
	0x2022, // •
	0x2013, // –
	0x2014, // —
	0x0,    // empty (0x98)
	0x2122, // ™
	0x0459, // љ
	0x203A, // ›
	0x045A, // њ
	0x045C, // ќ
	0x045B, // ћ
	0x045F, // џ

	0x00A0, //  
	0x040E, // Ў
	0x045E, // ў
	0x0408, // Ј
	0x00A4, // ¤
	0x0490, // Ґ
	0x00A6, // ¦
	0x00A7, // §
	0x0401, // Ё
	0x00A9, // ©
	0x0404, // Є
	0x00AB, // «
	0x00AC, // ¬
	0x00AD, // 
	0x00AE, // ®
	0x0407, // Ї

	0x00B0, // °
	0x00B1, // ±
	0x0406, // І
	0x0456, // і
	0x0491, // ґ
	0x00B5, // µ
	0x00B6, // ¶
	0x00B7, // ·
	0x0451, // ё
	0x2116, // №
	0x0454, // є
	0x00BB, // »
	0x0458, // ј
	0x0405, // Ѕ
	0x0455, // ѕ
	0x0457, // ї
};

wchar_t TranslateSymbolUsingCP1251(char Symbol)
{
	unsigned char RawSymbol = *(unsigned char*)&Symbol;

	if (RawSymbol < 0x80)
	{
		return wchar_t(RawSymbol);
	}

	if (RawSymbol < 0xc0)
	{
		return CP1251ConvertationTable[RawSymbol - 0x80];
	}

	return wchar_t(RawSymbol - 0xc0) + 0x410;
}