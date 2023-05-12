#include <LCClient/UI/ConversationList.h>

#include <LCClient/UI/FontManager.h>
#include <Logger/Logger.h>

#include <fstream>
#include <optional>

static std::optional<ConversationList::Contact> ReadContactFromFile(std::ifstream& inFile)
{
	using Contact = ConversationList::Contact;

	std::optional<Contact> c = std::nullopt;	// create a null optional contact object
	
	if (inFile.is_open() && inFile.peek() != std::char_traits<char>::eof())
	{
		Contact contact(0, "");
		inFile.read((char*)&contact, sizeof(Contact));
		c = contact;
	}

	return c;
}

static void WriteContactToFile(const ConversationList::Contact& contact, std::ofstream& outFile)
{
	using Contact = ConversationList::Contact;

	if (outFile.is_open())
	{
		outFile.write((const char*)&contact, sizeof(Contact));
	}
}

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
	if (m_Created)
		return;
	m_Font = FontManager::GetFont(FontUsage::FONT_USAGE_CONVERSATION_LIST);

	// Try to load contacts stored on device
	std::filesystem::path path = std::filesystem::path(getenv("USERPROFILE")).concat("/Documents/LocalChatCpp/config/contacts.dat");
	std::ifstream inFile(path, std::ios::in | std::ios::binary);

	if (inFile.is_open())
	{
		std::size_t numContacts = 0;

		inFile.read((char*) &numContacts, sizeof(std::size_t));

		std::optional<Contact> contact = std::nullopt;

		for (std::size_t i = 0; i < numContacts; i++)
		{
			contact = ReadContactFromFile(inFile);
			if (contact.has_value())
			{
				m_Contacts.emplace_back(contact.value());
			}
		}
	}

	// Initialize all Chats present in m_Contacts initially
	for (const Contact& contact : m_Contacts)
	{
		const auto& [hash, displayName] = contact;

		m_ChatsMap.insert(
			{
				hash,
				std::make_unique<Chat>(hash, displayName)
			}
		);

		// Call OnCreate for the newly created Chat
		m_ChatsMap.at(hash)->OnCreate();
	}

	m_Created = true;
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
			if (ImGui::Selectable(m_Contacts[contactIdx].name, curSelected == contactIdx))
				curSelected = contactIdx;
		}

		ImGui::TreePop();
		if (curSelected >= 0 && curSelected < m_Contacts.size())
		{
			auto it = m_ChatsMap.find(m_Contacts[curSelected].hash);
			if (it != m_ChatsMap.cend())
			{
				// chat has already been spawned
				// Set the "is clicked" field in the Chat
				it->second->SetClicked();
			}
			else
			{
				m_ChatsMap[m_Contacts[curSelected].hash] = std::make_unique<Chat>(m_Contacts[curSelected].hash, m_Contacts[curSelected].name);
				m_ChatsMap[m_Contacts[curSelected].hash]->OnCreate();
			}
		}
	}
	else
	{
		ImGui::PopFont();
	}

	// Now render all chats only if they have been clicked at once
	for (const auto& [clientContact, chat] : m_ChatsMap)
	{
		if (chat->IsClickedAtOnce())
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

ConversationList::~ConversationList()
{
	// Save all the contents of m_Contacts to the file

	std::filesystem::path path = std::filesystem::path(getenv("USERPROFILE")).concat("/Documents/LocalChatCpp/config/contacts.dat");

	std::ofstream file(path, std::ios::out | std::ios::binary);

	// first write the number of contacts
	std::size_t numContacts = m_Contacts.size();
	file.write((const char*) &numContacts, sizeof(std::size_t));

	for (const Contact& contact : m_Contacts)
	{
		WriteContactToFile(contact, file);
	}

	// Close the file
	file.close();
}