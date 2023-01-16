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
	{}

	void OnCreate()
	{
	}

	void OnImGuiRender()
	{
	}

	void SetTextureID(GLuint textureID) { if (textureID != m_TextureID) m_TextureID = textureID; }

	void CustomImGuiRender(bool renderRight = false)
	{
		static float width, height;
		width = 300.0f;
		height = 300.0f;
		static ImVec2 size = { width, height };
		
		if (!renderRight)
			ImGui::Image((ImTextureID)m_TextureID, size);
		else
		{
			float cursorPosX = ImGui::GetCursorPosX();
			float newX = ImGui::GetContentRegionMax().x - width;

			ImGui::SetCursorPosX(newX);

			// draw blob
			ImGui::Image((ImTextureID)m_TextureID, size);

			//reset cursor pos
			ImGui::SetCursorPosX(cursorPosX);
		}
	}

private:
	const Message& m_Message;
	GLuint m_TextureID;
};