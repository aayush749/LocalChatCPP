#include <LCServer/ClientApp.h>
#include <LCServer/LCServer.h>

#include <Message/TextMessage.h>
#include <utils/SerializationUtils.h>

extern LCServer GLOBAL_SERVER;

ClientApp::ClientApp(uint64_t clientHash, ntwk::Socket&& socket)
	:m_Hash(clientHash), m_Socket(std::move(socket)), m_Stream(m_Socket), m_PendingMessages(), m_ClientShouldStop(false)
	, m_ListenerThread(&ClientApp::Listen, this)
{
	// Send the client hash to the client
	
	std::wstring serializedHash = Serialize<std::wstring>(m_Hash);
	serializedHash += L'\r';
	m_Stream << serializedHash;

	// Receive the delimiter used for messages, by the client (can't be blank space (ASCII-32))
	getline(m_Stream, m_DefaultDelimiter, L' ');
}

ClientApp::ClientApp(ClientApp&& other) noexcept
	:m_Hash(other.m_Hash), m_Socket(std::move(other.m_Socket)),
	m_Stream(StreamTp(m_Socket)), m_PendingMessages(std::move(other.m_PendingMessages)), m_ClientShouldStop(false),
	m_ListenerThread()
{
	other.m_Hash = 0;
	other.m_ClientShouldStop = true;
	m_ListenerThread.swap(other.m_ListenerThread);
}

ClientApp::~ClientApp()
{
	if (m_ListenerThread.joinable())
		m_ListenerThread.join();
}

void ClientApp::Listen()
{
	while (!m_ClientShouldStop)
	{
		std::wstring buffer;
		getline(m_Stream, buffer, m_DefaultDelimiter[0]);

		if (buffer._Starts_with(L"TxtMsg|"))
		{
			TextMessage tm = TextMessage::DeSerialize(buffer);
			ProcessMessage(tm);
		}
		// Similarly would check for other message types
		else
		{
			// drop message
		}
	}
}

void ClientApp::RemoveMessage(std::list<MessageSPtr>::iterator iterator)
{
	m_PendingMessages.erase(iterator);
}

void ClientApp::ProcessMessage(const Message& msg)
{
	// Tries to send the message as it comes, if not possible, then adds to pending message list
	try
	{
		// If message is a text message
		const TextMessage& text = dynamic_cast<const TextMessage&>(msg);
		
		uint64_t recipientHash = text.GetRecipientHash();
		
		try
		{
			ntwk::Socket& recipientSocket = ::GLOBAL_SERVER.GetClientSockFromClientHash(recipientHash);

			std::wstring content;
			text.Serialize(content);
		
			// Send the message
			m_Stream << content;
		}
		catch (const std::out_of_range& e)
		{
			std::cerr << "Recipient" << recipientHash << " was not found, message not sent. Adding to pending message list.";
			m_PendingMessages.push_back(std::make_shared<TextMessage>(text));
		}
	}
	catch (std::bad_cast& e)
	{
		// Do nothing
	}

	// When nothing matches
	throw std::runtime_error("Unidentified message type passed.");
}
