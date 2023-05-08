#pragma once

#include <imgui.h>

#include <LCClient/UI/UIElement.h>
#include <LCClient/UI/MessageBlob.h>
#include <Message/Message.h>
#include <LCClient/LCClient.h>

#include <memory>
#include <imgui_internal.h>


class MessageBlob;

class Chat MAKE_UI_ELEMENT(Chat)

	using MessageUPtr = std::unique_ptr<Message>;
public:
	// Client Hash: The hash of the client represented by the current chat
	Chat(uint64_t clientHash, const char* displayName = nullptr);

	void PushIncomingTextMsg(std::unique_ptr<Message> msg);
	
	void OnCreate();
	void OnImGuiRender();

	bool IsClickedAtOnce() const { return m_IsClicked; }
	void SetClicked() { m_IsClicked = true; }

private:
	static int TextResizeCallback(ImGuiInputTextCallbackData* data);

	static bool CustomInputLineEx(const char* label, const char* hint, ImVector<char>* my_str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0);

private:
	bool m_IsClicked;
	uint64_t m_ClientHash;
	const char* m_DisplayName;
	std::vector<MessageUPtr> m_Messages;
	std::vector<std::unique_ptr<MessageBlob>> m_Blobs;
};