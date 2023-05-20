#pragma once

#include <imgui.h>

#include <LCClient/UI/UIElement.h>
#include <LCClient/UI/Chat.h>

#include <string>
#include <vector>
#include <initializer_list>
#include <unordered_map>

class ConversationList MAKE_UI_ELEMENT(ConversationList)
public:
	struct Contact
	{
		uint64_t hash;
		char name[256] = {0};

		Contact(uint64_t hashNum, const std::string& clientName)
			:hash(hashNum)
		{
			strncpy(name, clientName.c_str(), 256);
		}

		Contact(const Contact& other)
			:hash(other.hash)
		{
			strncpy(name, other.name, 256);
		}
	};

public:

	ConversationList();
	
	~ConversationList();
	
	ConversationList(const std::vector<Contact>& contacts);
	
	ConversationList(const std::initializer_list<Contact>& contacts);

	void OnCreate();

	void OnImGuiRender();

	void AddNewContact(uint64_t hash, const std::string& name);

	Chat* GetChatFromHash(uint64_t clientHash);

	/// <summary>
/// Only currently supports loading of text messages!
/// </summary>
/// <param name="friendHash"></param>
	void LoadMessagesFromDisk(const uint64_t friendHash);

private:
	std::vector<Contact> m_Contacts;
	std::unordered_map<uint64_t, std::unique_ptr<Chat>> m_ChatsMap;
	ImFont* m_Font = nullptr;
};