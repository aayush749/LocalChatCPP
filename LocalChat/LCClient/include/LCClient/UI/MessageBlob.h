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
#include <LCClient/UI/FontManager.h>

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
		if (s_TxtMsgFont)
			return;

		s_TxtMsgFont = FontManager::GetFont(FontUsage::FONT_USAGE_MESSAGE_BLOB);
	}

	void OnImGuiRender()
	{
	}

	void SetTextureID(GLuint textureID) { if (textureID != m_TextureID) m_TextureID = textureID; }

	void CustomImGuiRender(bool renderRight = false)
	{
		ImGui::PushFont(s_TxtMsgFont);

		ImVec2 contentRegionSz = ImGui::GetContentRegionAvail();
		m_Size = { contentRegionSz.x / 2, (float) GetBlobHeight(contentRegionSz.x / 2)};

		ImVec2 size = m_Size;
		const float width = size.x;
		const float height = size.y;

		if (!renderRight)
		{
			RenderTowardsLeft(size);
		}
		else
		{
			RenderTowardsRight(size);
		}

		ImGui::PopFont();
	}

private:
	void RenderTowardsLeft(ImVec2 size);
	void RenderTowardsRight(ImVec2 size);
private:
	inline static ImFont* s_TxtMsgFont = nullptr;

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

void MessageBlob::RenderTowardsLeft(ImVec2 size)
{
	const auto [width, height] = size;

	const ImVec2 oldCursorPos = ImGui::GetCursorPos();

	ImGui::Image((ImTextureID)m_TextureID, size);

	const ImVec2 finalCursorPos = ImGui::GetCursorPos();

	const float leftMarginPerc = 8.23;
	const float topMarginPerc = 8.23;

	float leftMargin = width * leftMarginPerc / 100.0f;
	float topMargin = height * topMarginPerc / 100.0f;

	ImGui::SetCursorPos({ oldCursorPos.x + leftMargin, oldCursorPos.y + topMargin });

	// text formatting if its a text message
	switch (m_Message.GetType())
	{
	case MessageType::MSG_TEXT:
	{
		const TextMessage& tm = static_cast<const TextMessage&>(m_Message);

		static float percentage = 0.90;
		const std::string content(tm.GetContent().begin(), tm.GetContent().end());
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + percentage * width);
		ImGui::TextColored(ImVec4(0.1, 0.1, 0.1, 1.0), content.c_str());
		ImGui::PopTextWrapPos();

		break;
	}
	}

	// restore the cursor position
	ImGui::SetCursorPos(finalCursorPos);
}

void MessageBlob::RenderTowardsRight(ImVec2 size)
{
	const auto [width, height] = size;

	float cursorPosX = ImGui::GetCursorPosX();
	float newX = ImGui::GetContentRegionMax().x - width;

	ImGui::SetCursorPosX(newX);

	ImVec2 oldCursorPos = ImGui::GetCursorPos();

	// draw blob
	ImGui::Image((ImTextureID)m_TextureID, size);

	ImVec2 newCursorPos = ImGui::GetCursorPos();

	const float leftMarginPerc = 8.23;
	const float topMarginPerc = 8.23;

	float leftMargin = width * leftMarginPerc / 100.0f;
	float topMargin = height * topMarginPerc / 100.0f;

	ImGui::SetCursorPos({ oldCursorPos.x + leftMargin, oldCursorPos.y + topMargin });

	// text formatting if its a text message
	switch (m_Message.GetType())
	{
	case MessageType::MSG_TEXT:
	{
		const TextMessage& tm = static_cast<const TextMessage&>(m_Message);

		static float percentage = 0.80;
		const std::string content(tm.GetContent().begin(), tm.GetContent().end());
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + percentage * width);
		ImGui::TextColored(ImVec4(0.1, 0.1, 0.1, 1.0), content.c_str());
		ImGui::PopTextWrapPos();

		break;
	}
	}

	//reset cursor pos
	ImGui::SetCursorPos(newCursorPos);
}