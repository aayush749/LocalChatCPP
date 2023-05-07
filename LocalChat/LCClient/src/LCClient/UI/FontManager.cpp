#include <LCClient/UI/FontManager.h>

void FontManager::SetFont(FontUsage usage, const char* fontPath, float size)
{
	s_FontsCache[(size_t)usage] = ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPath, size);
}

ImFont* FontManager::GetFont(FontUsage usage)
{
	return s_FontsCache[(size_t)usage];
}