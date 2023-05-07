#include <LCClient/UI/ConversationList.h>

#include <LCClient/UI/FontManager.h>
#include <Logger/Logger.h>

ConversationList::ConversationList()
	:UIElement<ConversationList>(ImGui::GetIO())
{
	OnCreate();
}

ConversationList::ConversationList(const std::vector<Contact>& contacts)
	:UIElement<ConversationList>(ImGui::GetIO())
	, m_Contacts(contacts)
{
	OnCreate();
}

ConversationList::ConversationList(const std::initializer_list<Contact>& contacts)
	:UIElement<ConversationList>(ImGui::GetIO())
	, m_Contacts(contacts)
{
	OnCreate();
}

void ConversationList::OnCreate()
{
	m_Font = FontManager::GetFont(FontUsage::FONT_USAGE_CONVERSATION_LIST);
}

void ConversationList::OnImGuiRender()
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

void ConversationList::AddNewContact(uint64_t hash, const std::string& name)
{
	m_Contacts.emplace_back(hash, name);
}

Chat* ConversationList::GetChatFromHash(uint64_t clientHash)
{
	auto it = m_ChatsMap.find(clientHash);
	if (it != m_ChatsMap.cend())
	{
		return it->second.get();
	}
	else
		return nullptr;
}