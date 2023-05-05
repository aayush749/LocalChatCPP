#pragma once

#include <imgui.h>

#include <LCClient/UI/UIElement.h>
#include <LCClient/UI/Chat.h>
#include <Logger/Logger.h>
#include <LCClient/UI/FontManager.h>

#include <string>
#include <vector>
#include <initializer_list>
#include <unordered_map>

class ConversationList MAKE_UI_ELEMENT(ConversationList)
public:
	struct Contact
	{
		uint64_t hash;
		std::string name;

		Contact(uint64_t hashNum, const std::string& clientName)
			:hash(hashNum), name(clientName)
		{}
	};

public:

	ConversationList()
		:UIElement<ConversationList>(ImGui::GetIO())
	{
		OnCreate();
	}
	
	ConversationList(const std::vector<Contact>& contacts)
		:UIElement<ConversationList>(ImGui::GetIO())
		,m_Contacts(contacts)
	{
		OnCreate();
	}
	
	ConversationList(const std::initializer_list<Contact>& contacts)
		:UIElement<ConversationList>(ImGui::GetIO())
		,m_Contacts(contacts)
	{
		OnCreate();
	}

	void OnCreate()
	{
		m_Font = FontManager::GetFont(FontUsage::FONT_USAGE_CONVERSATION_LIST);
	}

	void OnImGuiRender()
	{
		static int curSelected = -1;
		ImGui::PushFont(m_Font);
		if (ImGui::TreeNode("Conversations"))
		{
			ImGui::PopFont();

			for (int contactIdx = 0; contactIdx < m_Contacts.size(); contactIdx++)
			{
				if (ImGui::Selectable(m_Contacts[contactIdx].name.c_str(), curSelected == contactIdx))
					curSelected = contactIdx;
			}

			ImGui::TreePop();
			if (curSelected >= 0 && curSelected < m_Contacts.size())
			{
				auto it = m_ChatsMap.find(m_Contacts[curSelected].hash);
				if (it != m_ChatsMap.cend())
				{
					// chat has already been spawned 
				}
				else
				{
					m_ChatsMap[m_Contacts[curSelected].hash] = std::make_unique<Chat>(m_Contacts[curSelected].hash, m_Contacts[curSelected].name.c_str());
					m_ChatsMap[m_Contacts[curSelected].hash]->OnCreate();
				}
			}
		}
		else
		{
			ImGui::PopFont();
		}

		// Now render all chats
		for (const auto& [clientContact, chat] : m_ChatsMap)
		{
			chat->OnImGuiRender();
		}
	}

	void AddNewContact(uint64_t hash, const std::string& name)
	{
		m_Contacts.emplace_back(hash, name);
	}

	Chat* GetChatFromHash(uint64_t clientHash)
	{
		auto it = m_ChatsMap.find(clientHash);
		if (it != m_ChatsMap.cend())
		{
			return it->second.get();
		}
		else
			return nullptr;
	}

private:
	std::vector<Contact> m_Contacts;
	std::unordered_map<uint64_t, std::unique_ptr<Chat>> m_ChatsMap;
	ImFont* m_Font = nullptr;
};