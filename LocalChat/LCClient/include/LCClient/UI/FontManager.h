#pragma once

#include <imgui.h>

#include <array>

enum class FontUsage
{
	FONT_USAGE_CONVERSATION_LIST,
	FONT_USAGE_MESSAGE_BLOB,
	NUM_FONT_USAGE
};

class FontManager
{
public:
	static void SetFont(FontUsage usage, const char* fontPath, float size)
	{
		s_FontsCache[(size_t)usage] = ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPath, size);
	}

	static ImFont* GetFont(FontUsage usage)
	{
		return s_FontsCache[(size_t)usage];
	}
	
private:
	inline static std::array<ImFont*, (size_t) FontUsage::NUM_FONT_USAGE> s_FontsCache;
};