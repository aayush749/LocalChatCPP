#pragma once

#include <imgui.h>

#include <LCCLient/UI/TextureLoader.h>
#include <LCClient/UI/UIElement.h>
#include <LCClient/UI/MessageBlob.h>
#include <Logger/Logger.h>
#include <Message/Message.h>
#include <Message/TextMessage.h>

#include <memory>
#include <imgui_internal.h>

extern TextureLoader GLOBAL_TEX_LOADER;

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
			for (int i = 0; i < 10; i++)
			{
				if (i % 2 == 0)
					if (GLOBAL_TEX_LOADER.IsLoaded(TextureType::CHAT_BUBBLE_IN))
						mBlob[i].SetTextureID(
							GLOBAL_TEX_LOADER.GetTextureID(TextureType::CHAT_BUBBLE_IN).value()
						);

					else
						continue;
				else
					if (GLOBAL_TEX_LOADER.IsLoaded(TextureType::CHAT_BUBBLE_OUT))
						mBlob[i].SetTextureID(
							GLOBAL_TEX_LOADER.GetTextureID(TextureType::CHAT_BUBBLE_OUT).value()
						);
					else
						continue;

				mBlob[i].CustomImGuiRender(i % 2 != 0);
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
	std::vector<MessageUPtr> m_Messages;
};