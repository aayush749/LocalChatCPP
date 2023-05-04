#pragma once

#include <imgui.h>

#include <LCCLient/UI/TextureLoader.h>
#include <LCClient/UI/UIElement.h>
#include <LCClient/UI/MessageBlob.h>
#include <Logger/Logger.h>
#include <Message/Message.h>
#include <Message/TextMessage.h>
#include <LCClient/Audio/AudioManager.h>
#include <LCClient/LCClient.h>

#include <memory>
#include <imgui_internal.h>

extern TextureLoader GLOBAL_TEX_LOADER;
extern LCClient GLOBAL_CLIENT;

class Chat MAKE_UI_ELEMENT(Chat)

	using MessageUPtr = std::unique_ptr<Message>;
public:
	// Client Hash: The hash of the client represented by the current chat
	Chat(uint64_t clientHash, const char* displayName = nullptr)
		:BaseClassTp(ImGui::GetIO())
		,m_ClientHash(clientHash)
		,m_DisplayName(displayName)
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
		if (ImGui::Begin(m_DisplayName ? m_DisplayName : "Chat Window"))
		{
			for (auto& blob : m_Blobs)
			{
				blob->OnImGuiRender();
			}

			{
				const ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar;
				
				ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionAvail().x, 100));
				ImGui::Begin("Type New Message", NULL, ImGuiWindowFlags_HorizontalScrollbar);
				char buf[(size_t)1e2] = {0};
				
				static ImVector<char> my_str;
				if (my_str.empty())
					my_str.push_back(0);
				Chat::CustomInputLineEx("##MyStr", "Type message here...", &my_str, ImVec2(ImGui::GetContentRegionAvail().x * 0.919, ImGui::GetContentRegionAvail().y - 5.0f), ImGuiInputTextFlags_Multiline);

				ImGui::SameLine();
				if (ImGui::Button("Send", { 50, 30 }))
				{
					Logger::logfmt<Log::INFO>("%s", my_str.begin());
					std::wstring msg(my_str.begin(), my_str.begin() + my_str.size());

					m_Blobs.push_back(std::make_unique<MessageBlob>(
						std::make_unique<TextMessage>(GLOBAL_CLIENT.GetHash(), m_ClientHash, msg), true));

					GLOBAL_CLIENT.GetStream() << m_Blobs.back()->GetSerializedContent();

					// Play audio
					AudioManager::PlayAudio(AudioAlert::MSG_SENT);
				}
				

				ImGui::End();
			}
			
		}
		ImGui::End();
		
	}

private:
	static int TextResizeCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			ImVector<char>* my_str = (ImVector<char>*)data->UserData;
			IM_ASSERT(my_str->begin() == data->Buf);
			my_str->resize(data->BufSize); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
			data->Buf = my_str->begin();
		}
		return 0;
	}

	static bool CustomInputLineEx(const char* label, const char* hint, ImVector<char>* my_str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0)
	{
		IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
		return ImGui::InputTextEx(label, hint, my_str->begin(), (size_t)my_str->size(), size, flags | ImGuiInputTextFlags_CallbackResize, Chat::TextResizeCallback, (void*)my_str);
		//return ImGui::InputTextMultiline(label, my_str->begin(), (size_t)my_str->size(), size, flags | ImGuiInputTextFlags_CallbackResize, Chat::TextResizeCallback , (void*)my_str);
	}

private:
	uint64_t m_ClientHash;
	const char* m_DisplayName;
	std::vector<MessageUPtr> m_Messages;
	std::vector<std::unique_ptr<MessageBlob>> m_Blobs;
};