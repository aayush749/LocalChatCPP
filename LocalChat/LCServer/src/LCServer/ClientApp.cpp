#include <Logger/Logger.h>
#include <LCServer/ClientApp.h>
#include <LCServer/LCServer.h>

#include <Message/TextMessage.h>
#include <utils/SerializationUtils.h>

extern LCServer GLOBAL_SERVER;

ClientApp::ClientApp(uint64_t clientHash, ntwk::Socket&& socket)
	:m_Hash(clientHash), m_Socket(std::move(socket))
	,m_Stream(m_Socket), m_PendingMessages()
	,m_ClientShouldStop(false)
{
	m_Socket.SetNonBlockingMode(true);
	
	// Wait for the client to signal that its ready
	m_Stream.clear();
	std::wstring buf;
	while(buf.empty())
		getline(m_Stream, buf, L'\0');

	if (buf == L"start")
	{
		// Send the client hash to the client
		std::wstring serializedHash = Serialize<std::wstring>(m_Hash);
		serializedHash += L'\0';
		//m_Stream << serializedHash;
		m_Stream.write(serializedHash.c_str(), serializedHash.length());
		// Start listening
		m_ListenerThread = std::thread(&ClientApp::Listen, this);
	}
}

ClientApp::ClientApp(ClientApp&& other) noexcept
	:m_Hash(other.m_Hash), m_Socket(std::move(other.m_Socket)),
	m_Stream(StreamTp(m_Socket)), m_PendingMessages(std::move(other.m_PendingMessages)), m_ClientShouldStop(false),
	m_ListenerThread(std::move(other.m_ListenerThread))
{
	//m_Socket.SetNonBlockingMode(true);
	other.m_Hash = 0;
	other.m_ClientShouldStop = true;
}

ClientApp::~ClientApp()
{
	m_ClientShouldStop = true;
	if (m_ListenerThread.joinable())
		m_ListenerThread.join();
}

void ClientApp::Listen()
{
	Logger::logfmt<Log::INFO>("Started Listening for Client#%ld...", m_Hash);
	std::wistream& clientIStrm = dynamic_cast<std::wistream&>(m_Stream);

	while (!m_ClientShouldStop)
	{
		std::wstring buffer;
		getline(clientIStrm, buffer, L'\0');

		if (buffer.empty())
			continue;

		if (buffer == L"close")
			m_ClientShouldStop = true;

		if (buffer._Starts_with(L"TxtMsg|"))
		{
			TextMessage tm = TextMessage::DeSerialize(buffer.data());
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
			recipientSocket.SendWideBytes(content);
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
	//throw std::runtime_error("Unidentified message type passed.");s
}
