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
	
	ConversationList()
		:UIElement<ConversationList>(ImGui::GetIO())
	{
		OnCreate();
	}
	
	ConversationList(const std::vector<std::string>& contacts)
		:UIElement<ConversationList>(ImGui::GetIO())
		,m_Contacts(contacts)
	{
		OnCreate();
	}
	
	ConversationList(const std::initializer_list<std::string>& contacts)
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
				if (ImGui::Selectable(m_Contacts[contactIdx].c_str(), curSelected == contactIdx))
					curSelected = contactIdx;
			}

			ImGui::TreePop();
			if (curSelected >= 0 && curSelected < m_Contacts.size())
			{
				auto it = m_ChatsMap.find(m_Contacts[curSelected]);
				if (it != m_ChatsMap.cend())
				{
					// chat has already been spawned 
				}
				else
				{
					m_ChatsMap[m_Contacts[curSelected]] = std::make_unique<Chat>(1001, m_Contacts[curSelected].c_str());
					m_ChatsMap[m_Contacts[curSelected]]->OnCreate();
				}
			}
		}
		else
		{
			ImGui::PopFont();
		}

		// Now render all chats
		for (const auto& [clientName, chat] : m_ChatsMap)
		{
			chat->OnImGuiRender();
		}
	}

private:
	std::vector<std::string> m_Contacts;
	std::unordered_map<std::string, std::unique_ptr<Chat>> m_ChatsMap;
	ImFont* m_Font = nullptr;
};