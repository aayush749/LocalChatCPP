#pragma once

#include <imgui.h>

#include <LCClient/UI/UIElement.h>
#include <LCClient/UI/Chat.h>
#include <Logger/Logger.h>
#include <LCClient/UI/FontManager.h>

#include <string>
#include <vector>
#include <initializer_list>

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
				static Chat chat(1001, m_Contacts[curSelected].c_str());
			}
		}
		else
		{
			ImGui::PopFont();
		}
	}

private:
	std::vector<std::string> m_Contacts;
	ImFont* m_Font = nullptr;
};