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
	static void SetFont(FontUsage usage, const char* fontPath, float size);

	static ImFont* GetFont(FontUsage usage);
private:
	inline static std::array<ImFont*, (size_t) FontUsage::NUM_FONT_USAGE> s_FontsCache;
};