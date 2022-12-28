#include <Logger/Logger.h>
#include <LCServer/ClientApp.h>
#include <LCServer/LCServer.h>

#include <Message/TextMessage.h>
#include <utils/SerializationUtils.h>
#include <utils/Conversion.h>

extern LCServer GLOBAL_SERVER;

static bool IsANum(const std::wstring& str)
{
	for (wchar_t c : str)
	{
		if (!isdigit(c))
		{
			return false;
		}
	}
	return true;
}


ClientApp::ClientApp(uint64_t clientHash, ntwk::Socket&& socket)
	:m_Hash(clientHash), m_Socket(std::move(socket))
	,m_Stream(m_Socket), m_PendingMessages()
	,m_ClientShouldStop(false), m_IsValidClient(false)
{
	
	// Wait for the client to signal that its ready
	m_Stream.clear();
	std::wstring buf;
	getline(m_Stream, buf, L'\0');

	if (buf == L"start")
	{
		// Send the client hash to the client
		std::wstring serializedHash = Serialize<std::wstring>(m_Hash);
		serializedHash += L'\0';
		m_Stream << serializedHash;
		
		m_IsValidClient = true;

		// Start listening
		m_ListenerThread = std::thread(&ClientApp::Listen, this);
	}
	else if (IsANum(buf))
	{
		// If it is a number
		uint64_t tempHash = cnvrt::To<uint64_t>(buf);
		bool clientExists = ::GLOBAL_SERVER.ClientExists(tempHash);

		if (clientExists)
		{
			// Mark as a valid client
			m_IsValidClient = true;

			// Send an OK confirmation to the client
			m_Stream << L"OK\0";

			// Start Listening to client
			m_ListenerThread = std::thread(&ClientApp::Listen, this);
		}
		else
		{
			// Mark as an invalid client
			m_IsValidClient = false;
			m_Stream << L"ERR\0";
		}
	}
}

ClientApp::ClientApp(ClientApp&& other) noexcept
	:m_IsValidClient(other.m_IsValidClient), m_Hash(other.m_Hash), m_Socket(std::move(other.m_Socket)),
	m_Stream(StreamTp(m_Socket)), m_PendingMessages(std::move(other.m_PendingMessages)), m_ClientShouldStop(false),
	m_ListenerThread(std::move(other.m_ListenerThread))
{
	other.m_IsValidClient = false;
	other.m_Hash = 0;
	other.m_ClientShouldStop = true;
}

ClientApp::~ClientApp()
{
	m_ClientShouldStop = true;
	m_IsValidClient = false;
	if (m_ListenerThread.joinable())
		m_ListenerThread.join();

	m_Socket.Close();
}

void ClientApp::Listen()
{
	Logger::logfmt<Log::INFO>("Started Listening for Client#%ld...", m_Hash);
	std::wistream& clientIStrm = dynamic_cast<std::wistream&>(m_Stream);

	std::wstring buffer;
	while (!m_ClientShouldStop)
	{
		getline(clientIStrm, buffer, L'\0');
		clientIStrm.clear();	// clear the input buffer as soon as you read the data

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
			Logger::logfmt<Log::ERR, 2048>("Recipient #%ld was not found, message not sent. Adding to pending message list: %s", recipientHash, e.what());
			m_PendingMessages.push_back(std::make_shared<TextMessage>(text));
		}
	}
	catch (std::bad_cast& e)
	{
		// Do nothing
	}

	// When nothing matches
	//throw std::runtime_error("Unidentified message type passed.");
}
