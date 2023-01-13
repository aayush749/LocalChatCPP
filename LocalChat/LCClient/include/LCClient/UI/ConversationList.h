#pragma once

#include <LCClient/UI/UIElement.h>
#include <Logger/Logger.h>

#include <string>
#include <vector>
#include <initializer_list>

class ConversationList MAKE_UI_ELEMENT(ConversationList)
{
public:
	
	ConversationList()
		:UIElement<ConversationList>()
	{}
	
	ConversationList(const std::vector<std::string>& contacts)
		:UIElement<ConversationList>()
		,m_Contacts(contacts)
	{}
	
	ConversationList(const std::initializer_list<std::string>& contacts)
		:UIElement<ConversationList>()
		, m_Contacts(contacts)
	{}

	void OnImGuiRender()
	{
		std::ostringstream oss;
		for (const auto& s : m_Contacts)
		{
			oss << s << ", ";
		}

		Logger::logfmt<Log::INFO>("OnImGui Render called for a conversation list: %s", oss.str().c_str());
	}

	void OnCreate()
	{

	}

private:
	std::vector<std::string> m_Contacts;
};