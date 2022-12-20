#include <LCServer/ClientApp.h>

#include <Message/TextMessage.h>

ClientApp::ClientApp(uint64_t clientHash, ntwk::Socket&& socket)
	:m_Hash(clientHash), m_Socket(std::move(socket)), m_Stream(StreamTp(m_Socket)), m_PendingMessages()
{
	m_Stream << m_Hash;
}

void ClientApp::ProcessMessage(const Message& msg)
{
	try
	{
		// If message is a text message
		const TextMessage& text = dynamic_cast<const TextMessage&>(msg);
		std::wstring content;
		text.DeSerialize(content);
		
		// Send the message
		m_Stream << content;
	}
	catch (std::bad_cast& e)
	{
		// Do nothing
	}

	// When nothing matches
	throw std::runtime_error("Unidentified message type passed.");
}
