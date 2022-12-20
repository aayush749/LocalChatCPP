#include <LCServer/ClientApp.h>
#include <LCServer/LCServer.h>

#include <Message/TextMessage.h>

extern LCServer GLOBAL_SERVER;

ClientApp::ClientApp(uint64_t clientHash, ntwk::Socket&& socket)
	:m_Hash(clientHash), m_Socket(std::move(socket)), m_Stream(StreamTp(m_Socket)), m_PendingMessages(), m_ClientShouldStop(false)
	, m_ListenerThread(&ClientApp::Listen, this)
{
	// Send the client hash to the client
	m_Stream << m_Hash << L'♀';

	// Receive the delimiter used for messages, by the client
	m_Stream >> m_DefaultDelimiter;
}

void ClientApp::Listen()
{
	while (!m_ClientShouldStop)
	{
		std::wstring buffer;
		getline(m_Stream, buffer, m_DefaultDelimiter[0]);

		if (buffer._Starts_with(L"TxtMsg|"))
		{
			size_t ind = buffer.find_first_of('|');
			std::wstring_view msgView = std::wstring_view(buffer).substr(ind);
			const TextMessage* tm = reinterpret_cast<const TextMessage*>(msgView.data());
			ProcessMessage(*tm);
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
