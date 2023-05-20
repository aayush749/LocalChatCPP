#pragma once

#include <imgui.h>
#include <Glad/gl.h>
#include <direct.h>
#include <stb/stb_image.h>

#include <LCClient/UI/UIElement.h>
#include <Message/Message.h>

class MessageBlob MAKE_UI_ELEMENT(MessageBlob)
public:
	MessageBlob(std::unique_ptr<Message> msg, bool renderRight = true);

	void OnCreate();

	void OnImGuiRender();

	void SetTextureID(GLuint textureID) { if (textureID != m_TextureID) m_TextureID = textureID; }

	std::wstring GetSerializedContent();

	void Serialize(_Inout_ std::wstring& buffer);

	const uint64_t GetSenderHash() const { return m_MessagePtr->GetSenderHash(); }
	const uint64_t GetReceiverHash() const {	return m_MessagePtr->GetRecipientHash();	}
private:
	void RenderTowardsLeft(ImVec2 size);
	void RenderTowardsRight(ImVec2 size);
private:
	inline static ImFont* s_TxtMsgFont = nullptr;

private:
	size_t GetBlobHeight(const float currentBlobWidth);
private:
	const std::unique_ptr<Message> m_MessagePtr;
	ImVec2 m_Size;
	GLuint m_TextureID;
	bool m_RenderRight;
};