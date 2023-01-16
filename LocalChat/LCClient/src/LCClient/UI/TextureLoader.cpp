#include <stb/stb_image.h>

#include <LCClient/UI/TextureLoader.h>

bool TextureLoader::IsLoaded(TextureType textureName)
{
	return m_TextureIDsMap[(size_t)textureName].has_value();
}

std::optional<GLuint> TextureLoader::GetTextureID(TextureType textureName)
{
	return m_TextureIDsMap[(size_t)textureName];
}

bool TextureLoader::LoadImage(_In_ const char* path, _Out_ Image& img)
{
	auto& [width, height, channels, data] = img;
	data = stbi_load(path, &width, &height, &channels, 0);
	
	return data != nullptr;
}

void TextureLoader::UploadToGPU(_In_ const Image& img, _Out_ GLuint& textureID)
{
	auto& [width, height, channels, data] = img;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void 
TextureLoader::LoadAllTextures(
	const std::array<const char*, (size_t)TextureType::COUNT>& paths)
{
	for (size_t type = 0; type < paths.size(); type++)
	{
		if (!paths[type])
			continue;

		Image& img = m_Images[type];
		if (LoadImage(paths[type], img))
		{
			m_TextureIDsMap[type] = 0;
			UploadToGPU(img, m_TextureIDsMap[type].value());
		}
	}
}