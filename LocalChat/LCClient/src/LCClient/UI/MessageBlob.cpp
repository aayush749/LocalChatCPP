#include <LCClient/UI/MessageBlob.h>

#include <LCClient/UI/TextureLoader.h>
#include <LCClient/UI/TextureType.h>
#include <LCClient/UI/FontManager.h>
#include <Message/TextMessage.h>
#include <Logger/Logger.h>

extern TextureLoader GLOBAL_TEX_LOADER;

MessageBlob::MessageBlob(std::unique_ptr<Message> msg, bool renderRight)
	:BaseClassTp(ImGui::GetIO())
	, m_MessagePtr(std::move(msg)), m_TextureID(0)
	, m_Size()
	, m_RenderRight(renderRight)
{
	OnCreate();
}

void MessageBlob::OnCreate()
{
	if (m_RenderRight)
		SetTextureID(GLOBAL_TEX_LOADER.GetTextureID(TextureType::CHAT_BUBBLE_OUT).value());
	else
		SetTextureID(GLOBAL_TEX_LOADER.GetTextureID(TextureType::CHAT_BUBBLE_IN).value());

	if (s_TxtMsgFont)
		return;

	s_TxtMsgFont = FontManager::GetFont(FontUsage::FONT_USAGE_MESSAGE_BLOB);
}

void MessageBlob::OnImGuiRender()
{
	ImGui::PushFont(s_TxtMsgFont);

	ImVec2 contentRegionSz = ImGui::GetContentRegionAvail();
	m_Size = { contentRegionSz.x / 2, (float)GetBlobHeight(contentRegionSz.x / 2) };

	ImVec2 size = m_Size;
	const float width = size.x;
	const float height = size.y;

	if (!m_RenderRight)
	{
		RenderTowardsLeft(size);
	}
	else
	{
		RenderTowardsRight(size);
	}

	ImGui::PopFont();
}

size_t MessageBlob::GetBlobHeight(const float currentBlobWidth)
{
	const float minBlobHeight = 100.0f;
	float blobHeight;
	switch (m_MessagePtr->GetType())
	{
	case MessageType::MSG_TEXT:
	{
		blobHeight = static_cast<const TextMessage&>(*m_MessagePtr).GetContent().size();
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
	switch (m_MessagePtr->GetType())
	{
	case MessageType::MSG_TEXT:
	{
		const TextMessage& tm = static_cast<const TextMessage&>(*m_MessagePtr);

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
	switch (m_MessagePtr->GetType())
	{
	case MessageType::MSG_TEXT:
	{
		const TextMessage& tm = static_cast<const TextMessage&>(*m_MessagePtr);

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

std::wstring MessageBlob::GetSerializedContent()
{
	std::wstring serialized;

	m_MessagePtr->Serialize(serialized);

	return serialized;
}