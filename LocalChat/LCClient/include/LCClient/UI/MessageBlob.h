#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include <imgui.h>
#include <Glad/gl.h>
#include <direct.h>
#include <stb/stb_image.h>

#include <LCClient/UI/UIElement.h>
#include <Logger/Logger.h>
#include <Message/Message.h>
#include <Message/TextMessage.h>

class MessageBlob MAKE_UI_ELEMENT(MessageBlob)
public:
	MessageBlob(const Message& msg)
		:BaseClassTp(ImGui::GetIO())
		,m_Message(msg), m_TextureID(0)
		,m_Size()
	{
		OnCreate();
	}

	void OnCreate()
	{
		switch (m_Message.GetType())
		{
			case MessageType::MSG_TEXT:
			{
				// Calculate width and height of the message blob
				ImVec2 contentRegionSz = ImGui::GetContentRegionAvail();
				m_Size = { contentRegionSz.x / 2, 300.0f };
				const TextMessage& msgTxt = static_cast<const TextMessage&>(m_Message);
				const std::wstring_view content = msgTxt.GetContent();

				size_t numChars = content.size();
				Logger::logfmt<Log::INFO>("Message Length: %ld", numChars);
				Logger::logfmt<Log::INFO>("{%.3f, %.3f}", io.Fonts->TexWidth, io.Fonts->TexHeight);
				break;
			}
		}
	}

	void OnImGuiRender()
	{
	}

	void SetTextureID(GLuint textureID) { if (textureID != m_TextureID) m_TextureID = textureID; }

	void CustomImGuiRender(bool renderRight = false)
	{
		ImVec2 contentRegionSz = ImGui::GetContentRegionAvail();
		m_Size = { contentRegionSz.x / 2, (float) GetBlobHeight(contentRegionSz.x / 2)};

		ImVec2 size = m_Size;
		const float width = size.x;
		const float height = size.y;

		if (!renderRight)
		{
			const ImVec2 oldCursorPos = ImGui::GetCursorPos();

			ImGui::Image((ImTextureID)m_TextureID, size);

			const ImVec2 finalCursorPos = ImGui::GetCursorPos();

			const float leftMargin = 25.0f;
			const float topMargin = 5.0f;

			ImGui::SetCursorPos({ oldCursorPos.x + leftMargin, oldCursorPos.y + topMargin });
			
			// text formatting if its a text message
			switch (m_Message.GetType())
			{
				case MessageType::MSG_TEXT:
				{
					const TextMessage& tm = static_cast<const TextMessage&>(m_Message);
					
					const std::string content(tm.GetContent().begin(), tm.GetContent().end());
					ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + width);
					ImGui::TextColored(ImVec4(0.1, 0.1, 0.1, 1.0), content.c_str());
					ImGui::PopTextWrapPos();

					break;
				}
			}

			// restore the cursor position
			ImGui::SetCursorPos(finalCursorPos);
		}
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
	size_t GetBlobHeight(const float currentBlobWidth);
private:
	const Message& m_Message;
	ImVec2 m_Size;
	GLuint m_TextureID;
};

size_t MessageBlob::GetBlobHeight(const float currentBlobWidth)
{
	const float minBlobHeight = 100.0f;
	float blobHeight;
	switch (m_Message.GetType())
	{
		case MessageType::MSG_TEXT:
		{
			blobHeight = static_cast<const TextMessage&>(m_Message).GetContent().size();
			break;
		}
	}

	return max(minBlobHeight, blobHeight);
}