#pragma once

#include <imgui.h>

#include <LCClient/UI/UIElement.h>
#include <LCClient/UI/MessageBlob.h>
#include <Logger/Logger.h>
#include <Message/Message.h>
#include <Message/TextMessage.h>

#include <memory>

class Chat MAKE_UI_ELEMENT(Chat)

	using MessageUPtr = std::unique_ptr<Message>;
public:
	Chat(uint64_t clientHash)
		:BaseClassTp(ImGui::GetIO())
		,m_ClientHash(clientHash)
	{}

	void PushMsg(MessageUPtr& msg)
	{
		m_Messages.push_back(std::move(msg));
	}

	void OnCreate()
	{
		
	}

	void OnImGuiRender()
	{
		if (ImGui::Begin("Chat Window"))
		{
			for (const MessageUPtr& message : m_Messages)
			{
				if (message->GetSenderHash() == m_ClientHash)
				{
					// Message was sent by current client
					// So, Render at right
					static MessageBlob mblob(*message);
				}
				else
				{
					// Message was sent by different client
					// So, Render at left
				}
			}

			// Just Testing, don't ship with this
			static TextMessage tm(1001, 1002, L"Simple Text Message");
			static MessageBlob mBlob[10] = { MessageBlob(tm), MessageBlob(tm), MessageBlob(tm), MessageBlob(tm), MessageBlob(tm), MessageBlob(tm), MessageBlob(tm), MessageBlob(tm), MessageBlob(tm), MessageBlob(tm) };
			for (int i = 0; i < 10; i++)	mBlob[i].CustomImGuiRender();

			char buf[128] = { 0 };
			ImGui::InputTextWithHint("##text", "Type here...", buf, IM_ARRAYSIZE(buf));
			ImGui::SameLine();
			if (ImGui::Button("Send", { 50, 30 }))
			{
				Logger::logfmt<Log::INFO>("Send button clicked by Client#%ld!", m_ClientHash);
			}
		}
		ImGui::End();
		
	}

private:
	uint64_t m_ClientHash;
	std::vector<MessageUPtr> m_Messages;
};