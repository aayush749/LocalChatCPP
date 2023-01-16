#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include <imgui.h>
#include <Glad/gl.h>
#include <direct.h>
#include <stb/stb_image.h>
#include <LCClient/UI/UIElement.h>
#include <Logger/Logger.h>
#include <Message/Message.h>

class MessageBlob MAKE_UI_ELEMENT(MessageBlob)
public:
	MessageBlob(const Message& msg)
		:BaseClassTp(ImGui::GetIO())
		,m_Message(msg)
		,m_ImgData(nullptr)
	{
		LoadImageFromFile("../../../../LocalChat/LCClient/res/images/chat/chat_bubble_out.png");
		if (m_ImgData == nullptr)
		{
			char curPath[1024] = { 0 };
			_getcwd(curPath, sizeof(curPath));
			Logger::logfmt<Log::ERR>("Could not load message blob texture, current path is : \"%s\"", curPath);
			throw std::runtime_error("Can't load texture");
		}

		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);

		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_ImgData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void OnCreate()
	{
	}

	void OnImGuiRender()
	{
	}

	void CustomImGuiRender()
	{
		ImGui::Image((ImTextureID)m_TextureID, ImVec2(300, 300));
	}

	~MessageBlob()
	{
		if (!m_ImgData)
		{
			glDeleteTextures(1, &m_TextureID);
			stbi_image_free(m_ImgData);
			m_ImgData = nullptr;
			m_Width = m_Height = m_Channels = 0;
		}
	}

private:
	void LoadImageFromFile(const char* path);
private:
	const Message& m_Message;
	GLuint m_TextureID;
	int m_Width, m_Height, m_Channels = 4;
	unsigned char* m_ImgData;
};

void MessageBlob::LoadImageFromFile(const char* path)
{
	m_ImgData = stbi_load(path, &m_Width, &m_Height, &m_Channels, 0);
}