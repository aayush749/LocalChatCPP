#include <LCClient/UI/Chat.h>

#include <LCCLient/UI/TextureLoader.h>
#include <Message/TextMessage.h>
#include <LCClient/UI/MessageBlob.h>
#include <LCClient/Audio/AudioManager.h>
#include <Logger/Logger.h>

extern TextureLoader GLOBAL_TEX_LOADER;
extern LCClient GLOBAL_CLIENT;


Chat::Chat(uint64_t clientHash, const char* displayName)
	:BaseClassTp(ImGui::GetIO())
	, m_ClientHash(clientHash)
	, m_DisplayName(displayName)
	, m_IsClicked(false)
{}

void Chat::PushIncomingTextMsg(std::unique_ptr<Message> msg)
{
	//m_Messages.push_back(std::move(msg));

	// This method takes ownership of the 'msg'
	// and then re-transfers that ownership to the newly created message blob
	// which is created at the end of the m_Blobs vector

	m_Blobs.push_back(std::make_unique<MessageBlob>(
		std::move(msg), false));

	AudioManager::PlayAudio(AudioAlert::MSG_RECV);
}

void Chat::OnCreate()
{

}

void Chat::OnImGuiRender()
{
	bool chatWindowOpen = false;
	if (ImGui::Begin((m_DisplayName ? m_DisplayName : "Chat Window"), &chatWindowOpen))
	{
		for (auto& blob : m_Blobs)
		{
			blob->OnImGuiRender();
		}

		{
			bool textAreaWindowOpen = false;
			const ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar;

			ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionAvail().x, 100));
			ImGui::Begin("Type New Message", &textAreaWindowOpen, ImGuiWindowFlags_HorizontalScrollbar);
			char buf[(size_t)1e2] = { 0 };

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

int Chat::TextResizeCallback(ImGuiInputTextCallbackData* data)
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

bool Chat::CustomInputLineEx(const char* label, const char* hint, ImVector<char>* my_str, const ImVec2& size, ImGuiInputTextFlags flags)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	return ImGui::InputTextEx(label, hint, my_str->begin(), (size_t)my_str->size(), size, flags | ImGuiInputTextFlags_CallbackResize, Chat::TextResizeCallback, (void*)my_str);
	//return ImGui::InputTextMultiline(label, my_str->begin(), (size_t)my_str->size(), size, flags | ImGuiInputTextFlags_CallbackResize, Chat::TextResizeCallback , (void*)my_str);
}