#pragma once

#include <imgui.h>

#include <LCClient/UI/UIElement.h>
#include <LCClient/UI/Chat.h>
#include <Logger/Logger.h>

#include <string>
#include <vector>
#include <initializer_list>

class ConversationList MAKE_UI_ELEMENT(ConversationList)
public:
	
	ConversationList()
		:UIElement<ConversationList>(ImGui::GetIO())
	{}
	
	ConversationList(const std::vector<std::string>& contacts)
		:UIElement<ConversationList>(ImGui::GetIO())
		,m_Contacts(contacts)
	{}
	
	ConversationList(const std::initializer_list<std::string>& contacts)
		:UIElement<ConversationList>(ImGui::GetIO())
		,m_Contacts(contacts)
	{}

	void OnCreate()
	{
		m_Font = BaseClassTp::io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\CascadiaCode.ttf", 24.0f);
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
				static Chat chat(1001);
				/*ImGui::Text("Selected chat of \"%s\"", m_Contacts[curSelected].c_str());*/

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