#pragma once

#include <glad/gl.h>
#include <LCClient/UI/TextureType.h>

#include <array>
#include <optional>

class TextureLoader
{
	struct Image
	{
		int m_Width, m_Height, m_Channels = 4;
		unsigned char* m_Data = nullptr;
	};
public:
	TextureLoader() = default;
	TextureLoader(const TextureLoader&) = delete;
	TextureLoader(TextureLoader&&) = delete;
	
	void LoadAllTextures(const std::array<const char*, (size_t) TextureType::COUNT>& paths);
	bool IsLoaded(TextureType textureName);
	std::optional<GLuint> GetTextureID(TextureType textureName);

private:
	bool LoadImage(_In_ const char* path, _Out_ Image& img);
	void UploadToGPU(_In_ const Image& img, _Out_ GLuint& textureID);
private:
	std::array<std::optional<GLuint>, (size_t) TextureType::COUNT> m_TextureIDsMap;
	std::array<Image, (size_t)TextureType::COUNT> m_Images;
};